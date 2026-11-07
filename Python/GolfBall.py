import numpy as np
from scipy.integrate import odeint
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.patches import Circle, FancyArrowPatch
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d import proj3d

# =============================================
# PARAMÈTRES PHYSIQUES (basés sur votre modèle Scilab)
# =============================================

# Propriétés de la balle
m = 0.04545  # Masse (kg)
diam = 0.043  # Diamètre (m)
A = np.pi * (diam / 2) ** 2  # Section transversale (m²)
I = (2 / 5) * m * (diam / 2) ** 2  # Moment d'inertie (kg·m²)

# Propriétés de l'air
rho_air = 1.292 * 273.15 / (20 + 273.15)  # Densité (kg/m³) à 20°C — même formule que Golfball.sci
mu = 1.5e-5  # Viscosité dynamique (m²/s)
g = 9.81  # Accélération gravitationnelle (m/s²)

# Coefficients aérodynamiques
C_d0 = 0.36  # Coefficient de traînée de base (relation de White, comme Golfball.sci)
C_l = 0.15   # Coefficient de portance (Magnus)

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
Km = 1 - 0.3556 * miss  # miss en cm (à convertir en m si nécessaire)

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
    # Déballage des variables
    x, y, z, vx, vy, vz, wx, wy, wz = y

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
        C_m = C_l * (diam / 2) ** 2 * (spin_norm / v_norm) ** 0.4  # facteur (diam/2)^2 comme Golfball.sci
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

    return [dxdt, dydt, dzdt, dvxdt, dvydt, dvzdt, dWdt[0], dWdt[1], dWdt[2]]

# =============================================
# MODÈLE DE REBOND (basé sur Penner)
# =============================================

def handle_bounce(y, t):
    x, y_pos, z, vx, vy, vz, wx, wy, wz = y

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
    mu_friction_critic = 2 * (vix_prime + (diam / 2) * wz) / (7 * (1 + eball) * abs(viy_prime))

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

    return [x, y_new, z, vx_new, vy_new, vz_new, wx_new, wy_new, wz_new]

# =============================================
# MODÈLE DE ROULEMENT
# =============================================

def handle_rolling(y, t):
    x, y_pos, z, vx, vy, vz, wx, wy, wz = y

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

    return [x_new, y_new, z_new, vx_new, vy_new, vz_new, wx_new, wy_new, wz_new]

# =============================================
# SIMULATION COMPLÈTE (VOL + REBONDS + ROULEMENT)
# =============================================

def simulate_golf_ball(t_max=10, dt=0.01, max_bounces=3):
    # Conditions initiales
    y0 = [x0, y0, z0, vx0, vy0, vz0, spin_x, spin_y, spin_z]
    t = np.arange(0, t_max, dt)
    y = np.zeros((len(t), len(y0)))
    y[0] = y0

    bounce_count = 0
    is_rolling = False

    for i in range(1, len(t)):
        # Si la balle est en roulement
        if is_rolling:
            y[i] = handle_rolling(y[i-1], t[i-1])
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
# EXÉCUTION DE LA SIMULATION
# =============================================

t, y, bounce_count = simulate_golf_ball(t_max=10, dt=0.01, max_bounces=3)
x, y_pos, z, vx, vy, vz, wx, wy, wz = y.T

# =============================================
# ANIMATION 3D AVEC MATPLOTLIB
# =============================================

fig = plt.figure(figsize=(12, 8))
ax = fig.add_subplot(111, projection='3d')

# Limites des axes
ax.set_xlim(0, max(x) * 1.1)
ax.set_ylim(0, max(y_pos) * 1.1)
ax.set_zlim(-max(abs(z)) * 1.1, max(abs(z)) * 1.1)
ax.set_xlabel('Distance (m)')
ax.set_ylabel('Hauteur (m)')
ax.set_zlabel('Dév. latérale (m)')
ax.set_title('Simulation du vol d\'une balle de golf (3D)')

# Dessiner le sol
xx, zz = np.meshgrid(np.linspace(0, max(x) * 1.1, 10), np.linspace(-max(abs(z)) * 1.1, max(abs(z)) * 1.1, 10))
yy = np.zeros_like(xx)
ax.plot_surface(xx, yy, zz, color='green', alpha=0.3)

# Initialiser la balle et les forces
ball = ax.plot([], [], [], 'o', markersize=8, color='white', markeredgecolor='black')[0]
velocity_arrow = FancyArrowPatch((0, 0, 0), (0, 0, 0), arrowstyle='->', color='blue', mutation_scale=20)
ax.add_patch(velocity_arrow)
drag_arrow = FancyArrowPatch((0, 0, 0), (0, 0, 0), arrowstyle='->', color='red', mutation_scale=20)
ax.add_patch(drag_arrow)
magnus_arrow = FancyArrowPatch((0, 0, 0), (0, 0, 0), arrowstyle='->', color='green', mutation_scale=20)
ax.add_patch(magnus_arrow)
gravity_arrow = FancyArrowPatch((0, 0, 0), (0, 0, 0), arrowstyle='->', color='orange', mutation_scale=20)
ax.add_patch(gravity_arrow)
spin_arrow = FancyArrowPatch((0, 0, 0), (0, 0, 0), arrowstyle='->', color='purple', mutation_scale=20)
ax.add_patch(spin_arrow)

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
        velocity_arrow.set_data(
            [x[frame], x[frame] + vx[frame] * scale / v_norm],
            [y_pos[frame], y_pos[frame] + vy[frame] * scale / v_norm]
        )
        velocity_arrow.set_3d_properties([z[frame], z[frame] + vz[frame] * scale / v_norm])

    # Traînée (opposée à la vitesse)
    F_D_norm = 0.5 * rho_air * C_d0 * A * v_norm
    drag_arrow.set_data(
        [x[frame], x[frame] - vx[frame] * F_D_norm * scale / m],
        [y_pos[frame], y_pos[frame] - vy[frame] * F_D_norm * scale / m]
    )
    drag_arrow.set_3d_properties([z[frame], z[frame] - vz[frame] * F_D_norm * scale / m])

    # Portance (Magnus)
    spin_norm = np.sqrt(wx[frame] ** 2 + wy[frame] ** 2 + wz[frame] ** 2)
    if v_norm > 0 and spin_norm > 0:
        F_M_norm = 0.5 * rho_air * C_l * A * v_norm * (diam / 2) ** 2 * (spin_norm / v_norm) ** 0.4
        F_Mx = F_M_norm * (wy[frame] * vz[frame] - wz[frame] * vy[frame])
        F_My = F_M_norm * (wz[frame] * vx[frame] - wx[frame] * vz[frame])
        F_Mz = F_M_norm * (wx[frame] * vy[frame] - wy[frame] * vx[frame])
        magnus_arrow.set_data(
            [x[frame], x[frame] + F_Mx * scale / m],
            [y_pos[frame], y_pos[frame] + F_My * scale / m]
        )
        magnus_arrow.set_3d_properties([z[frame], z[frame] + F_Mz * scale / m])

    # Gravité
    gravity_arrow.set_data(
        [x[frame], x[frame]],
        [y_pos[frame], y_pos[frame] - m * g * scale / m]
    )
    gravity_arrow.set_3d_properties([z[frame], z[frame]])

    # Spin
    spin_arrow.set_data(
        [x[frame], x[frame] - 0.2],
        [y_pos[frame], y_pos[frame] + 0.4]
    )
    spin_arrow.set_3d_properties([z[frame], z[frame]])

    # Mettre à jour le temps et les rebonds
    time_text.set_text(f'Temps: {t[frame]:.2f} s')
    time_text.set_position((ax.get_xlim()[1] * 0.7, ax.get_ylim()[1] * 0.9, ax.get_zlim()[1] * 0.9))
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