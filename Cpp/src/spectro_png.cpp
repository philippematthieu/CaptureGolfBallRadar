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
#include "PlotCImg.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "usage: spectro_png <capture.csv|wav> [sortie.png]\n"); return 1; }
    string out = (argc > 2) ? argv[2] : "spectrogramme.png";
    Capture c = load_capture(argv[1]);
    EstRadar e = estimate(c);
    fill_spec(e, c.r1, 2048, 1024);          // spectrogramme canal R1 (bande 800-6500 Hz)

    const int ML = 64, MB = 34, W = e.spec_nframe * 14 + ML, H = e.spec_nbin * 3 + MB;
    CImg<unsigned char> img(W, H, 1, 3, 0);
    // colormap viridis
    const double vir[6][3] = {{68,1,84},{72,35,116},{59,82,139},{33,145,140},{94,201,98},{253,231,37}};
    auto cmap = [&](double v) {
        v = std::max(0.0, std::min(1.0, v));
        double x = v * 5; int i = std::min(4, (int)x); double f = x - i;
        unsigned char r = (unsigned char)(vir[i][0] + (vir[i+1][0]-vir[i][0])*f);
        unsigned char g = (unsigned char)(vir[i][1] + (vir[i+1][1]-vir[i][1])*f);
        unsigned char b = (unsigned char)(vir[i][2] + (vir[i+1][2]-vir[i][2])*f);
        return std::vector<unsigned char>{r, g, b};
    };
    // transposé : X = frame (temps), Y = bin freq (haut = haute freq)
    for (int fr = 0; fr < e.spec_nframe; fr++)
        for (int b = 0; b < e.spec_nbin; b++) {
            double v = e.spec[(size_t)fr * e.spec_nbin + b] / 255.0;
            auto cc = cmap(v);
            int x0 = ML + fr * 14, y0 = MB + (e.spec_nbin - 1 - b) * 3;
            for (int dy = 0; dy < 3; dy++) for (int dx = 0; dx < 14; dx++)
                if (x0 + dx < W && y0 + dy < H) img(x0 + dx, y0 + dy, 0, 0) = cc[0], img(x0 + dx, y0 + dy, 0, 1) = cc[1], img(x0 + dx, y0 + dy, 0, 2) = cc[2];
        }
    // ridge cyan post-impact, lissé, seuillé
    const double noir[3] = {0, 255, 255}, rouge[3] = {255, 40, 40}, blanc[3] = {255, 255, 255};
    for (int fr = e.spec_i0; fr < e.spec_nframe; fr++) {
        if (e.spec_power[fr] < 0.03) continue;
        double r = e.spec_ridge[fr];
        if (fr > e.spec_i0 && fr < e.spec_nframe - 1 && e.spec_power[fr-1] > 0.03 && e.spec_power[fr+1] > 0.03)
            r = (e.spec_ridge[fr-1] + r + e.spec_ridge[fr+1]) / 3;
        int x = ML + fr * 14 + 7;
        int y = MB + (int)((1.0 - (r - e.spec_fmin) / (e.spec_fmax - e.spec_fmin)) * (e.spec_nbin - 1) * 3);
        if (x >= 0 && x < W && y >= 0 && y < H) img.draw_circle(x, y, 2, noir);
    }
    // impact : ligne verticale rouge
    int xi = ML + e.spec_i0 * 14;
    img.draw_line(xi, MB, xi, H - MB, rouge);
    // axes : labels Y km/h à gauche, X secondes en bas
    char t[64];
    for (int km = 0; km <= 300; km += 50) {
        double f = km * KV;
        if (f < e.spec_fmin || f > e.spec_fmax) continue;
        int y = MB + (int)((1.0 - (f - e.spec_fmin) / (e.spec_fmax - e.spec_fmin)) * (e.spec_nbin - 1) * 3);
        snprintf(t, sizeof(t), "%d km/h", km);
        img.draw_text(4, y - 5, t, blanc);
        img.draw_line(ML - 4, y, ML, y, blanc);
    }
    double tmax = e.spec_nframe * 1024.0 / FS;
    for (double s = 0; s <= tmax + 0.001; s += 0.1) {
        int x = ML + (int)(s / tmax * (e.spec_nframe - 1) * 14);
        snprintf(t, sizeof(t), "%.1fs", s);
        img.draw_text(x, H - MB + 6, t, blanc);
    }
    snprintf(t, sizeof(t), "spectrogramme R1 — %s", argv[1]);
    img.draw_text(ML, 6, t, blanc);

    if (out.size() >= 4 && out.compare(out.size() - 4, 4, ".png") == 0) img.save_png(out.c_str());
    else img.save_bmp((out.substr(0, out.size() - 4) + ".bmp").c_str());
    printf("OK -> %s  (vBall=%.1f km/h radiale, spin=%.0f rpm, alpha0=%+.2f deg, t0=frame %d)\n", out.c_str(),
           e.vball_kmh, e.spin_rpm, e.alpha0_deg, e.spec_i0);
    return 0;
}
