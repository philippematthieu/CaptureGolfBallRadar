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

using namespace std;

// mode captation : ./simu_headless --capture ../sci/2019-02-24-F7-123m-....csv [club] [spinScale]
static int run_from_capture(const char* path, const char* clubType, double spinScale) {
    Capture c = load_capture(path);
    EstRadar e = estimate(c);
    printf("capture %s : vBall=%.1f km/h (radiale), spin=%.0f rpm, alpha0=%+.2f deg, t0=%.3f s, %zu ech/canal\n",
           path, e.vball_kmh, e.spin_rpm, e.alpha0_deg, e.t0_s, c.r1.size());
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

    if (argc < 2) {
        cout << "usage: Simu_headless Type ClubV0kmh AlphaClubPathDeg GamaFacePathDeg ShaftLeanImpDeg [spinScale]" << endl;
        cout << "       Simu_headless --capture fichier.csv|.wav [club] [spinScale]" << endl;
        cout << "clubs: Dr B5 H3 F5 F6 F7 F8 F9 PW AW SW LW Pt" << endl;
        return 1;
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
    char svgname[256];
    snprintf(svgname, sizeof(svgname), "trajectoire_%s.svg", sac[j].getType().c_str());
    write_traj_svg(svgname, mf,
                   "Trajectoire " + sac[j].getType() + " — portée " + std::to_string((int)portee) + " m, apex " +
                   std::to_string((int)apex) + " m");
    cout << "SVG écrit : " << svgname << " (ouvrir avec Safari/Aperçu)" << endl;
    return 0;
}
