#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Pipe de validation aveugle (point 1) : CSV radar -> estimation (vBall, SpinTotal) -> comparaison
aux references encodees dans le nom de fichier.

CSV : 2 lignes (canal R1, canal R2), valeurs float ascii separees par ';' (export Scilab des .wav).
Nom  : 2019-02-24-F7-<carry>m-<vball>kmh-<backspin>trm-<sidespin>trm-cs<club>.csv

Usage : python3 pipe_validation.py [dossier_csv]
Sortie : tableau par coup + MAE vitesse (radiale et corrigee cos 20 deg) et spin.
"""
import os, re, sys, glob
import numpy as np

FS   = 44100.0
KV   = 19.49     # Hz / (km/h)   : f_D = KV * V[km/h]  (radar 10.525 GHz)
KS   = 0.1567    # Hz / RPM      : excursion micro-Doppler 2 r w / lambda
NFFT = 4096      # fenetre spectrogramme (93 ms)
HOP  = NFFT // 2 # recouvrement 50 %
FMIN, FMAX = 1000.0, 6000.0   # passe-bande vitesse balle (Hz) ~ 51-308 km/h
THETA_DEG = 20.0              # angle effectif radar/balle observe (~15-20 deg)


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
    """Passe-bande par masque spectral (zero-phase)."""
    X = np.fft.rfft(x)
    f = np.fft.rfftfreq(len(x), 1.0 / FS)
    X[(f < fmin) | (f > fmax)] = 0.0
    return np.fft.irfft(X, len(x))


def parabolic_peak(P, k):
    """Interpolation parabolique sous-bin du pic k : delta = (P[k-1]-P[k+1]) / (2*(P[k-1]-2P[k]+P[k+1]))"""
    if k <= 0 or k >= len(P) - 1:
        return float(k)
    denom = P[k - 1] - 2.0 * P[k] + P[k + 1]
    if denom == 0:
        return float(k)
    delta = 0.5 * (P[k - 1] - P[k + 1]) / denom
    if abs(delta) > 1:
        delta = 0.0
    return k + delta


def spectrogram(x, nfft=NFFT, hop=HOP):
    """Ridge du pic principal par frame (fenetre Hann, recouvrement 50 %, interpolation parabolique)."""
    w = np.hanning(nfft)
    nframes = 1 + max(0, (len(x) - nfft)) // hop
    f = np.fft.rfftfreq(nfft, 1.0 / FS)
    ridge = np.zeros(nframes)
    power = np.zeros(nframes)
    spec_max = None
    for i in range(nframes):
        seg = x[i * hop:i * hop + nfft] * w
        P = np.abs(np.fft.rfft(seg)) ** 2
        k = int(np.argmax(P))
        ridge[i] = (parabolic_peak(P, k)) * FS / nfft
        power[i] = P[k]
    t = (np.arange(nframes) * hop + nfft / 2.0) / FS
    return t, ridge, power, f


def estimate_channel(x):
    """Retourne (vBall_radial_kmh, spin_rpm, t_impact) pour un canal."""
    x = x - x.mean()
    xf = bandpass(x, FMIN, FMAX)
    t, ridge, power, f = spectrogram(xf)
    i0 = int(np.argmax(power))                    # impact = frame la plus energetique
    t0 = t[i0]
    win = (t > t0) & (t < t0 + 0.05)              # 50 ms apres impact
    if win.sum() < 3:
        win = t > t0
    vball = float(np.median(ridge[win]) / KV)     # vitesse radiale

    # Spin : excursion micro-Doppler autour du ridge sur les frames fortes apres impact
    w = np.hanning(NFFT)
    spins = []
    for i in range(i0, len(t)):
        if power[i] < 0.1 * power[i0]:
            continue
        seg = xf[i * HOP:i * HOP + NFFT] * w
        P = np.abs(np.fft.rfft(seg)) ** 2
        fc = ridge[i]
        band = (f > fc - 3000) & (f < fc + 3000) & (np.abs(f - fc) > 150)  # bords +/- autour du centre
        if band.sum() < 4:
            continue
        Pb = P[band]
        if Pb.max() < 0.05 * P.max():
            continue
        fb = f[band][Pb > 0.05 * Pb.max()]
        if len(fb) < 2:
            continue
        df = (fb.max() - fb.min()) / 2.0          # demi-excursion
        spins.append(df / KS)
    spin = float(np.median(spins)) if spins else float('nan')
    return vball, spin, t0


def main():
    d = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))
    files = sorted(glob.glob(os.path.join(d, '2019-02-24-F7-*.csv')))
    print(f"{len(files)} CSV trouves dans {d}\n")
    hdr = f"{'fichier':<46} {'vRef':>5} {'vR1':>6} {'vR2':>6} {'vMes':>6} {'vMes/20°':>8} {'err%':>6} | {'spinRef':>7} {'spinMes':>7} {'err%':>6} | {'csRef':>5}"
    print(hdr)
    print('-' * len(hdr))
    errs_v, errs_vcorr, errs_s = [], [], []
    for fn in files:
        ref = parse_name(fn)
        if ref is None:
            print('nom non reconnu:', fn)
            continue
        rows = load_csv(fn)
        res = [estimate_channel(r) for r in rows[:2]]
        v_r = [r[0] for r in res]
        sp = [r[1] for r in res if not np.isnan(r[1])]
        # rejet inter-radars : si les 2 canaux divergent de plus de 15 %, on garde le canal
        # le plus coherent (le plus proche de la médiane), flag 'rejet' (cf. leçon 25.1 : faux lobes)
        rejet = ''
        if len(v_r) == 2 and abs(v_r[0] - v_r[1]) / np.mean(v_r) > 0.15:
            med = np.median(v_r)
            keep = int(np.argmin([abs(v - med) for v in v_r]))
            v_r = [v_r[keep]]
            rejet = 'REJET'
        v_mes = float(np.mean(v_r))
        v_corr = v_mes / np.cos(np.radians(THETA_DEG))
        sp_mes = float(np.mean(sp)) if sp else float('nan')
        err_v = 100 * (v_mes - ref['vball']) / ref['vball']
        err_vc = 100 * (v_corr - ref['vball']) / ref['vball']
        err_s = 100 * (sp_mes - ref['backspin']) / ref['backspin'] if sp else float('nan')
        errs_v.append(err_v); errs_vcorr.append(err_vc)
        if sp: errs_s.append(err_s)
        print(f"{os.path.basename(fn):<46} {ref['vball']:>5} {res[0][0]:>6.1f} {res[1][0] if len(res)>1 else float('nan'):>6.1f} "
              f"{v_mes:>6.1f} {v_corr:>8.1f} {err_v:>+6.1f} | {ref['backspin']:>7} "
              f"{sp_mes:>7.0f} {err_s:>+6.1f} | {ref['club']:>5} {rejet}")
    print('-' * len(hdr))
    mae = lambda e: float(np.mean(np.abs(e)))
    print(f"MAE vitesse radiale : {mae(errs_v):.1f} %   |  MAE corrigee cos{THETA_DEG:.0f}deg : {mae(errs_vcorr):.1f} %")
    if errs_s:
        print(f"MAE spin (vs backspin ref) : {mae(errs_s):.1f} %   (n={len(errs_s)})")


if __name__ == '__main__':
    main()
