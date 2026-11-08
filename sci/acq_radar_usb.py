#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Acquisition USB des 2 radars Doppler (R1/R2) via carte son USB (ex. Arduino Nano ->
entree ligne d'une carte son USB) connectee au MacBook. Remplace l'acquisition PC Windows.

Le montage envoie le signal Doppler audio des 2 radars sur les 2 canaux de la carte son :
  canal 0 (gauche) = R1,  canal 1 (droite) = R2,  Fs = 44100 Hz.

Usage :
  python3 acq_radar_usb.py --list                       # lister les entrees audio (brancher la carte USB)
  python3 acq_radar_usb.py --record 5 --out ma_capture   # enregistrer 5 s -> ma_capture.wav + .csv
  python3 acq_radar_usb.py --monitor                    # acquisition continue, detection d'impact,
                                                         # sauvegarde auto d'une fenetre autour de l'impact
  python3 acq_radar_usb.py --selftest                    # test hors hardware : signal synthetique ->
                                                         # wav/csv -> pipe_validation (valide toute la chaine)

Le CSV produit est au format 2 lignes separees par ';' (R1 puis R2), compatible
avec pipe_validation.py (load_csv). Nommez le fichier avec la convention
  <date>-F7-<carry>m-<vball>kmh-<backspin>trm-<sidespin>trm-cs<club>.csv
pour que pipe_validation retrouve les references dans le nom (ex. --ref "102m-151kmh-4000trm-700trm-cs115").

Dependances : pip install sounddevice numpy   (et brew install portaudio sur macOS)
"""
import argparse, os, sys, time, wave, datetime
import numpy as np

FS = 44100
KV = 19.49  # Hz / (km/h)

# ---------------------------------------------------------------- device
def list_devices():
    import sounddevice as sd
    print("Entrees audio detectees (branchez la carte son USB pour la voir) :")
    for i, d in enumerate(sd.query_devices()):
        if d['max_input_channels'] > 0:
            mark = " <- stereo (carte USB ?)" if d['max_input_channels'] >= 2 else ""
            defaut = " (defaut)" if i == sd.default.device[0] else ""
            print(f"  [{i}] {d['name']}: {d['max_input_channels']} ch in, Fs={d['default_samplerate']:.0f} Hz{defaut}{mark}")

def pick_device(prefer=None):
    import sounddevice as sd
    if prefer is not None:
        return int(prefer)
    for i, d in enumerate(sd.query_devices()):
        if d['max_input_channels'] >= 2:   # la carte son USB = 2 canaux (R1/R2)
            return i
    return sd.default.device[0]

# ---------------------------------------------------------------- record
def record(duration, device, fs=FS, channels=2):
    import sounddevice as sd
    name = sd.query_devices(device)['name']
    ch = min(channels, int(sd.query_devices(device)['max_input_channels']))
    if ch < channels:
        print(f"  note: device [{device}] n'a que {ch} canal(aux), record en {ch} ch")
    print(f"Record {duration} s @ {fs} Hz, {ch} ch, device [{device}] {name} ...")
    data = sd.rec(int(duration * fs), samplerate=fs, channels=ch, device=device, dtype='float32')
    sd.wait()
    if data.ndim == 1:
        data = data[:, None]
    return np.asarray(data)  # (N, channels)

def record_stream(seconds, device, fs=FS):
    """Enregistrement par blocs (monitoring) : yield des blocs (N,2)."""
    import sounddevice as sd
    with sd.InputStream(samplerate=fs, channels=2, device=device, dtype='float32') as st:
        remaining = int(seconds * fs)
        while remaining > 0:
            n = min(4410, remaining)  # blocs de 100 ms
            blk = st.read(n)[0]
            yield blk
            remaining -= n

# ---------------------------------------------------------------- save WAV + CSV
def save_wav_csv(data, base, fs=FS):
    x = np.clip(np.asarray(data, dtype=np.float64), -1.0, 1.0)
    wav, csv = base + '.wav', base + '.csv'
    with wave.open(wav, 'wb') as w:
        w.setnchannels(x.shape[1] if x.ndim > 1 else 1)
        w.setsampwidth(2)
        w.setframerate(fs)
        w.writeframes((x * 32767).astype('<i2').tobytes())
    with open(csv, 'w', encoding='utf-8') as f:
        for ch in range(x.shape[1] if x.ndim > 1 else 1):
            col = x[:, ch] if x.ndim > 1 else x
            f.write(';'.join(f"{v:.6f}" for v in col) + '\n')
    return wav, csv

def default_name(ref=None):
    ts = datetime.datetime.now().strftime('%Y-%m-%d-%H%M%S')
    if ref:
        return f"{ts}-F7-{ref}"
    return f"{ts}-acq"

# ---------------------------------------------------------------- detection impact
def frame_powers(x, fs=FS, nfft=2048, hop=1024):
    w = np.hanning(nfft)
    n = 1 + max(0, (len(x) - nfft) // hop)
    P = np.zeros(n)
    for i in range(n):
        seg = x[i * hop:i * hop + nfft]
        P[i] = (np.abs(np.fft.rfft(seg * w)) ** 2).sum()
    return P, hop

def detect_impact(data, fs=FS, nfft=2048, hop=1024, seuil_rel=0.5):
    """t0 = trame de max energie (pic club). Retourne index echantillon."""
    mono = data.mean(axis=1) if data.ndim > 1 else data
    P, hop = frame_powers(mono, fs, nfft, hop)
    i0 = int(np.argmax(P))
    return i0 * hop, P

# ---------------------------------------------------------------- monitoring temps reel
def monitor(device, outdir, fs=FS, win_avant=0.05, win_apres=0.40, seuil=3.0):
    """Acquisition continue par blocs de 100 ms. Quand l'energie d'une trame
    depasse seuil * median(recent), on sauvegarde [t0-avant, t0+apres] en wav/csv."""
    import sounddevice as sd
    os.makedirs(outdir, exist_ok=True)
    print(f"Monitoring sur device [{device}] {sd.query_devices(device)['name']} — Ctrl-C pour arreter")
    ring = np.zeros((int(1.0 * fs), 2))   # anneau 1 s
    pos = 0
    recent = []
    n = 0
    try:
        for blk in record_stream(3600, device, fs):  # 1 h max, arret au Ctrl-C
            ring[pos:pos + len(blk)] = blk
            pos = (pos + len(blk)) % len(ring)
            mono = blk.mean(axis=1)
            P, hop = frame_powers(mono, fs, nfft=1024, hop=512)
            e = P.max()
            recent.append(e)
            if len(recent) > 50:
                recent.pop(0)
            med = np.median(recent) if recent else 0.0
            n += 1
            if n % 10 == 0:
                # fréquence pic du dernier bloc
                w = np.hanning(1024)
                Sp = np.abs(np.fft.rfft(mono[-1024:] * w)) ** 2
                fpic = np.argmax(Sp) * fs / 1024
                print(f"  t={n*0.1:6.1f}s  E={e:9.1f}  med={med:9.1f}  fpic={fpic:6.0f} Hz ({fpic/KV:5.1f} km/h)", flush=True)
            if med > 0 and e > seuil * med and e > 1e3:
                # impact detecte : reconstruire la fenetre autour du max
                t0 = (pos - len(blk)) % len(ring)
                i0 = (t0 + int(np.argmax(P) * 512)) % len(ring)
                a = (i0 - int(win_avant * fs)) % len(ring)
                b = (i0 + int(win_apres * fs)) % len(ring)
                seg = np.concatenate([ring[a:], ring[:b]]) if a < b else np.concatenate([ring[a:], ring[:b]])
                base = os.path.join(outdir, default_name(args.ref))
                w_, c_ = save_wav_csv(seg, base, fs)
                print(f"  >>> IMPACT t={n*0.1:.1f}s  E={e:.0f} -> sauvegarde {w_} / {c_}", flush=True)
                recent.clear()
    except KeyboardInterrupt:
        print("\nArret monitoring.")

# ---------------------------------------------------------------- selftest (hors hardware)
def selftest(outdir, fs=FS):
    """Genere une captation MODELE via Python/gen_capture_modele.py (swing complet + vol +
    micro-Doppler patch, 2 canaux R1/R2), la convertit en WAV + CSV, puis lance pipe_validation."""
    os.makedirs(outdir, exist_ok=True)
    here = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, os.path.join(here, '..', 'Python'))
    sys.path.insert(0, here)
    import gen_capture_modele as gm
    csv = os.path.join(outdir, 'SELFTEST-F7-102m-151kmh-4000trm-700trm-cs115.csv')
    # meme config que la captation modele 2025-11-08 (F7, 151 km/h, spin 4000, side 700, cs 115)
    gm.gen_capture(csv, 151.0, 16.5, 3.0, 4000.0, 700.0, 115.0,
                   k_courbe=np.radians(-6.0) / (0.25 ** 2) / 2)
    # CSV -> matrice -> WAV (teste le chemin de sauvegarde audio)
    rows = [np.array([float(v) for v in l.split(';') if v.strip()]) for l in open(csv, encoding='utf-8') if l.strip()]
    data = np.column_stack(rows[:2])
    wav, csv2 = save_wav_csv(data, csv[:-4], fs)
    print(f"selftest: {csv} + {wav}")
    # valide via pipe_validation
    import pipe_validation as pv
    rows = pv.load_csv(csv2)
    res = [pv.estimate_channel(r) for r in rows[:2]]
    v, spin, conf = pv.aggregate(res)
    al = pv.compute_alpha(res[0], res[1])
    print(f"pipe_validation sur le selftest : vBall={v:.1f} km/h (ref 151), "
          f"spin={spin:.0f} rpm (ref 4000), conf={conf}")
    if al:
        print(f"  alpha0={al['alpha0']:+.2f} deg, dAlpha={al['dalpha_bf']:+.2f} deg")
    return csv2

# ---------------------------------------------------------------- main
if __name__ == '__main__':
    ap = argparse.ArgumentParser(description="Acquisition USB 2 radars Doppler (carte son USB) -> WAV/CSV")
    ap.add_argument('--list', action='store_true', help='lister les entrees audio')
    ap.add_argument('--device', type=int, default=None, help='index du device (voir --list)')
    ap.add_argument('--record', type=float, default=None, metavar='SEC', help='duree d enregistrement (s)')
    ap.add_argument('--monitor', action='store_true', help='monitoring continu avec detection d impact')
    ap.add_argument('--selftest', action='store_true', help='test hors hardware (signal synthetique + pipe_validation)')
    ap.add_argument('--out', default=None, help='prefixe de sortie (defaut: date-heure)')
    ap.add_argument('--outdir', default='.', help='dossier de sortie')
    ap.add_argument('--ref', default=None, help='refs pour le nom: "102m-151kmh-4000trm-700trm-cs115"')
    ap.add_argument('--fs', type=int, default=FS, help='frequence d echantillonnage (defaut 44100)')
    args = ap.parse_args()

    if args.list:
        list_devices()
        sys.exit(0)
    if args.selftest:
        selftest(args.outdir, args.fs)
        sys.exit(0)

    dev = pick_device(args.device)
    base = args.out or os.path.join(args.outdir, default_name(args.ref))
    if args.monitor:
        monitor(dev, args.outdir, args.fs)
    elif args.record:
        data = record(args.record, dev, args.fs)
        wav, csv = save_wav_csv(data, base, args.fs)
        print(f"sauvegarde: {wav} / {csv}")
        i0, P = detect_impact(data, args.fs)
        print(f"impact detecte a t={i0/args.fs:.3f} s")
        print("pipe de validation : python3 pipe_validation.py " + os.path.dirname(csv))
    else:
        ap.print_help()
