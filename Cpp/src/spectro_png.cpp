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

    // ratio 6:1 comme les spectro Python : largeur(temps) = 6 x hauteur(vitesse km/h)
    const int ML = 64, MB = 34, MT = 22;
    const double SY = 0.45;                       // px par km/h en Y
    double kmhMax = (e.spec_fmax - e.spec_fmin) / KV;
    int Himg = (int)(kmhMax * SY);                // hauteur zone image (px)
    int Wimg = 6 * Himg;                          // largeur = 6 x hauteur (ratio 6:1)
    int W = ML + Wimg + 16, H = MT + Himg + MB;
    CImg<unsigned char> img(W, H, 1, 3, 0);
    // colormap viridis
    const double vir[6][3] = {{68,1,84},{72,35,116},{59,82,139},{33,145,140},{94,201,98},{253,231,37}};
    auto cmap = [&](double v) {
        v = std::max(0.0, std::min(1.0, v));
        double x = v * 5; int i = std::min(4, (int)x); double f = x - i;
        return std::vector<unsigned char>{(unsigned char)(vir[i][0]+(vir[i+1][0]-vir[i][0])*f),
                                          (unsigned char)(vir[i][1]+(vir[i+1][1]-vir[i][1])*f),
                                          (unsigned char)(vir[i][2]+(vir[i+1][2]-vir[i][2])*f)};
    };
    auto X = [&](int fr){ return ML + (int)((double)fr / (e.spec_nframe - 1) * Wimg); };
    auto Y = [&](double f){ return MT + (int)((1.0 - (f - e.spec_fmin) / (e.spec_fmax - e.spec_fmin)) * Himg); };
    // densité transposée : X = temps (frame), Y = fréquence (haut = haut km/h)
    for (int fr = 0; fr < e.spec_nframe; fr++)
        for (int b = 0; b < e.spec_nbin; b++) {
            double v = e.spec[(size_t)fr * e.spec_nbin + b] / 255.0;
            auto cc = cmap(v);
            int x = X(fr), y = Y(e.spec_fmin + (b + 0.5) / e.spec_nbin * (e.spec_fmax - e.spec_fmin));
            if (x < W && y < H) img(x, y, 0, 0) = cc[0], img(x, y, 0, 1) = cc[1], img(x, y, 0, 2) = cc[2];
        }
    // ridge balle cyan post-impact, lissé, seuillé
    const double cyan[3] = {0, 255, 255}, rouge[3] = {255, 40, 40}, blanc[3] = {255, 255, 255};
    for (int fr = e.spec_i0; fr < e.spec_nframe; fr++) {
        if (e.spec_power[fr] < 0.03) continue;
        double r = e.spec_ridge[fr];
        if (fr > e.spec_i0 && fr < e.spec_nframe - 1 && e.spec_power[fr-1] > 0.03 && e.spec_power[fr+1] > 0.03)
            r = (e.spec_ridge[fr-1] + r + e.spec_ridge[fr+1]) / 3;
        img.draw_circle(X(fr), Y(r), 2, cyan);
    }
    // shoot / impact : ligne verticale rouge + label
    int xi = X(e.spec_i0);
    img.draw_line(xi, MT, xi, MT + Himg, rouge);
    img.draw_line(xi - 3, MT, xi, MT + 6, rouge); img.draw_line(xi + 3, MT, xi, MT + 6, rouge);
    char t[96];
    snprintf(t, sizeof(t), "IMPACT t=%.3f s", e.spec_i0 * 1024.0 / FS);
    img.draw_text(xi + 6, MT + 10, t, rouge);
    // axes : Y km/h à gauche, X secondes en bas
    for (int km = 0; km <= 300; km += 50) {
        double f = km * KV;
        if (f < e.spec_fmin || f > e.spec_fmax) continue;
        int y = Y(f);
        snprintf(t, sizeof(t), "%d km/h", km);
        img.draw_text(4, y - 5, t, blanc);
        img.draw_line(ML - 4, y, ML, y, blanc);
    }
    double tmax = e.spec_nframe * 1024.0 / FS;
    for (double s = 0; s <= tmax + 0.001; s += 0.1) {
        int x = ML + (int)(s / tmax * Wimg);
        snprintf(t, sizeof(t), "%.1fs", s);
        img.draw_text(x, MT + Himg + 6, t, blanc);
    }
    snprintf(t, sizeof(t), "spectrogramme R1 — %s  (ratio 6:1, Y=km/h)", in.c_str());
    img.draw_text(ML, 6, t, blanc);

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
