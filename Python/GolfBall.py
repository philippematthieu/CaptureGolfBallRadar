import os
import sys
import numpy as np
from scipy.integrate import odeint

try:  # matplotlib optionnel : le module reste importable en headless (pipe de validation, tests)
    import matplotlib.pyplot as plt
    from matplotlib.animation import FuncAnimation
    from matplotlib.patches import Circle, FancyArrowPatch
    from mpl_toolkits.mplot3d import Axes3D
    from mpl_toolkits.mplot3d import proj3d
    HAS_MPL = True
except ImportError:
    HAS_MPL = False

# =============================================
# PARAMÈTRES PHYSIQUES (basés sur votre modèle Scilab)
# =============================================

# Propriétés de la balle
m = 0.04545  # Masse (kg)
diam = 0.043  # Diamètre (m)
A = np.pi * (diam / 2) ** 2  # Section transversale (m²)

# Propriétés de l'air
rho_air = 1.292 * 273.15 / (20 + 273.15)  # Densité (kg/m³) à 20°C — même formule que Golfball.sci
mu = 1.5e-5  # Viscosité CINEMATIQUE de l'air (m²/s) à 20°C (pas dynamique)
g = 9.81  # Accélération gravitationnelle (m/s²)

# Coefficients aérodynamiques
C_d0 = 0.36  # Coefficient de traînée de base (relation de White, comme Golfball.sci)
C_l = 0.64   # Coefficient de portance Magnus = Cl1('D') du driver dans Golfball.sci (GetSac)

# Paramètres du club (Driver)
club_type = 'D'
loft_deg = 11  # Degrés
club_mass = 0.300  # kg
e_coeff = 0.83  # Coefficient de restitution
coeff_spin = 180  # Coefficient de spin
coeff_spin_lift = 500  # Coefficient de spin latéral
Cl1 = 0.64  # Coefficient de portance
Cm = 4.3e-2  # Coefficient de traînée

# Conditions initiales
v_club_kmh = 151  # Vitesse du club (km/h)
alpha_club_path_deg = -4  # Degrés (chemin du club)
gama_face_path_deg = 2  # Degrés (angle de la face)
shaf_lean_imp_deg = 0  # Degrés (inclinaison du shaft)
miss = 0  # cm (décalage du point d'impact)

# Conversion en unités SI
v_club = v_club_kmh * 1000 / 3600  # m/s
alpha_club_path = np.radians(alpha_club_path_deg)
gama_face_path = np.radians(gama_face_path_deg)
shaf_lean_imp = np.radians(shaf_lean_imp_deg)
theta_loft = np.radians(loft_deg + shaf_lean_imp_deg)

# =============================================
# CALCUL DES CONDITIONS INITIALES (basé sur votre modèle Scilab)
# =============================================

# Vitesse de la balle après impact (modèle de Penner)
Ke = (1 + e_coeff) / (1 + m / club_mass)
Kp = 1 / (1 + m / club_mass + 5 / 2)
Km = 1 - 0.3556 * miss  # miss en cm (cohérent avec le facteur 1-0.3556*miss de Golfball.sci)

V0_ball_bfnms = v_club * np.cos(theta_loft) * Ke
V0_ball_bfpms = -v_club * np.sin(theta_loft) * Kp
V0_ball_ms = np.sqrt(V0_ball_bfnms ** 2 + V0_ball_bfpms ** 2) * Km

# Angle de lancement
launch_angle = theta_loft + np.arctan2(V0_ball_bfpms, V0_ball_bfnms)
launch_angle_deg = np.degrees(launch_angle)

# Vitesse initiale (3D)
vx0 = V0_ball_ms * np.cos(launch_angle) * np.cos(alpha_club_path)
vy0 = V0_ball_ms * np.sin(launch_angle)
vz0 = -V0_ball_ms * np.cos(launch_angle) * np.sin(alpha_club_path)

# Spin initial (rad/s)
spin_x = 0
spin_y = V0_ball_ms * np.sin(launch_angle) * np.sin(gama_face_path) * coeff_spin_lift / 9.54929659643  # sur Vy, converti rpm->rad/s comme Golfball.sci
spin_z = V0_ball_ms * np.cos(launch_angle) * np.sin(theta_loft) * coeff_spin / 9.54929659643  # sur Vx, converti rpm->rad/s comme Golfball.sci

# Position initiale
x0, y0, z0 = 0, 0.1, 0  # y0 légèrement au-dessus du sol

# =============================================
# ÉQUATIONS DIFFÉRENTIELLES POUR LE VOL
# =============================================

def flight_ode(y, t, m, diam, rho_air, mu, C_d0, C_l, g):
    # Déballage des variables (cl1 en 10e position, mis a 0 apres un rebond comme v(10) dans Golfball.sci)
    x, y, z, vx, vy, vz, wx, wy, wz, cl1 = y

    # Norme de la vitesse
    v_norm = np.sqrt(vx ** 2 + vy ** 2 + vz ** 2)

    # Nombre de Reynolds
    Re = rho_air * v_norm * diam / mu

    # Coefficient de traînée (relation de White, identique à Golfball.sci)
    C_d = C_d0 + 24 / Re + 6 / (1 + np.sqrt(Re))

    # Force de traînée
    F_D = -0.5 * rho_air * C_d * A * v_norm * np.array([vx, vy, vz])

    # Force de Magnus (portance)
    # Spin en rad/s, v_norm en m/s
    spin_norm = np.sqrt(wx ** 2 + wy ** 2 + wz ** 2)
    if v_norm > 0:
        # Coefficient de Magnus (simplifié)
        C_m = cl1 * (diam / 2) ** 2 * (spin_norm / v_norm) ** 0.4  # facteur (diam/2)^2 comme Golfball.sci (cl1 = Cl1, 0 apres rebond)
        F_M = 0.5 * rho_air * C_m * A * v_norm * np.array([
            wy * vz - wz * vy,
            wz * vx - wx * vz,
            wx * vy - wy * vx
        ])
    else:
        F_M = np.zeros(3)

    # Force gravitationnelle
    F_G = np.array([0, -m * g, 0])

    # Décélération du spin (frottement aérodynamique)
    # k est un coefficient empirique
    k = 1e-5
    dWdt = -k * np.array([wx, wy, wz]) * v_norm

    # Équations différentielles
    dxdt = vx
    dydt = vy
    dzdt = vz
    dvxdt = (F_D[0] + F_M[0]) / m
    dvydt = (F_D[1] + F_M[1] + F_G[1]) / m
    dvzdt = (F_D[2] + F_M[2]) / m

    return [dxdt, dydt, dzdt, dvxdt, dvydt, dvzdt, dWdt[0], dWdt[1], dWdt[2], 0]  # cl1 constant entre les rebonds

# =============================================
# MODÈLE DE REBOND (basé sur Penner)
# =============================================

def handle_bounce(y, t):
    x, y_pos, z, vx, vy, vz, wx, wy, wz, cl1 = y

    # Vitesse d'impact
    vi = np.sqrt(vx ** 2 + vz ** 2)
    viy = vy

    # Angle d'impact (en radians)
    impact_angle = np.arctan2(vi, abs(viy))

    # Angle de rebond (modèle de Penner)
    theta_rebond = 0.2687807 * (np.sqrt(vi ** 2 + viy ** 2) / 18.6) * (impact_angle / 0.7749262)

    # Vitesse après rebond
    vix_prime = vi * np.cos(theta_rebond) - abs(viy) * np.sin(theta_rebond)
    viy_prime = vi * np.sin(theta_rebond) + abs(viy) * np.cos(theta_rebond)

    # Coefficient de restitution (eball)
    if abs(viy_prime) < 20:
        eball = 0.510 - 0.0375 * abs(viy_prime) + 0.000903 * abs(viy_prime) ** 2
    else:
        eball = 0.120

    # Frottement
    mu_friction = 0.40
    viyp_abs = max(abs(viy_prime), 1e-6)  # garde anti-division par zero (comme Golfball.sci)
    mu_friction_critic = 2 * (vix_prime + (diam / 2) * wz) / (7 * (1 + eball) * viyp_abs)

    if mu_friction < mu_friction_critic:
        # Glissement
        vrx_prime = vix_prime - mu_friction * abs(viy_prime) * (1 + eball)
        vry_prime = eball * abs(viy_prime)
        Wr = wz - (5 * mu_friction * diam) * abs(viy_prime) * (1 + eball)
    else:
        # Roulement
        vrx_prime = (5 / 7) * vix_prime - (2 / 7) * (diam / 2) * wz
        vry_prime = eball * abs(viy_prime)
        Wr = -vrx_prime / (diam / 2)

    # Vitesse finale après rebond
    vr = vrx_prime * np.cos(theta_rebond) - vry_prime * np.sin(theta_rebond)
    vry = vrx_prime * np.sin(theta_rebond) + vry_prime * np.cos(theta_rebond)

    # Mise à jour des vitesses
    vx_new = vr / vi * vx if vi > 0 else 0
    vy_new = vry
    vz_new = vr / vi * vz if vi > 0 else 0

    # Mise à jour du spin
    wx_new = 0  # On néglige le spin latéral après rebond
    wy_new = 0
    wz_new = Wr

    # Position après rebond (légèrement au-dessus du sol)
    y_new = 0.01

    return [x, y_new, z, vx_new, vy_new, vz_new, wx_new, wy_new, wz_new, 0]  # cl1 = 0 : plus d'effet Magnus apres rebond (Pos(10,$)=0 dans Golfball.sci)

# =============================================
# MODÈLE DE ROULEMENT
# =============================================

def handle_rolling(y, t):
    x, y_pos, z, vx, vy, vz, wx, wy, wz, cl1 = y

    # Coefficient de frottement (green)
    rhog = 0.131

    # Décélération due au frottement
    v_norm = np.sqrt(vx ** 2 + vz ** 2)
    if v_norm > 0:
        ax = - (5 / 7) * rhog * g * vx / v_norm
        az = - (5 / 7) * rhog * g * vz / v_norm
    else:
        ax, az = 0, 0

    ay = 0  # Pas de mouvement vertical en roulement

    # Mise à jour des vitesses
    vx_new = vx + ax * 0.01  # Petit pas de temps pour le roulement
    vy_new = 0
    vz_new = vz + az * 0.01

    # Mise à jour de la position
    x_new = x + vx * 0.01
    y_new = 0.01  # Au sol
    z_new = z + vz * 0.01

    # Spin (on néglige la décélération du spin en roulement)
    wx_new, wy_new, wz_new = 0, 0, 0

    return [x_new, y_new, z_new, vx_new, vy_new, vz_new, wx_new, wy_new, wz_new, 0]

# =============================================
# SIMULATION COMPLÈTE (VOL + REBONDS + ROULEMENT)
# =============================================

def load_capture(path):
    """Charge une captation radar : CSV (2 lignes ';' = R1/R2, export Scilab) ou WAV (stéréo).
    Retourne (rows[2], fs)."""
    if path.lower().endswith('.wav'):
        import wave
        with wave.open(path, 'rb') as w:
            n, ch, sw, fs = w.getnframes(), w.getnchannels(), w.getsampwidth(), w.getframerate()
            raw = w.readframes(n)
        if sw == 2:
            data = np.frombuffer(raw, dtype='<i2').astype(np.float64) / 32768.0
        elif sw == 4:
            data = np.frombuffer(raw, dtype='<i4').astype(np.float64) / 2.0 ** 31
        else:
            data = np.frombuffer(raw, dtype=np.uint8).astype(np.float64) / 128.0 - 1.0
        data = data.reshape(-1, ch).T
        rows = [data[0], data[1] if ch > 1 else data[0]]
        return rows, fs
    rows = []
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            line = line.strip()
            if line:
                rows.append(np.array([float(v) for v in line.replace(',', '.').split(';') if v.strip()]))
    return rows, 44100

def estimate_from_capture(path):
    """Estime (vBall km/h radiale, SpinTotal rpm, alpha0 deg) d'une captation via sci/pipe_validation.py."""
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sci'))
    import pipe_validation as pv
    rows, fs = load_capture(path)
    res = [pv.estimate_channel(r) for r in rows[:2]]
    v, spin, conf = pv.aggregate(res)
    al = pv.compute_alpha(res[0], res[1] if len(res) > 1 else res[0])
    a0 = float(al['alpha0']) if al else 0.0
    print(f"capture {os.path.basename(path)} : vBall={v:.1f} km/h (radiale), SpinTotal={spin:.0f} rpm, "
          f"alpha0={a0:+.2f} deg, conf={conf}")
    return v, spin, a0


def default_simu_bin():
    """Chemin du binaire C++ simu_headless (analyse radar avancée : V club, spin 3 voies, back/side)."""
    return os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'Cpp', 'src', 'simu_headless'))


def analyze_capture_cpp(path, club='F7', spin_scale=1.0, simu_bin=None, timeout=300, case_angle=None):
    """Analyse une captation avec le binaire C++ `simu_headless --capture` et parse sa ligne ESTIM.

    Parité C++ : le C++ est l'outil principal (plus rapide, et `--live <s>` enregistre directement
    la carte son radar, sauve `live_capture.wav`, puis analyse — ce fichier se relit ici avec --capture).

    Retourne un dict de floats : vball_kmh, vball_corr_kmh, spin_rpm, spin_patch_rpm, spin_phase_rpm,
    spin_peigne_rpm, spin_exc_rpm, spin_patch_conf, spin_phase_conf, spin_peigne_conf, spin_exc_conf,
    vclub_radar_kmh, vclub_kmh, smash, alpha0_deg, dalpha_deg, t0_s, launch_deg, launch_conf, gfp_est_deg,
    backspin_est_rpm, sidespin_est_rpm, da_est_deg, portee_m, apex_m, z_final_m, temps_s (+ '_stdout').
    Lève FileNotFoundError si le binaire manque, ValueError si la ligne ESTIM est absente."""
    import re
    import subprocess
    bin_ = simu_bin or default_simu_bin()
    if not os.path.exists(bin_):
        raise FileNotFoundError(f"binaire C++ introuvable : {bin_} (compiler : voir Cpp/README.md)")
    cmd = [bin_, '--capture', os.path.abspath(path), club, repr(float(spin_scale))]
    if case_angle is not None:
        cmd += ['--caseangle_deg', repr(float(case_angle))]
    r = subprocess.run(cmd,
                       capture_output=True, text=True, timeout=timeout,
                       cwd=os.path.dirname(bin_))  # le binaire écrit ses HTML/SVG dans son cwd
    out = r.stdout + r.stderr
    m = re.search(r'^ESTIM (.*)$', out, re.M)
    if not m:
        raise ValueError("pas de ligne ESTIM dans la sortie de simu_headless :\n" + out[-2000:])
    d = {'_stdout': out}
    for tok in m.group(1).split():
        k, _, v = tok.partition('=')
        try:
            d[k] = float(v)
        except ValueError:
            d[k] = v
    return d


def estimate_from_capture_full(path, club='F7', spin_scale=1.0, simu_bin=None, case_angle=None):
    """Estimation complète d'une captation : essaie le binaire C++ (V club, spin fusion 3 voies,
    décomposition back/side par courbure dAlpha), sinon repli sur pipe_validation (vBall, spin patch, alpha0).
    case_angle = inclinaison de la valise radar vers le haut en degrés (passé au C++ via --caseangle).

    Retourne (v0_ms, spin_rpm, azim_deg, side_rpm, info) — side_rpm = None si non décomposé (spin nul)."""
    try:
        est = analyze_capture_cpp(path, club=club, spin_scale=spin_scale, simu_bin=simu_bin, case_angle=case_angle)
    except (FileNotFoundError, ValueError) as ex:
        print(f"analyse C++ indisponible ({ex}) -> repli sur pipe_validation")
        v_kmh, spin_rpm, a0 = estimate_from_capture(path)
        v0_ms = v_kmh / 3.6 / np.cos(np.radians(20.0))  # correction angle radar ~20°
        return v0_ms, spin_rpm, a0, None, {}
    v_corr = est.get('vball_corr_kmh', 0.0)
    if not np.isfinite(v_corr) or v_corr <= 0:
        v_corr = est['vball_kmh']
    v0_ms = v_corr / 3.6  # vball_corr_kmh est déjà corrigée cos(20°) côté C++
    spin_rpm = est.get('spin_rpm', 0.0) or 0.0
    azim = est.get('alpha0_deg', 0.0)
    side = est.get('sidespin_est_rpm')
    if side is None or not np.isfinite(side):
        side = None
    nan = float('nan')
    print(f"capture {os.path.basename(path)} (C++) : vBall={est['vball_kmh']:.1f} km/h radiale "
          f"({v_corr:.1f} corr. cos20), spin={spin_rpm:.0f} rpm "
          f"(patch={est.get('spin_patch_rpm', nan):.0f}, phase={est.get('spin_phase_rpm', nan):.0f}, "
          f"peigne={est.get('spin_peigne_rpm', nan):.0f}, exc={est.get('spin_exc_rpm', nan):.0f}), "
          f"Vclub={est.get('vclub_kmh', nan):.1f} km/h, "
          f"smash={est.get('smash', nan):.3f}, alpha0={azim:+.2f} deg, dAlpha={est.get('dalpha_deg', 0.0):+.2f} deg")
    if side is not None:
        back = est.get('backspin_est_rpm', nan)
        print(f"   -> back/side estimes : {back:.0f} / {side:+.0f} rpm  "
              f"(GamaFacePath={est.get('gfp_est_deg', 0.0):+.1f} deg, hypot={np.hypot(back, side):.0f} = spin radar)")
    return v0_ms, spin_rpm, azim, side, est

def simulate_golf_ball(t_max=10, dt=0.01, max_bounces=3, v0_ms=None, launch_deg=None, azim_deg=None,
                       spin_rpm=None, side_rpm=None):
    global vx0, vy0, vz0, spin_x, spin_y, spin_z
    if v0_ms is not None:
        # conditions initiales imposées (ex. mesurées sur une captation radar)
        la = np.radians(launch_deg if launch_deg is not None else launch_angle_deg)
        az = np.radians(azim_deg if azim_deg is not None else 0.0)
        spin_tot = (spin_rpm * 2 * np.pi / 60) if spin_rpm else 0.0
        side = (side_rpm * 2 * np.pi / 60) if side_rpm else 0.0
        back = np.sqrt(max(spin_tot ** 2 - side ** 2, 0.0))
        vx0 = v0_ms * np.cos(la) * np.cos(az)
        vy0 = v0_ms * np.sin(la)
        vz0 = -v0_ms * np.cos(la) * np.sin(az)
        spin_x, spin_y, spin_z = 0.0, side, back
    # Conditions initiales (y_init pour ne pas masquer le global y0 = position initiale)
    y_init = [x0, y0, z0, vx0, vy0, vz0, spin_x, spin_y, spin_z, C_l]  # C_l = Cl1 en derniere position (mis a 0 apres rebond)
    t = np.arange(0, t_max, dt)
    y = np.zeros((len(t), len(y_init)))
    y[0] = y_init

    bounce_count = 0
    is_rolling = False

    for i in range(1, len(t)):
        # Si la balle est en roulement
        if is_rolling:
            y[i] = handle_rolling(y[i-1], t[i-1])
            # arrêt si vitesse horizontale < 3 cm/s (seuil events2 de Golfball.sci)
            if np.sqrt(y[i, 3] ** 2 + y[i, 5] ** 2) < 0.03:
                y[i:, :] = y[i]
                break
            continue

        # Si la balle touche le sol (y <= 0.01)
        if y[i-1, 1] <= 0.01 and bounce_count < max_bounces:
            y[i] = handle_bounce(y[i-1], t[i-1])
            bounce_count += 1
            continue

        # Sinon, intégrer les équations du vol
        y_temp = odeint(
            flight_ode,
            y[i-1],
            [t[i-1], t[i]],
            args=(m, diam, rho_air, mu, C_d0, C_l, g)
        )
        y[i] = y_temp[-1]

        # Si la balle touche le sol après l'intégration
        if y[i, 1] <= 0.01 and bounce_count < max_bounces:
            y[i] = handle_bounce(y[i-1], t[i-1])
            bounce_count += 1

        # Si la balle est au sol et a déjà rebondi max_bounces fois
        if y[i, 1] <= 0.01 and bounce_count >= max_bounces:
            is_rolling = True
            y[i] = handle_rolling(y[i-1], t[i-1])

    return t, y, bounce_count


# =============================================
# BASE DE CLUBS = GetSac() de Golfball.sci
# =============================================

SAC = {
    'Type':           ['D','W5','H3','5','6','7','8','9','PW','AW','SW','LW','PU'],
    'Loft':           [11,18,18,23,26,30,34.5,39,44,52,54,60,3],
    'Poids':          [0.300,0.250,0.250,0.200,0.200,0.200,0.200,0.200,0.200,0.200,0.200,0.200,0.300],
    'ecoeff':         [0.83,0.78,0.76,0.87,0.84,0.68,0.70,0.60,0.78,0.68,0.68,0.40,0.68],
    'coeffSpin':      [180,280.6435,250.0,300.0,240,400,300.7425,400.99,501.2375,441.089,452.51722,500,501.2375],
    'coeffSpinLift':  [500,500,500,420,420,580,420,308,308,308,308,308,308],
    'Cl1':            [0.64,0.65,0.54,0.54,0.83,0.42,0.53,0.53,0.52,0.52,0.51,0.51,0.0],
    'Cm':             [4.3e-2,2.8e-2,2.6e-2,1.8e-2,1.6e-2,1.1e-2,1.1e-2,0.9e-2,0.62e-2,0.5e-2,0.45e-2,0.4e-2,0.0],
}

def _club_idx(TypeClub):
    return SAC['Type'].index(TypeClub)


def LaunchAngle(v0Ballkmh, v0Clubkmh, TypeClub, SpinLR):
    """Inverse (portage de LaunchAngle dans Golfball.sci) : retrouve thetaLoft, ShafLeanImp,
    launchAngle, SpinZ, gamaFacePath à partir des vitesses mesurées et du spin latéral."""
    i = _club_idx(TypeClub)
    mm = 0.04545
    v0Ballms = v0Ballkmh / 3.6
    V0Clubms = v0Clubkmh / 3.6
    SmashFactor = v0Ballms / V0Clubms
    miss = 0
    Km = 1 - 0.3556 * miss  # miss en cm
    Ke = (1 + SAC['ecoeff'][i]) / (1 + mm / SAC['Poids'][i])
    Kp = 1 / (1 + mm / SAC['Poids'][i] + 5 / 2)
    # garde du domaine de asin : min(1, abs(...)) comme dans Golfball.sci
    thetaLoft = np.arcsin(min(1.0, np.sqrt(abs((SmashFactor ** 2 - Ke ** 2) / (Kp ** 2 - Ke ** 2)))))
    V0Ballbfnms = V0Clubms * np.cos(thetaLoft) * Ke
    V0Ballbfpms = -V0Clubms * np.sin(thetaLoft) * Kp
    launchAngle = thetaLoft + np.arctan2(V0Ballbfpms, V0Ballbfnms)
    SpinZ = v0Ballms * np.cos(launchAngle) * np.sin(thetaLoft) * SAC['coeffSpin'][i]
    thetaLoft_deg = np.degrees(thetaLoft)
    launchAngle_deg = np.degrees(launchAngle)
    ShafLeanImp = thetaLoft_deg - SAC['Loft'][i]
    denom = v0Ballms * np.sin(launchAngle) * SAC['coeffSpinLift'][i]
    gamaFacePath = np.degrees(np.arcsin(min(1.0, abs(SpinLR / denom)))) if abs(denom) > 1e-12 else 0.0
    return thetaLoft_deg, ShafLeanImp, launchAngle_deg, SpinZ, gamaFacePath


def BallGolf(V0Ballkmh, Psi, miss, e, m_, M, coeffSpin=180.0):
    """Inverse (portage de BallGolf dans Golfball.sci) : retrouve V0Club, loft dynamique et SpinZ
    à partir de V0Ball et de l'angle de décollage Psi (degrés). miss en cm.
    (Dans le .sci, SpinZ était retourné non calculé -> bug; ici il est calculé.)"""
    D = (1 + m_ / M) / ((1 + m_ / M + 5 / 2) * (1 + e))
    Psi_rd = np.radians(Psi)
    V0Ballms = V0Ballkmh / 3.6
    a = np.tan(Psi_rd) * D
    if abs(a) < 1e-12:
        a = 1e-12  # garde anti-division par zero si Psi = 0 (comme Golfball.sci)
    b = D - 1
    c = np.tan(Psi_rd)
    A2 = (-b - np.sqrt(b * b - 4 * a * c)) / (2 * a)  # racine A2 comme dans Golfball.sci
    dynamicLoft_rd = np.arctan(A2)
    V0Clubms = np.sqrt(abs(V0Ballms ** 2 / ((1 - 0.3556 * miss) * (
        (np.cos(dynamicLoft_rd) * (1 + e) / (1 + m_ / M)) ** 2
        + (-np.sin(dynamicLoft_rd) / (1 + m_ / M + 5 / 2)) ** 2))))
    SpinZ = V0Ballms * np.cos(dynamicLoft_rd) * np.sin(dynamicLoft_rd) * coeffSpin / 9.54929659643
    return V0Clubms * 3.6, np.degrees(dynamicLoft_rd), SpinZ


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser(description="Simulation 3D du vol d'une balle de golf. "
                                             "Avec une captation (.csv 2 lignes ';' ou .wav stéréo), "
                                             "les conditions initiales sont mesurées dessus (pipe_validation).")
    ap.add_argument('capture', nargs='?', default=None,
                    help="fichier de captation radar : .csv (2 lignes ';' = canaux R1/R2) ou .wav (stéréo 44,1 kHz)")
    ap.add_argument('--club', default='F7', help="club pour l'analyse C++ de la captation (défaut F7)")
    g = ap.add_mutually_exclusive_group()
    g.add_argument('--launch', '--launch_deg', dest='launch', type=float, default=None,
                   help='launch angle ψ forcé en degrés, passé tel quel à la simu (défaut : mesuré sur la captation, sinon 16.5). '
                        'EXCLUSIF avec --caseangle_deg (ψ forcé = on n\'y ajoute pas ε)')
    g.add_argument('--caseangle_deg', '--caseangle', dest='caseangle_deg', type=float, default=None,
                   help='inclinaison de la valise radar vers le haut en degrés (ex: 11), passée au C++ (--caseangle_deg)')
    ap.add_argument('--elev', type=float, default=10.0, help='elevation caméra 3D en degrés (défaut 10 = vue golfeur)')
    ap.add_argument('--azim', type=float, default=180.0, help='azimut caméra 3D en degrés (défaut 180 = de derrière, comme le golfeur ; -60 = vue plongeante)')
    args = ap.parse_args()

    v0_ms = None
    launch_deg = args.launch   # None -> mesuré sur la captation (launch_deg ESTIM), sinon 16.5
    azim_deg = 0.0
    spin_rpm = None
    side_rpm = None
    _est = {}
    if args.capture:
        if not os.path.exists(args.capture):
            print(f"ERREUR: fichier de captation introuvable : {args.capture}")
            print("usage : GolfBall.py [capture.csv|capture.wav] [--club F7] [--launch 16.5] [--elev 10] [--azim 180]")
            print("exemples de fichiers existants :")
            sci = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sci')
            for f in sorted(os.listdir(sci)):
                if f.endswith(('.csv', '.wav')) and 'F7' in f:
                    print(f"  ../sci/{f}")
            raise SystemExit(2)
        # analyse complète : binaire C++ (V club, spin 3 voies, back/side par courbure) avec repli pipe_validation
        v0_ms, spin_rpm, azim_deg, side_rpm, _est = estimate_from_capture_full(args.capture, club=args.club,
                                                                               case_angle=args.caseangle_deg)
        if launch_deg is None:
            ld = float(_est.get('launch_deg', 0.0) or 0.0)
            lc = float(_est.get('launch_conf', 0.0) or 0.0)
            ca = float(_est.get('caseangle_deg', 0.0) or 0.0)
            if ld > 0 and (lc >= 0.5 or ca != 0.0):
                launch_deg = ld   # conf suffisante, ou case angle renseigné (géométrie connue : fit+ε fiable)
                print(f"-> launch angle mesuré sur la captation : {launch_deg:.2f} deg (conf={lc:.2f}, caseangle={ca:.1f})")
            else:
                launch_deg = 16.5   # fit absent ou peu confiant (butée de grille) -> défaut historique
                print(f"-> launch angle fit peu fiable (mesuré={ld:.2f} deg, conf={lc:.2f}) "
                      f"-> repli sur {launch_deg} deg (ou --launch/--caseangle_deg pour ajuster)")
        print(f"-> simulation lancée avec V0={v0_ms*3.6:.1f} km/h, spin={spin_rpm:.0f} rpm, "
              f"side={side_rpm if side_rpm is not None else 0:.0f} rpm, "
              f"azimut={azim_deg:+.2f} deg, launch={launch_deg} deg")

    if launch_deg is None:
        launch_deg = 16.5   # pas de captation -> défaut historique

    if not HAS_MPL:
        # mode headless : résultats texte uniquement (pas d'animation)
        t, y, bounce_count = simulate_golf_ball(t_max=10, dt=0.01, max_bounces=3, v0_ms=v0_ms,
                                                launch_deg=launch_deg, azim_deg=azim_deg,
                                                spin_rpm=spin_rpm, side_rpm=side_rpm)
        _rpm = lambda w: w * 60.0 / (2.0 * np.pi)
        print(f'Club: {args.club if args.capture else club_type}  '
              f'V0={((v0_ms if v0_ms is not None else V0_ball_ms) * 3.6):.1f} km/h  '
              f'spin={spin_rpm if spin_rpm else _rpm(np.hypot(spin_y, spin_z)):.0f} rpm  '
              f'back={_rpm(spin_z):.0f} / side={_rpm(spin_y):+.0f} rpm')
        print(f'Rebonds: {bounce_count}  Portee: {y[-1, 0]:.1f} m  Apex: {y[:, 1].max():.1f} m  '
              f'z_final={y[-1, 2]:+.1f} m  Tfin: {t[-1]:.1f} s')
        raise SystemExit(0)
    # =============================================
    # EXÉCUTION DE LA SIMULATION
    # =============================================

    t, y, bounce_count = simulate_golf_ball(t_max=10, dt=0.01, max_bounces=3, v0_ms=v0_ms,
                                            launch_deg=launch_deg, azim_deg=azim_deg,
                                            spin_rpm=spin_rpm, side_rpm=side_rpm)
    x, y_pos, z, vx, vy, vz, wx, wy, wz, cl1 = y.T

    # =============================================
    # ANIMATION 3D AVEC MATPLOTLIB
    # =============================================

    # panneau d'infos (mêmes infos que le panneau fixe du HTML 3D C++, Plot3D.h)
    def _rpm(w):
        return w * 60.0 / (2.0 * np.pi)
    nan = float('nan')
    v0_kmh = (v0_ms if v0_ms is not None else V0_ball_ms) * 3.6
    spin_tot_used = spin_rpm if spin_rpm else _rpm(np.hypot(spin_y, spin_z))
    info = [f"Club         : {args.club if args.capture else club_type}"]
    if args.capture and _est:
        info.append(f"V club radar : {_est.get('vclub_kmh', nan):.1f} km/h   smash={_est.get('smash', nan):.3f}")
        info.append(f"vBall radar  : {_est.get('vball_kmh', nan):.1f} km/h  ({_est.get('vball_corr_kmh', nan):.1f} corr. cos20)")
        info.append(f"spin 3 voies : patch={_est.get('spin_patch_rpm', 0):.0f}  phase={_est.get('spin_phase_rpm', 0):.0f}  "
                    f"peigne={_est.get('spin_peigne_rpm', 0):.0f} rpm")
        if side_rpm is not None:
            info.append(f"back/side est: {_est.get('backspin_est_rpm', 0):.0f} / {side_rpm:+.0f} rpm  "
                        f"(GamaFacePath={_est.get('gfp_est_deg', 0):+.1f} deg)")
        info.append(f"alpha0={azim_deg:+.2f} deg  dAlpha={_est.get('dalpha_deg', 0):+.2f} deg  t0={_est.get('t0_s', 0):.3f} s")
    else:
        info.append(f"V club       : {v_club_kmh:.1f} km/h")
        info.append(f"club path    : {alpha_club_path_deg:+.1f} deg   face/path: {gama_face_path_deg:+.1f} deg   "
                    f"shaft lean: {shaf_lean_imp_deg:+.1f} deg")
    info.append(f"V0 balle     : {v0_kmh:.1f} km/h   launch={launch_deg:.1f} deg   azim={azim_deg:+.2f} deg")
    info.append(f"Spin total   : {spin_tot_used:.0f} rpm")
    info.append(f"Back / Side  : {_rpm(spin_z):.0f} / {_rpm(spin_y):+.0f} rpm")
    info.append(f"Portee       : {x[-1]:.1f} m   Apex: {y_pos.max():.1f} m")
    info.append(f"z final      : {z[-1]:+.1f} m   Rebonds: {bounce_count}   Tfin: {t[-1]:.1f} s")

    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection='3d')

    # Limites des axes
    ax.set_xlim(-10, max(x) * 1.1)  # caméra derrière la balle : on voit le départ
    ax.set_ylim(0, max(y_pos) * 1.1)
    ax.set_zlim(-max(abs(z)) * 1.1, max(abs(z)) * 1.1)
    ax.set_xlabel('Distance (m)')
    ax.set_ylabel('Hauteur (m)')
    ax.set_zlabel('Dév. latérale (m)')
    ax.set_title('Simulation du vol d\'une balle de golf (3D)'
                 + (f' — {os.path.basename(args.capture)}' if args.capture else f' — club {club_type}'))
    ax.view_init(elev=args.elev, azim=args.azim)  # défaut 10/180 : vue de derrière, comme le golfeur (cf. Pasted image 20250120171635.png)

    # panneau d'infos fixe en haut à gauche (comme le panneau 250 px du HTML C++)
    info_text = ax.text2D(0.02, 0.98, '\n'.join(info), transform=ax.transAxes,
                          fontsize=9, family='monospace', va='top', ha='left',
                          bbox=dict(boxstyle='round,pad=0.4', fc='white', ec='gray', alpha=0.85))

    # Dessiner le sol
    xx, zz = np.meshgrid(np.linspace(0, max(x) * 1.1, 10), np.linspace(-max(abs(z)) * 1.1, max(abs(z)) * 1.1, 10))
    yy = np.zeros_like(xx)
    ax.plot_surface(xx, yy, zz, color='green', alpha=0.3)

    # Initialiser la balle et les forces (quiver 3D : API stable avec matplotlib >= 3.10)
    ball = ax.plot([], [], [], 'o', markersize=8, color='white', markeredgecolor='black')[0]

    def _arrow(color):
        return ax.quiver([0], [0], [0], [0.0], [0.0], [0.0], color=color,
                         arrow_length_ratio=0.15, linewidth=2)
    velocity_arrow = _arrow('blue')
    drag_arrow = _arrow('red')
    magnus_arrow = _arrow('green')
    gravity_arrow = _arrow('orange')
    spin_arrow = _arrow('purple')

    # Légende
    ax.legend(
        [ball, velocity_arrow, drag_arrow, magnus_arrow, gravity_arrow, spin_arrow],
        ['Balle', 'Vitesse $\\mathbf{v}$', 'Traînée $\\mathbf{F}_D$', 'Portance $\\mathbf{F}_M$', 'Gravité $\\mathbf{F}_g$', 'Spin $\\mathbf{\\omega}$'],
        loc='upper right'
    )

    # Trajectoire
    trajectory, = ax.plot([], [], [], 'b-', alpha=0.5, linewidth=1)

    # Texte pour le temps et les rebonds
    time_text = ax.text(0, 0, 0, '', fontsize=10, color='black')
    bounce_text = ax.text(0, 0, 0, '', fontsize=10, color='black')

    def update(frame):
        # Mettre à jour la position de la balle
        ball.set_data([x[frame]], [y_pos[frame]])
        ball.set_3d_properties([z[frame]])

        # Mettre à jour la trajectoire
        trajectory.set_data(x[:frame+1], y_pos[:frame+1])
        trajectory.set_3d_properties(z[:frame+1])

        # Mettre à jour les flèches des forces
        scale = 0.5  # Échelle pour les flèches

        # Vitesse
        v_norm = np.sqrt(vx[frame] ** 2 + vy[frame] ** 2 + vz[frame] ** 2)
        if v_norm > 0:
            velocity_arrow.set_segments([[
                (x[frame], y_pos[frame], z[frame]),
                (x[frame] + vx[frame] * scale / v_norm, y_pos[frame] + vy[frame] * scale / v_norm,
                 z[frame] + vz[frame] * scale / v_norm)]])

        # Traînée (opposée à la vitesse)
        F_D_norm = 0.5 * rho_air * C_d0 * A * v_norm
        drag_arrow.set_segments([[
            (x[frame], y_pos[frame], z[frame]),
            (x[frame] - vx[frame] * F_D_norm * scale / m, y_pos[frame] - vy[frame] * F_D_norm * scale / m,
             z[frame] - vz[frame] * F_D_norm * scale / m)]])

        # Portance (Magnus)
        spin_norm = np.sqrt(wx[frame] ** 2 + wy[frame] ** 2 + wz[frame] ** 2)
        if v_norm > 0 and spin_norm > 0:
            F_M_norm = 0.5 * rho_air * cl1[frame] * A * v_norm * (diam / 2) ** 2 * (spin_norm / v_norm) ** 0.4
            F_Mx = F_M_norm * (wy[frame] * vz[frame] - wz[frame] * vy[frame])
            F_My = F_M_norm * (wz[frame] * vx[frame] - wx[frame] * vz[frame])
            F_Mz = F_M_norm * (wx[frame] * vy[frame] - wy[frame] * vx[frame])
            magnus_arrow.set_segments([[
                (x[frame], y_pos[frame], z[frame]),
                (x[frame] + F_Mx * scale / m, y_pos[frame] + F_My * scale / m, z[frame] + F_Mz * scale / m)]])

        # Gravité
        gravity_arrow.set_segments([[
            (x[frame], y_pos[frame], z[frame]),
            (x[frame], y_pos[frame] - g * scale, z[frame])]])

        # Spin
        spin_arrow.set_segments([[
            (x[frame], y_pos[frame], z[frame]),
            (x[frame] - 0.2, y_pos[frame] + 0.4, z[frame])]])

        # Mettre à jour le temps, la distance parcourue et la déviation latérale courante
        time_text.set_text(f'Temps: {t[frame]:.2f} s   dist: {x[frame]:.1f} m   dev. lat.: {z[frame]:+.1f} m')
        time_text.set_position((ax.get_xlim()[1] * 0.55, ax.get_ylim()[1] * 0.9, ax.get_zlim()[1] * 0.9))
        bounce_text.set_text(f'Rebonds: {bounce_count}')
        bounce_text.set_position((ax.get_xlim()[1] * 0.7, ax.get_ylim()[1] * 0.85, ax.get_zlim()[1] * 0.9))

        return ball, velocity_arrow, drag_arrow, magnus_arrow, gravity_arrow, spin_arrow, trajectory, time_text, bounce_text

    # Créer l'animation
    ani = FuncAnimation(
        fig,
        update,
        frames=len(t),
        interval=50,  # Délai entre les frames (ms)
        blit=False,
        repeat=True
    )

    # Sauvegarder l'animation (nécessite ffmpeg ou imagemagick)
    ani.save('golf_ball_flight_3d.mp4', writer='ffmpeg', fps=30, dpi=300)

    # Afficher l'animation
    plt.tight_layout()
    plt.show()