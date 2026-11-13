#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Harnais de validation automatique sur toutes les captations sci/*-F7-*.csv (point 4).

Pour chaque captation :
  - référence lue dans le NOM de fichier (parse_name, avec le signe du side spin) :
    carry (m), vball (km/h), backspin (rpm), sidespin (rpm) ;
  - analyse C++ complète via GolfBall.analyze_capture_cpp (simu_headless --capture,
    ligne ESTIM : vBall, spin 3 voies fusionnées, V club, smash, alpha0/dAlpha,
    GamaFacePath balayé, back/side estimés, portée simulée) ;
  - rejoue la simu Python (GolfBall.simulate_golf_ball) avec les estimés
    (launch = launch_deg mesuré sur le ridge par le C++ si conf >= 0,5 ou
    --caseangle_deg renseigné, sinon repli 16.5°) -> portée Python.

Sortie : table console + sci/validation.html (cellules colorees selon l'erreur).
Le nom de fichier n'est utilisé QUE pour la comparaison, jamais dans les calculs.

Usage : .venv/bin/python sci/validate_all.py [--out sci/validation.html]
Code de sortie : 1 si une captation échoue, 0 sinon.
"""
import argparse
import glob
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(ROOT, 'Python'))
sys.path.insert(0, HERE)

import numpy as np  # noqa: E402
import GolfBall as G  # noqa: E402
import pipe_validation as pv  # noqa: E402

LAUNCH_DEG = 16.5  # repli si le launch mesuré est absent ou peu confiant (conf < 0,5 sans --caseangle_deg)


def launch_from_est(est, case_angle=None):
    """Launch retenu pour la simu Python : launch_deg mesuré (ESTIM) si conf >= 0,5
    ou case angle renseigné (géométrie connue), sinon repli 16.5°. Retourne (deg, tag)."""
    ld = float(est.get('launch_deg', 0.0) or 0.0)
    lc = float(est.get('launch_conf', 0.0) or 0.0)
    ca = float(case_angle) if case_angle is not None else 0.0   # seulement le case angle CLI compte pour le repli
    if ld > 0 and (lc >= 0.5 or ca != 0.0):
        return ld, f'{ld:.1f}(c{lc:.2f})'
    return LAUNCH_DEG, f'{LAUNCH_DEG:.1f}*repli'


def parse_ref(fn):
    """Référence du nom de fichier, AVEC le signe du side : -123m-150kmh-5274trm-210trm-cs111.csv"""
    base = os.path.basename(fn)
    m = re.search(r'-F7-(\d+)m-(\d+)kmh-(\d+)trm([+-])(\d+)trm-?cs-?(\d+)\.csv$', base)
    if not m:
        return None
    carry, vball, back, sign, side, cs = m.groups()
    return dict(carry=int(carry), vball=int(vball), backspin=int(back),
                sidespin=int(sign + side), cs=int(cs))


def pct(est, ref):
    if ref == 0:
        return 0.0 if est == 0 else float('inf')
    return 100.0 * (est - ref) / abs(ref)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out', default=os.path.join(HERE, 'validation.html'))
    ap.add_argument('--glob', default='*-F7-*.csv')
    ap.add_argument('--caseangle_deg', type=float, default=None,
                    help='inclinaison de la valise radar vers le haut (ex: 11), passée au C++ (--caseangle_deg)')
    args = ap.parse_args()

    files = sorted(glob.glob(os.path.join(HERE, args.glob)))
    print(f'{len(files)} captations trouvées dans {HERE}\n')
    hdr = (f"{'fichier':<46} {'vRef':>4} {'vEst':>6} {'err%':>6} {'smash':>6} | "
           f"{'spinRef':>7} {'spinEst':>7} {'err%':>6} | {'backEst':>7} {'sideEst':>7} {'sideRef':>7} | "
           f"{'g*':>5} {'dA':>6} {'psy':>9} | {'carRef':>6} {'carC++':>6} {'carPy':>6} {'err%':>6}")
    print(hdr)
    print('-' * len(hdr))

    rows = []
    n_fail = 0
    for fn in files:
        ref = parse_ref(fn)
        name = os.path.basename(fn)
        if ref is None:
            print(f'{name:<46}  nom non reconnu')
            continue
        try:
            est = G.analyze_capture_cpp(fn, club='F7', case_angle=args.caseangle_deg)
            v_est = est['vball_corr_kmh']          # corrigée cos20° (+ case angle si --caseangle_deg)
            spin_est = est['spin_rpm']
            back_est = est.get('backspin_est_rpm', float('nan'))
            side_est = est.get('sidespin_est_rpm', float('nan'))
            gfp = est.get('gfp_est_deg', 0.0)
            dalpha = est.get('dalpha_deg', 0.0)
            smash = est.get('smash', float('nan'))
            car_cpp = est.get('portee_m', float('nan'))
            launch_used, launch_tag = launch_from_est(est, args.caseangle_deg)
            # rejoue en Python avec les estimés (launch mesuré si fiable, sinon repli)
            t, y, b = G.simulate_golf_ball(v0_ms=v_est / 3.6, launch_deg=launch_used,
                                           azim_deg=est.get('alpha0_deg', 0.0),
                                           spin_rpm=spin_est,
                                           side_rpm=0.0 if side_est != side_est else side_est)
            car_py = y[-1, 0]
            z_py = y[-1, 2]
        except Exception as ex:
            n_fail += 1
            print(f'{name:<46}  ERREUR: {ex}')
            rows.append(dict(name=name, ref=ref, err=True))
            continue
        ev, es, ec = pct(v_est, ref['vball']), pct(spin_est, ref['backspin']), pct(car_py, ref['carry'])
        print(f"{name:<46} {ref['vball']:>4} {v_est:>6.1f} {ev:>+6.1f} {smash:>6.3f} | "
              f"{ref['backspin']:>7} {spin_est:>7.0f} {es:>+6.1f} | {back_est:>7.0f} {side_est:>+7.0f} {ref['sidespin']:>+7} | "
              f"{gfp:>+5.1f} {dalpha:>+6.2f} {launch_tag:>9} | {ref['carry']:>6} {car_cpp:>6.1f} {car_py:>6.1f} {ec:>+6.1f}")
        rows.append(dict(name=name, ref=ref, est=est, v_est=v_est, spin_est=spin_est,
                         back_est=back_est, side_est=side_est, gfp=gfp, dalpha=dalpha,
                         smash=smash, car_cpp=car_cpp, car_py=car_py, z_py=z_py,
                         launch_used=launch_used, launch_tag=launch_tag,
                         ev=ev, es=es, ec=ec))

    ok = [r for r in rows if not r.get('err')]
    if ok:
        mv = np.mean([abs(r['ev']) for r in ok])
        ms = np.mean([abs(r['es']) for r in ok])
        mc = np.mean([abs(r['ec']) for r in ok])
        print(f'\nmoyennes |err| sur {len(ok)} captations : vBall {mv:.1f} %   spin {ms:.1f} %   portee(Py) {mc:.1f} %')

    # ---- rapport HTML ----
    def cell(v, err):
        if err is None or not np.isfinite(err):
            return f'<td>{v}</td>'
        c = '#c8e6c9' if abs(err) <= 10 else ('#ffe0b2' if abs(err) <= 25 else '#ffcdd2')
        return f'<td style="background:{c}">{v}<br><small>{err:+.1f}%</small></td>'

    with open(args.out, 'w') as f:
        f.write('<!DOCTYPE html><html><head><meta charset="utf-8"><title>validation captations</title>'
                '<style>body{font-family:Helvetica}table{border-collapse:collapse;font-size:12px}'
                'td,th{border:1px solid #999;padding:3px 6px;text-align:right}td:first-child,th:first-child{text-align:left}'
                '</style></head><body><h1>Validation des captations radar (simu_headless --capture, ligne ESTIM)</h1>'
                f'<p>{len(files)} fichiers, {len(ok)} analysés, {n_fail} en erreur. '
                f'launch Python = launch_deg mesuré (ESTIM) si conf &ge; 0,5 ou --caseangle_deg, sinon repli {LAUNCH_DEG}° (*). '
                f'La référence vient du nom de fichier (comparaison seule).</p>'
                '<table><tr><th>fichier</th><th>vBall ref</th><th>vBall est</th><th>smash</th>'
                '<th>spin ref</th><th>spin est</th><th>back est</th><th>side est (ref)</th><th>g*</th><th>dAlpha</th><th>launch utilisé</th>'
                '<th>carry ref</th><th>carry C++</th><th>carry Py</th><th>z final Py</th></tr>\n')
        for r in rows:
            if r.get('err'):
                f.write(f'<tr><td>{r["name"]}</td><td colspan="14" style="background:#ffcdd2">ERREUR</td></tr>\n')
                continue
            ref, est = r['ref'], r['est']
            f.write(f'<tr><td>{r["name"]}</td>'
                    f'{cell(ref["vball"], None)}{cell(round(r["v_est"], 1), r["ev"])}'
                    f'<td>{r["smash"]:.3f}</td>'
                    f'<td>{ref["backspin"]}</td>{cell(round(r["spin_est"]), r["es"])}'
                    f'<td>{r["back_est"]:.0f}</td>'
                    f'<td>{r["side_est"]:+.0f} (ref {ref["sidespin"]:+d})</td>'
                    f'<td>{r["gfp"]:+.1f}°</td><td>{r["dalpha"]:+.2f}°</td>'
                    f'<td>{r["launch_tag"]}</td>'
                    f'<td>{ref["carry"]}</td><td>{r["car_cpp"]:.1f}</td>'
                    f'{cell(round(r["car_py"], 1), r["ec"])}'
                    f'<td>{r["z_py"]:+.1f} m</td></tr>\n')
        f.write('</table></body></html>\n')
    print(f'rapport HTML écrit : {args.out}')
    return 1 if n_fail else 0


if __name__ == '__main__':
    raise SystemExit(main())
