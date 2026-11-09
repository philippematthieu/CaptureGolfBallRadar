//============================================================================
// Capture.h — lecture captation radar (CSV 2 lignes ';' ou WAV stéréo) et
// estimation simplifiée : vBall (ridge STFT post-impact), spin total
// (périodicité du patch via enveloppe de Hilbert), alpha0 (appariement stéréo).
// Header-only, C++11, aucune dépendance externe (FFT radix-2 itérative).
//============================================================================
#ifndef CAPTURE_H_
#define CAPTURE_H_
#if defined(__GNUC__)
#define CAP_UNUSED __attribute__((unused))
#else
#define CAP_UNUSED
#endif

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double KV = 19.49;        // Hz / (km/h) radar 10.525 GHz (f_D = KV * V_kmh)
static const double HP_KMH = 50.0;     // passe-haut avant spectrogramme d'affichage (km/h) : rejette le lobe du club (comme plot_spectro.py --hp 50)
static const double VMAX_KMH = 250.0;  // vue pleine spectrogramme : axe Y 0..250 km/h
static const double KS = 0.1567;       // Hz / RPM (excursion micro-Doppler, d = 0.04267 m, doc §10)
static const double BALL_D = 0.04267;  // diamètre balle (m), doc §10/§2
static const double FS = 44100.0;
static const double B_BASE = 0.30;     // baseline radars (m)
static const double L0 = 1.50;         // distance initiale balle (m)
static const double THETA_RADAR = 20.0 * M_PI / 180.0;  // biais géométrique radar (corrigé après)

// interpolation sous-bin parabolique du pic (comme pipe_validation.parabolic_peak)
CAP_UNUSED static double parabolic_peak(const std::vector<double>& P, int k) {
    if (k <= 0 || k >= (int)P.size() - 1) return (double)k;
    double den = P[k-1] - 2.0*P[k] + P[k+1];
    if (den == 0.0) return (double)k;
    double d = 0.5 * (P[k-1] - P[k+1]) / den;
    return k + (std::fabs(d) <= 1.0 ? d : 0.0);
}

// ---------------- FFT itérative radix-2 (longueur = puissance de 2) --------
CAP_UNUSED static void fft(std::vector<double>& re, std::vector<double>& im, bool inverse) {
    const int n = (int)re.size();
    for (int i = 1, j = 0; i < n; i++) {          // bit-reversal
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) { std::swap(re[i], re[j]); std::swap(im[i], im[j]); }
    }
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2.0 * M_PI / len * (inverse ? 1.0 : -1.0);
        double wr = cos(ang), wi = sin(ang);
        for (int i = 0; i < n; i += len) {
            double cwr = 1.0, cwi = 0.0;
            for (int j = 0; j < len / 2; j++) {
                double ur = re[i + j], ui = im[i + j];
                double vr = re[i + j + len / 2] * cwr - im[i + j + len / 2] * cwi;
                double vi = re[i + j + len / 2] * cwi + im[i + j + len / 2] * cwr;
                re[i + j] = ur + vr; im[i + j] = ui + vi;
                re[i + j + len / 2] = ur - vr; im[i + j + len / 2] = ui - vi;
                double nwr = cwr * wr - cwi * wi;
                cwi = cwr * wi + cwi * wr; cwr = nwr;
            }
        }
    }
    if (inverse) for (int i = 0; i < n; i++) { re[i] /= n; im[i] /= n; }
}

// ---------------- passe-haut FFT (brique) : zero les bins |f| < fcut ----------
CAP_UNUSED static std::vector<double> highpass(const std::vector<double>& x, double fcut) {
    int n = (int)x.size();
    int N = 1; while (N < n) N <<= 1;                 // FFT radix-2 : zero-pad
    std::vector<double> re(N, 0.0), im(N, 0.0);
    double m = 0; for (int i = 0; i < n; i++) m += x[i]; m /= n;
    for (int i = 0; i < n; i++) re[i] = x[i] - m;
    fft(re, im, false);
    for (int k = 0; k < N; k++) {
        double f = (k <= N / 2) ? k * FS / N : (k - N) * FS / N;
        if (std::fabs(f) < fcut) { re[k] = 0.0; im[k] = 0.0; }
    }
    fft(re, im, true);
    std::vector<double> y(n);
    for (int i = 0; i < n; i++) y[i] = re[i];
    return y;
}

// ---------------- chargement ------------------------------------------------
struct Capture {
    std::vector<double> r1, r2;   // canaux R1/R2
    double fs = FS;
};

CAP_UNUSED static Capture load_capture(const std::string& path) {
    Capture c;
    std::string low = path;
    std::transform(low.begin(), low.end(), low.begin(), ::tolower);
    if (low.size() >= 4 && low.compare(low.size() - 4, 4, ".wav") == 0) {
        FILE* f = fopen(path.c_str(), "rb");
        if (!f) { fprintf(stderr, "ERREUR: %s introuvable\n", path.c_str()); exit(2); }
        unsigned char hdr[44];
        if (fread(hdr, 1, 44, f) != 44) { fclose(f); exit(2); }
        int ch = hdr[22] | (hdr[23] << 8);
        int bits = hdr[34] | (hdr[35] << 8);
        c.fs = hdr[24] | (hdr[25] << 8) | (hdr[26] << 16) | (hdr[27] << 24);
        // data chunk
        unsigned char sz[4];
        long pos = 44;
        while (pos < 1 << 20) {
            fseek(f, pos, SEEK_SET);
            if (fread(sz, 1, 4, f) != 4) break;
            unsigned dsize = sz[0] | (sz[1] << 8) | (sz[2] << 16) | (sz[3] << 24);
            char id[5] = {0};
            fseek(f, pos + 4, SEEK_SET);
            if (fread(id, 1, 4, f) != 4) break;
            if (memcmp(id, "data", 4) == 0) { fseek(f, pos + 8, SEEK_SET); break; }
            pos += 8 + dsize;
        }
        long n = (long)(c.fs * 4); // ~4 s max
        std::vector<double> data;
        if (bits == 16) {
            std::vector<int16_t> buf(n * ch);
            long got = (long)fread(buf.data(), 2, n * ch, f);
            for (long i = 0; i < got; i++) data.push_back(buf[i] / 32768.0);
        } else if (bits == 8) {
            std::vector<uint8_t> buf(n * ch);
            long got = (long)fread(buf.data(), 1, n * ch, f);
            for (long i = 0; i < got; i++) data.push_back(buf[i] / 128.0 - 1.0);
        } else { // 32 bits float ou int
            std::vector<int32_t> buf(n * ch);
            long got = (long)fread(buf.data(), 4, n * ch, f);
            for (long i = 0; i < got; i++) data.push_back(buf[i] / 2147483648.0);
        }
        fclose(f);
        long nfr = (long)data.size() / ch;
        c.r1.resize(nfr); c.r2.resize(nfr);
        for (long i = 0; i < nfr; i++) {
            c.r1[i] = data[i * ch];
            c.r2[i] = (ch > 1) ? data[i * ch + 1] : data[i * ch];
        }
    } else { // CSV 2 lignes ';'
        std::ifstream in(path.c_str());
        if (!in) { fprintf(stderr, "ERREUR: %s introuvable\n", path.c_str()); exit(2); }
        std::string line;
        int row = 0;
        while (std::getline(in, line) && row < 2) {
            std::vector<double>& dst = (row == 0) ? c.r1 : c.r2;
            std::stringstream ss(line);
            std::string tok;
            while (std::getline(ss, tok, ';')) {
                if (!tok.empty()) dst.push_back(strtod(tok.c_str(), NULL));
            }
            row++;
        }
        if (c.r2.empty()) c.r2 = c.r1;
    }
    return c;
}

// ---------------- spectrogramme + ridge --------------------------------------
CAP_UNUSED static void spectrogram(const std::vector<double>& x, int nfft, int hop,
                        std::vector<double>& ridge, std::vector<double>& power,
                        std::vector<double>& tgrid) {
    int n = (int)x.size();
    int nframes = 1 + (n - nfft) / hop;
    std::vector<double> w(nfft);
    for (int i = 0; i < nfft; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (nfft - 1)); // Hann
    ridge.assign(nframes, 0.0); power.assign(nframes, 0.0); tgrid.assign(nframes, 0.0);
    std::vector<double> re(nfft), im(nfft);
    for (int fr = 0; fr < nframes; fr++) {
        for (int i = 0; i < nfft; i++) { re[i] = x[fr * hop + i] * w[i]; im[i] = 0.0; }
        fft(re, im, false);
        std::vector<double> P(nfft / 2);
        int kmax = 1; double pmax = 0.0;
        for (int k = 1; k < nfft / 2; k++) {
            double f = k * FS / nfft;
            P[k] = re[k] * re[k] + im[k] * im[k];
            if (f < 1000.0 || f > 6000.0) { P[k] = -1.0; continue; }  // passe-bande vitesse balle
            if (P[k] > pmax) { pmax = P[k]; kmax = k; }
        }
        ridge[fr] = parabolic_peak(P, kmax) * FS / nfft;   // sous-bin (comme Python)
        power[fr] = pmax;
        tgrid[fr] = (fr * hop + nfft / 2.0) / FS;
    }
}

// ---------------- estimation (conforme pipe_validation.py) -------------------
struct EstRadar {
    double vball_kmh;      // radiale
    double vball_corr_kmh; // corrigée cos(20°)
    double spin_rpm;       // périodicité du patch
    double alpha0_deg;     // azimut initial (appariement stéréo)
    double dalpha_deg;     // courbure début-fin
    double t0_s;
    // spectrogramme embarqué pour affichage fixe (densité 0..255, bande FMIN..FMAX Hz, axe Y en km/h via /KV)
    std::vector<unsigned char> spec;   // [nframes * nbin], ligne = frame temporelle
    int spec_nbin = 0, spec_nframe = 0, spec_hop = 0;
    double spec_fmin = 0, spec_fmax = 0;
    std::vector<double> spec_ridge;    // Hz par frame
    std::vector<double> spec_power;    // puissance max par frame (0..1 normalisée)
    int spec_i0 = 0;                   // frame impact
};

CAP_UNUSED static void fill_spec(EstRadar& e, const std::vector<double>& sig, int NFFT, int HOP) {
        // conforme plot_spectro.py : passe-haut 50 km/h, vue pleine 0..250 km/h,
        // ridge = crête pleine bande par frame (interpolation parabolique, comme Python)
        const double FMIN = 0.0, FMAX = VMAX_KMH * KV;      // 0..4872,5 Hz
        std::vector<double> s = highpass(sig, HP_KMH * KV); // passe-haut 974,5 Hz (50 km/h)
        int nframes = 1 + ((int)s.size() - NFFT) / HOP;
        int k0 = (int)(FMIN * NFFT / FS), k1 = (int)(FMAX * NFFT / FS);
        if (k1 >= NFFT / 2) k1 = NFFT / 2 - 1;
        e.spec_nbin = k1 - k0 + 1;
        e.spec_nframe = nframes;
        e.spec_hop = HOP;
        e.spec_fmin = FMIN; e.spec_fmax = FMAX;
        e.spec.resize((size_t)nframes * e.spec_nbin);
        e.spec_ridge.assign(nframes, 0.0);
        e.spec_power.assign(nframes, 0.0);
        std::vector<double> w(NFFT), re(NFFT), im(NFFT), P(NFFT / 2);
        for (int i = 0; i < NFFT; i++) w[i] = 0.5 - 0.5 * cos(2.0 * M_PI * i / (NFFT - 1));
        // passe 1 : p99 pour la normalisation dB
        std::vector<double> allp;
        for (int fr = 0; fr < nframes; fr++) {
            for (int i = 0; i < NFFT; i++) { re[i] = s[fr * HOP + i] * w[i]; im[i] = 0; }
            fft(re, im, false);
            for (int k = k0; k <= k1; k++) allp.push_back(re[k] * re[k] + im[k] * im[k]);
        }
        std::sort(allp.begin(), allp.end());
        double gmax = allp.empty() ? 1.0 : allp[(size_t)(allp.size() * 0.99)];  // p99 comme matplotlib
        if (gmax <= 0) gmax = 1;
        // passe 2 : image dB [-80..0] + ridge pleine bande + puissance
        for (int fr = 0; fr < nframes; fr++) {
            for (int i = 0; i < NFFT; i++) { re[i] = s[fr * HOP + i] * w[i]; im[i] = 0; }
            fft(re, im, false);
            for (int k = 0; k < NFFT / 2; k++) P[k] = re[k] * re[k] + im[k] * im[k];
            int kb = 1; double pmax = 0;
            for (int k = 1; k < NFFT / 2; k++) if (P[k] > pmax) { pmax = P[k]; kb = k; }
            e.spec_ridge[fr] = parabolic_peak(P, kb) * FS / NFFT;
            e.spec_power[fr] = pmax / gmax;
            for (int k = k0; k <= k1; k++) {
                double db = 10.0 * log10(P[k] / gmax + 1e-12);
                if (db < -80.0) db = -80.0;
                e.spec[(size_t)fr * e.spec_nbin + (k - k0)] = (unsigned char)(255.0 * (db + 80.0) / 80.0);
            }
        }
        int i0l = 0; for (size_t i = 1; i < e.spec_power.size(); i++) if (e.spec_power[i] > e.spec_power[i0l]) i0l = (int)i;
        e.spec_i0 = i0l;
}

CAP_UNUSED static EstRadar estimate(const Capture& c) {
    EstRadar e; e.vball_kmh = 0; e.vball_corr_kmh = 0; e.spin_rpm = 0; e.alpha0_deg = 0; e.dalpha_deg = 0; e.t0_s = 0;
    const int NFFT = 2048, HOP = NFFT / 2;
    std::vector<double> ridge, power, tg;
    spectrogram(c.r1, NFFT, HOP, ridge, power, tg);
    // impact = frame max énergie
    int i0 = 0; for (size_t i = 1; i < power.size(); i++) if (power[i] > power[i0]) i0 = (int)i;
    e.t0_s = tg[i0];
    // vBall = médiane du ridge post-impact (50 ms) sur CHAQUE canal, puis rejet inter-radars > 15 %
    // (comme pipe_validation : si les 2 canaux divergent = faux lobe, on garde la médiane des 2)
    auto vball_canal = [&](const std::vector<double>& x) -> double {
        std::vector<double> r2, p2, t2;
        spectrogram(x, NFFT, HOP, r2, p2, t2);
        int j0 = 0; for (size_t i = 1; i < p2.size(); i++) if (p2[i] > p2[j0]) j0 = (int)i;
        std::vector<double> post;
        for (size_t i = j0; i < r2.size() && t2[i] < t2[j0] + 0.05; i++) post.push_back(r2[i]);
        if (post.empty()) return 0.0;
        std::sort(post.begin(), post.end());
        return post[post.size() / 2] / KV;
    };
    double v1 = vball_canal(c.r1), v2 = vball_canal(c.r2);
    if (v1 > 0 && v2 > 0) {
        double med = 0.5 * (v1 + v2);
        if (std::fabs(v1 - v2) / med > 0.15) {          // divergence > 15 % -> faux lobe, on prend la médiane
            e.vball_kmh = med;
        } else {
            e.vball_kmh = med;
        }
    } else {
        e.vball_kmh = (v1 > 0) ? v1 : v2;
    }
    e.vball_corr_kmh = e.vball_kmh / cos(THETA_RADAR);
    // --- spin par périodicité du patch, avec score d'harmoniques (comme Python) ---
    int nseg = (int)(0.25 * FS);
    if ((i0 * HOP + nseg) < (int)c.r1.size() && nseg > NFFT) {
        int N = 1; while (N < nseg) N <<= 1;
        std::vector<double> re(N, 0.0), im(N, 0.0);
        for (int i = 0; i < nseg; i++) re[i] = c.r1[i0 * HOP + i];
        fft(re, im, false);
        std::vector<double> h(N, 0.0);
        h[0] = 1.0; for (int k = 1; k < N / 2; k++) h[k] = 2.0; h[N / 2] = 1.0;
        for (int k = 0; k < N; k++) { re[k] *= h[k]; im[k] *= h[k]; }
        fft(re, im, true);                                   // signal analytique
        std::vector<double> env(N);
        for (int i = 0; i < N; i++) env[i] = sqrt(re[i] * re[i] + im[i] * im[i]);
        double m = 0; for (int i = 0; i < N; i++) m += env[i]; m /= N;
        for (int i = 0; i < N; i++) { re[i] = (env[i] - m) * (0.5 - 0.5 * cos(2.0 * M_PI * i / (N - 1))); im[i] = 0; }
        fft(re, im, false);
        std::vector<double> Pe(N / 2);
        std::vector<int> loc;
        double pmax = 0;
        for (int k = 1; k < N / 2; k++) {
            double f = k * FS / N;
            Pe[k] = re[k] * re[k] + im[k] * im[k];
            if (f >= 50.0 && f <= 200.0 && Pe[k] > 0.1 * 0 + 0) {} // bande f_rot 3000..12000 rpm
            if (f >= 50.0 && f <= 200.0 && Pe[k] > pmax) pmax = Pe[k];
            if (f >= 50.0 && f <= 200.0 && k > 0 && k < N / 2 - 1 && Pe[k] >= Pe[k-1] && Pe[k] >= Pe[k+1] && Pe[k] > 0.1 * pmax)
                loc.push_back(k);
        }
        if (loc.empty() && pmax > 0) { int k = 1; for (int j = 1; j < N / 2; j++) if (Pe[j] > Pe[k]) k = j; loc.push_back(k); }
        // score d'harmoniques : le fondamental explique le plus de pics (évite f_rot/2)
        double df = FS / N;
        int best = loc.empty() ? -1 : loc[0]; double bestscore = -1;
        for (int p : loc) {
            double f0 = p * df, score = Pe[p];
            for (int q : loc) {
                if (q == p) continue;
                int k = (int)lround(q * df / f0);
                if (k >= 1 && std::fabs(q * df - k * f0) < 2 * df) score += Pe[q];
            }
            if (score > bestscore) { bestscore = score; best = p; }
        }
        if (best < 0 && !loc.empty()) best = loc[0];              // fallback : 1er pic local
        if (best < 0) { int k = 1; for (int j = 1; j < N / 2; j++) if (Pe[j] > Pe[k]) k = j; if (Pe[k] > 0) best = k; } // fallback : max global
        if (best > 0) e.spin_rpm = 60.0 * parabolic_peak(Pe, best) * df;
    }
    // --- alpha(t) par appariement stéréo : f2 cherché au voisinage de f1 (±300 Hz), |f1-f2|<=30 Hz ---
    // spectrogrammes des 2 canaux
    std::vector<double> ridge2, power2, tg2;
    spectrogram(c.r2, NFFT, HOP, ridge2, power2, tg2);
    std::vector<double> tt, al;
    double v_ms = e.vball_kmh / KV * 1000.0 / 3600.0;
    for (size_t i = i0; i < ridge.size(); i++) {
        if (power[i] < 0.02 * power[i0]) continue;
        double f1 = ridge[i];
        // chercher le pic de R2 dans ±300 Hz autour de f1
        int nframes2 = (int)ridge2.size();
        if ((int)i >= nframes2) continue;
        // spectre de la frame i du canal 2
        std::vector<double> re(NFFT), im(NFFT);
        std::vector<double> w(NFFT);
        for (int j = 0; j < NFFT; j++) w[j] = 0.5 - 0.5 * cos(2.0 * M_PI * j / (NFFT - 1));
        for (int j = 0; j < NFFT; j++) { re[j] = c.r2[i * HOP + j] * w[j]; im[j] = 0; }
        fft(re, im, false);
        std::vector<double> P2(NFFT / 2);
        for (int k = 1; k < NFFT / 2; k++) P2[k] = re[k] * re[k] + im[k] * im[k];
        int kbest = -1; double pbest = 0;
        for (int k = 1; k < NFFT / 2; k++) {
            double f = k * FS / NFFT;
            if (f < f1 - 300 || f > f1 + 300) continue;
            if (P2[k] > pbest) { pbest = P2[k]; kbest = k; }
        }
        if (kbest < 0 || pbest < 0.01 * power2[i0]) continue;
        double f2 = parabolic_peak(P2, kbest) * FS / NFFT;
        if (std::fabs(f1 - f2) > 30.0) continue;              // cohérence géométrique
        double fc = (f1 + f2) / 2.0, fd = (f1 - f2) / 2.0;
        if (fc <= 0) continue;
        double L = L0 + v_ms / cos(THETA_RADAR) * (tg[i] - tg[i0]);  // L(t) évolutif
        tt.push_back(tg[i]);
        al.push_back(atan((2.0 * L / B_BASE) * (fd / fc)) * 180.0 / M_PI);
    }
    fill_spec(e, c.r1, 512, 128);   // fenêtre 11,6 ms comme plot_spectro.py (ridge bien résolu en temps)
    if (al.size() >= 3) {
        // alpha0 = médiane fenêtre initiale 50 ms ; dalpha = médiane(fin 100 ms) - médiane(début)
        std::vector<double> a0, af;
        for (size_t i = 0; i < tt.size(); i++) {
            if (tt[i] <= tg[i0] + 0.05) a0.push_back(al[i]);
            if (tt[i] >= tt.back() - 0.10) af.push_back(al[i]);
        }
        if (!a0.empty() && !af.empty()) {
            std::sort(a0.begin(), a0.end()); std::sort(af.begin(), af.end());
            e.alpha0_deg = a0[a0.size() / 2];
            e.dalpha_deg = af[af.size() / 2] - a0[a0.size() / 2];
        }
    }
    return e;
}


// signal Doppler synthetique a partir d'une trajectoire simulee (mode club, pour le spectrogramme en bas)
CAP_UNUSED static std::vector<double> gen_signal_simu(const std::vector< std::vector<double> >& mf, double vclub_kmh, double fs = FS) {
    const double lam = 299792458.0 / 10.525e9;
    const double r = BALL_D / 2.0;
    int N = (int)(mf.size() * 0.01 * fs) + (int)(0.15 * fs);
    std::vector<double> s(N, 0.0);
    double phase = 0.0;
    int t0i = (int)(0.10 * fs);
    double spin = 6000.0 * 2.0 * M_PI / 60.0;
    for (int n = 0; n < N; n++) {
        double t = n / fs;
        double fD;
        if (n < t0i) {
            double vc = vclub_kmh / 3.6 * pow((double)n / t0i, 2.5);
            fD = KV * vc * 3.6;
        } else {
            int k = (int)((t - 0.10) / 0.01);
            if (k >= (int)mf.size()) k = (int)mf.size() - 1;
            if (k < 0) k = 0;
            double V = sqrt(mf[k][1]*mf[k][1] + mf[k][3]*mf[k][3] + mf[k][5]*mf[k][5]);
            fD = KV * V * 3.6 * cos(2.0 * M_PI / 180.0);
        }
        phase += 2.0 * M_PI * fD / fs;
        double patch = 0.3 * sin((2.0 * r / lam) * sin(spin * t));
        double sig = cos(phase) * (1.0 + patch);
        if (n > t0i - 200 && n < t0i + 200) sig += 3.0 * exp(-pow((n - t0i) / 60.0, 2.0)) * cos(2.0 * M_PI * 2200.0 * t);
        s[n] = 0.4 * sig + 0.02 * (rand() / (double)RAND_MAX - 0.5) * 2.0;
    }
    return s;
}

#endif // CAPTURE_H_
