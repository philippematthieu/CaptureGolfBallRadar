#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Validation synthétique de bout en bout du pipeline captation -> analyse -> simu (points 1 & 2).

  1. génère des captations radar SYNTHÉTIQUES de vérité connue (gen_capture_modele.gen_capture :
     v0, launch, alpha_path, spin total, side spin, V club, courbure k_courbe) ;
  2. les analyse avec le binaire C++ `simu_headless --capture` via GolfBall.analyze_capture_cpp
     (ligne ESTIM) : vBall, spin fusion 3 voies, V club, alpha0/dAlpha, GamaFacePath balayé,
     back/side spin estimés ;
  3. compare aux valeurs vraies du générateur -> tranche la CONVENTION DE SIGNE de dAlpha
     (side connu => signe de sidespin_est attendu) ;
  4. rejoue la simu Python (GolfBall.simulate_golf_ball) avec le side spin estimé (point 2)
     et vérifie que la trajectoire courbe du même côté que la trajectoire synthétique.

Usage : python3 tests/test_synth_capture.py     (numpy + scipy requis, ex. ~/venv-golf/bin/python)
Code de sortie 0 si tous les contrôles passent, 1 sinon.
"""
import math
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))   # Cpp/tests -> racine du dépôt
sys.path.insert(0, os.path.join(ROOT, 'Python'))

import numpy as np  # noqa: E402
import GolfBall as G  # noqa: E402
import gen_capture_modele as gcm  # noqa: E402
import pipe_validation as pv  # noqa: E402

SIMU = G.default_simu_bin()

# name, v0_kmh, launch_deg, alpha_path_deg, spin_tot_rpm, side_rpm, cs_kmh
CONFIGS = [
    ('synth_side_pos.csv', 151.0, 16.5, 3.0, 4000.0, +700.0, 115.0),    # slice "droite" connu
    ('synth_side_neg.csv', 166.0, 16.0, 3.0, 4990.0, -1312.0, 118.0),  # hook connu
]

# beta de modulation du générateur pour les configs principales (2r/lambda ~ 1.5 : peigne modéré,
# ridge stable comme les vraies balles alvéolées ; le beta physique 4*pi*r/lambda ~ 9.4 casserait
# le ridge sur le synthétique). La voie excursion est validée à part avec beta physique + low noise.
BETA_GEN = 2.0 * (G.diam / 2.0) / (299792458.0 / 10.525e9)
LAM = 299792458.0 / 10.525e9


def build_simu():
    """Compile simu_headless si le binaire manque (voir Cpp/README.md)."""
    if os.path.exists(SIMU):
        return
    print('compilation de simu_headless (make -C Cpp) ...')
    subprocess.run(['make', '-C', os.path.join(ROOT, 'Cpp'), 'build/simu_headless'], check=True)


def check(label, ok, detail):
    print(f"   [{'OK' if ok else 'ECHEC'}] {label} : {detail}")
    return ok


def main():
    build_simu()
    tmp = tempfile.mkdtemp(prefix='synth_captures_')
    print(f'captations synthétiques dans {tmp}\n')
    all_ok = True
    for name, v0, launch, apath, spin, side, cs in CONFIGS:
        print(f'=== {name} : v0={v0} km/h, launch={launch}°, alpha_path={apath}°, '
              f'spin={spin:.0f} rpm, side={side:+.0f} rpm, Vclub={cs} km/h (k_courbe=0, '
              f'courbure = pur effet Magnus de la simu de vol)')
        path = os.path.join(tmp, name)
        carry, beta = gcm.gen_capture(path, v0, launch, apath, spin, side, cs, k_courbe=0.0, beta=BETA_GEN)
        t_vol, V_vol, alpha_vol, _ = gcm.simu_vol(v0, launch, apath, spin, side)
        dalpha_true = math.degrees(alpha_vol[-1] - alpha_vol[0])
        print(f'   vérité : carry(vol)={carry:.0f} m, dAlpha_vol={dalpha_true:+.3f}°, beta={beta:+.1f}°')

        est = G.analyze_capture_cpp(path, club='F7')
        print(f"   C++ ESTIM : vBall={est['vball_kmh']:.1f} (corr {est['vball_corr_kmh']:.1f}) km/h, "
              f"spin={est['spin_rpm']:.0f} rpm (patch {est['spin_patch_rpm']:.0f}, phase {est['spin_phase_rpm']:.0f}, "
              f"peigne {est['spin_peigne_rpm']:.0f}), Vclub={est['vclub_kmh']:.1f} km/h, smash={est['smash']:.3f}, "
              f"alpha0={est['alpha0_deg']:+.2f}°, dAlpha={est['dalpha_deg']:+.3f}°, g*={est['gfp_est_deg']:+.1f}°, "
              f"back={est['backspin_est_rpm']:.0f} / side={est['sidespin_est_rpm']:+.0f} rpm")

        # 1. vBall (le générateur met le lobe balle à V*cos(2°+alpha) ~ V)
        all_ok &= check('vBall', abs(est['vball_kmh'] - v0) / v0 < 0.10,
                        f"{est['vball_kmh']:.1f} vs {v0:.0f} km/h")
        # 2. spin total (voie fusion ; le spin reste le maillon faible, ~30 % toléré ici)
        all_ok &= check('spin total', abs(est['spin_rpm'] - spin) / spin < 0.30,
                        f"{est['spin_rpm']:.0f} vs {spin:.0f} rpm")
        # 2b. voie excursion micro-Doppler (point 5) : validée à part ci-dessous avec beta physique
        print(f"   [info] spin excursion beta : exc={est.get('spin_exc_rpm', 0.0):.0f} rpm (beta_gen={BETA_GEN:.2f}, spin vrai {spin:.0f})")
        # 2c. launch angle mesuré (point 3) : fit balistique sur le ridge post-impact
        ld = est.get('launch_deg', 0.0)
        all_ok &= check('launch mesuré', 0 < ld and abs(ld - launch) < 2.0,
                        f"{ld:.2f} vs {launch} deg (conf={est.get('launch_conf', 0.0):.2f})")
        # 3. cohérence du vecteur spin : hypot(back, side) == spin radar
        h = math.hypot(est['backspin_est_rpm'], est['sidespin_est_rpm'])
        coh_ok = (est['spin_rpm'] < 1 and h < 1) or \
                 (est['spin_rpm'] >= 1 and abs(h - est['spin_rpm']) / est['spin_rpm'] < 0.02)
        all_ok &= check('hypot(back,side)=spin', coh_ok,
                        f"hypot={h:.0f} vs spin={est['spin_rpm']:.0f} rpm")
        # 4. CONVENTION DE SIGNE : side estimé doit avoir le signe du side généré
        sign_ok = (est['sidespin_est_rpm'] == 0 and side == 0) or \
                  (est['sidespin_est_rpm'] * side > 0)
        all_ok &= check('signe side (convention dAlpha)', sign_ok,
                        f"side_est={est['sidespin_est_rpm']:+.0f} vs side_vrai={side:+.0f} rpm "
                        f"-> {'CONVENTION OK' if sign_ok else 'CONVENTION INVERSEE'}")
        # 5. la courbure simulée retenue colle à la courbure stéréo mesurée
        all_ok &= check('da_est ~ dAlpha', abs(est['da_est_deg'] - est['dalpha_deg']) < 0.5,
                        f"da_est={est['da_est_deg']:+.3f}° vs dAlpha={est['dalpha_deg']:+.3f}°")

        # cross-check estimateurs Python (pipe_validation) sur le même fichier
        rows, fs = G.load_capture(path)
        res = [pv.estimate_channel(r) for r in rows[:2]]
        v_py, spin_py, conf = pv.aggregate(res)
        print(f'   pipe_validation : vBall={v_py:.1f} km/h, spin={spin_py:.0f} rpm, conf={conf}')
        all_ok &= check('vBall python', abs(v_py - v0) / v0 < 0.15, f"{v_py:.1f} vs {v0:.0f} km/h")

        # 6. point 2 : rejoue la simu Python avec le side estimé -> courbure du même côté
        t, y, b = G.simulate_golf_ball(t_max=10, dt=0.01, v0_ms=est['vball_corr_kmh'] / 3.6,
                                       launch_deg=launch, azim_deg=est['alpha0_deg'],
                                       spin_rpm=est['spin_rpm'], side_rpm=est['sidespin_est_rpm'])
        z_final = y[-1, 2]
        curve_ok = (z_final * dalpha_true > 0) or abs(dalpha_true) < 0.02
        all_ok &= check('simu Python courbe du bon côté', curve_ok,
                        f"z_final={z_final:+.1f} m vs dAlpha_vrai={dalpha_true:+.3f}° "
                        f"(portee={y[-1, 0]:.1f} m, apex={y[:, 1].max():.1f} m, rebonds={b})")
        print()

    # ---- voie excursion micro-Doppler (point 5) : beta physique 4*pi*r/lambda + faible bruit,
    # sinon le bruit de phase (~±350 Hz/ech) domine l'excursion et la voie est illisible ----
    print('\n=== synth_exc_beta_phys.csv : validation de la voie excursion (beta=4πr/λ, noise=0.002)')
    p = os.path.join(tmp, 'synth_exc_beta_phys.csv')
    spin_c = 4500.0
    gcm.gen_capture(p, 151.0, 16.5, 3.0, spin_c, 0.0, 115.0, k_courbe=0.0, beta=None, noise=0.002)
    est_c = G.analyze_capture_cpp(p, club='F7')
    exc = est_c.get('spin_exc_rpm', 0.0)
    print(f"   spin vrai={spin_c:.0f} rpm  exc={exc:.0f} rpm  (fusion={est_c['spin_rpm']:.0f}, "
          f"patch={est_c['spin_patch_rpm']:.0f}, phase={est_c['spin_phase_rpm']:.0f})")
    all_ok &= check('spin excursion beta (physique)', exc > 0 and abs(exc - spin_c) / spin_c < 0.30,
                    f"exc={exc:.0f} vs {spin_c:.0f} rpm")

    print('RÉSULTAT :', 'TOUS LES CONTRÔLES PASSENT' if all_ok else 'DES CONTRÔLES ONT ÉCHOUÉ')
    return 0 if all_ok else 1


if __name__ == '__main__':
    raise SystemExit(main())
