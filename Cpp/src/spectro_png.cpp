//============================================================================
// spectro_png.cpp — génère un PNG de la densité spectrale d'une captation radar
// (CSV 2 lignes ';' ou WAV stéréo), style spectro_F7-*.png du README :
//   axe X = temps (s), axe Y = km/h (f/19.49), colormap viridis, ridge balle cyan
//   (post-impact, lissé), impact en rouge. Réutilise Capture.h (FFT/STFT) et CImg.
// Compile : g++ -O2 -std=c++11 -Dcimg_display=0 -DUSE_CIMG -I. -o spectro_png \
//             spectro_png.cpp   (CImg header-only, PNG si libpng sinon BMP)
// Usage   : ./spectro_png <capture.csv|wav> [sortie.png]
//============================================================================
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include "Capture.h"
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wchar-subscripts"
#pragma GCC diagnostic ignored "-Wnontrivial-memcall"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "CImg.h"
using namespace cimg_library;

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
    int Hpan = (H - MT - MB - GAP) / 2;          // hauteur d'un panneau
    CImg<unsigned char> img(W, H, 1, 3, 255);    // fond blanc
    auto hot = [](double v) {                    // colormap 'hot' de matplotlib
        v = std::max(0.0, std::min(1.0, v));
        unsigned char r = (unsigned char)std::min(255.0, v * 3.0 * 255.0);
        unsigned char g = (unsigned char)std::min(255.0, std::max(0.0, v * 3.0 - 1.0) * 255.0);
        unsigned char b = (unsigned char)std::min(255.0, std::max(0.0, v * 3.0 - 2.0) * 255.0);
        return std::vector<unsigned char>{r, g, b};
    };
    const double cyan[3] = {0, 200, 255}, rouge[3] = {220, 30, 30}, noir[3] = {0, 0, 0};
    auto draw_panel = [&](const EstRadar& e, int y0, const char* label) {
        double fmax_kmh = 300.0 * KV;            // 0..300 km/h en Y
        double fmin = 0.0;
        int Wimg = W - ML - 14;
        auto X = [&](int fr){ return ML + (int)((double)fr / (e.spec_nframe - 1) * Wimg); };
        auto Y = [&](double f){ return y0 + Hpan - (int)((f - fmin) / fmax_kmh * Hpan); };
        // densité (X=temps, Y=freq, haut = haut km/h)
        for (int fr = 0; fr < e.spec_nframe; fr++)
            for (int b = 0; b < e.spec_nbin; b++) {
                double v = pow(e.spec[(size_t)fr * e.spec_nbin + b] / 255.0, 0.7);
                auto cc = hot(v);
                double f = e.spec_fmin + (b + 0.5) / e.spec_nbin * (e.spec_fmax - e.spec_fmin);
                if (f > fmax_kmh) continue;
                int x = X(fr), y = Y(f);
                if (x < W && y >= y0 && y < y0 + Hpan) { img(x, y, 0, 0) = cc[0], img(x, y, 0, 1) = cc[1], img(x, y, 0, 2) = cc[2]; }
            }
        // cadre du panneau
        img.draw_rectangle(ML, y0, ML + Wimg, y0 + Hpan, noir);
        // ridge cyan post-impact lissé seuillé
        for (int fr = e.spec_i0; fr < e.spec_nframe; fr++) {
            if (e.spec_power[fr] < 0.03) continue;
            double r = e.spec_ridge[fr];
            if (fr > e.spec_i0 && fr < e.spec_nframe - 1 && e.spec_power[fr-1] > 0.03 && e.spec_power[fr+1] > 0.03)
                r = (e.spec_ridge[fr-1] + r + e.spec_ridge[fr+1]) / 3;
            if (r > fmax_kmh) continue;
            img.draw_circle(X(fr), Y(r), 2, cyan);
        }
        // impact : ligne verticale rouge + label
        int xi = X(e.spec_i0);
        img.draw_line(xi, y0, xi, y0 + Hpan, rouge);
        char t[96];
        snprintf(t, sizeof(t), "impact t=%.3fs", e.spec_i0 * 1024.0 / FS);
        img.draw_text(xi + 5, y0 + 12, t, rouge);
        // label du canal + titre panneau
        img.draw_text(6, y0 + 4, label, noir);
        // axe Y km/h à gauche
        for (int km = 0; km <= 300; km += 50) {
            int y = Y(km * KV);
            snprintf(t, sizeof(t), "%d", km);
            img.draw_text(30, y - 5, t, noir);
            img.draw_line(ML - 4, y, ML, y, noir);
        }
        img.draw_text(4, y0 + Hpan / 2, "km/h", noir);
    };
    draw_panel(e, MT, "R1");
    draw_panel(e2, MT + Hpan + GAP, "R2");
    // axe X temps en bas
    char t[64];
    double tmax = e.spec_nframe * 1024.0 / FS;
    for (double s = 0; s <= tmax + 0.001; s += 0.1) {
        int x = ML + (int)(s / tmax * (W - ML - 14));
        snprintf(t, sizeof(t), "%.1f s", s);
        img.draw_text(x, H - MB + 8, t, noir);
    }
    snprintf(t, sizeof(t), "Densite spectrale radar — %s", in.c_str());
    img.draw_text(ML, 8, t, noir);

    if (out.size() >= 4 && out.compare(out.size() - 4, 4, ".png") == 0) {
        try {
            img.save_png(out.c_str());          // nécessite libpng (brew install libpng)
        } catch (const cimg_library::CImgException&) {  // CImgIOException en hérite
            // pas de libpng / pas d'outil externe : fallback BMP propre (pas de plantage)
            out = out.substr(0, out.size() - 4) + ".bmp";
            img.save_bmp(out.c_str());
            fprintf(stderr, "(libpng absent -> BMP écrit à la place: %s ; brew install libpng pour du PNG)\n", out.c_str());
        }
    } else {
        img.save_bmp(out.c_str());
    }
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
