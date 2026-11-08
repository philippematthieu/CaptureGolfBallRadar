#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Pipe de validation aveugle (points 1+2) : CSV radar -> (vBall, SpinTotal) -> comparaison
aux references encodees dans le nom de fichier.

CSV : 2 lignes (canal R1, canal R2), valeurs float ascii separees par ';' (export Scilab des .wav).
Nom  : 2019-02-24-F7-<carry>m-<vball>kmh-<backspin>trm-<sidespin>trm-?cs-?<club>.csv

Spin (point 2) :
- tracking des 3 cretes par frame : f_c (translation), f+ / f- (bords micro-Doppler)
- Spin_edge   = (f+ - f-) / 2 / KS            (excursion Doppler, KS = 0.1567 Hz/RPM)
- Spin_period = 60 * f_rot                    (periodicite du patch, f_rot = RPM/60)
- controles qualite : symetrie des bords eta=0.2, cohérence inter-radars 20 %,
  mediane temporelle ; sinon confiance 'non identifiable'.

Usage : python3 pipe_validation.py [dossier_csv]
"""
import os, re, sys, glob
import numpy as np

FS   = 44100.0
KV   = 19.49     # Hz / (km/h)   : f_D = KV * V[km/h]  (radar 10.525 GHz)
KS   = 0.1567    # Hz / RPM      : excursion micro-Doppler 2 r w / lambda
NFFT = 2048      # fenetre spectrogramme (46 ms, ~2x plus de frames pour le suivi court)
HOP  = NFFT // 2 # recouvrement 50 %
FMIN, FMAX = 1000.0, 6000.0   # passe-bande vitesse balle (Hz)
THETA_DEG = 20.0              # angle effectif radar/balle observe
ETA = 0.2                     # controle symetrie des bords micro-Doppler
REL_DB = 30.0                 # seuil relatif (dB) pour les bords f+ / f-
EDGE_MIN, EDGE_MAX = 100.0, 4000.0  # bandes de recherche des bords autour de f_c (Hz)
COH_RADARS = 0.20             # cohérence inter-radars (20 %)
FROT_MIN, FROT_MAX = 50.0, 200.0  # bande f_rot = RPM/60 attendu (3000-12000 rpm)


def parse_name(fn):
    m = re.search(r'-F7-(\d+)m-(\d+)kmh-(\d+)trm-(\d+)trm-?cs-?(\d+)\.csv$', os.path.basename(fn))
    if not m:
        return None
    carry, vball, backspin, sidespin, club = map(int, m.groups())
    return dict(carry=carry, vball=vball, backspin=backspin, sidespin=sidespin, club=club)


def load_csv(fn):
    rows = []
    with open(fn, encoding='utf-8', errors='replace') as f:
        for line in f:
            line = line.strip()
            if line:
                rows.append(np.array([float(v) for v in line.split(';') if v.strip()]))
    return rows


def bandpass(x, fmin, fmax):
    X = np.fft.rfft(x)
    f = np.fft.rfftfreq(len(x), 1.0 / FS)
    X[(f < fmin) | (f > fmax)] = 0.0
    return np.fft.irfft(X, len(x))


def parabolic_peak(P, k):
    if k <= 0 or k >= len(P) - 1:
        return float(k)
    denom = P[k - 1] - 2.0 * P[k] + P[k + 1]
    if denom == 0:
        return float(k)
    delta = 0.5 * (P[k - 1] - P[k + 1]) / denom
    return k + (delta if abs(delta) <= 1 else 0.0)


def hilbert_abs(x):
    """|signal analytique| via FFT (Hilbert)."""
    n = len(x)
    X = np.fft.fft(x)
    h = np.zeros(n)
    h[0] = 1.0
    h[1:(n + 1) // 2] = 2.0
    if n % 2 == 0:
        h[n // 2] = 1.0
    return np.abs(np.fft.ifft(X * h))


def spectrogram(x, nfft=NFFT, hop=HOP):
    w = np.hanning(nfft)
    nframes = 1 + max(0, (len(x) - nfft)) // hop
    f = np.fft.rfftfreq(nfft, 1.0 / FS)
    ridge = np.zeros(nframes)
    power = np.zeros(nframes)
    specs = []
    for i in range(nframes):
        seg = x[i * hop:i * hop + nfft] * w
        P = np.abs(np.fft.rfft(seg)) ** 2
        k = int(np.argmax(P))
        ridge[i] = parabolic_peak(P, k) * FS / nfft
        power[i] = P[k]
        specs.append(P)
    t = (np.arange(nframes) * hop + nfft / 2.0) / FS
    return t, ridge, power, f, specs


def track_edges(P, f, fc):
    """Bords micro-Doppler f+ / f- autour de fc (seuil -REL_DB dB du pic global), avec interpolation."""
    pmax = P.max()
    thr = pmax * 10 ** (-REL_DB / 10.0)
    band_hi = (f > fc + EDGE_MIN) & (f < fc + EDGE_MAX)
    band_lo = (f > fc - EDGE_MAX) & (f < fc - EDGE_MIN)
    if not (band_hi.any() and band_lo.any()):
        return None
    khi = np.where(band_hi)[0][np.argmax(P[band_hi])]
    klo = np.where(band_lo)[0][np.argmax(P[band_lo])]
    if P[khi] < thr or P[klo] < thr:
        return None
    fplus = parabolic_peak(P, khi) * FS / NFFT
    fminus = parabolic_peak(P, klo) * FS / NFFT
    return fminus, fplus


def spin_peaks_mean(xf, t0, rel_db=REL_DB):
    """Approche 'pics intenses' : densité spectrale moyenne (pondérée par la puissance) apres impact,
    pics locaux > -rel_db dB du max ; centre = pic max, f- = pic fort sous fc, f+ = pic fort au-dessus.
    Retourne (spin_rpm, symetrie) ; symetrie = |(fc-f-)-(f+-fc)|/df  (eta du doc §11)."""
    t, ridge, power, f, specs = spectrogram(xf)
    i0 = int(np.argmin(np.abs(t - t0)))
    after = np.arange(len(t)) >= i0
    w = power[after]
    if w.sum() == 0:
        return float('nan'), float('nan')
    S = np.average(np.array(specs)[after], axis=0, weights=w)
    thr = S.max() * 10 ** (-rel_db / 10.0)
    idx = np.where((S[1:-1] >= S[:-2]) & (S[1:-1] >= S[2:]) & (S[1:-1] > thr))[0] + 1
    if len(idx) < 3:
        return float('nan'), float('nan')
    kc = int(np.argmax(S))
    fc = parabolic_peak(S, kc) * FS / NFFT
    fp = f[idx]
    lo = fp < fc - EDGE_MIN
    hi = fp > fc + EDGE_MIN
    if not (lo.any() and hi.any()):
        return float('nan'), float('nan')
    fminus = fp[lo][int(np.argmax(S[idx[lo]]))]
    fplus = fp[hi][int(np.argmax(S[idx[hi]]))]
    df = (fplus - fminus) / 2.0
    if df <= 0:
        return float('nan'), float('nan')
    sym = abs((fc - fminus) - (fplus - fc)) / df
    return df / KS, sym


def estimate_channel(x):
    x = x - x.mean()
    xf = bandpass(x, FMIN, FMAX)
    t, ridge, power, f, specs = spectrogram(xf)
    i0 = int(np.argmax(power))                    # impact = frame la plus energetique
    t0 = t[i0]
    win = (t > t0) & (t < t0 + 0.05)
    if win.sum() < 3:
        win = t > t0
    vball = float(np.median(ridge[win]) / KV)

    # --- Spin par excursion : tracking 3 cretes f_c / f+ / f- frame par frame ---
    spins_edge = []
    for i in range(i0, len(t)):
        if power[i] < 0.05 * power[i0]:
            continue
        edges = track_edges(specs[i], f, ridge[i])
        if edges is None:
            continue
        fminus, fplus = edges
        fc = ridge[i]
        df = (fplus - fminus) / 2.0
        if df <= 0:
            continue
        # controle de symetrie des bords : |(fc-f-) - (f+-fc)| < eta * df
        if abs((fc - fminus) - (fplus - fc)) > ETA * df:
            continue
        spins_edge.append(df / KS)
    spin_edge = float(np.median(spins_edge)) if spins_edge else float('nan')
    # --- Hybride : pics intenses (centre/min/max) si symetrie < eta, prioritaire sur l'excursion frame ---
    spin_pk, sym_pk = spin_peaks_mean(xf, t0)
    if not np.isnan(spin_pk) and sym_pk < ETA:
        spin_edge = spin_pk   # bords propres -> pics fiables
        n_edge_note = f'peaks(sym={sym_pk:.2f})'
    else:
        n_edge_note = f'edge n={len(spins_edge)}'

    # --- Spin par periodicite du patch : spectre de l'enveloppe (Hilbert) apres impact ---
    # Le fondamental f_rot est le pic qui explique le plus d'harmoniques (k*f_rot) :
    # cela evite les sous-harmoniques (f_rot/2) et harmoniques pures.
    seg = xf[int(t0 * FS):int((t0 + 0.25) * FS)]  # 250 ms apres impact pour la periodicite
    spin_period = float('nan')
    if len(seg) > NFFT:
        env = hilbert_abs(seg)
        env = env - env.mean()
        env = env * np.hanning(len(env))
        Pe = np.abs(np.fft.rfft(env)) ** 2
        fe = np.fft.rfftfreq(len(env), 1.0 / FS)
        band = (fe >= FROT_MIN) & (fe <= FROT_MAX)
        if band.any():
            Pb = Pe.copy()
            Pb[~band] = 0
            pmax = Pb.max()
            # pics locaux au-dessus de 10 % du max dans la bande
            loc = np.where(band & (Pe > 0.1 * pmax))[0]
            loc = loc[(loc > 0) & (loc < len(Pe) - 1) & (Pe[loc] >= Pe[loc - 1]) & (Pe[loc] >= Pe[loc + 1])]
            if len(loc) == 0:
                loc = np.array([int(np.argmax(Pb))])
            df = fe[1] - fe[0]
            best, best_score = None, -1.0
            for p in loc:
                f0 = fe[p]
                score = Pe[p]
                for q in loc:
                    if q == p:
                        continue
                    k = round(fe[q] / f0)
                    if k >= 1 and abs(fe[q] - k * f0) < 2 * df:
                        score += Pe[q]
                if score > best_score:
                    best_score, best = score, p
            f_rot = parabolic_peak(Pe, best) * FS / len(env)
            spin_period = 60.0 * f_rot            # f_rot = RPM/60

    return dict(vball=vball, spin_edge=spin_edge, spin_period=spin_period, t0=t0,
                n_edge=len(spins_edge), note=n_edge_note, t=t, ridge=ridge, power=power,
                specs=specs, fgrid=f)


# --- Direction horizontale alpha(t) : appariement stereo f_C / f_Delta (cf. doc §8, §12) ---
B_BASE = 0.30     # baseline radars (m)
L0 = 1.50         # distance initiale balle (m)
FDIFF_MAX = 30.0  # Hz : rejet des paires de ridges non coherentes (|f1-f2| > 30 Hz)

def compute_alpha(r1, r2):
    """alpha(t) en degres a partir des ridges apparies des 2 radars.
    alpha = arctan( (2 L(t)/b) * (f_Delta / f_C) ), L(t) ~ L0 + V cos(theta) (t-t0).
    Appariement : f1 = ridge R1 (balle) ; f2 = pic de R2 dans +/-300 Hz autour de f1
    (le ridge global de R2 peut rester sur le club : faux lobe, cf. leçon 25.1)."""
    t = r1['t']
    i0 = int(np.argmin(np.abs(t - r1['t0'])))
    f1 = r1['ridge']
    p1 = r1['power']
    fg = r2['fgrid']
    p2max = r2['power'].max()
    tt, fc_l, fd_l = [], [], []
    for i in range(i0, len(t)):
        if p1[i] < 0.02 * p1.max():
            continue
        P2 = r2['specs'][i]
        band = (fg > f1[i] - 300) & (fg < f1[i] + 300)
        if not band.any():
            continue
        kb = np.where(band)[0]
        k = kb[int(np.argmax(P2[kb]))]
        if P2[k] < 0.01 * p2max:
            continue
        f2 = parabolic_peak(P2, k) * FS / NFFT
        if abs(f1[i] - f2) > FDIFF_MAX * 10:   # 300 Hz : on a déjà restreint la bande
            continue
        tt.append(t[i])
        fc_l.append((f1[i] + f2) / 2.0)
        fd_l.append((f1[i] - f2) / 2.0)
    if len(tt) < 3:
        return None
    tt = np.array(tt); fc = np.array(fc_l); fd = np.array(fd_l)
    v_ms = np.median(fc) / KV * 1000.0 / 3600.0          # m/s (radial)
    L = L0 + v_ms / np.cos(np.radians(THETA_DEG)) * (tt - t[i0])  # approx L(t)
    alpha = np.degrees(np.arctan((2.0 * L / B_BASE) * (fd / fc)))
    w0 = (tt >= t[i0]) & (tt <= t[i0] + 0.05)   # fenetre initiale ~50 ms (le hop fait 46 ms)
    wf = tt >= tt[-1] - 0.10                     # fenetre finale ~100 ms
    if w0.sum() < 1 or wf.sum() < 1:
        return None
    alpha0 = float(np.median(alpha[w0]))
    dalpha_bf = float(np.median(alpha[wf]) - np.median(alpha[w0]))
    return dict(alpha0=alpha0, dalpha_bf=dalpha_bf, alpha_t=alpha, t_t=tt)



def aggregate(res):
    """Fusion des canaux + confiance. Retourne (vball, spin, conf)."""
    v_r = [r['vball'] for r in res]
    vball = float(np.mean(v_r))
    spins = [r['spin_edge'] for r in res if not np.isnan(r['spin_edge'])]
    pers  = [r['spin_period'] for r in res if not np.isnan(r['spin_period'])]
    spin_edge = float(np.median(spins)) if spins else float('nan')
    spin_per = float(np.median(pers)) if pers else float('nan')

    # choix de l'estimateur : concordance edge/period > fusion, sinon period (plus robuste),
    # edge seul si pas de period, rien sinon
    if not np.isnan(spin_edge) and not np.isnan(spin_per):
        if abs(spin_edge - spin_per) / ((spin_edge + spin_per) / 2) < COH_RADARS:
            spin, conf = float(np.mean([spin_edge, spin_per])), f"edge+period OK (n={len(spins)})"
        else:
            spin, conf = spin_per, f"edge KO, period seul (n={len(spins)})"
    elif not np.isnan(spin_per):
        spin, conf = spin_per, 'period seul'
    elif not np.isnan(spin_edge):
        spin, conf = spin_edge, f'edge seul (n={len(spins)})'
    else:
        return vball, float('nan'), 'non identifiable'
    # coherence inter-radars sur l'estimateur retenu
    if len(spins) > 1 and abs(spins[0] - spins[1]) / np.mean(spins) > COH_RADARS:
        return vball, float('nan'), 'non identifiable (radars divergent)'
    return vball, spin, conf


def main():
    d = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))
    files = sorted(glob.glob(os.path.join(d, '2019-02-24-F7-*.csv')))
    print(f"{len(files)} CSV trouves dans {d}\n")
    hdr = (f"{'fichier':<44} {'vRef':>4} {'vMes':>6} {'v/20°':>6} {'err%':>6} | "
           f"{'spinRef':>7} {'edge':>6} {'period':>6} {'spin':>6} {'err%':>6} | {'betaRef':>6} {'alpha0':>7} {'dAlpha':>7} {'signe':>4} | conf")
    print(hdr)
    print('-' * len(hdr))
    errs_v, errs_vc, errs_s = [], [], []
    for fn in files:
        ref = parse_name(fn)
        if ref is None:
            print('nom non reconnu:', fn)
            continue
        rows = load_csv(fn)
        res = [estimate_channel(r) for r in rows[:2]]
        # rejet vitesse inter-radars a 15 % (faux lobes)
        v_r = [r['vball'] for r in res]
        rejet = ''
        if len(v_r) == 2 and abs(v_r[0] - v_r[1]) / np.mean(v_r) > 0.15:
            med = np.median(v_r)
            res = [res[int(np.argmin([abs(v - med) for v in v_r]))]]
            rejet = 'REJET'
        v_mes, sp_mes, conf = aggregate(res)
        v_corr = v_mes / np.cos(np.radians(THETA_DEG))
        err_v = 100 * (v_mes - ref['vball']) / ref['vball']
        err_vc = 100 * (v_corr - ref['vball']) / ref['vball']
        err_s = 100 * (sp_mes - ref['backspin']) / ref['backspin'] if not np.isnan(sp_mes) else float('nan')
        errs_v.append(err_v); errs_vc.append(err_vc)
        if not np.isnan(sp_mes):
            errs_s.append(err_s)
        edge = np.nanmedian([r['spin_edge'] for r in res])
        per = np.nanmedian([r['spin_period'] for r in res])
        al = compute_alpha(res[0], res[1] if len(res) > 1 else res[0])
        beta_ref = np.degrees(np.arctan2(ref['sidespin'], ref['backspin']))
        if al is not None:
            a_str = f"a0={al['alpha0']:>+6.2f} dA={al['dalpha_bf']:>+6.2f}"
            signe_ok = 'OK' if np.sign(al['dalpha_bf']) == np.sign(ref['sidespin']) else 'KO'
            a_str += f" {signe_ok}"
        else:
            a_str = "alpha: non resolu"
        print(f"{os.path.basename(fn):<44} {ref['vball']:>4} {v_mes:>6.1f} {v_corr:>6.1f} {err_v:>+6.1f} | "
              f"{ref['backspin']:>7} {edge:>6.0f} {per:>6.0f} {sp_mes:>6.0f} {err_s:>+6.1f} | "
              f"bRef={beta_ref:>+5.1f} {a_str} | {conf} {rejet}")
    print('-' * len(hdr))
    mae = lambda e: float(np.mean(np.abs(e)))
    print(f"MAE vitesse radiale : {mae(errs_v):.1f} %   |  MAE corrigee cos{THETA_DEG:.0f}deg : {mae(errs_vc):.1f} %")
    if errs_s:
        print(f"MAE spin : {mae(errs_s):.1f} %   (n={len(errs_s)})")


if __name__ == '__main__':
    main()
