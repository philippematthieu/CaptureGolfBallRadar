//============================================================================
// spectro_png.cpp — génère l'image de la densité spectrale d'une captation radar
// (CSV 2 lignes ';' ou WAV stéréo), style spectro_F7-*.png du README :
//   160x125 (fenêtre /4), 2 panneaux R1/R2, zoom Y ±600 Hz autour du ridge, axe X = temps (s), axe Y = km/h (f/KV),
//   colormap 'hot' (fond noir, trace feu), ridge balle cyan (post-impact,
//   lissé), impact en rouge. Réutilise Capture.h (FFT/STFT) + BmpImg.h
//   (writer PNG/BMP maison, zéro dépendance).
// Compile : g++ -O2 -std=c++11 -I. -o spectro_png spectro_png.cpp
// Usage   : ./spectro_png <capture.csv|wav> [sortie.png] [-o|--open]
//           sortie .png = vrai PNG, autre extension = BMP 24 bits
//============================================================================
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include "Capture.h"
#include "BmpImg.h"

using namespace std;

int main(int argc, char** argv) {
        bool openAfter = false;
    std::vector<std::string> args2;
    for (int i = 1; i < argc; i++) { std::string a = argv[i]; if (a == "-o" || a == "--open") openAfter = true; else args2.push_back(a); }
    if (args2.empty()) { fprintf(stderr, "usage: spectro_png <capture.csv|wav> [sortie.png] [-o|--open]\n"); return 1; }
    string in = args2[0];
    string out = (args2.size() > 1) ? args2[1] : "spectrogramme.png";
    Capture c = load_capture(in);
    EstRadar e = estimate(c);
    const int NFFT = 2048, HOP = 256;        // hop 256 : 4x plus de frames, bandes fines
    fill_spec(e, c.r1, NFFT, HOP);           // spectrogramme canal R1 (bande 800-6500 Hz)

    // rendu compact : 640x500, 2 panneaux R1 (haut) / R2 (bas), zoom Y ±600 Hz autour du
    // ridge post-impact (pour voir les raies micro-Doppler), axe Y km/h, axe X s,
    // colormap 'hot', ridge cyan, impact rouge
    EstRadar e2 = estimate(c);   // canal R2 (e contient déjà R1)
    fill_spec(e2, c.r2, NFFT, HOP);
    const int W = 160, H = 125, ML = 30, MT = 14, MB = 18, GAP = 8;   // fenêtre /4
    int Hpan = (H - MT - MB - GAP) / 2;
    BmpImg img(W, H, 255, 255, 255);   // fond blanc
    auto hot = [](double v) {           // colormap 'hot' matplotlib
        v = std::max(0.0, std::min(1.0, v));
        return std::vector<uint8_t>{(uint8_t)std::min(255.0, v*3.0*255.0),
                                    (uint8_t)std::min(255.0, std::max(0.0, v*3.0-1.0)*255.0),
                                    (uint8_t)std::min(255.0, std::max(0.0, v*3.0-2.0)*255.0)};
    };
    const uint8_t CYAN[3] = {0, 200, 255}, ROUGE[3] = {220, 30, 30}, NOIR[3] = {0, 0, 0};
    auto draw_panel = [&](const EstRadar& e, int y0, const char* label) {
        int Wimg = W - ML - 12;
        // zoom Y : ±600 Hz autour de la médiane du ridge post-impact (raies visibles)
        std::vector<double> rr;
        for (int fr = e.spec_i0; fr < e.spec_nframe; fr++)
            if (e.spec_power[fr] > 0.03) rr.push_back(e.spec_ridge[fr]);
        double fz0 = e.spec_fmin, fz1 = e.spec_fmax;
        if (rr.size() >= 3) {
            std::sort(rr.begin(), rr.end());
            double fm = rr[rr.size() / 2];
            fz0 = std::max(e.spec_fmin, fm - 600.0);
            fz1 = std::min(e.spec_fmax, fm + 600.0);
        }
        auto X = [&](int fr){ return ML + (int)((double)fr / e.spec_nframe * Wimg); };
        auto Y = [&](double f){ return y0 + Hpan - (int)((f - fz0) / (fz1 - fz0) * Hpan); };
        img.rect(ML, y0, ML + Wimg, y0 + Hpan, 0, 0, 0, true);   // fond noir du panneau
        for (int fr = 0; fr < e.spec_nframe; fr++) {            // bandes verticales par frame (comme imshow)
            int x0 = X(fr), x1 = X(fr + 1); if (x1 <= x0) x1 = x0 + 1;
            for (int b = 0; b < e.spec_nbin; b++) {
                double f = e.spec_fmin + (b + 0.5) / e.spec_nbin * (e.spec_fmax - e.spec_fmin);
                if (f < fz0 || f > fz1) continue;
                double v = e.spec[(size_t)fr * e.spec_nbin + b] / 255.0;
                v = v * v * v;   // gamma 3 : fond sombre, trace "feu" comme matplotlib hot
                auto cc = hot(v);
                int yb = Y(f), yt = (b + 1 < e.spec_nbin) ? Y(e.spec_fmin + (b + 1.5) / e.spec_nbin * (e.spec_fmax - e.spec_fmin)) : y0;
                if (yt < y0) yt = y0;
                for (int x = x0; x < x1; x++) for (int y = yt; y <= yb; y++) img.set(x, y, cc[0], cc[1], cc[2]);
            }
        }
        img.rect(ML, y0, ML + Wimg, y0 + Hpan, NOIR[0], NOIR[1], NOIR[2]);
        int prevx = -1, prevy = -1;                             // ridge cyan post-impact, segments relies
        for (int fr = e.spec_i0; fr < e.spec_nframe; fr++) {
            if (e.spec_power[fr] < 0.03) { prevx = -1; continue; }
            double r = e.spec_ridge[fr];
            if (fr > e.spec_i0 && fr < e.spec_nframe - 1 && e.spec_power[fr-1] > 0.03 && e.spec_power[fr+1] > 0.03)
                r = (e.spec_ridge[fr-1] + r + e.spec_ridge[fr+1]) / 3;
            if (r < fz0 || r > fz1) { prevx = -1; continue; }
            int x = X(fr) + (X(fr + 1) - X(fr)) / 2, y = Y(r);
            if (prevx >= 0)
                for (int dy = -1; dy <= 1; dy++)
                    img.line(prevx, prevy + dy, x, y + dy, CYAN[0], CYAN[1], CYAN[2]);
            else
                img.circle(x, y, 2, CYAN[0], CYAN[1], CYAN[2]);
            prevx = x; prevy = y;
        }
        int xi = X(e.spec_i0) + (X(e.spec_i0 + 1) - X(e.spec_i0)) / 2;   // impact vertical rouge
        img.line(xi, y0, xi, y0 + Hpan, ROUGE[0], ROUGE[1], ROUGE[2]);
        char t[96];
        snprintf(t, sizeof(t), "t=%.3fs", e.spec_i0 * HOP / FS);
        img.text(xi + 3, y0 + 3, t, ROUGE[0], ROUGE[1], ROUGE[2]);
        img.text(6, y0 + 4, label, NOIR[0], NOIR[1], NOIR[2]);
        for (int km = (int)(fz0 / KV / 20) * 20; km <= (int)(fz1 / KV); km += 20) {  // axe Y km/h (plage zoom)
            int y = Y(km * KV);
            snprintf(t, sizeof(t), "%d", km);
            img.text(8, y - 3, t, NOIR[0], NOIR[1], NOIR[2]);
            img.line(ML - 4, y, ML, y, NOIR[0], NOIR[1], NOIR[2]);
        }
        img.text(2, y0 + Hpan / 2, "km/h", NOIR[0], NOIR[1], NOIR[2]);
    };
    draw_panel(e, MT, "R1");
    draw_panel(e2, MT + Hpan + GAP, "R2");
    double tmax = e.spec_nframe * HOP / FS;                     // axe X temps
    char t[64];
    for (double s = 0; s <= tmax + 0.001; s += 0.1) {
        int x = ML + (int)(s / tmax * (W - ML - 6));
        snprintf(t, sizeof(t), "%.1fs", s);
        img.text(x, H - MB + 4, t, NOIR[0], NOIR[1], NOIR[2]);
    }
    {   // titre court : basename tronqué à 18 caractères
        std::string base = in.substr(in.find_last_of("/\\") + 1);
        if (base.size() > 18) base = base.substr(0, 18);
        snprintf(t, sizeof(t), "Spectro - %s", base.c_str());
        img.text(2, 3, t, NOIR[0], NOIR[1], NOIR[2]);
    }
    bool ok;
    if (out.size() >= 4 && out.compare(out.size() - 4, 4, ".png") == 0)
        ok = img.save_png(out);          // vrai PNG (zlib stored, zéro dépendance)
    else
        ok = img.save_bmp(out);          // BMP 24 bits sinon
    if (!ok) { fprintf(stderr, "ERREUR: ecriture %s impossible\n", out.c_str()); return 2; }

    if (openAfter) {
#ifdef __APPLE__
        { std::string cmd = "open " + out; system(cmd.c_str()); }
#else
        { std::string cmd = "xdg-open " + out; system(cmd.c_str()); }
#endif
    }
    printf("OK -> %s  (vBall=%.1f km/h radiale, spin=%.0f rpm, alpha0=%+.2f deg, t0=frame %d)\n", out.c_str(),
           e.vball_kmh, e.spin_rpm, e.alpha0_deg, e.spec_i0);
    return 0;
}
