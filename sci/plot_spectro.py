#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Affiche / sauvegarde la densité spectrale d'une captation radar (CSV 2 lignes ';' ou WAV),
style des sci/img/spectro_F7-*.png : 2 panneaux R1/R2, axe Y en km/h (f/KV, limité à 350 km/h),
axe X en s, colormap 'hot', ridge balle cyan (post-impact), impact rouge.
Vue pleine par défaut : axe Y 0–250 km/h (f/KV), ridge (crête par frame) en cyan sur toute la
duree (lobe du club avant impact, puis balle), impact en pointillés blancs. Le titre reprend la
reference parsée dans le nom de fichier (carry / vBall / spin).
Option --zoom N : zoom ±N Hz autour de vBall estimé (filtrage centré sur vBall).
Fenêtre STFT 512 (11,6 ms, hop 128) par défaut : ridge bien résolu en temps
(résolution fréquence 86 Hz ≈ 4,4 km/h par bin, affinée par interpolation parabolique).
Passe-haut à 50 km/h (974,5 Hz) appliqué au signal avant le spectrogramme (--hp N, 0 = off) :
rejette le lobe du club sous 50 km/h.

Usage :
    python3 plot_spectro.py <capture.csv|wav> [sortie.png] [--nfft 512] [--hop 128] [--hp 50] [--zoom 256] [--open]
Exemples :
    python3 plot_spectro.py 2019-02-24-F7-133m-160kmh-5432trm-311trm-cs117.csv spectro.png --open
    python3 plot_spectro.py 2019-02-24-F7-137m-166kmh-4990trm-1312trm-cs118.csv out.png --zoom 256
"""
import os, sys
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from pipe_validation import FS, KV, spectrogram, load_csv, estimate_channel, parse_name, bandpass


def load_wav(fn):
    import wave
    with wave.open(fn, 'rb') as w:
        ch, sw, fs = w.getnchannels(), w.getsampwidth(), w.getframerate()
        raw = w.readframes(w.getnframes())
    if sw == 2:
        a = np.frombuffer(raw, dtype='<i2').astype(float) / 32768.0
    elif sw == 1:
        a = (np.frombuffer(raw, dtype=np.uint8).astype(float) - 128.0) / 128.0
    else:
        a = np.frombuffer(raw, dtype='<i4').astype(float) / 2147483648.0
    a = a.reshape(-1, ch)
    return [a[:, 0], a[:, 1] if ch > 1 else a[:, 0]]

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt


VMAX_KMH = 250.0   # vue pleine : axe Y 0..250 km/h (f/KV), une balle de golf ne dépasse pas ~250 km/h
HP_KMH = 50.0      # passe-haut avant spectrogramme (km/h) : rejette le lobe du club ; 0 = désactivé

def panneau(ax, x, nfft, hop, label, zoom_hz=None, hp_kmh=HP_KMH):
    if hp_kmh and hp_kmh > 0:                # passe-haut FFT à hp_kmh * KV Hz
        x = bandpass(x - x.mean(), hp_kmh * KV, FS / 2.0)
    t, ridge, power, f, specs = spectrogram(x, nfft, hop)
    S = np.array(specs).T                    # [nbin, nframe] pour imshow
    i0 = int(np.argmax(power))
    # vitesse estimée (comme pipe_validation)
    try:
        vball = float(estimate_channel(x)['vball'])
    except Exception:
        vball = float(np.median(ridge[i0:][power[i0:] > 0.03 * power[i0]])) / KV
    fest = vball * KV
    if zoom_hz is None:                      # vue pleine 0..VMAX_KMH
        y0, y1 = 0.0, VMAX_KMH
    else:                                    # zoom ±zoom_hz centré sur vBall (filtrage)
        y0 = max(f[0] / KV, (fest - zoom_hz) / KV)
        y1 = min(VMAX_KMH, (fest + zoom_hz) / KV)
    db = 10 * np.log10(S / S.max() + 1e-12)
    ax.imshow(db, origin='lower', aspect='auto', cmap='hot', vmin=-80, vmax=0,
              extent=[t[0], t[-1], f[0] / KV, f[-1] / KV])
    if zoom_hz is None:
        ax.plot(t, ridge / KV, '-c', lw=1)   # ridge sur toute la duree (club puis balle)
    else:
        post = (power[i0:] > 0.03 * power[i0]) & (np.abs(ridge[i0:] - fest) <= zoom_hz)
        ax.plot(t[i0:][post], ridge[i0:][post] / KV, '.c', ms=3)
    ax.axvline(t[i0], color='w', ls=':', lw=1)
    ax.text(t[i0] + 0.002, y1 - 0.08 * (y1 - y0), 'impact', color='w', fontsize=8)
    ax.set_ylabel('km/h')
    ax.set_title(f'{label}  (vBall={vball:.1f} km/h)', fontsize=9, loc='left')
    ax.set_xlim(t[0], t[-1]); ax.set_ylim(y0, y1)
    return t, ridge, power, f, i0


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    opts = set(a for a in sys.argv[1:] if a.startswith('--'))
    def optval(name, defv):
        return int(sys.argv[sys.argv.index(name) + 1]) if name in sys.argv else defv
    def optfloat(name, defv):
        return float(sys.argv[sys.argv.index(name) + 1]) if name in sys.argv else defv
    if not args:
        print(__doc__); return 1
    path = args[0]
    out = args[1] if len(args) > 1 else os.path.splitext(os.path.basename(path))[0] + '_spectro.png'
    nfft = optval('--nfft', 512)
    hop = optval('--hop', 128)
    zoom = optval('--zoom', None)   # None = vue pleine 0..250 km/h
    hp = optfloat('--hp', HP_KMH)   # passe-haut (km/h), 0 = off

    if path.lower().endswith('.wav'):
        rows = load_wav(path)
    else:
        rows = load_csv(path)
    rows = rows[:2] if len(rows) > 1 else rows * 2

    fig, axes = plt.subplots(2, 1, figsize=(9.9, 7.7), dpi=100)
    for ax, x, lab in zip(axes, rows, ('R1', 'R2')):
        panneau(ax, x, nfft, hop, lab, zoom, hp)
    axes[1].set_xlabel('s')
    stem = os.path.splitext(os.path.basename(path))[0]
    ref = parse_name(path)
    title = (f"{stem}  (ref: {ref['vball']} km/h, {ref['backspin']} rpm, carry {ref['carry']} m)"
             if ref else stem)
    fig.suptitle(title)
    fig.tight_layout()
    fig.savefig(out)
    mode = 'vue pleine 0-250 km/h' if zoom is None else f'zoom ±{zoom} Hz autour de vBall'
    hpstr = f'passe-haut {hp:.0f} km/h' if hp > 0 else 'pas de passe-haut'
    print(f'OK -> {out}  (nfft={nfft}, hop={hop}, {mode}, {hpstr})')
    if '--open' in opts:
        os.system(f'open "{out}"' if sys.platform == 'darwin' else f'xdg-open "{out}"')
    return 0


if __name__ == '__main__':
    sys.exit(main())
