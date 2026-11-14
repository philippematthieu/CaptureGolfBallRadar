# Documentation technique

Langue : **FR** · [EN](README_EN.md)

> Ce document synthétise les **hypothèses, modèles, essais, erreurs rencontrées**, résultats expérimentaux, conclusions et travaux restants autour du launch monitor golf par radar Doppler 10,525 GHz (acquisition audio 44,1 kHz, balle patchée ruban aluminium, traitement fréquentiel Scilab, modèle de vol 3D).

## Documents techniques fondamentaux

- **[Golf Radar.md](Golf%20Radar.md)** — document de référence : hypothèses, modèles physiques (Doppler, micro-Doppler du patch, spin, Magnus), essais et erreurs rencontrées, résultats sur les captations réelles, limites et travaux restants. La règle centrale : distinguer ce qui est **mesuré**, **estimé**, **déduit**, et **non identifié** de façon robuste.
- **[Cpp/README.md](../Cpp/README.md)** — documentation du simulateur C++ (architecture, analyse radar multi-voies, workflow mesure ↔ modèle inverse, genèse et validation chiffrée).
- **[sci/](../sci/)** — scripts Scilab historiques (`Golfball.sci`, `FFT_Mat.sce`, `testsFFT.sci`, `comet3d2.sci`…) et captations radar réelles (`sci/*.csv`, `*.wav`).
- **[tex/](../tex/)** — documents LaTeX (`VolBalleGolf.tex` : théorie du vol de balle de golf).
- **[BRANCHES.md](BRANCHES.md)** — plan de branches et jalons du dépôt : `main`, tags de release (v1.x) et de jalons, et la branche d'exploration préservée `hauts-spins` (impasse documentée, ne pas merger).
