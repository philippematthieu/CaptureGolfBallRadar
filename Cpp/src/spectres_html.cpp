//============================================================================
// spectres_html.cpp — visualise tous les spectres d'une captation radar en HTML
// autonome (pur canvas 2D, aucune lib externe).
//   ./spectres_html fichier.csv|.wav [-o]
// Produit spectres_<nom>.html avec les panneaux :
//   1. spectrogramme complet R1 (passe-haut 50 km/h, 0..250 km/h, ridge, impact)
//   2. zoom lobe balle : passe-bande f_D ± 2,5 kHz post-impact (raies micro-Doppler)
//   3. spectre 1D moyen post-impact autour de f_D (peigne de raies n*f_rot)
//   4. lobe club : fenêtre [t0-0,35s, t0], passe-bande vClub0 ± 15 %
//   5. patch : enveloppe de Hilbert post-impact + spectre 40..220 Hz (f_rot)
//============================================================================
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include "Capture.h"

using namespace std;

// ---------------- spectrogramme dans une bande [flo,fhi] Hz ----------------
struct Spec {
    int nframe = 0, nbin = 0, hop = 0;
    double fmin = 0, fmax = 0, tstart = 0;
    vector<unsigned char> img;   // [fr*nbin + b], b=0 -> fmin (bas)
    vector<double> ridge;        // Hz par frame
    vector<double> tg;           // temps centre de frame (s)
};

static Spec spectro_band(const vector<double>& x, int nfft, int hop,
                         double flo, double fhi, int i0, int i1) {
    Spec s;
    s.hop = hop; s.fmin = flo; s.fmax = fhi; s.tstart = i0 / FS;
    int k0 = std::max(1, (int)(flo * nfft / FS));
    int k1 = std::min(nfft / 2 - 1, (int)(fhi * nfft / FS) + 1);
    s.nbin = k1 - k0 + 1;
    vector<double> w(nfft);
    for (int i = 0; i < nfft; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (nfft - 1));
    vector<double> re(nfft), im(nfft), P(nfft / 2);
    vector<double> allp;
    int fr = 0;
    for (int st = i0; st + nfft <= i1 && st + nfft <= (int)x.size(); st += hop, fr++) {
        for (int i = 0; i < nfft; i++) { re[i] = x[st + i] * w[i]; im[i] = 0; }
        fft(re, im, false);
        for (int k = k0; k <= k1; k++) allp.push_back(re[k] * re[k] + im[k] * im[k]);
    }
    std::sort(allp.begin(), allp.end());
    double gmax = allp.empty() ? 1.0 : allp[(size_t)(allp.size() * 0.99)];
    if (gmax <= 0) gmax = 1;
    s.img.assign((size_t)std::max(0, (i1 - i0) / hop + 1) * s.nbin, 0);
    s.ridge.assign(s.img.size() / s.nbin, 0.0);
    s.tg.assign(s.img.size() / s.nbin, 0.0);
    s.nframe = (int)s.ridge.size();
    fr = 0;
    for (int st = i0; st + nfft <= i1 && st + nfft <= (int)x.size(); st += hop, fr++) {
        for (int i = 0; i < nfft; i++) { re[i] = x[st + i] * w[i]; im[i] = 0; }
        fft(re, im, false);
        for (int k = 0; k < nfft / 2; k++) P[k] = re[k] * re[k] + im[k] * im[k];
        int kb = k0; double pb = 0;
        for (int k = k0; k <= k1; k++) if (P[k] > pb) { pb = P[k]; kb = k; }
        s.ridge[fr] = parabolic_peak(P, kb) * FS / nfft;
        s.tg[fr] = (st + nfft / 2.0) / FS;
        for (int k = k0; k <= k1; k++) {
            double db = 10.0 * log10(P[k] / gmax + 1e-12);
            if (db < -80.0) db = -80.0;
            s.img[(size_t)fr * s.nbin + (k - k0)] = (unsigned char)(255.0 * (db + 80.0) / 80.0);
        }
    }
    return s;
}

// ---------------- spectre 1D moyen sur une fenêtre ----------------
static void mean_spectrum(const vector<double>& x, int nfft, int hop, int i0, int i1,
                          double flo, double fhi, vector<double>& freqs, vector<double>& db) {
    int k0 = std::max(1, (int)(flo * nfft / FS));
    int k1 = std::min(nfft / 2 - 1, (int)(fhi * nfft / FS) + 1);
    int nb = k1 - k0 + 1;
    vector<double> acc(nb, 0.0), w(nfft);
    for (int i = 0; i < nfft; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (nfft - 1));
    vector<double> re(nfft), im(nfft);
    int cnt = 0;
    for (int st = i0; st + nfft <= i1 && st + nfft <= (int)x.size(); st += hop) {
        for (int i = 0; i < nfft; i++) { re[i] = x[st + i] * w[i]; im[i] = 0; }
        fft(re, im, false);
        for (int k = k0; k <= k1; k++) acc[k - k0] += re[k] * re[k] + im[k] * im[k];
        cnt++;
    }
    if (cnt == 0) cnt = 1;
    double pmax = 0; for (int i = 0; i < nb; i++) { acc[i] /= cnt; if (acc[i] > pmax) pmax = acc[i]; }
    if (pmax <= 0) pmax = 1;
    freqs.resize(nb); db.resize(nb);
    for (int i = 0; i < nb; i++) {
        freqs[i] = (k0 + i) * FS / nfft;
        double d = 10.0 * log10(acc[i] / pmax + 1e-12);
        db[i] = (d < -80.0) ? -80.0 : d;
    }
}

// ---------------- enveloppe de Hilbert ----------------
static vector<double> hilbert_env(const vector<double>& x, int i0, int n) {
    int N = 1; while (N < n) N <<= 1;
    vector<double> re(N, 0.0), im(N, 0.0);
    for (int i = 0; i < n; i++) re[i] = x[i0 + i];
    fft(re, im, false);
    vector<double> h(N, 0.0);
    h[0] = 1.0; for (int k = 1; k < N / 2; k++) h[k] = 2.0; h[N / 2] = 1.0;
    for (int k = 0; k < N; k++) { re[k] *= h[k]; im[k] *= h[k]; }
    fft(re, im, true);
    vector<double> env(n);
    for (int i = 0; i < n; i++) env[i] = sqrt(re[i] * re[i] + im[i] * im[i]);
    return env;
}

// ---------------- JSON ----------------
static void json_spec(FILE* f, const Spec& s, double t0mark) {
    fprintf(f, "{type:'spec',t0:%.4f,fmin:%.1f,fmax:%.1f,hop:%d,tstart:%.4f,nframe:%d,nbin:%d,ridge:[",
            t0mark, s.fmin, s.fmax, s.hop, s.tstart, s.nframe, s.nbin);
    for (int i = 0; i < s.nframe; i++) fprintf(f, "%.1f,", s.ridge[i]);
    fprintf(f, "],img:[");
    for (size_t i = 0; i < s.img.size(); i++) fprintf(f, "%d,", (int)s.img[i]);
    fprintf(f, "]}");
}
static void json_escape(FILE* f, const char* s) {
    for (const char* p = s; *p; p++) {
        if (*p == '\'' || *p == '\\') fputc('\\', f);
        fputc(*p, f);
    }
}
static void json_curve(FILE* f, const char* titre, double x0, double x1,
                       const vector<double>& xs, const vector<double>& ys,
                       const vector<double>& vlines, const char* xunit) {
    fprintf(f, "{type:'curve',titre:'");
    json_escape(f, titre);
    fprintf(f, "',x0:%.6g,x1:%.6g,xunit:'%s',xs:[", x0, x1, xunit);
    for (size_t i = 0; i < xs.size(); i++) fprintf(f, "%.6g,", xs[i]);
    fprintf(f, "],ys:[");
    for (size_t i = 0; i < ys.size(); i++) fprintf(f, "%.6g,", ys[i]);
    fprintf(f, "],vlines:[");
    for (size_t i = 0; i < vlines.size(); i++) fprintf(f, "%.6g,", vlines[i]);
    fprintf(f, "]}");
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "usage: spectres_html fichier.csv|.wav [-o]\n"); return 1; }
    bool openAfter = argc > 2 && string(argv[2]) == "-o";
    string path = argv[1];
    Capture c = load_capture(path);
    EstRadar e = estimate(c);
    // analyses spin de Capture.h (avec spectres exposés pour affichage)
    vector<double> acLag, acVal, pfHz, pfDb;
    e.spin_peigne_rpm = estimate_spin_peigne_ac(c, e, &acLag, &acVal);
    e.spin_phase_rpm = estimate_spin_phase_spec(c, e, &pfHz, &pfDb, &e.spin_exc_rpm);
    double spin_fus = fusion_spin(e.spin_rpm, e.spin_phase_rpm, e.spin_peigne_rpm, e.spin_exc_rpm);
    double fD = e.vball_kmh * KV;                 // porteuse balle (Hz)
    double frot = spin_fus / 60.0;                // fréquence de rotation retenue (fusion)
    int i0 = (int)(e.t0_s * FS);
    double vclub0 = e.vball_kmh / 1.35;
    double fc = vclub0 * KV;
    double bwc = std::max(200.0, 0.30 * fc);
    double vclub_radar = estimate_club_speed(c, e);

    // panneau 2 : zoom lobe balle
    vector<double> xb = bandpass(c.r1, fD, 5000.0);
    Spec s2 = spectro_band(xb, 2048, 256, fD - 2500, fD + 2500, i0, std::min((int)c.r1.size(), i0 + (int)(0.6 * FS)));
    // panneau 3 : spectre 1D raies (marqueurs sur f_rot retenu par fusion)
    vector<double> fq, db;
    mean_spectrum(xb, 8192, 2048, i0, std::min((int)c.r1.size(), i0 + (int)(0.5 * FS)), fD - 2500, fD + 2500, fq, db);
    vector<double> vl;
    vl.push_back(fD);
    for (int n = 1; n <= 6; n++) { vl.push_back(fD + n * frot); vl.push_back(fD - n * frot); }
    // panneau 4 : lobe club
    vector<double> xc = bandpass(c.r1, fc, bwc);
    Spec s4 = spectro_band(xc, 1024, 128, fc - bwc / 2, fc + bwc / 2,
                           std::max(0, i0 - (int)(0.35 * FS)), i0 + 1024);
    // panneau 5 : patch
    int nseg = (int)(0.25 * FS);
    vector<double> env = hilbert_env(c.r1, i0, std::min(nseg, (int)c.r1.size() - i0));
    vector<double> tenv, yen;
    for (int i = 0; i < (int)env.size(); i += 20) { tenv.push_back(i / FS); yen.push_back(env[i]); }
    // spectre de l'enveloppe (detrend + hann + FFT)
    vector<double> ev = env;
    double m = 0; for (double v : ev) m += v; m /= ev.size();
    for (auto& v : ev) v -= m;
    int N = 1; while (N < (int)ev.size()) N <<= 1;
    vector<double> re(N, 0.0), im(N, 0.0);
    for (int i = 0; i < (int)ev.size(); i++) re[i] = ev[i] * (0.5 - 0.5 * cos(2.0 * M_PI * i / (ev.size() - 1)));
    fft(re, im, false);
    vector<double> fp, dp;
    {
        int k0 = (int)(40.0 * N / FS), k1 = std::min(N / 2 - 1, (int)(220.0 * N / FS));
        double pmax = 0;
        for (int k = k0; k <= k1; k++) { double p = re[k]*re[k]+im[k]*im[k]; if (p > pmax) pmax = p; }
        if (pmax <= 0) pmax = 1;
        for (int k = k0; k <= k1; k++) {
            fp.push_back(k * FS / N);
            double d = 10.0 * log10((re[k]*re[k]+im[k]*im[k]) / pmax + 1e-12);
            dp.push_back(d < -80 ? -80 : d);
        }
    }

    // ---------------- HTML ----------------
    string base = path;
    size_t slash = base.find_last_of('/');
    if (slash != string::npos) base = base.substr(slash + 1);
    string out = "spectres_" + base;
    for (auto& ch : out) if (ch == ' ') ch = '_';
    size_t dot = out.rfind('.');
    if (dot != string::npos) out = out.substr(0, dot);
    out += ".html";
    FILE* f = fopen(out.c_str(), "w");
    if (!f) { fprintf(stderr, "écriture %s impossible\n", out.c_str()); return 2; }
    fprintf(f, "<!DOCTYPE html><html><head><meta charset='utf-8'><title>spectres %s</title>\n", path.c_str());
    fprintf(f, "<style>body{margin:10px;font-family:Helvetica;background:#fff} h1{font-size:18px} "
               ".infos{font-size:13px;background:#1e1e2e;color:#eee;padding:8px 12px;border-radius:6px;margin-bottom:12px} "
               ".infos b{color:#8ab4ff} .panneau{border:1px solid #bbb;border-radius:6px;margin-bottom:14px} "
               ".panneau h2{font-size:14px;margin:0;padding:6px 10px;background:#eef2ff;border-bottom:1px solid #ccc} "
               ".panneau .note{font-size:12px;color:#555;padding:4px 10px 8px} canvas{display:block;width:100%%}\n"
               "</style></head><body>\n");
    fprintf(f, "<h1>Spectres radar — %s</h1>\n", path.c_str());
    fprintf(f, "<div class='infos'>vBall = <b>%.1f km/h</b> radiale (%.1f corr. cos20) &middot; f_D = %.0f Hz &middot; "
               "spin patch = <b>%.0f rpm</b> &middot; spin phase = <b>%.0f rpm</b> &middot; spin peigne = <b>%.0f rpm</b> &middot; "
               "<b>fusion = %.0f rpm</b> (f_rot = %.2f Hz) &middot; t0 = %.3f s &middot; "
               "vClub passe-bande = <b>%s</b> km/h (modèle vBall/1.35 = %.1f) &middot; %zu éch/canal</div>\n",
            e.vball_kmh, e.vball_corr_kmh, fD, e.spin_rpm, e.spin_phase_rpm, e.spin_peigne_rpm, spin_fus, frot, e.t0_s,
            std::isnan(vclub_radar) ? "NaN" : (std::to_string(vclub_radar) + "").c_str(), vclub0, c.r1.size());

    // panneau 1 : spectre complet (depuis estimate/fill_spec)
    fprintf(f, "<div class='panneau'><h2>1. Spectrogramme complet R1 — passe-haut 50 km/h, vue 0..250 km/h (%.0f..%.0f Hz), ridge cyan, impact pointillé</h2>\n",
            e.spec_fmin, e.spec_fmax);
    fprintf(f, "<canvas id='p0'></canvas><div class='note'>Lobe club avant t0 (vitesse montante), saut à l'impact, puis lobe balle qui décroit. Le passe-haut 50 km/h masque le lobe club sur cette vue.</div></div>\n");
    // panneau 2
    fprintf(f, "<div class='panneau'><h2>2. Zoom lobe balle — passe-bande f_D ± 2,5 kHz (%.0f..%.0f Hz), fenêtre post-impact 0,6 s</h2>\n", fD - 2500, fD + 2500);
    fprintf(f, "<canvas id='p1'></canvas><div class='note'>Le lobe balle isolé du lobe club : on voit les <b>raies micro-Doppler</b> (lignes horizontales espacées de f_rot = %.2f Hz = %.0f rpm) autour de la porteuse f_D = %.0f Hz.</div></div>\n", frot, e.spin_rpm, fD);
    // panneau 3
    fprintf(f, "<div class='panneau'><h2>3. Spectre 1D moyen post-impact (0,5 s), bande f_D ± 2,5 kHz — peigne de raies n·f_rot</h2>\n");
    fprintf(f, "<canvas id='p2'></canvas><div class='note'>Porteuse f_D en blanc ; raies latérales attendues à f_D ± n·f_rot (cyan, n=1..6, f_rot = %.2f Hz = <b>%.0f rpm</b> retenu par fusion). L'espacement des raies mesurées donne le spin indépendamment de la vitesse.</div></div>\n", frot, spin_fus);
    // panneau 4
    fprintf(f, "<div class='panneau'><h2>4. Lobe club — fenêtre [t0−0,35 s, t0], passe-bande vClub₀=%.1f km/h (%.0f Hz) ± 15 %%</h2>\n", vclub0, fc);
    fprintf(f, "<canvas id='p3'></canvas><div class='note'>Vitesse de la tête de club montante avant l'impact ; la crête (ridge cyan) au quantile 90 %% donne <b>vClub = %s km/h</b> (ligne rouge si mesurée).</div></div>\n",
            std::isnan(vclub_radar) ? "NaN" : (std::to_string((int)(vclub_radar + 0.5)) + "").c_str());
    // panneau 5
    fprintf(f, "<div class='panneau'><h2>5. Patch micro-Doppler — enveloppe de Hilbert post-impact (250 ms) et son spectre 40..220 Hz</h2>\n");
    fprintf(f, "<canvas id='p4'></canvas><canvas id='p5'></canvas><div class='note'>L'enveloppe bat à f_rot ; le pic du spectre (ligne rouge à %.2f Hz) donne le <b>spin patch = %.0f rpm</b>.</div></div>\n", e.spin_rpm / 60.0, e.spin_rpm);
    // panneau 6 : autocorrélation du spectre (peigne) — sortie de estimate_spin_peigne_ac
    fprintf(f, "<div class='panneau'><h2>6. Autocorrélation du spectre Welch autour de f_D (peigne) — lag en Hz</h2>\n");
    fprintf(f, "<canvas id='p6'></canvas><div class='note'>Pic au lag f_rot = %.2f Hz -> <b>spin peigne = %.0f rpm</b> (normalisé par ac[0]).</div></div>\n",
            e.spin_peigne_rpm / 60.0, e.spin_peigne_rpm);
    // panneau 7 : spectre de la fréquence instantanée (phase) — sortie de estimate_spin_phase_spec
    fprintf(f, "<div class='panneau'><h2>7. Spectre de la fréquence instantanée du lobe balle (dérivée de phase, detrend 0,1 s) 40..200 Hz</h2>\n");
    fprintf(f, "<canvas id='p7'></canvas><div class='note'>Pic à f_rot = %.2f Hz -> <b>spin phase = %.0f rpm</b>.</div></div>\n",
            e.spin_phase_rpm / 60.0, e.spin_phase_rpm);

    fprintf(f, "<script>\nvar P=[\n");
    // p0 : spectre complet depuis e.spec
    fprintf(f, "{type:'spec',t0:%.4f,fmin:%.1f,fmax:%.1f,hop:%d,tstart:0,nframe:%d,nbin:%d,ridge:[",
            e.t0_s, e.spec_fmin, e.spec_fmax, e.spec_hop, e.spec_nframe, e.spec_nbin);
    for (int i = 0; i < e.spec_nframe; i++) fprintf(f, "%.1f,", e.spec_ridge[i]);
    fprintf(f, "],img:[");
    for (size_t i = 0; i < e.spec.size(); i++) fprintf(f, "%d,", (int)e.spec[i]);
    fprintf(f, "]},\n");
    json_spec(f, s2, e.t0_s); fprintf(f, ",\n");
    json_curve(f, "spectre 1D post-impact (dB), axe X = Hz, labels km/h en haut", fD - 2500, fD + 2500, fq, db, vl, "Hz"); fprintf(f, ",\n");
    json_spec(f, s4, e.t0_s); fprintf(f, ",\n");
    json_curve(f, "enveloppe de Hilbert post-impact", 0, (double)env.size() / FS, tenv, yen, {0.0}, "s"); fprintf(f, ",\n");
    json_curve(f, "spectre de l enveloppe 40..220 Hz (dB)", 40, 220, fp, dp, {e.spin_rpm / 60.0}, "Hz"); fprintf(f, ",\n");
    json_curve(f, "autocorrelation du spectre autour de f_D (peigne), ac/ac0", 0, 200, acLag, acVal, {e.spin_peigne_rpm / 60.0}, "Hz"); fprintf(f, ",\n");
    json_curve(f, "spectre de la frequence instantanee 40..200 Hz (dB)", 40, 200, pfHz, pfDb, {e.spin_phase_rpm / 60.0}, "Hz"); fprintf(f, "\n];\n");

    fputs(R"JS(
var FS=44100.0;
var VIR=[[68,1,84],[72,35,116],[59,82,139],[33,145,140],[94,201,98],[253,231,37]];
function cmap(v){v=Math.max(0,Math.min(1,v));var x=v*5,i=Math.floor(x),f=x-i;if(i>4)i=4;
return [VIR[i][0]+(VIR[i+1][0]-VIR[i][0])*f,VIR[i][1]+(VIR[i+1][1]-VIR[i][1])*f,VIR[i][2]+(VIR[i+1][2]-VIR[i][2])*f];}
var ML=64,MR=70,MT=22,MB=26;
function drawSpec(p,cv){
var W=cv.clientWidth;if(W<100)W=900;var H=260;cv.width=W;cv.height=H;
var off=document.createElement('canvas');off.width=p.nframe;off.height=p.nbin;var og=off.getContext('2d');
var im=og.createImageData(p.nframe,p.nbin);
for(var fr=0;fr<p.nframe;fr++)for(var b=0;b<p.nbin;b++){var v=p.img[fr*p.nbin+b]/255.0;var cc=cmap(v);
var i=((p.nbin-1-b)*p.nframe+fr)*4;im.data[i]=cc[0];im.data[i+1]=cc[1];im.data[i+2]=cc[2];im.data[i+3]=255;}
og.putImageData(im,0,0);
var g=cv.getContext('2d');var w=W-ML-MR,h=H-MT-MB;
g.fillStyle='#000';g.fillRect(0,0,W,H);g.imageSmoothingEnabled=true;g.drawImage(off,ML,MT,w,h);
// ridge
g.strokeStyle='#00ffff';g.lineWidth=2;g.beginPath();
for(var fr=0;fr<p.nframe;fr++){var fx=ML+fr/(p.nframe-1)*w;var fy=MT+(1-(p.ridge[fr]-p.fmin)/(p.fmax-p.fmin))*h;if(fr==0)g.moveTo(fx,fy);else g.lineTo(fx,fy);}g.stroke();
// impact t0
g.strokeStyle='#fff';g.setLineDash([4,3]);g.lineWidth=1.5;
var tx=ML+(p.t0-p.tstart)/(p.hop/FS)/(p.nframe-1)*w; // approx: t0 en frame depuis tstart
// plus robuste : t0 - tstart en secondes / (hop/FS)
tx=ML+((p.t0-p.tstart)*FS/p.hop)/(p.nframe-1)*w;
if(tx>ML&&tx<ML+w){g.beginPath();g.moveTo(tx,MT);g.lineTo(tx,MT+h);g.stroke();g.setLineDash([]);g.fillStyle='#fff';g.font='11px Helvetica';g.fillText('impact t0='+p.t0.toFixed(3)+'s',tx+4,MT+12);}
// labels Y en km/h
g.font='11px Helvetica';g.fillStyle='#ccc';
for(var km=0;km<=350;km+=25){var fq=km*19.49;if(fq<p.fmin||fq>p.fmax)continue;var fy=MT+(1-(fq-p.fmin)/(p.fmax-p.fmin))*h;
g.fillText(km+'',4,fy+3);g.strokeStyle='#333';g.beginPath();g.moveTo(ML,fy);g.lineTo(ML+w,fy);g.stroke();}
// labels X en s
var tmax=p.tstart+(p.nframe-1)*p.hop/FS;
for(var t=Math.ceil(p.tstart*10)/10;t<=tmax+0.001;t+=0.1){var x=ML+(t-p.tstart)/(tmax-p.tstart)*w;g.fillStyle='#ccc';g.fillText(t.toFixed(1)+'s',x+2,H-8);}
g.fillStyle='#888';g.fillText('Hz',ML+w+6,MT+10);g.fillText('km/h ->',ML+w+6,MT+22);
}
function drawCurve(p,cv){
var W=cv.clientWidth;if(W<100)W=900;var H=220;cv.width=W;cv.height=H;var g=cv.getContext('2d');
var w=W-ML-MR,h=H-MT-MB;
var ymin=Math.min.apply(null,p.ys),ymax=Math.max.apply(null,p.ys);if(ymax-ymin<1e-9){ymax=ymin+1;}
g.fillStyle='#000';g.fillRect(0,0,W,H);
g.strokeStyle='#fff';g.strokeRect(ML,MT,w,h);
g.strokeStyle='#00aaff';g.lineWidth=1.5;g.beginPath();
for(var i=0;i<p.xs.length;i++){var x=ML+(p.xs[i]-p.x0)/(p.x1-p.x0)*w;var y=MT+(1-(p.ys[i]-ymin)/(ymax-ymin))*h;if(i==0)g.moveTo(x,y);else g.lineTo(x,y);}g.stroke();
g.strokeStyle='#ff4444';g.setLineDash([5,4]);g.lineWidth=1.2;
for(var i=0;i<p.vlines.length;i++){var x=ML+(p.vlines[i]-p.x0)/(p.x1-p.x0)*w;if(x<ML||x>ML+w)continue;g.beginPath();g.moveTo(x,MT);g.lineTo(x,MT+h);g.stroke();}
g.setLineDash([]);
g.fillStyle='#ccc';g.font='11px Helvetica';
for(var t=p.x0;t<=p.x1;t+=(p.x1-p.x0)/8){var x=ML+(t-p.x0)/(p.x1-p.x0)*w;g.fillText(t.toFixed(p.x1-p.x0<10?2:0)+p.xunit,x+2,H-8);}
for(var v=0;v<=4;v++){var y=MT+h-v*h/4;g.fillText((ymin+(ymax-ymin)*v/4).toFixed(ymax-ymin<10?2:0),4,y+3);}
// labels raies en km/h au-dessus pour le spectre 1D (xunit Hz)
if(p.xunit=='Hz'){g.fillStyle='#8ab4ff';for(var km=Math.ceil(p.x0/19.49/25)*25;km*19.49<=p.x1;km+=25){var x=ML+(km*19.49-p.x0)/(p.x1-p.x0)*w;if(x<ML||x>ML+w)continue;g.fillText(km+' km/h',x-12,MT+10);}}
g.fillStyle='#888';g.fillText(p.titre,ML+4,MT-8>10?MT-8:12);
}
function render(){for(var i=0;i<P.length;i++){var cv=document.getElementById('p'+i);if(!cv)continue;
if(P[i].type=='spec')drawSpec(P[i],cv);else drawCurve(P[i],cv);}}
render();addEventListener('load',render);
addEventListener('resize',render);
)JS", f);
    fprintf(f, "</script></body></html>\n");
    fclose(f);
    printf("HTML écrit : %s\n", out.c_str());
    printf("vBall=%.1f km/h (f_D=%.0f Hz), spin patch=%.0f / phase=%.0f / peigne=%.0f -> fusion=%.0f rpm (f_rot=%.2f Hz), t0=%.3f s, vClub=%s km/h\n",
           e.vball_kmh, fD, e.spin_rpm, e.spin_phase_rpm, e.spin_peigne_rpm, spin_fus, frot, e.t0_s,
           std::isnan(vclub_radar) ? "NaN" : (std::to_string((int)(vclub_radar + 0.5)) + "").c_str());
    if (openAfter) {
#ifdef __APPLE__
        system(("open " + out).c_str());
#else
        system(("xdg-open " + out).c_str());
#endif
    }
    return 0;
}
