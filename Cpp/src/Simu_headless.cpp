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
        + ",hop:" + std::to_string(e.spec_hop)
        + ",i0:" + std::to_string(e.spec_i0) + ",pow:[";
    for (size_t i = 0; i < e.spec_power.size(); i++) { char b[32]; snprintf(b, 32, "%.3f,", e.spec_power[i]); s += b; }
    s += "],ridge:[";
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
static std::string fmt1nan(double x, const char* u) {
    if (std::isnan(x)) return std::string("NaN ") + u;
    return fmt1(x, u);
}
static std::string fmt0(double x, const char* u) {
    char b[64]; snprintf(b, sizeof(b), "%.0f %s", x, u); return b;
}
static std::string fmt0nan(double x, const char* u) {
    if (std::isnan(x)) return std::string("NaN ") + u;
    return fmt0(x, u);
}

// mode captation fichier : ./simu_headless --capture ../sci/2019-02-24-F7-123m-....csv [club] [spinScale] [--mode standard|fusion] [--launch deg]
// mode captation live carte son : ./simu_headless --live <secondes> [club] [spinScale] [--source fichier.wav pour rejouer un fichier au lieu de la carte son]
static int run_from_capture(Capture c, const std::string& label, const char* clubType, double spinScale, bool openAfter, const std::string& mode, double launchOverrideDeg, double caseDeg) {
    const char* path = label.c_str();
    EstRadar e = estimate(c, caseDeg);
    const double spin_patch_rpm = e.spin_rpm;   // voie patch seule (avant fusion)
    if (mode == "fusion") {
        e.spin_peigne_rpm = estimate_spin_peigne_ac(c, e, nullptr, nullptr, &e.spin_peigne_conf);
        e.spin_phase_rpm = estimate_spin_phase_spec(c, e, nullptr, nullptr, &e.spin_exc_rpm,
                                                     &e.spin_exc_conf, &e.spin_phase_conf);
        double sf = fusion_spin(e.spin_rpm, e.spin_patch_conf,
                                e.spin_phase_rpm, e.spin_phase_conf,
                                e.spin_peigne_rpm, e.spin_peigne_conf,
                                e.spin_exc_rpm, e.spin_exc_conf);
        printf("mode fusion : patch=%.0f (conf %.1f), phase=%.0f (conf %.1f), peigne=%.0f (conf %.1f), "
               "exc(beta)=%.0f (conf %.1f) rpm -> spin retenu=%.0f rpm\n",
               e.spin_rpm, e.spin_patch_conf, e.spin_phase_rpm, e.spin_phase_conf,
               e.spin_peigne_rpm, e.spin_peigne_conf, e.spin_exc_rpm, e.spin_exc_conf, sf);
        e.spin_rpm = sf;
    }
    if (!std::isnan(launchOverrideDeg)) {
        e.launch_deg = launchOverrideDeg;   // --launch XX : override de la mesure (conf -1 = forcé)
        e.launch_conf = -1.0;
    }
    printf("capture %s : vBall=%.1f km/h radiale (%.1f corr. cos20%s), spin=%.0f rpm, alpha0=%+.2f deg, dAlpha=%+.2f deg, launch=%.2f deg (conf %.2f), caseangle=%.1f deg, t0=%.3f s, %zu ech/canal\n",
           path, e.vball_kmh, e.vball_corr_kmh, e.case_deg != 0.0 ? "+case" : "", e.spin_rpm, e.alpha0_deg, e.dalpha_deg, e.launch_deg, e.launch_conf, e.case_deg, e.t0_s, c.r1.size());
    // club par défaut F7
    vector<Club> sac(13);
    sac[5] = Club("F7", 0.200, 20.0, 30, 0.83, 380.00, 420.0, 0.54, 122.3, 0.0, 0.0, -6.5);
    unsigned j = 5;
    for (unsigned i = 0; i < sac.size(); i++) if (sac[i].getType() == clubType) j = i;
    // vitesse club : passe-bande radar autour de vBall/1.35 (lobe du club avant t0), sinon modèle
    double vclub_model = e.vball_kmh / 1.35;
    double vclub_radar = estimate_club_speed(c, e);
    double vclub = std::isnan(vclub_radar) ? vclub_model : vclub_radar;
    // vitesse club estimée = vBall / 1.35 (smash typique)
    sac[j].setClubV0Kmh(vclub);
    sac[j].setAlphaClubPathDegre(e.alpha0_deg);
    // décomposition back/side spin par modèle inverse contraint par la COURBURE de trajectoire :
    // pour chaque GamaFacePath on simule, on mesure la courbure azimutale sur ~150 ms post-impact
    // (même fenêtre que la mesure stéréo dAlpha), et on retient le g qui colle à dAlpha radar.
    // Le shaft lean reste au cas idéal du club (pas d'info radar dessus).
    // Le vecteur spin retenu est ensuite rescalé au spin total radar :
    // hypot(backspin_est, sidespin_est) == spin_radar (backspin <= spin total).
    double gfp_est = 0.0, backspin_est = NAN, sidespin_est = NAN, da_est = 0.0;
    if (e.spin_rpm > 0) {
        double best = 1e18, gbest = 0.0, syb = 0.0, szb = 0.0, stb = 1.0, dabest = 0.0;
        for (double g = -15.0; g <= 15.0001; g += 0.5) {
            sac[j].setGamaFacePathDegre(g);
            Ball probe("Decathlon", 320, 3, sac[j], 18.0, 0.01);
            double sy = probe.getSpinYOrgrpm(), sz = probe.getSpinZOrgrpm();
            double st = std::max(std::hypot(sy, sz), 1.0);
            probe.rescaleSpin(e.spin_rpm / st);          // spin total = spin radar
            probe.runSimu();
            auto mf = probe.getMatriceFlight();
            int k1 = (int)mf.size() - 1, k2 = std::min(15, k1);   // ~150 ms post-impact (dt=0.01)
            double a0 = atan2(mf[k1 > 1 ? 1 : 0][3], mf[k1 > 1 ? 1 : 0][1]) * 180.0 / M_PI;
            double a1 = atan2(mf[k2][3], mf[k2][1]) * 180.0 / M_PI;
            double da = a1 - a0;                          // courbure azimutale simulée (deg)
            double err = std::fabs(da - e.dalpha_deg);    // colle à la courbure stéréo mesurée
            if (err < best) { best = err; gbest = g; syb = sy; szb = sz; stb = st; dabest = da; }
        }
        sac[j].setGamaFacePathDegre(gbest);
        gfp_est = gbest; da_est = dabest;
        double k = e.spin_rpm / stb;                      // rescale au spin total mesuré
        backspin_est = syb * k;
        sidespin_est = szb * k;
    }
    // --- loft dynamique estimé depuis ψ (inversion de launchAngle = loft_dyn + atan(v0p/v0n)) ---
    // v0n = Vclub·cos(loft_dyn)·(1+e)/(1+m/M), v0p = −Vclub·sin(loft_dyn)/(1+m/M+2,5) : on balaie le shaft lean
    // et on retient celui dont le launch calculé colle à ψ (mesuré ou forcé --launch_deg).
    double loftDynEst = NAN, shaftLeanEst = NAN;
    if (e.launch_deg > 0) {
        double target = e.launch_deg * M_PI / 180.0;
        double loftStatic = sac[j].getLoft();
        double vc = vclub / 3.6, m = 0.04545, M = sac[j].getPoids(), ec = sac[j].getEcoeff();
        double best = 1e18;
        for (double sl = -20.0; sl <= 10.0001; sl += 0.25) {
            double ld = (loftStatic + sl) * M_PI / 180.0;
            double v0n = vc * cos(ld) * (1.0 + ec) / (1.0 + m / M);
            double v0p = -vc * sin(ld) / (1.0 + m / M + 2.5);
            double err = std::fabs(ld + atan(v0p / v0n) - target);
            if (err < best) { best = err; shaftLeanEst = sl; loftDynEst = loftStatic + sl; }
        }
        printf("loft dynamique estimé depuis ψ=%.2f° : loft_dyn=%.2f° (shaft lean=%.2f° vs loft statique %.1f°)\n",
               e.launch_deg, loftDynEst, shaftLeanEst, loftStatic);
    }
    // --- cross-check spin : deuxième estimation du loft dynamique via spin_radar ---
    // modèle (Ball.cpp L64-66) : spinY_rpm = V0·cos(ψ)·sin(loft_dyn)·CoeffBackSpin  (le /9,549 et ×9,549 s'annulent)
    double loftDynSpin = NAN, spinFromLoft = NAN, spinLoftRatio = NAN, coeffBsEmp = NAN;
    if (e.spin_rpm > 0 && e.launch_deg > 0 && !std::isnan(loftDynEst)) {
        double V0 = e.vball_corr_kmh / 3.6;   // m/s (corrigée géométrie)
        double coeff = sac[j].getCoeffBackSpin();
        double cpsi = cos(e.launch_deg * M_PI / 180.0);
        double sld = sin(loftDynEst * M_PI / 180.0);
        double arg = e.spin_rpm / (V0 * cpsi * coeff);
        if (arg > -1.0 && arg < 1.0) loftDynSpin = asin(arg) * 180.0 / M_PI;
        spinFromLoft = V0 * cpsi * sld * coeff;
        spinLoftRatio = spinFromLoft / e.spin_rpm;
        if (V0 * cpsi * sld > 1e-9)
            coeffBsEmp = e.spin_rpm / (V0 * cpsi * sld);   // coefficient de backspin effectif calibré sur la mesure
        printf("cross-check spin : loft_dyn(spin)=%.2f° vs loft_dyn(ψ)=%.2f° | spin prédit depuis loft(ψ)=%.0f rpm vs spin radar=%.0f rpm (ratio %.2f) | coeff backspin effectif=%.0f (club: %.0f)\n",
               loftDynSpin, loftDynEst, spinFromLoft, e.spin_rpm, spinLoftRatio, coeffBsEmp, coeff);
    }
    if (!std::isnan(shaftLeanEst))
        sac[j].setShaftLeanImpDegre(shaftLeanEst);         // le club reflète le loft dynamique estimé (avant de construire la balle)
    Ball theBall("Decathlon", 320, 3, sac[j], 18.0, 0.01);
    if (!std::isnan(launchOverrideDeg))
        theBall.setLaunchAngleDegre(launchOverrideDeg);   // --launch XX : force l'angle de decollage (prioritaire)
    else if (e.launch_deg > 0 && (e.launch_conf >= 0.5 || caseDeg != 0.0)) {
        theBall.setLaunchAngleDegre(e.launch_deg);        // launch mesuré sur le ridge (conf suffisante, ou case angle renseigné : géométrie connue)
        printf("launch de la simu = mesuré sur captation : %.2f deg (conf %.2f, caseangle %.1f)\n", e.launch_deg, e.launch_conf, caseDeg);
    }   // sinon : loft du club (repli, comme le 16.5° de GolfBall.py)
    if (e.spin_rpm > 0)
        theBall.rescaleSpin(spinScale * e.spin_rpm / std::max(std::hypot(theBall.getSpinYOrgrpm(), theBall.getSpinZOrgrpm()), 1.0));
    else
        theBall.rescaleSpin(0.0);
    theBall.runSimu();
    auto mf = theBall.getMatriceFlight();
    printf("RESULT depuis captation : portee=%.1f m  apex=%.1f m  z_final=%+.1f m  temps=%.1f s\n",
           mf.back()[0], theBall.getMaxHeight(), mf.back()[2], theBall.getTempsTotal());
    // ligne machine (parsée par Python/GolfBall.py analyze_capture_cpp) : une seule ligne key=value, NaN -> "nan"
    printf("ESTIM vball_kmh=%.2f vball_corr_kmh=%.2f spin_rpm=%.1f spin_patch_rpm=%.1f spin_phase_rpm=%.1f spin_peigne_rpm=%.1f spin_exc_rpm=%.1f spin_patch_conf=%.2f spin_phase_conf=%.2f spin_peigne_conf=%.2f spin_exc_conf=%.2f vclub_radar_kmh=%.2f vclub_kmh=%.2f smash=%.4f alpha0_deg=%.3f dalpha_deg=%.3f t0_s=%.4f launch_deg=%.2f launch_conf=%.2f caseangle_deg=%.2f loft_dyn_est_deg=%.2f shaftlean_est_deg=%.2f loft_dyn_spin_deg=%.2f spin_from_loft_rpm=%.1f spin_loft_ratio=%.3f coeff_backspin_emp=%.1f gfp_est_deg=%.2f backspin_est_rpm=%.1f sidespin_est_rpm=%.1f da_est_deg=%.3f portee_m=%.2f apex_m=%.2f z_final_m=%.2f temps_s=%.2f\n",
           e.vball_kmh, e.vball_corr_kmh, e.spin_rpm, spin_patch_rpm, e.spin_phase_rpm, e.spin_peigne_rpm, e.spin_exc_rpm,
           e.spin_patch_conf, e.spin_phase_conf, e.spin_peigne_conf, e.spin_exc_conf,
           vclub_radar, vclub, (std::isnan(vclub_radar) ? NAN : e.vball_kmh / vclub_radar),
           e.alpha0_deg, e.dalpha_deg, e.t0_s, e.launch_deg, e.launch_conf, e.case_deg, loftDynEst, shaftLeanEst, loftDynSpin, spinFromLoft, spinLoftRatio, coeffBsEmp, gfp_est, backspin_est, sidespin_est, da_est,
           mf.back()[0], theBall.getMaxHeight(), mf.back()[2], theBall.getTempsTotal());
    { std::string inf = kv("vBall radar", fmt1(e.vball_kmh, "km/h") + "  (" + fmt1(e.vball_corr_kmh, "corr. cos20") + ")")
  + kv("V club radar (passe-bande)", fmt1nan(vclub_radar, "km/h") + (std::isnan(vclub_radar) ? "  — lobe club non détecté" : ""))
  + kv("V club modèle (vBall/1.35)", fmt1(vclub_model, "km/h"))
  + kv("Smash mesuré (vBall/Vclub)", std::isnan(vclub_radar) ? std::string("NaN") : fmt1(e.vball_kmh / vclub_radar, ""))
  + kv("Spin total radar", fmt0(e.spin_rpm, "rpm") + (mode == "fusion" ? "  (fusion)" : "  (patch)"))
  + kv("Spin phase (f inst)", fmt0(e.spin_phase_rpm, "rpm"))
  + kv("Spin peigne (raies)", fmt0(e.spin_peigne_rpm, "rpm"))
  + kv("Spin excursion (β=4πr/λ)", fmt0(e.spin_exc_rpm, "rpm"))
  + kv("BackSpin estimé", fmt0nan(backspin_est, "rpm") + (std::isnan(backspin_est) ? "  — spin nul, non décomposable" : "  (vecteur modèle rescalé à spin total radar)"))
  + kv("SideSpin estimé", fmt0nan(sidespin_est, "rpm"))
  + kv("GamaFacePath estimé", fmt1nan(gfp_est, "deg") + (std::isnan(gfp_est) ? "" : "  (ajusté sur courbure dAlpha=" + fmt1(da_est, "deg") + ")"))
  + kv("alpha0", fmt1(e.alpha0_deg, "deg"))
  + kv("Delta alpha", fmt1(e.dalpha_deg, "deg")) + kv("Impact t0", fmt1(e.t0_s, "s"))
  + kv("Launch angle (fit ridge)", e.launch_deg > 0 ? fmt1(e.launch_deg, "deg") + "  (conf " + fmt1(e.launch_conf, "") + ")" : std::string("n/a"))
  + kv("Case angle (valise)", fmt1(e.case_deg, "deg"))
  + kv("Loft dynamique estimé (depuis ψ)", std::isnan(loftDynEst) ? std::string("n/a") : fmt1(loftDynEst, "deg") + "  (shaft lean " + fmt1(shaftLeanEst, "deg") + ")")
  + kv("Cross-check spin (loft depuis spin)", std::isnan(loftDynSpin) ? std::string("n/a") : fmt1(loftDynSpin, "deg") + "  |  spin prédit depuis loft(ψ) = " + fmt1(spinFromLoft, "rpm") + "  (ratio vs radar " + fmt1(spinLoftRatio, "") + ")")
  + kv("Portee", fmt1(mf.back()[0], "m")) + kv("Hauteur max", fmt1(theBall.getMaxHeight(), "m"))
  + kv("z final", fmt1(mf.back()[2], "m"));
  write_traj_3d_html("trajectoire_capture_3d.html", mf, "Trajectoire 3D depuis captation", inf, spec_to_json(e)); }
    printf("HTML 3D ecrit : trajectoire_capture_3d.html\n");
    if (openAfter) {
#ifdef __APPLE__
        system("open trajectoire_capture_3d.html");
#else
        system("xdg-open trajectoire_capture_3d.html");
#endif
    }
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
    sac[5]  = Club("F7",  0.200 , 20.0, 30   , 0.83 ,  380.00 ,     420.0 ,        0.54,  122.3,   0.0,           0.0,        -6.5);
    sac[6]  = Club("F8",  0.200 , 20.0, 34.5 , 0.83 ,  340.00 ,       420.0 ,        0.53,  120.0,   0.0,           0.0,        -8.0);
    sac[7]  = Club("F9",  0.200 , 20.0, 39   , 0.83 ,  400.99 ,       308.0 ,        0.53,  115.9,   0.0,           0.0,        -8.0);
    sac[8]  = Club("PW",  0.200 , 20.0, 44   , 0.78 ,  438.24 ,       280.0 ,        0.52,  112.7,   0.0,           0.0,       -12.0);
    sac[9]  = Club("AW",  0.200 , 20.0, 52   , 0.78 ,  470.09 ,       308.0 ,        0.52,  112.7,   0.0,           0.0,       -17.0);
    sac[10] = Club("SW",  0.200 , 20.0, 54   , 0.71 ,  520.00 ,       308.0 ,        0.15,  104.6,   0.0,           0.0,       -15.0);
    sac[11] = Club("LW",  0.200 , 20.0, 60   , 0.40 ,  500.00 ,       308.0 ,        0.15,  100.0,   0.0,          -3.0,       -12.0);
    sac[12] = Club("Pt",  0.300 , 20.0, 03   , 0.68 ,  550.24 ,       308.0 ,        0.00,   10.00,   0.0,           0.0,         1.0);

    bool openAfter = false;
    std::string mode = "fusion";   // défaut : patch + phase + peigne fusionnés
    double launchOverrideDeg = NAN; // --launch XX en mode --capture/--live : force l'angle de décollage
    double caseDeg = 0.0;           // --caseangle XX : inclinaison de la valise vers le haut (dé-projection v_r = V·cos(ψ−ε))
    std::string liveSource;         // --source f.wav en mode --live : rejoue un fichier au lieu de la carte son
    { std::vector<std::string> args2; for (int i = 1; i < argc; i++) { std::string a = argv[i];
        if (a == "--open" || a == "-o") openAfter = true;
        else if (a == "--mode" && i + 1 < argc) mode = argv[++i];
        else if (a.rfind("--mode=", 0) == 0) mode = a.substr(7);
        else if (a == "--launch" && i + 1 < argc) launchOverrideDeg = strtod(argv[++i], NULL);
        else if (a.rfind("--launch=", 0) == 0) launchOverrideDeg = strtod(a.c_str() + 9, NULL);
        else if (a == "--caseangle_deg" && i + 1 < argc) caseDeg = strtod(argv[++i], NULL);
        else if (a.rfind("--caseangle_deg=", 0) == 0) caseDeg = strtod(a.c_str() + 16, NULL);
        else if (a == "--caseangle" && i + 1 < argc) caseDeg = strtod(argv[++i], NULL);   // alias
        else if (a.rfind("--caseangle=", 0) == 0) caseDeg = strtod(a.c_str() + 12, NULL); // alias
        else if (a == "--launch_deg" && i + 1 < argc) launchOverrideDeg = strtod(argv[++i], NULL);   // alias de --launch
        else if (a.rfind("--launch_deg=", 0) == 0) launchOverrideDeg = strtod(a.c_str() + 13, NULL); // alias
        else if (a == "--source" && i + 1 < argc) liveSource = argv[++i];
        else if (a.rfind("--source=", 0) == 0) liveSource = a.substr(9);
        else args2.push_back(a); } argc = (int)args2.size() + 1; for (size_t i = 0; i < args2.size(); i++) argv[i + 1] = strdup(args2[i].c_str()); }
    if (argc < 2 || string(argv[1]) == "--help" || string(argv[1]) == "-h" || string(argv[1]) == "help") {
        cout << "Simulateur de vol de balle de golf (C++) — vol + 3 rebonds + roulement\n"
                "\n"
                "usage:\n"
                "  simu_headless Type ClubV0kmh AlphaClubPathDeg GamaFacePathDeg ShaftLeanImpDeg [spinScale]\n"
                "  simu_headless --capture fichier.csv|.wav [club] [spinScale] [--launch_deg deg] [--caseangle_deg deg]\n"
                "  simu_headless --live <secondes> [club] [spinScale] [--source f.wav] [--launch deg]\n"
                "                  enregistre la carte son (radar en entrée audio, macOS), sauve live_capture.wav,\n"
                "                  puis analyse comme --capture ; --source rejoue un fichier à la place (test)\n"
                "  simu_headless --help\n"
                "  simu_headless F7 122.3 0 0 -6.5 --open   # ouvre Safari directement\n"
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
                "  --mode m        analyse du spin : 'fusion' (défaut : patch Hilbert + fréquence\n"
                "                  instantanée + peigne de raies micro-Doppler, fusionnés) ou\n"
                "                  'standard' (patch Hilbert seul)\n"
                "  --launch_deg deg (mode --capture, alias --launch) force l'angle de décollage ψ de la simu ;\n"
                "                  sans --launch_deg, le launch est celui mesuré sur le ridge (fit + case angle)\n"
                "  --caseangle_deg deg   inclinaison de la valise vers le haut (ex: 11, alias --caseangle) :\n"
                "                  v_r = V·cos(ψ−ε), le fit mesure ψ−ε et ε est rajouté au launch estimé (et à la\n"
                "                  correction de vBall) ; avec --caseangle_deg renseigné, le launch estimé pilote\n"
                "                  la simu même si conf < 0,5. EXCLUSIF avec --launch_deg (erreur si les deux)\n"
                "\n"
                "sorties (dans le dossier courant):\n"
                "  console: RESULT portee=... apex=... z_final=... temps=...\n"
                "  trajectoire_<club>.svg      image 2D (profil + dessus)\n"
                "  trajectoire_<club>.png/.bmp image 2D CImg (si compile avec -DUSE_CIMG)\n"
                "  trajectoire_3d.html          scene 3D interactive three.js (souris: tourner/zoom;\n"
                "                              slider = vitesse anim; projection au sol en pointilles)\n"
                "\n"
                "exemples (jaune = avec --open auto):\n"
                "\033[33m  ./simu_headless F7 122.3 0 0 -6.5 --open\033[0m\n"
                "\033[33m  ./simu_headless Dr 170.6 -8 2 6\033[0m\n"
                "\033[33m  ./simu_headless --capture ../../sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.wav --open\033[0m\n"
                "voir Cpp/README.md pour la compilation\n";
        return (argc < 2) ? 1 : 0;
    }
    if (!std::isnan(launchOverrideDeg) && caseDeg != 0.0) {
        fprintf(stderr, "ERREUR: --launch_deg et --caseangle_deg sont exclusifs — le launch forcé est déjà ψ,\n"
                        "on n'y ajoute pas le case angle. Choisissez l'un des deux.\n");
        return 2;
    }
    if (string(argv[1]) == "--capture") {
        if (argc < 3) { cout << "il manque le fichier de captation" << endl; return 1; }
        Capture c = load_capture(argv[2]);
        return run_from_capture(c, argv[2], argc > 3 ? argv[3] : "F7", argc > 4 ? strtod(argv[4], NULL) : 1.0, openAfter, mode, launchOverrideDeg, caseDeg);
    }
    if (string(argv[1]) == "--live") {
        double secs = argc > 2 ? strtod(argv[2], NULL) : 2.0;
        const char* club = argc > 3 ? argv[3] : "F7";
        double ss = argc > 4 ? strtod(argv[4], NULL) : 1.0;
        Capture c = liveSource.empty() ? record_live(secs) : load_capture(liveSource);
        if (liveSource.empty()) {
            save_wav("live_capture.wav", c);   // sauvegarde pour relecture Python (parité --capture)
            fprintf(stderr, "captation live sauvegardée : live_capture.wav\n");
        } else {
            fprintf(stderr, "--source %s : replay fichier via le pipeline live (test)\n", liveSource.c_str());
        }
        if (c.r1.empty()) { fprintf(stderr, "ERREUR: aucune donnée audio\n"); return 2; }
        return run_from_capture(c, liveSource.empty() ? "live_capture.wav" : liveSource, club, ss, openAfter, mode, launchOverrideDeg, caseDeg);
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
    { std::string inf = kv("Club", sac[j].getType())
  + kv("V club (modèle)", fmt1(sac[j].getClubV0kmh(), "km/h"))
  + kv("V0 balle", fmt1(theBall.getV0BallInitms() * 3.6, "km/h"))
  + kv("Launch", fmt1(theBall.getLaunchAngle() * 180.0 / M_PI, "deg"))
  + kv("BackSpin", fmt0nan(theBall.getSpinYOrgrpm(), "rpm")) + kv("SideSpin", fmt0nan(theBall.getSpinZOrgrpm(), "rpm"))
  + kv("Portee", fmt1(portee, "m")) + kv("Hauteur max", fmt1(apex, "m")) + kv("z final", fmt1(zfinal, "m"));
  { EstRadar es; auto sig = gen_signal_simu(mf, sac[j].getClubV0kmh()); fill_spec(es, sig, 512, 128);
  write_traj_3d_html("trajectoire_3d.html", mf, titre3d, inf, spec_to_json(es)); }
  }
    cout << "HTML 3D ecrit : trajectoire_3d.html (ouvrir dans Safari, tourner avec la souris)" << endl;
    #ifdef USE_CIMG
    write_traj_png_cimg("trajectoire_" + sac[j].getType() + ".png", mf, titre3d);
    cout << "PNG CImg ecrit : trajectoire_" << sac[j].getType() << ".png" << endl;
#endif
    write_traj_svg(svgname, mf,
                   "Trajectoire " + sac[j].getType() + " — portée " + std::to_string((int)portee) + " m, apex " +
                   std::to_string((int)apex) + " m");
    cout << "SVG écrit : " << svgname << " (ouvrir avec Safari/Aperçu)" << endl;
    if (openAfter) { cout << "ouverture de trajectoire_3d.html ..." << endl;
#ifdef __APPLE__
        system("open trajectoire_3d.html");
#else
        system("xdg-open trajectoire_3d.html");
#endif
    }
    return 0;
}
