//============================================================================
// PlotSVG.h — trace la trajectoire de la balle en SVG vectoriel (zéro dépendance,
// lisible dans n'importe quel navigateur / Aperçu Mac). Deux vues :
//   - profil   : x (longueur) vs z (hauteur) + sol + apex
//   - dessus   : x (longueur) vs y (déviation latérale)
// matriceFlight : lignes [x, vx, y, vy, z, vz] (repère C++ : z = hauteur)
//============================================================================
#ifndef PLOTSVG_H_
#define PLOTSVG_H_

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <algorithm>

static void write_traj_svg(const std::string& path,
                           const std::vector< std::vector<double> >& mf,
                           const std::string& titre) {
    if (mf.empty()) return;
    // bornes
    double xmax = 0, zmax = 0, ymax = 1e-9;
    int iapex = 0;
    for (size_t i = 0; i < mf.size(); i++) {
        xmax = std::max(xmax, mf[i][0]);
        if (mf[i][4] > mf[iapex][4]) iapex = (int)i;
        zmax = std::max(zmax, mf[i][4]);
        ymax = std::max(ymax, std::fabs(mf[i][2]));
    }
    xmax *= 1.05; zmax *= 1.15; ymax *= 1.2;
    const int W = 900, H = 380, marge = 50, mid = H / 2 + 30;
    FILE* f = fopen(path.c_str(), "w");
    if (!f) return;
    fprintf(f, "<svg xmlns='http://www.w3.org/2000/svg' width='%d' height='%d' font-family='Helvetica' font-size='12'>\n", W, H + 40);
    fprintf(f, "<rect width='%d' height='%d' fill='white'/>\n", W, H + 40);
    fprintf(f, "<text x='%d' y='22' font-size='15' font-weight='bold'>%s</text>\n", marge, titre.c_str());

    auto sx = [&](double x) { return marge + x / xmax * (W - 2.0 * marge); };
    // ---- vue profil (haut) : x vs z hauteur ----
    auto sz = [&](double z) { return mid - z / zmax * (mid - marge); };
    // cadre + sol
    fprintf(f, "<rect x='%d' y='%d' width='%d' height='%d' fill='#eef7ee' stroke='#888'/>\n",
            marge, marge, W - 2 * marge, mid - marge);
    fprintf(f, "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='green' stroke-width='3'/>\n",
            marge, (int)sz(0), W - marge, (int)sz(0));
    // grille verticale tous les 20 m
    for (int xm = 0; xm <= (int)xmax; xm += 20)
        fprintf(f, "<line x1='%.0f' y1='%d' x2='%.0f' y2='%d' stroke='#ccc'/><text x='%.0f' y='%d' fill='#666'>%d m</text>\n",
                sx(xm), mid, sx(xm), marge, sx(xm) - 8, mid + 12, xm);
    for (int zm = 0; zm <= (int)zmax; zm += 10)
        fprintf(f, "<line x1='%d' y1='%.0f' x2='%d' y2='%.0f' stroke='#ccc'/><text x='4' y='%.0f' fill='#666'>%d m</text>\n",
                marge, sz(zm), W - marge, sz(zm), sz(zm) + 4, zm);
    // trajectoire profil
    fprintf(f, "<polyline fill='none' stroke='#1a56db' stroke-width='2' points='");
    for (size_t i = 0; i < mf.size(); i += 2) fprintf(f, "%.0f,%.0f ", sx(mf[i][0]), sz(mf[i][4]));
    fprintf(f, "'/>\n");
    // apex
    fprintf(f, "<circle cx='%.0f' cy='%.0f' r='4' fill='red'/><text x='%.0f' y='%.0f' fill='red'>apex %.1f m @ %.0f m</text>\n",
            sx(mf[iapex][0]), sz(mf[iapex][4]), sx(mf[iapex][0]) + 8, sz(mf[iapex][4]) - 6, mf[iapex][4], mf[iapex][0]);
    fprintf(f, "<text x='%d' y='%d' font-weight='bold'>Profil (x, hauteur z) — portée %.1f m</text>\n",
            marge, marge - 8, mf.back()[0]);

    // ---- vue de dessus (bas) : x vs y latéral ----
    int y0 = mid + 20, h2 = H - mid - 40;
    auto sy = [&](double y) { return y0 + h2 / 2.0 - y / ymax * (h2 / 2.0); };
    fprintf(f, "<rect x='%d' y='%d' width='%d' height='%d' fill='#f4f6fb' stroke='#888'/>\n",
            marge, y0, W - 2 * marge, h2);
    fprintf(f, "<line x1='%d' y1='%.0f' x2='%d' y2='%.0f' stroke='#999' stroke-dasharray='5,4'/>\n",
            marge, sy(0), W - marge, sy(0));
    fprintf(f, "<polyline fill='none' stroke='#c026d3' stroke-width='2' points='");
    for (size_t i = 0; i < mf.size(); i += 2) fprintf(f, "%.0f,%.0f ", sx(mf[i][0]), sy(mf[i][2]));
    fprintf(f, "'/>\n");
    fprintf(f, "<circle cx='%.0f' cy='%.0f' r='4' fill='red'/><text x='%.0f' y='%.0f' fill='red'>fin : z latéral %+.1f m</text>\n",
            sx(mf.back()[0]), sy(mf.back()[2]), sx(mf.back()[0]) - 150, sy(mf.back()[2]) - 8, mf.back()[2]);
    fprintf(f, "<text x='%d' y='%d' font-weight='bold'>Vue de dessus (x, latéral y)</text>\n", marge, y0 - 6);
    fprintf(f, "</svg>\n");
    fclose(f);
}

#endif // PLOTSVG_H_
