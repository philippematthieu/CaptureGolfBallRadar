//============================================================================
// spectro_png.cpp — génère l'image de la densité spectrale d'une captation radar
// (CSV 2 lignes ';' ou WAV stéréo), style spectro_F7-*.png du README :
//   990x770, 2 panneaux R1/R2, axe X = temps (s), axe Y = km/h (f/KV),
//   colormap 'hot' (fond noir, trace feu), ridge balle cyan (post-impact,
//   lissé), impact en rouge. Réutilise Capture.h (FFT/STFT) + BmpImg.h (BMP 24 bits
//   maison, zéro dépendance ; convertir en PNG sur Mac : sips -s format png x.bmp).
// Compile : g++ -O2 -std=c++11 -I. -o spectro_png spectro_png.cpp
// Usage   : ./spectro_png <capture.csv|wav> [sortie.bmp] [-o|--open]
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
    fill_spec(e, c.r1, 2048, 1024);          // spectrogramme canal R1 (bande 800-6500 Hz)

    // rendu identique aux spectro_F7-*.png de Python : 990x770, fond blanc, colormap 'hot',
    // 2 panneaux verticaux R1 (haut) / R2 (bas), axe Y en km/h, axe X en s, ridge cyan, impact rouge
    EstRadar e2 = estimate(c);   // canal R2 (e contient déjà R1)
    fill_spec(e2, c.r2, 2048, 1024);
    const int W = 990, H = 770, ML = 58, MT = 30, MB = 40, GAP = 26;
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
        int Wimg = W - ML - 14;
        auto X = [&](int fr){ return ML + (int)((double)fr / e.spec_nframe * Wimg); };
        auto Y = [&](double f){ return y0 + Hpan - (int)((f - e.spec_fmin) / (e.spec_fmax - e.spec_fmin) * Hpan); };
        for (int fr = 0; fr < e.spec_nframe; fr++) {            // bandes verticales par frame (comme imshow)
            int x0 = X(fr), x1 = X(fr + 1); if (x1 <= x0) x1 = x0 + 1;
            for (int b = 0; b < e.spec_nbin; b++) {
                double f = e.spec_fmin + (b + 0.5) / e.spec_nbin * (e.spec_fmax - e.spec_fmin);
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
            if (r > e.spec_fmax) { prevx = -1; continue; }
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
        snprintf(t, sizeof(t), "impact t=%.3fs", e.spec_i0 * 1024.0 / FS);
        img.text(xi + 6, y0 + 10, t, ROUGE[0], ROUGE[1], ROUGE[2]);
        img.text(6, y0 + 4, label, NOIR[0], NOIR[1], NOIR[2]);
        for (int km = (int)(e.spec_fmin / KV / 50) * 50; km <= (int)(e.spec_fmax / KV); km += 50) {  // axe Y km/h
            int y = Y(km * KV);
            snprintf(t, sizeof(t), "%d", km);
            img.text(30, y - 3, t, NOIR[0], NOIR[1], NOIR[2]);
            img.line(ML - 4, y, ML, y, NOIR[0], NOIR[1], NOIR[2]);
        }
        img.text(4, y0 + Hpan / 2, "km/h", NOIR[0], NOIR[1], NOIR[2]);
    };
    draw_panel(e, MT, "R1");
    draw_panel(e2, MT + Hpan + GAP, "R2");
    double tmax = e.spec_nframe * 1024.0 / FS;                  // axe X temps
    char t[64];
    for (double s = 0; s <= tmax + 0.001; s += 0.1) {
        int x = ML + (int)(s / tmax * (W - ML - 14));
        snprintf(t, sizeof(t), "%.1fs", s);
        img.text(x, H - MB + 10, t, NOIR[0], NOIR[1], NOIR[2]);
    }
    snprintf(t, sizeof(t), "Densite spectrale radar - %s", in.c_str());
    img.text(ML, 8, t, NOIR[0], NOIR[1], NOIR[2]);
    std::string bmp = out;
    if (bmp.size() >= 4 && bmp.compare(bmp.size() - 4, 4, ".png") == 0) bmp = bmp.substr(0, bmp.size() - 4) + ".bmp";
    img.save_bmp(bmp);
    out = bmp;   // pour le -o et le message

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
