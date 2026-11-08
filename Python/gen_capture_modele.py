#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Génère une captation radar MODELE idéale (2 canaux) dans sci/ pour valider β et α.

Config par défaut : fer 7, V0 = 151 km/h, launch = 16.5°, spin total = 4000 rpm,
side spin = 700 rpm (slice droite) -> backspin = 3938 rpm, β = 10.1°.

Signal : porteuse Doppler f_D(t) = KV*V(t)*cos(alpha_radar) (V(t) décroît par traînée,
R1/R2 à ±2°), + modulation de phase du patch exp(-i*(2r/λ)*sin(ωt)) avec ω = spin total,
+ pic club à t0, + bruit ~28 dB. Fs = 44100, CSV 2 lignes ';' (comme les vraies captations).

Usage : python3 gen_capture_modele.py
Sortie : sci/2025-11-08-F7-<carry>m-151kmh-4000trm-700trm-cs115.csv
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

# ---------- config ----------
V0_kmh = 151.0
LAUNCH_DEG = 16.5
SPIN_TOT_RPM = 4000.0
SIDE_RPM = 700.0            # slice droite
CS_KMH = 115.0              # vitesse club (cs)
ALPHA_PATH_DEG = 3.0        # départ à droite
DUR = 0.35                  # s (>= 300 ms min pour la courbure, cf. doc §36)
T0 = 0.10                   # impact à 100 ms
ALPHA_RADAR_DEG = (-2.0, 2.0)   # angles de visée R1 / R2

# ---------- 1. simu de vol (pour le carry et V(t)) ----------
m, diam, rho, mu, Cd0, g = G.m, G.diam, G.rho_air, G.mu, G.C_d0, G.g
Cl1 = 0.54  # fer 7 (SAC)
V0 = V0_kmh / 3.6
launch = np.radians(LAUNCH_DEG)
alpha_path = np.radians(ALPHA_PATH_DEG)
back_rpm = np.sqrt(SPIN_TOT_RPM**2 - SIDE_RPM**2)
beta_deg = np.degrees(np.arctan2(SIDE_RPM, back_rpm))
w_tot = SPIN_TOT_RPM * 2 * np.pi / 60
w_side = SIDE_RPM * 2 * np.pi / 60
w_back = back_rpm * 2 * np.pi / 60
vx0 = V0 * np.cos(launch) * np.cos(alpha_path)
vy0 = V0 * np.sin(launch)
vz0 = -V0 * np.cos(launch) * np.sin(alpha_path)
y0 = [0, 0.1, 0, vx0, vy0, vz0, 0, w_side, w_back, Cl1]
t = np.arange(0, 6.0, 0.005)
y = np.zeros((len(t), len(y0)))
y[0] = y0
i_land = len(t) - 1
for i in range(1, len(t)):
    y[i] = odeint(G.flight_ode, y[i-1], [t[i-1], t[i]], args=(m, diam, rho, mu, Cd0, Cl1, g))[-1]
    if y[i, 1] <= 0.01 and y[i-1, 1] > 0.01:
        i_land = i
        break
carry = y[i_land, 0]
Vnorm = np.linalg.norm(y[:i_land, 3:6], axis=1)
alpha_traj = np.degrees(np.arctan2(y[:i_land, 5], y[:i_land, 3]))
print(f"SIMU: carry={carry:.1f} m, beta={beta_deg:.2f} deg, alpha0={alpha_traj[0]:+.2f} deg, "
      f"dAlpha={alpha_traj[-1]-alpha_traj[0]:+.2f} deg, t_vol={t[i_land]:.2f}s")

# ---------- 2. signal radar 2 canaux ----------
N = int(DUR * FS)
tt = np.arange(N) / FS
rng = np.random.default_rng(42)
r = diam / 2
lam = 299792458 / 10.525e9

V_kmh = np.interp(tt, t[:i_land], Vnorm * 3.6, left=V0_kmh, right=Vnorm[i_land-1] * 3.6)
rows = []
for c, a_rad in enumerate(ALPHA_RADAR_DEG):
    fD = KV * V_kmh * np.cos(np.radians(a_rad))
    fD = np.where(tt < T0, KV * CS_KMH, fD)          # avant impact : le club
    phase = 2 * np.pi * np.cumsum(fD) / FS
    phase += (2 * r / lam) * np.sin(w_tot * 2 * np.pi * tt)   # micro-Doppler du patch
    rows.append(0.5 * np.cos(phase) + 0.02 * rng.standard_normal(N))

name = f"2025-11-08-F7-{int(round(carry))}m-151kmh-4000trm-700trm-cs115.csv"
out = os.path.join(SCI, name)
with open(out, 'w') as f:
    for row in rows:
        f.write(';'.join(f'{v:.9f}' for v in row) + '\n')
print('OK ->', out)
print(f"config: F7, V0=151 km/h, launch=16.5°, spin_tot=4000 rpm (back={back_rpm:.0f}, side=700, "
      f"beta={beta_deg:.1f}°), carry={carry:.0f} m, duree={DUR*1000:.0f} ms, t0={T0*1000:.0f} ms")
