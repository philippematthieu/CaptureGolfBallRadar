//============================================================================
// PlotCImg.h — sauve la trajectoire en PNG via CImg (headless : -Dcimg_display=0,
// pas besoin de XQuartz). Deux panneaux : profil (x, hauteur z) et dessus (x, latéral y).
// Compile : g++ -O2 -std=c++11 -Dcimg_display=0 ... -I.   (PNG si libpng, sinon BMP/PNM)
//============================================================================
#ifndef PLOTCIMG_H_
#define PLOTCIMG_H_

#include "CImg.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace cimg_library;

static void write_traj_png_cimg(const std::string& path,
                                const std::vector< std::vector<double> >& mf,
                                const std::string& titre) {
    if (mf.empty()) return;
    double xmax = 1, zmax = 1, ymax = 1e-9;
    for (size_t i = 0; i < mf.size(); i++) {
        xmax = std::max(xmax, mf[i][0]);
        zmax = std::max(zmax, mf[i][4]);
        ymax = std::max(ymax, std::fabs(mf[i][2]));
    }
    xmax *= 1.05; zmax *= 1.15; ymax *= 1.2;
    const int W = 900, H = 760, marge = 60;
    CImg<double> img(W, H, 1, 3, 255);
    const double bleu[3] = {26, 86, 219}, violet[3] = {192, 38, 211}, vert[3] = {40, 140, 40},
                 rouge[3] = {255, 0, 0}, noir[3] = {0, 0, 0}, gris[3] = {200, 200, 200};
    img.draw_text(marge, 12, titre.c_str(), noir);

    auto sx = [&](double x) { return (float)(marge + x / xmax * (W - 2 * marge)); };
    // ---- profil (haut) ----
    int mid = 40 + 300;
    auto sz = [&](double z) { return (float)(mid - z / zmax * (mid - 40 - marge)); };
    img.draw_rectangle(marge, 40, W - marge, mid, gris);          // cadre
    img.draw_line(marge, sz(0), W - marge, sz(0), vert);           // sol
    for (int xm = 0; xm <= (int)xmax; xm += 20) {
        img.draw_line(sx(xm), mid, sx(xm), 40, gris);
        char t[32]; snprintf(t, 32, "%d m", xm); img.draw_text((int)sx(xm) - 8, mid + 4, t, noir);
    }
    for (size_t i = 1; i < mf.size(); i++)
        img.draw_line(sx(mf[i - 1][0]), sz(mf[i - 1][4]), sx(mf[i][0]), sz(mf[i][4]), bleu);
    // apex
    int ia = 0; for (size_t i = 1; i < mf.size(); i++) if (mf[i][4] > mf[ia][4]) ia = (int)i;
    img.draw_circle(sx(mf[ia][0]), sz(mf[ia][4]), 5, rouge);
    char t2[64]; snprintf(t2, 64, "apex %.1f m @ %.0f m — portee %.0f m", mf[ia][4], mf[ia][0], mf.back()[0]);
    img.draw_text(marge, 28, t2, rouge);

    // ---- dessus (bas) ----
    int y0 = mid + 30, h2 = H - y0 - 40;
    auto sy = [&](double y) { return (float)(y0 + h2 / 2.0 - y / ymax * (h2 / 2.0)); };
    img.draw_rectangle(marge, y0, W - marge, y0 + h2, gris);
    img.draw_line(marge, sy(0), W - marge, sy(0), gris);
    for (size_t i = 1; i < mf.size(); i++)
        img.draw_line(sx(mf[i - 1][0]), sy(mf[i - 1][2]), sx(mf[i][0]), sy(mf[i][2]), violet);
    img.draw_circle(sx(mf.back()[0]), sy(mf.back()[2]), 5, rouge);
    char t3[64]; snprintf(t3, 64, "z final %+.1f m", mf.back()[2]);
    img.draw_text(marge, y0 - 14, "Vue de dessus (x, lateral y)", noir);
    img.draw_text(W - 160, y0 + h2 - 8, t3, rouge);

    std::string p = path;
    if (p.size() >= 4 && p.compare(p.size() - 4, 4, ".png") == 0) {
#ifdef cimg_use_png
        img.save_png(p.c_str());
#else
        img.save_bmp((p.substr(0, p.size() - 4) + ".bmp").c_str()); // pas de libpng -> BMP
#endif
    } else {
        img.save_bmp(p.c_str());
    }
}

#endif // PLOTCIMG_H_
