#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Génère des captations radar MODELE (2 canaux stéréo) dans sci/, avec SWING COMPLET :
- avant t0 : vitesse du club V_club(t) = cs*(t/t0)^2.5 (accélération progressive) -> montée Doppler
- à t0 : impact (saut de fréquence club -> balle)
- après t0 : balle en vol, V(t) décroît (traînée), micro-Doppler du patch (modulation de phase),
  et f_Δ(t) module par la courbure α(t) = α0 + 0.5*k*(t-t0)² (effet Magnus du side spin)
Fs = 44100, CSV 2 lignes ';' (comme les vraies captations).

Deux configs générées :
  - défaut : F7, 151 km/h, launch 16.5°, spin 4000 rpm, side 700 (slice droite, β=10,1°)
  - "137"   : paramètres du coup réel 2019-02-24-F7-137m-166kmh-4990trm-1312trm-cs118

Usage : python3 gen_capture_modele.py [137]
"""
import os, sys
import numpy as np
from scipy.integrate import odeint

HERE = os.path.dirname(os.path.abspath(__file__))
SCI = os.path.join(HERE, '..', 'sci')
sys.path.insert(0, SCI)
sys.path.insert(0, HERE)

import GolfBall as G
from pipe_validation import FS, KV

FS_ = FS
LAM = 299792458 / 10.525e9


def simu_vol(V0_kmh, launch_deg, alpha_path_deg, spin_tot_rpm, side_rpm):
    """Simu de vol (sans rebond) -> t, V(t) [m/s], alpha(t) [rad], carry, i_land."""
    m, diam, rho, mu, Cd0, g = G.m, G.diam, G.rho_air, G.mu, G.C_d0, G.g
    Cl1 = 0.54  # fer 7 (SAC)
    V0 = V0_kmh / 3.6
    launch = np.radians(launch_deg)
    ap = np.radians(alpha_path_deg)
    back = np.sqrt(spin_tot_rpm**2 - side_rpm**2)
    y0 = [0, 0.1, 0, V0*np.cos(launch)*np.cos(ap), V0*np.sin(launch), -V0*np.cos(launch)*np.sin(ap),
          0, side_rpm*2*np.pi/60, back*2*np.pi/60, Cl1]
    t = np.arange(0, 6.0, 0.005)
    y = np.zeros((len(t), len(y0)))
    y[0] = y0
    i_land = len(t) - 1
    for i in range(1, len(t)):
        y[i] = odeint(G.flight_ode, y[i-1], [t[i-1], t[i]], args=(m, diam, rho, mu, Cd0, Cl1, g))[-1]
        if y[i, 1] <= 0.01 and y[i-1, 1] > 0.01:
            i_land = i
            break
    V = np.linalg.norm(y[:i_land, 3:6], axis=1)
    alpha = np.arctan2(y[:i_land, 5], y[:i_land, 3])
    return t[:i_land], V, alpha, y[i_land, 0]


def gen_capture(name, v0_kmh, launch_deg, alpha_path_deg, spin_tot_rpm, side_rpm, cs_kmh,
                dur=0.45, t0=0.12, alpha_radar_deg=(-2.0, 2.0), k_courbe=0.0, seed=42, beta=None,
                noise=0.02):
    """Génère le CSV stéréo du swing complet. k_courbe : courbure α(t)=α0+0.5*k*(t-t0)² après impact (rad/s²).
    beta : amplitude de la modulation de phase micro-Doppler ; None = beta physique 4*pi*r/lambda
    (~9.4, sphère miroir -> peigne très large, ridge instable) ; mettre ~1.0 pour un peigne
    modéré (ridge stable, proche des vraies balles alvéolées)."""
    t_vol, V_vol, alpha_vol, carry = simu_vol(v0_kmh, launch_deg, alpha_path_deg, spin_tot_rpm, side_rpm)
    back = np.sqrt(spin_tot_rpm**2 - side_rpm**2)
    beta_tilt = np.degrees(np.arctan2(side_rpm, back))  # angle d'inclinaison de l'axe de spin (affichage)
    w_tot = spin_tot_rpm * 2 * np.pi / 60
    r = G.diam / 2

    N = int(dur * FS_)
    tt = np.arange(N) / FS_
    rng = np.random.default_rng(seed)
    # vitesse "objet" vue par le radar : club avant t0 (accélération), balle après
    V_obj = np.where(tt < t0, cs_kmh/3.6 * (np.clip(tt, 0, t0)/t0)**2.5, 0.0)
    V_obj[tt >= t0] = np.interp(tt[tt >= t0] - t0, t_vol, V_vol, left=V_vol[0], right=V_vol[-1])
    V_kmh = V_obj * 3.6
    # direction horizontale α(t) : constante = alpha_path avant impact, puis courbure après
    alpha_t = np.where(tt < t0, np.radians(alpha_path_deg), 0.0)
    alpha_t[tt >= t0] = np.radians(alpha_path_deg) + np.interp(tt[tt >= t0] - t0, t_vol, alpha_vol) \
        + 0.5 * k_courbe * (tt[tt >= t0] - t0)**2

    rows = []
    # amplitude : club qui monte (t/t0)^2.5 avant impact, balle à l'impact puis atténuation ~1/R
    # post-impact (sinon plateau d'énergie -> t0 = argmax n'importe où dans le vol)
    R_post = np.concatenate([[0.0], np.cumsum(V_obj[:-1]) / FS_])
    R_post[tt < t0] = 0.0
    for a_rad in alpha_radar_deg:
        fD = KV * V_kmh * np.cos(np.radians(a_rad) + alpha_t)   # f_D par canal, modulée par α(t)
        phase = 2 * np.pi * np.cumsum(fD) / FS_
        # micro-Doppler patch (spin total) : phase = beta * sin(w_tot * t), w_tot en rad/s
        # (peigne de raies espacées de f_rot = spin/60 Hz ; excursion Δf = 2·beta·f_rot)
        if beta is None:
            beta = 4 * np.pi * r / LAM   # physique aller-retour radar, sphère parfaite
        phase += beta * np.sin(w_tot * tt)
        amp = np.where(tt < t0, 0.30 * (np.clip(tt, 0, t0) / t0) ** 2.5,
                       0.5 / (1.0 + R_post / 50.0))
        rows.append(amp * np.cos(phase) + noise * rng.standard_normal(N))

    out = os.path.join(SCI, name)
    with open(out, 'w') as f:
        for row in rows:
            f.write(';'.join(f'{v:.9f}' for v in row) + '\n')
    print(f"OK -> {out}")
    print(f"   config: V0={v0_kmh} km/h, launch={launch_deg}°, spin_tot={spin_tot_rpm} rpm "
          f"(back={back:.0f}, side={side_rpm}, β={beta_tilt:.1f}°), cs={cs_kmh}, carry={carry:.0f} m, "
          f"dur={dur*1000:.0f} ms, t0={t0*1000:.0f} ms, k_courbe={k_courbe}")
    return carry, beta


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == 'slice':
        # GROS SLICE droite : spin total 9418 rpm dont side 7102 (back 6185, β≈48,9°),
        # courbure α(t) ≈ -8° sur 300 ms après impact. Fichier nommé comme un vrai coup.
        gen_capture("2025-11-08-F7-131m-162kmh-9418trm-7102trm-cs124.csv",
                    162.0, 16.0, 3.0, 9418.0, 7102.0, 124.0,
                    k_courbe=np.radians(-8.0) / (0.30**2) / 2)
    elif len(sys.argv) > 1 and sys.argv[1] == '137':
        # config du coup réel F7-137m-166kmh-4990trm-1312trm-cs118 (pour comparaison spectro)
        _, _, _, carry137 = simu_vol(166.0, 16.0, 3.0, 4990.0, 1312.0)
        gen_capture(f"2025-11-08-F7-{int(round(carry137))}m-166kmh-4990trm-1312trm-cs118.csv",
                    166.0, 16.0, 3.0, 4990.0, 1312.0, 118.0,
                    k_courbe=np.radians(-6.0) / (0.25**2) / 2)   # Δα≈-6° sur 250 ms (comme la simu)
    else:
        # config idéale par défaut (slice droite)
        gen_capture("2025-11-08-F7-102m-151kmh-4000trm-700trm-cs115.csv", 151.0, 16.5, 3.0,
                    4000.0, 700.0, 115.0, k_courbe=np.radians(-6.0) / (0.25**2) / 2)
