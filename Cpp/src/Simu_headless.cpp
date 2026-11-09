//============================================================================
// Simu_headless.cpp — main sans CImg (compilable en console sur Mac/Linux)
// Usage : Simu_headless Type ClubV0kmh AlphaClubPathDeg GamaFacePathDeg ShaftLeanImpDeg [spinScale]
//   ex  : Simu_headless F7 122.3 0 0 -6.5 1.0     (spin normal)
//         Simu_headless F7 122.3 0 0 -6.5 0.0     (spin nul : test Magnus)
//============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "Club.h"
#include "Ball.h"
#include "Capture.h"
#include "PlotSVG.h"
#include "Plot3D.h"
#ifdef USE_CIMG
#include "PlotCImg.h"
#endif

using namespace std;


static std::string spec_to_json(const EstRadar& e) {
    if (e.spec.empty()) return "";
    std::string s = "{nbin:" + std::to_string(e.spec_nbin) + ",nframe:" + std::to_string(e.spec_nframe)
        + ",fmin:" + std::to_string(e.spec_fmin) + ",fmax:" + std::to_string(e.spec_fmax)
        + ",i0:" + std::to_string(e.spec_i0) + ",ridge:[";
    for (size_t i = 0; i < e.spec_ridge.size(); i++) { char b[32]; snprintf(b, 32, "%.1f,", e.spec_ridge[i]); s += b; }
    s += "],img:[";
    for (size_t i = 0; i < e.spec.size(); i++) { char b[8]; snprintf(b, 8, "%d,", (int)e.spec[i]); s += b; }
    s += "]}";
    return s;
}

static std::string kv(const char* k, const std::string& v) {
    return std::string("<div class='k'>") + k + "</div><div class='v'>" + v + "</div>";
}
static std::string fmt1(double x, const char* u) {
    char b[64]; snprintf(b, sizeof(b), "%.1f %s", x, u); return b;
}
static std::string fmt0(double x, const char* u) {
    char b[64]; snprintf(b, sizeof(b), "%.0f %s", x, u); return b;
}

// mode captation : ./simu_headless --capture ../sci/2019-02-24-F7-123m-....csv [club] [spinScale]
static int run_from_capture(const char* path, const char* clubType, double spinScale) {
    Capture c = load_capture(path);
    EstRadar e = estimate(c);
    printf("capture %s : vBall=%.1f km/h radiale (%.1f corr. cos20), spin=%.0f rpm, alpha0=%+.2f deg, dAlpha=%+.2f deg, t0=%.3f s, %zu ech/canal\n",
           path, e.vball_kmh, e.vball_corr_kmh, e.spin_rpm, e.alpha0_deg, e.dalpha_deg, e.t0_s, c.r1.size());
    // club par défaut F7
    vector<Club> sac(13);
    sac[5] = Club("F7", 0.200, 20.0, 30, 0.83, 380.00, 420.0, 0.54, 122.3, 0.0, 0.0, -6.5);
    unsigned j = 5;
    for (unsigned i = 0; i < sac.size(); i++) if (sac[i].getType() == clubType) j = i;
    // vitesse club estimée = vBall / 1.35 (smash typique)
    sac[j].setClubV0Kmh(e.vball_kmh / 1.35);
    sac[j].setAlphaClubPathDegre(e.alpha0_deg);
    Ball theBall("Decathlon", 320, 3, sac[j], 18.0, 0.01);
    theBall.rescaleSpin(spinScale * e.spin_rpm / max(theBall.getSpinYOrgrpm(), 1.0));
    theBall.runSimu();
    auto mf = theBall.getMatriceFlight();
    printf("RESULT depuis captation : portee=%.1f m  apex=%.1f m  z_final=%+.1f m  temps=%.1f s\n",
           mf.back()[0], theBall.getMaxHeight(), mf.back()[2], theBall.getTempsTotal());
    { std::string inf = kv("vBall", fmt1(e.vball_kmh, "km/h") + "  (" + fmt1(e.vball_corr_kmh, "corr.") + ")")
  + kv("SpinTotal", fmt0(e.spin_rpm, "rpm")) + kv("alpha0", fmt1(e.alpha0_deg, "deg"))
  + kv("Delta alpha", fmt1(e.dalpha_deg, "deg")) + kv("Impact t0", fmt1(e.t0_s, "s"))
  + kv("Portee", fmt1(mf.back()[0], "m")) + kv("Apex", fmt1(theBall.getMaxHeight(), "m"));
  write_traj_3d_html("trajectoire_capture_3d.html", mf, "Trajectoire 3D depuis captation", inf, spec_to_json(e)); }
    printf("HTML 3D ecrit : trajectoire_capture_3d.html\n");
    write_traj_svg("trajectoire_capture.svg", mf, "Trajectoire depuis captation — portée " +
                   std::to_string((int)mf.back()[0]) + " m");
    printf("SVG écrit : trajectoire_capture.svg\n");
    return 0;
}

int main(int argc, char *argv[]) {
    vector<Club> sac(13);
    unsigned j = 0;
    //        Type, Poids, Temp, Loft, Ecoeff, CoeffBackSpin, CoeffSpinLift, Cl1,   ClubV0, AlphaClubPath, GamaFacePath, ShaftLeanImp
    sac[0]  = Club("Dr",  0.300 , 20.0, 11.0 , 0.738,  200.00 ,       1500.0,         0.64,  170.6,  -8.0,           2.0,         6.0);
    sac[1]  = Club("B5",  0.280 , 20.0, 18   , 0.78 ,  281.64 ,       500.0 ,        0.65,  151.6,   5.0,          -5.0,        -3.0);
    sac[2]  = Club("H3",  0.250 , 20.0, 18   , 0.80 ,  322.00 ,       500.0 ,        0.54,  136.8,   0.0,           0.0,        -2.5);
    sac[3]  = Club("F5",  0.200 , 20.0, 23   , 0.85 ,  335.00 ,       420.0 ,        0.54,  127.1,   0.0,           0.0,        -5.0);
    sac[4]  = Club("F6",  0.200 , 20.0, 26   , 0.83 ,  355.00 ,       420.0 ,        0.54,  125.5,   0.0,           0.0,        -5.0);
    sac[5]  = Club("F7",  0.200 , 20.0, 30   , 0.83 ,  380.00 ,       420.0 ,        0.54,  122.3,   0.0,           0.0,        -6.5);
    sac[6]  = Club("F8",  0.200 , 20.0, 34.5 , 0.83 ,  340.00 ,       420.0 ,        0.53,  120.0,   0.0,           0.0,        -8.0);
    sac[7]  = Club("F9",  0.200 , 20.0, 39   , 0.83 ,  400.99 ,       308.0 ,        0.53,  115.9,   0.0,           0.0,        -8.0);
    sac[8]  = Club("PW",  0.200 , 20.0, 44   , 0.78 ,  438.24 ,       280.0 ,        0.52,  112.7,   0.0,           0.0,       -12.0);
    sac[9]  = Club("AW",  0.200 , 20.0, 52   , 0.78 ,  470.09 ,       308.0 ,        0.52,  112.7,   0.0,           0.0,       -17.0);
    sac[10] = Club("SW",  0.200 , 20.0, 54   , 0.71 ,  520.00 ,       308.0 ,        0.15,  104.6,   0.0,           0.0,       -15.0);
    sac[11] = Club("LW",  0.200 , 20.0, 60   , 0.40 ,  500.00 ,       308.0 ,        0.15,  100.0,   0.0,          -3.0,       -12.0);
    sac[12] = Club("Pt",  0.300 , 20.0, 03   , 0.68 ,  550.24 ,       308.0 ,        0.00,   10.00,   0.0,           0.0,         1.0);

    if (argc < 2 || string(argv[1]) == "--help" || string(argv[1]) == "-h" || string(argv[1]) == "help") {
        cout << "Simulateur de vol de balle de golf (C++) — vol + 3 rebonds + roulement\n"
                "\n"
                "usage:\n"
                "  simu_headless Type ClubV0kmh AlphaClubPathDeg GamaFacePathDeg ShaftLeanImpDeg [spinScale]\n"
                "  simu_headless --capture fichier.csv|.wav [club] [spinScale]\n"
                "  simu_headless --help\n"
                "\n"
                "clubs: Dr B5 H3 F5 F6 F7 F8 F9 PW AW SW LW Pt\n"
                "\n"
                "options:\n"
                "  Type            club (liste ci-dessus)\n"
                "  ClubV0kmh       vitesse club en km/h (ex: 122.3)\n"
                "  AlphaClubPathDeg  azimut du chemin de club en degres (ex: 0)\n"
                "  GamaFacePathDeg   angle face/trajectoire en degres -> side spin (ex: 0)\n"
                "  ShaftLeanImpDeg   shaft lean a l'impact en degres -> loft dynamique\n"
                "  spinScale       echelle du spin, 1.0 = normal, 0 = pas de spin (test Magnus)\n"
                "  --capture f     lit une captation radar (CSV 2 lignes ';' ou WAV stereo 44,1 kHz),\n"
                "                  estime vBall/spin/alpha0, puis simule depuis ces mesures\n"
                "\n"
                "sorties (dans le dossier courant):\n"
                "  console: RESULT portee=... apex=... z_final=... temps=...\n"
                "  trajectoire_<club>.svg      image 2D (profil + dessus)\n"
                "  trajectoire_<club>.png/.bmp image 2D CImg (si compile avec -DUSE_CIMG)\n"
                "  trajectoire_3d.html          scene 3D interactive three.js (souris: tourner/zoom;\n"
                "                              slider = vitesse anim; projection au sol en pointilles)\n"
                "\n"
                "exemples:\n"
                "  ./simu_headless F7 122.3 0 0 -6.5\n"
                "  ./simu_headless Dr 170.6 -8 2 6\n"
                "  ./simu_headless --capture ../../sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.wav\n"
                "voir Cpp/README.md pour la compilation\n";
        return (argc < 2) ? 1 : 0;
    }
    if (string(argv[1]) == "--capture") {
        if (argc < 3) { cout << "il manque le fichier de captation" << endl; return 1; }
        return run_from_capture(argv[2], argc > 3 ? argv[3] : "F7", argc > 4 ? strtod(argv[4], NULL) : 1.0);
    }
    for (unsigned i = 0; i < sac.size(); i++)
        if (sac[i].getType() == argv[1]) j = i;

    if (argc > 2) sac[j].setClubV0Kmh(strtod(argv[2], NULL));
    if (argc > 3) sac[j].setAlphaClubPathDegre(strtod(argv[3], NULL));
    if (argc > 4) sac[j].setGamaFacePathDegre(strtod(argv[4], NULL));
    if (argc > 5) sac[j].setShaftLeanImpDegre(strtod(argv[5], NULL));
    double spinScale = (argc > 6) ? strtod(argv[6], NULL) : 1.0;

    Ball theBall("Decathlon", 320, 3, sac[j], 18.0, 0.01);
    if (spinScale != 1.0) theBall.rescaleSpin(spinScale);

    cout << "Club=" << sac[j].getType()
         << "  Vclub=" << sac[j].getClubV0kmh() << " km/h"
         << "  launch=" << theBall.getLaunchAngle() * 180.0 / M_PI << " deg"
         << "  V0balle=" << theBall.getV0BallInitms() * 3.6 << " km/h"
         << "  spinY=" << theBall.getSpinYOrgrpm() << " rpm  spinZ=" << theBall.getSpinZOrgrpm() << " rpm"
         << "  spinScale=" << spinScale << endl;

    theBall.runSimu();

    std::vector< std::vector<double> > mf = theBall.getMatriceFlight();
    double portee = mf.back()[0], apex = theBall.getMaxHeight(), zfinal = mf.back()[2];
    cout << "RESULT portee=" << portee << " m  apex=" << apex << " m  z_final=" << zfinal
         << " m  temps=" << theBall.getTempsTotal() << " s  chute=" << theBall.getIndexChute() << endl;
    // graphique SVG de la trajectoire (profil + dessus), ouvrable dans un navigateur
    std::string titre3d = "Trajectoire 3D " + sac[j].getType() + " — portée " + std::to_string((int)portee) + " m, apex " + std::to_string((int)apex) + " m";
    char svgname[256];
    snprintf(svgname, sizeof(svgname), "trajectoire_%s.svg", sac[j].getType().c_str());
    { std::string inf = kv("Club", sac[j].getType() + "  " + fmt1(sac[j].getClubV0kmh(), "km/h"))
  + kv("V0 balle", fmt1(theBall.getV0BallInitms() * 3.6, "km/h"))
  + kv("Launch", fmt1(theBall.getLaunchAngle() * 180.0 / M_PI, "deg"))
  + kv("BackSpin", fmt0(theBall.getSpinYOrgrpm(), "rpm")) + kv("SideSpin", fmt0(theBall.getSpinZOrgrpm(), "rpm"))
  + kv("Portee", fmt1(portee, "m")) + kv("Apex", fmt1(apex, "m")) + kv("z final", fmt1(zfinal, "m"));
  write_traj_3d_html("trajectoire_3d.html", mf, titre3d, inf); }
    cout << "HTML 3D ecrit : trajectoire_3d.html (ouvrir dans Safari, tourner avec la souris)" << endl;
    #ifdef USE_CIMG
    write_traj_png_cimg("trajectoire_" + sac[j].getType() + ".png", mf, titre3d);
    cout << "PNG CImg ecrit : trajectoire_" << sac[j].getType() << ".png" << endl;
#endif
    write_traj_svg(svgname, mf,
                   "Trajectoire " + sac[j].getType() + " — portée " + std::to_string((int)portee) + " m, apex " +
                   std::to_string((int)apex) + " m");
    cout << "SVG écrit : " << svgname << " (ouvrir avec Safari/Aperçu)" << endl;
    return 0;
}
