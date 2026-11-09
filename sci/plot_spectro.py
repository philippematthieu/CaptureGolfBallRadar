#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Affiche / sauvegarde la densité spectrale d'une captation radar (CSV 2 lignes ';' ou WAV),
style des sci/img/spectro_F7-*.png : 2 panneaux R1/R2, axe Y en km/h (f/KV, limité à 350 km/h),
axe X en s, colormap 'hot', ridge balle cyan (post-impact), impact rouge.
L'axe Y est zoomé à ±512 Hz autour de la vitesse estimée vBall (filtrage centré sur vBall :
rejette l'énergie basse fréquence ~50 km/h du club avant impact).

Usage :
    python3 plot_spectro.py <capture.csv|wav> [sortie.png] [--nfft 512] [--hop 128] [--open]
Exemples :
    python3 plot_spectro.py 2019-02-24-F7-137m-166kmh-4990trm-1312trm-cs118.csv spectro.png --open
    python3 plot_spectro.py ../sci/2025-11-08-F7-131m-162kmh-9418trm-7102trm-cs124.csv out.png --nfft 512 --hop 128
"""
import os, sys
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from pipe_validation import FS, KV, spectrogram, load_csv, estimate_channel


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


VMAX_KMH = 350.0   # une balle de golf ne dépasse pas ~350 km/h : on coupe l'axe Y
ZOOM_HZ = 512.0    # zoom Y ±512 Hz centré sur la vitesse estimée vBall (filtrage)

def panneau(ax, x, nfft, hop, label):
    t, ridge, power, f, specs = spectrogram(x, nfft, hop)
    S = np.array(specs).T                    # [nbin, nframe] pour imshow
    i0 = int(np.argmax(power))
    # vitesse estimée (comme pipe_validation) -> zoom centré dessus, rejette les faux pics ~50 km/h
    try:
        vball = float(estimate_channel(x)['vball'])
    except Exception:
        vball = float(np.median(ridge[i0:][power[i0:] > 0.03 * power[i0]])) / KV
    fest = vball * KV
    y0 = max(f[0] / KV, (fest - ZOOM_HZ) / KV)
    y1 = min(VMAX_KMH, (fest + ZOOM_HZ) / KV)
    db = 10 * np.log10(S / S.max() + 1e-12)
    ax.imshow(db, origin='lower', aspect='auto', cmap='hot', vmin=-80, vmax=0,
              extent=[t[0], t[-1], f[0] / KV, f[-1] / KV])
    post = (power[i0:] > 0.03 * power[i0]) & (np.abs(ridge[i0:] - fest) <= ZOOM_HZ)
    ax.plot(t[i0:][post], ridge[i0:][post] / KV, '.c', ms=3, label='ridge balle')
    ax.axvline(t[i0], color='r', lw=1)
    ax.text(t[i0] + 0.002, y1 - 0.08 * (y1 - y0), f'impact t={t[i0]:.3f}s', color='r', fontsize=8)
    ax.set_ylabel('km/h')
    ax.set_title(f'{label}  (vBall={vball:.1f} km/h)', fontsize=9, loc='left')
    ax.set_xlim(t[0], t[-1]); ax.set_ylim(y0, y1)   # zoom ±512 Hz autour de vBall
    return t, ridge, power, f, i0


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    opts = set(a for a in sys.argv[1:] if a.startswith('--'))
    def optval(name, defv):
        return int(sys.argv[sys.argv.index(name) + 1]) if name in sys.argv else defv
    if not args:
        print(__doc__); return 1
    path = args[0]
    out = args[1] if len(args) > 1 else os.path.splitext(os.path.basename(path))[0] + '_spectro.png'
    nfft = optval('--nfft', 2048)
    hop = optval('--hop', nfft // 2)

    if path.lower().endswith('.wav'):
        rows = load_wav(path)
    else:
        rows = load_csv(path)
    rows = rows[:2] if len(rows) > 1 else rows * 2

    fig, axes = plt.subplots(2, 1, figsize=(9.9, 7.7), dpi=100)
    for ax, x, lab in zip(axes, rows, ('R1', 'R2')):
        panneau(ax, x, nfft, hop, lab)
    axes[1].set_xlabel('s')
    fig.suptitle(f'Densité spectrale radar — {os.path.basename(path)}')
    fig.tight_layout()
    fig.savefig(out)
    print(f'OK -> {out}  (nfft={nfft}, hop={hop})')
    if '--open' in opts:
        os.system(f'open "{out}"' if sys.platform == 'darwin' else f'xdg-open "{out}"')
    return 0


if __name__ == '__main__':
    sys.exit(main())
