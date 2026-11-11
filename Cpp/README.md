# Simulateur de vol de balle de golf — C++ (`Cpp/src/`)

Simulation complète : vol (traînée de White, Magnus/Smits & Smith, gravité) + 3 rebonds (Penner/Roh & Lee) + roulement, avec sorties texte, image 2D (SVG / PNG-CImg), **scène 3D interactive HTML** (three.js, rotation souris) et **analyse radar avancée** des captations (vitesse club, spin multi-voies, décomposition back/side spin).

## 📑 Sommaire

- [Compilation](#compilation-mac--linux-g-ou-clang)
- [Commandes](#commandes)
- [Options](#options)
- [Analyse radar d'une captation — ce qui est estimé et comment](#analyse-radar-dune-captation---capture--ce-qui-est-estimé-et-comment)
  - [Workflow détaillé : du spin total au back/side spin](#workflow-détaillé--du-spin-total-au-backside-spin-contraindre-par-la-courbure)
    - [Étape 1 — Captation et mesures radar](#étape-1--captation-et-mesures-radar-captureh-estimate)
    - [Étape 2 — Simulations multiples (balayage GamaFacePath)](#étape-2--simulations-multiples-avec-le-modèle-balayage-de-gamafacepath)
    - [Étape 3 — Corrélation avec les mesures radar](#étape-3--corrélation-avec-les-mesures-radar)
    - [Étape 4 — Simulation finale et génération HTML/JS](#étape-4--simulation-finale-et-génération-du-htmljs-plot3dh)
- [Visualisation HTML 3D](#visualisation-html-3d-trajectoire_3dhtml--trajectoire_capture_3dhtml)
- [`spectres_html` — les 8 panneaux](#spectres_html--les-8-panneaux-de-spectres-dune-captation)
- [PNG de la densité spectrale](#png-de-la-densité-spectrale-dune-captation)
- [Sorties produites](#sorties-produites-dans-cppsrc)
- [Fichiers clés](#fichiers-clés)
- [Genèse — discussions, théorie, étapes](#genèse--discussions-théorie-et-étapes-qui-ont-conduit-à-ce-code)
  - [0. Méthode de travail — le processus créatif à deux](#0-méthode-de-travail--le-processus-créatif-à-deux)
  - [1. Point de départ : le radar voit plus que la balle](#1-point-de-départ--le-radar-voit-plus-que-la-balle)
  - [2. Théorie du signal réfléchi par une balle en rotation](#2-théorie-du-signal-réfléchi-par-une-balle-en-rotation)
  - [3. Du spin total au back/side spin](#3-du-spin-total-au-backside-spin--contraindre-par-la-courbure)
  - [4. De la mesure à la visualisation](#4-de-la-mesure-à-la-visualisation)
  - [5. Validation : tableau de cohérence sur les 12 captations](#5-validation--tableau-de-cohérence-sur-les-12-captations)
  - [6. Pièges rencontrés (et corrigés)](#6-pièges-rencontrés-et-corrigés)
- [Tests](#tests)

## Compilation (Mac / Linux, g++ ou clang++)

```bash
cd Cpp/src

# version de base (console + SVG + HTML 3D, aucune dépendance)
g++ -O2 -std=c++11 -o simu_headless \
    Simu_headless.cpp Ball.cpp Club.cpp EquationODE.cpp EquationODEFlight.cpp \
    EquationODERoll.cpp EquationODEEventFlight.cpp EquationODEEventRoll.cpp SolverODE.cpp

# outil de visualisation multi-spectres d'une captation (HTML autonome, pur canvas 2D)
g++ -O2 -std=c++11 -I. -o spectres_html spectres_html.cpp

# PNG de densité spectrale (voir plus bas)
g++ -O2 -std=c++11 -I. -o spectro_png spectro_png.cpp

# version avec image PNG via CImg (headless : -Dcimg_display=0 ; PNG si libpng, sinon BMP)
g++ -O2 -std=c++11 -Dcimg_display=0 -DUSE_CIMG -I. -o simu_headless_cimg \
    Simu_headless.cpp Ball.cpp Club.cpp EquationODE.cpp EquationODEFlight.cpp \
    EquationODERoll.cpp EquationODEEventFlight.cpp EquationODEEventRoll.cpp SolverODE.cpp
```

L'ancien `Simu.cpp` (avec `CImg::display_graph` en fenêtre) se compile sur PC/avec XQuartz : remplacer `Simu_headless.cpp` par `Simu.cpp` et ajouter `-Dcimg_display=1` + les libs d'affichage.

## Commandes

```bash
./simu_headless --help        # aide complète : usage, options, sorties, exemples (en couleur)

# simulation "club" : Type ClubV0(km/h) AlphaClubPath(deg) GamaFacePath(deg) ShaftLean(deg) [spinScale]
./simu_headless F7 122.3 0 0 -6.5 --open     # fer 7 à 122,3 km/h, ouvre le HTML 3D
./simu_headless Dr 170.6 -8 2 6              # driver
./simu_headless F7 122.3 0 0 -6.5 0          # spin nul (test Magnus)

# depuis une captation radar (CSV 2 lignes ';' ou WAV stéréo 44,1 kHz) — analyse fusion par défaut
./simu_headless --capture ../../sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.wav --open
./simu_headless --capture ../../sci/2025-11-08-F7-131m-162kmh-9418trm-7102trm-cs124.csv F7 1.0
./simu_headless --capture <fichier> --mode standard     # ancien : patch Hilbert seul

# spectres multi-panneaux d'une captation (8 panneaux, voir plus bas)
./spectres_html ../../sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.csv -o
for f in ../../sci/*.csv; do ./spectres_html "$f"; done   # tous les spectres + index
```

Clubs disponibles : `Dr B5 H3 F5 F6 F7 F8 F9 PW AW SW LW Pt`. `-o` / `--open` ouvre le HTML généré automatiquement (`open` sur Mac).

## Options

| Argument | Défaut | Rôle |
|---|---|---|
| `Type` | — | club (voir liste) |
| `ClubV0` | config club | vitesse club en km/h |
| `AlphaClubPath` | 0 | direction du chemin de club (deg) → azimut |
| `GamaFacePath` | 0 | angle face/trajectoire (deg) → side spin |
| `ShaftLeanImp` | config | shaft lean à l'impact (deg) → loft dynamique |
| `spinScale` | 1.0 | échelle du spin (0 = pas de spin) |
| `--capture f.csv\|f.wav [club] [spinScale]` | — | analyse la captation radar puis simule depuis les mesures |
| `--mode fusion\|standard` | `fusion` | analyse du spin : 4 voies fusionnées par confiance (défaut) ou patch seul |
| `--open` / `-o` | non | ouvre automatiquement le HTML 3D |

## Analyse radar d'une captation (`--capture`) — ce qui est estimé et comment

Toutes les briques sont dans `Capture.h` (header-only, FFT radix-2 maison) :

| Mesure | Méthode | Fonction |
|---|---|---|
| **vBall** | ridge STFT (2048/1024) post-impact, médiane 50 ms, 2 canaux, correction cos20°. **2 passes** : 1ʳᵉ pleine bande 1000–6000 Hz, puis 2ᵉ restreinte à [0,85·f_D, 1,10·f_D] autour de la vitesse à priori — l'écho club (plus basse fréquence, très puissant, il décélère après l'impact) n'accroche plus le ridge des 1ʳᵉʳᵉˢ frames post-impact | `estimate()` |
| **t0 (impact)** | frame d'énergie max | `estimate()` |
| **V club** | **passe-bande FFT autour de vBall/1.35** sur la fenêtre [t0−0,3 s, t0] (le lobe Doppler de la tête de club est dans le signal avant l'impact), crête au quantile 90 % | `estimate_club_speed()` |
| **smash** | vBall / Vclub mesuré | (dans `Simu_headless.cpp`) |
| **spin — 4 voies** | ① patch : segment 250 ms post-impact **filtré passe-bande autour de f_D = vBall·KV ± 1 kHz (vitesse à priori du ridge → rejette l'écho de la tête de club)**, puis enveloppe de Hilbert, spectre 50–200 Hz, score d'harmoniques · ② **phase** : signal analytique du lobe balle filtré autour de f_D, fréquence instantanée detrendée, pic à f_rot · ③ **peigne** : autocorrélation du spectre Welch autour de f_D ± 2 kHz (lag = f_rot) · ④ **excursion β** : excursion crête-crête de la fréquence instantanée lissée (300 ms post-impact, detrend linéaire), `spin = Δf/(2·KS)` avec β = 4πr/λ. Chaque voie sort aussi sa **confiance** = pic / fond (médiane spectrale ; pour l'excursion : Δf / (1,4826·MAD)) | `estimate()` (`spin_patch_conf`), `estimate_spin_phase_spec()` (params `exc_rpm`, `exc_conf`, `conf`), `estimate_spin_peigne_ac()` (param `conf`) |
| **spin retenu** | `fusion_spin()` **pondérée par confiance** : référence = voie valide (rpm>0, conf>0) de plus grande confiance ; moyenne des voies cohérentes à ±15 % de la référence, pondérées par leur confiance ; seule la référence sinon. La confiance de la voie excursion est rabattue ×0,25 (biais systématique connu : β effectif < 4πr/λ sur balle réelle). Confiance exposée dans `ESTIM` (`spin_*_conf=`) | `fusion_spin()` (8 args) |
| **α₀, Δα** | appariement stéréo R1/R2 (Δf ≤ 30 Hz), α = atan((2L/B)·fd/fc), médianes début/fin | `estimate()` |
| **back/side spin** | modèle inverse : balayage de `GamaFacePath` (−15°..+15°) avec `runSimu`, courbure azimutale sur ~150 ms post-impact ajustée au **Δα stéréo**, vecteur spin rescalé au spin total radar (donc `hypot(back,side) = spin_radar`) | `Simu_headless.cpp` |
| **launch angle ψ** | **fit balistique du ridge post-impact** : signal analytique passe-bande (f_D ± 1 kHz) sur [t0, t0+0,45 s], fréquence instantanée lissée sur ~1,5 période de rotation (moyenne la modulation micro-Doppler ±β·f_rot), agrégée en 12 bins sur [0, 0,35 s] (bins de bord exclus) → v_r(t) mesurée. Le radar mesure v_r ≈ c·V(t) (facteur d'échelle c libre, THETA_RADAR quasi constant) : on intègre le vol 2D (traînée de White, gravité, Magnus comme `flight_ode`) pour chaque ψ ∈ [4°, 26°] pas 0,25°, V0 = v_r(t₀), et on retient ψ* = argmin SSE avec c = least-squares ; `launch_conf = 1 − SSE/SST` | `estimate()` (`launch_deg`, `launch_conf`) |

Le mode `--capture` enchaîne : mesures radar → V club (passe-bande, sinon modèle vBall/1.35) → spin fusion → ajustement GamaFacePath par courbure → `Ball::runSimu()` → HTML 3D + SVG. Le shaft lean reste au cas idéal du club (non identifiable au radar).

### Workflow détaillé : du spin total au back/side spin (contraindre par la courbure)

```
┌───────────────────────────┐   ┌────────────────────────────────┐   ┌─────────────────────────────┐   ┌──────────────────────────┐
│ 1. CAPTATION RADAR        │ → │ 2. SIMULATIONS MULTIPLES       │ → │ 3. CORRÉLATION MESURES      │ → │ 4. GÉNÉRATION HTML/JS    │
│  (Capture.h)              │   │  (modèle inverse, Ball)        │   │  RADAR vs SIMU              │   │  (Plot3D.h)              │
└───────────────────────────┘   └────────────────────────────────┘   └─────────────────────────────┘   └──────────────────────────┘
```

#### Étape 1 — Captation et mesures radar (`Capture.h`, `estimate()`)

- **Lecture** : CSV 2 lignes `;` ou WAV stéréo 44,1 kHz → canaux R1/R2 (`load_capture`).
- **vBall** : STFT (NFFT 2048, hop 1024), pic dans la bande 1000–6000 Hz, interpolation parabolique sous-bin, médiane du ridge sur 50 ms post-impact, rejet inter-radars > 15 %, correction cos 20° → `vball_kmh`, `vball_corr_kmh`. **2ᵉ passe à bande restreinte** [0,85·f_D, 1,10·f_D] : le fort écho club (fréquence plus basse) décélère après l'impact et masquait le début du vol de la balle sur le ridge pleine bande ; en bande restreinte le ridge accroche la balle dès l'impact (vBall raffiné + appariement stéréo utilisent cette passe).
- **t0 (impact)** : frame d'énergie max du spectrogramme.
- **V club** : le lobe Doppler de la tête de club existe avant t0 → passe-bande FFT autour de `vBall/1.35` sur `[t0−0,3 s, t0]`, crête au quantile 90 % (`estimate_club_speed`) → `vclub_radar`, d'où **smash = vBall/vClub** (fallback modèle vBall/1.35 si NaN).
- **Spin total — 4 voies** (mode `fusion` par défaut) :
  1. *patch* : segment 250 ms post-impact **filtré passe-bande autour de f_D = vBall·KV ± 1 kHz** (vitesse à priori du ridge — rejette l'écho de la tête de club et le bruit hors lobe, comme le passe-haut 50 km/h du spectrogramme d'affichage mais centré sur le lobe balle), puis enveloppe de Hilbert, spectre 50–200 Hz, score d'harmoniques (évite f_rot/2) ;
  2. *phase* : lobe balle filtré passe-bande autour de f_D, signal analytique, fréquence instantanée `dφ/dt` detrendée (moyenne glissante 0,1 s), pic à f_rot (`estimate_spin_phase_spec`) ;
  3. *peigne* : spectre Welch (NFFT 8192) autour de f_D ± 2 kHz, detrend glissant, autocorrélation normalisée en lag, score de fondamental (`estimate_spin_peigne_ac`) ;
  4. *excursion β* : fi lissée (2 ms) sur 300 ms post-impact, detrend **linéaire**, percentiles 2,5/97,5 → `spin = Δf/(2·KS)`.
  - Chaque voie a une **confiance** (pic/médiane du spectre ; excursion : Δf/(1,4826·MAD)) exposée dans `ESTIM` (`spin_patch_conf`, `spin_phase_conf`, `spin_peigne_conf`, `spin_exc_conf`).
  - `fusion_spin()` **pondérée par confiance** : référence = voie de plus grande confiance, moyenne pondérée des voies à ±15 % d'elle ; repli sur la 1ʳᵉ voie rpm>0 si aucune confiance. Confiance excursion rabattue ×0,25 (biais β systématique).
- **α₀ et Δα** : appariement stéréo — pour chaque frame post-impact, pic de R2 dans ±300 Hz autour de f1(R1), cohérence |f1−f2| ≤ 30 Hz, `α = atan((2L/B)·fd/fc)` avec L(t) évolutif ; α₀ = médiane des 50 ms initiales, Δα = médiane(fin 100 ms) − médiane(début) = **courbure de trajectoire mesurée**.

#### Étape 2 — Simulations multiples avec le modèle (balayage de `GamaFacePath`)

Le radar donne le spin **total** (norme) mais pas sa direction. Le modèle `Ball` lie `GamaFacePath` (angle face/trajectoire) au vecteur spin `(spinY, spinZ)` :

- pour chaque `g` de −15° à +15° (pas 0,5°) :
  - `Club` avec `ClubV0 = vclub`, `AlphaClubPath = α₀`, `GamaFacePath = g` ;
  - `Ball probe(...)` → `spinY(g)`, `spinZ(g)` ;
  - `probe.rescaleSpin(spin_radar / hypot(spinY, spinZ))` → spin total calé sur la mesure radar ;
  - `probe.runSimu()` → trajectoire `mf` (vol + rebonds + roulement, RK4 dt = 0,01 s) ;
  - **courbure azimutale simulée** : `da = atan2(vy,vx)` à la frame ~150 ms post-impact (même fenêtre que la mesure stéréo) moins la frame initiale.
- on retient `g*` qui minimise `err = |da(g) − Δα_radar|`.

#### Étape 3 — Corrélation avec les mesures radar

- **Vecteur spin retenu** : `(spinY(g*), spinZ(g*))` rescalé au spin total mesuré → `backspin_est = spinY·k`, `sidespin_est = spinZ·k` avec `k = spin_radar / hypot(spinY,spinZ)`, donc toujours `hypot(back, side) = spin_radar` (backspin ≤ spin total — cohérence garantie).
- **Cohérences vérifiées** sur les 12 captures : smash 1,18–1,40 stable ; side spin détecté quand Δα ≠ 0 (ex. cs104 : −246 rpm vs réf −630) ; signe à surveiller (convention Δα stéréo vs repère simu).
- **Limites** : shaft lean non identifiable au radar (cas idéal du club retenu) ; le launch angle ψ est maintenant mesuré par fit du ridge post-impact (étape 1, `launch_deg`/`launch_conf`) et peut contraindre le loft dynamique.

#### Étape 4 — Simulation finale et génération du HTML/JS (`Plot3D.h`)

- `Ball theBall(...)` avec le club ajusté (`vclub`, `α₀`, `g*`) + `rescaleSpin(spinScale·spin_radar/hypot(...))` → `runSimu()` → `mf` (matrice `[x, vx, y, vy, z, vz]`).
- `write_traj_3d_html("trajectoire_capture_3d.html", mf, titre, inf, specJson)` génère un HTML autonome :
  - **panneau infos** (gauche, 250 px) : toutes les mesures/estimations (vBall, V club radar/modèle, smash, spin total/phase/peigne, back/side estimés, GamaFacePath, α₀, Δα, t0, portée, hauteur max, z final) ;
  - **scène 3D three.js** (centre) : trajectoire, balle animée (slider vitesse), OrbitControls, sol **carré** (côté = max(1,1×portée, 50 m)), grille ~10 m, projection au sol en pointillés, apex rouge ;
  - **spectrogramme docké en bas** (`specJson` = `spec_to_json(e)`) : `left:250px; right:0`, redimensionnable (drag), zoom interactif (drag = zone, molette = zoom au curseur, dblclick/clic droit = reset), colormap viridis, ridge cyan, impact pointillé, axes km/h et s.
- `write_traj_svg("trajectoire_capture.svg", ...)` pour la vue 2D.

Fichiers impliqués : `Simu_headless.cpp` (orchestration `--capture`), `Capture.h` (mesures), `Ball/Club/EquationODE/SolverODE` (modèle), `Plot3D.h` (HTML 3D + spectrogramme), `PlotSVG.h` (SVG).

## Visualisation HTML 3D (`trajectoire_3d.html` / `trajectoire_capture_3d.html`)

Layout HMI produit par `Plot3D.h` :

- **panneau infos fixe à gauche** (250 px) : club, V club (radar/modèle), smash, vBall, spins (total/phase/peigne), back/side estimés, GamaFacePath estimé, α₀, Δα, t0, portée, hauteur max, z final
- **scène 3D three.js au centre** : souris = tourner/zoom, slider = vitesse de l'animation de la balle, projection au sol en pointillés, apex en rouge ; **fairway toujours carré** : côté = max(1,1 × portée, 50 m), grille ~10 m
- **spectrogramme docké en bas** : collé au panneau gauche et au bord droit (`left:250px; right:0`), **redimensionnable** (barre de drag en haut, double-clic = reset), **zoom interactif** : drag = zoom sur une zone, molette = zoom au curseur, double-clic / clic droit = reset ; ridge cyan, impact pointillé, axes km/h et temps

## `spectres_html` — les 8 panneaux de spectres d'une captation

HTML autonome (pur canvas 2D, aucune lib), produit `spectres_<nom>.html` :

1. spectrogramme complet R1 (passe-haut 50 km/h, 0..250 km/h, ridge, impact)
2. zoom lobe balle : passe-bande f_D ± 2,5 kHz post-impact (raies micro-Doppler)
3. spectre 1D moyen post-impact autour de f_D — peigne de raies, marqueurs f_D ± n·f_rot (f_rot retenu par fusion)
4. lobe club : fenêtre [t0−0,35 s, t0], passe-bande vClub₀ ± 15 %
5. patch : enveloppe de Hilbert 250 ms + spectre 40..220 Hz (marqueur f_rot patch)
6. autocorrélation du spectre Welch autour de f_D (sortie de `estimate_spin_peigne_ac`)
7. spectre de la fréquence instantanée du lobe balle 40..200 Hz (sortie de `estimate_spin_phase_spec`)
8. barre d'infos : vBall, f_D, patch/phase/peigne/**fusion**, vClub, t0

Les panneaux 3, 6, 7 utilisent directement les analyseurs de `Capture.h` (pas de recalcul parallèle).

## PNG de la densité spectrale d'une captation

Outil séparé `spectro_png` (réutilise Capture.h + BmpImg.h, **zéro dépendance**) — style spectro_F7-*.png : **640×500**, 2 panneaux R1/R2, zoom Y ±512 Hz autour de vBall post-impact, axe X = temps (s), axe Y = km/h (f/19.49, limité à 350 km/h), colormap `hot`, ridge cyan, impact rouge. STFT 512 (Hann 11,6 ms), hop 128. Sortie PNG (writer maison, zlib stored) ou BMP 24 bits.

```bash
./spectro_png ../../sci/2019-02-24-F7-137m-166kmh-4990trm-1312trm-cs118.csv spectro_137.png -o
```

## Sorties produites (dans `Cpp/src/`)

- console : `RESULT portee=... m  apex=... m  z_final=... m  temps=... s`
- console (mode `--capture`) : ligne machine **`ESTIM key=value ...`** (une seule ligne, `nan` pour les non-détectés) : `vball_kmh vball_corr_kmh spin_rpm spin_patch_rpm spin_phase_rpm spin_peigne_rpm spin_exc_rpm spin_patch_conf spin_phase_conf spin_peigne_conf spin_exc_conf vclub_radar_kmh vclub_kmh smash alpha0_deg dalpha_deg t0_s launch_deg launch_conf gfp_est_deg backspin_est_rpm sidespin_est_rpm da_est_deg portee_m apex_m z_final_m temps_s` — parsée par `Python/GolfBall.py` (`analyze_capture_cpp`) pour rejouer la simu Python avec le side spin estimé et le launch angle mesuré (`--launch` sinon)
- `trajectoire_<club>.svg` — image 2D vectorielle (profil + dessus)
- `trajectoire_<club>.png` / `.bmp` — image 2D raster (CImg, compile `-DUSE_CIMG`)
- `trajectoire_3d.html` / `trajectoire_capture_3d.html` — scène 3D interactive + panneau infos + spectrogramme docké/zoomable
- `spectres_*.html` + `spectres_index.html` — panneaux de spectres (outil `spectres_html`)

## Fichiers clés

- `Ball.{h,cpp}` modèle balle + rebonds/roulement, `Club.{h,cpp}` club, `EquationODE*.{h,cpp}` EDO, `SolverODE.{h,cpp}` RK4 + zero-crossing
- `Simu_headless.cpp` main console (simu + mode `--capture` + génération HTML)
- `Capture.h` lecture CSV/WAV, FFT, filtres passe-haut/passe-bande, estimation radar (vBall, V club, spin 3 voies, α₀/Δα, spectrogramme)
- `PlotSVG.h` / `PlotCImg.h` / `Plot3D.h` sorties graphiques (SVG, PNG, HTML 3D), `spectres_html.cpp` visualisation multi-spectres, `lib/` three.js local

---

## Genèse — discussions, théorie et étapes qui ont conduit à ce code

Cette section retrace la démarche : les idées de départ, la théorie physique sous-jacente, les itérations de test, et comment chaque idée a été consolidée en code.

### 0. Méthode de travail — le processus créatif à deux

Ce projet avance par une **collaboration homme–machine** où chacun a un rôle complémentaire, et dont les forces se cumulent :

- **L'utilisateur (Philippe) — l'expert métier, la source des idées et des « tricks ».** C'est lui qui connaît le radar 10,525 GHz et le golf, et qui *voit* les spectrogrammes. C'est lui qui repère ce que le code ne « voit » pas : l'écho de la tête de club, très puissant et plus bas en fréquence, qui masque le début du vol de la balle (→ passe-bande autour de f_D sur la voie patch, puis 2ᵉ passe du ridge à bande restreinte [0,85·f_D, 1,10·f_D], puis ridge affiché restreint dès l'impact) ; le biais systématique de la voie excursion β sur balle réelle (→ rabais ×0,25 de sa confiance dans la fusion) ; la fusion pondérée par confiance ; le schéma vertical avec étiquettes de données sur les flèches… Il oriente, valide, demande les reverts, et fixe les priorités.
- **L'assistant (Mistral Vibe, agent CLI de codage) — l'exécutant rapide et systématique.** Il code vite et sans fatigue, écrit les harnais de validation (`tests/test_synth_capture.py` sur captations synthétiques de vérité connue, `sci/validate_all.py` sur les 12 vraies captations, comparées aux valeurs de référence des noms de fichiers), mesure l'avant/après **chiffré** de chaque idée, documente (ce README, le schéma Mermaid) et commit/push chaque amélioration individuellement. Mais seul, il **loupe des tricks** : il ne lit pas un spectrogramme comme l'utilisateur, et l'idée de rejeter l'écho club par une bande restreinte autour de f_D ne lui serait pas venue spontanément.
- **Le processus, itération par itération :** idée de l'utilisateur → implémentation par l'assistant → validation chiffrée (tests synthétiques + tableau des 12 captations) → revue des régressions (ex. cs111/cs124-2025 s'étaient dégradés quand la voie excursion prenait la main comme référence → d'où le rabais ×0,25) → commit + push. **Les chiffres tranchent** : au fil de ces itérations, vBall est passé de 8,1 % à **6,2 %** d'erreur moyenne, le spin de 44,4 % à **30,0 %**, la portée Python de 12,6 % à **11,6 %**.

### 1. Point de départ : le radar voit plus que la balle

L'estimation initiale (`estimate()`) ne lisait que le lobe balle post-impact (vBall, spin par patch, α₀/Δα stéréo). L'idée novatrice : **le signal contient aussi le lobe de la tête de club avant l'impact** (saut de vitesse caractéristique), donc la vitesse du club y est mesurable. Consolidé en `estimate_club_speed()` : passe-bande FFT autour de vBall/1.35 sur [t0−0,3 s, t0], crête au quantile 90 % → **V club mesurée** (ex. 117 km/h sur cs111), d'où le **smash mesuré** = vBall/Vclub ≈ 1,2–1,4, très stable sur les 12 captures. C'est ce smash qui, via le modèle inverse, contraint ensuite la décomposition du spin.

### 2. Théorie du signal réfléchi par une balle en rotation

Une balle de diamètre d = 42,67 mm à 10,525 GHz (λ = 28,5 mm) tournant à ω module la phase de l'écho : `s(t) = cos(2π f_D t + β sin(2π f_rot t))` avec **β = 4πr/λ ≈ 0,036**. Par les fonctions de Bessel, le spectre est un **peigne de raies** à `f_D ± n·f_rot` (f_rot = spin/60, excursion ±KS·rpm avec KS = 0,1567 Hz/rpm). Deux conséquences exploitées :

- le **patch** (modulation d'amplitude vue par l'enveloppe de Hilbert) bat à f_rot → spin (voie ①) ;
- les **raies latérales** autour de f_D portent le même f_rot (voie ③, peigne) ;
- la **fréquence instantanée** du signal analytique du lobe balle porte `β f_rot cos(2π f_rot t)` après detrend → pic à f_rot (voie ②, la plus robuste quand le patch échoue).

### 3. Du spin total au back/side spin : contraindre par la courbure

**Le problème.** Le radar ne donne que la norme du spin (`spin_radar`), pas sa direction dans le plan (back vs side). Or la direction de l'axe de spin, c'est exactement ce qui fait courber la trajectoire (slice/draw). Plusieurs pistes discutées :

- **smash factor** : avec V club mesurée (étape 1) et vBall, le smash = vBall/vClub ≈ 1,2–1,4 est stable, mais il ne dit rien directement sur l'angle face/chemin — un smash plus faible peut venir d'un mauvais centrage, pas forcément d'un angle de face. Il contraint le modèle (via `ClubV0`) mais ne décompose pas le spin.
- **shaft lean** : non identifiable au radar (aucune signature Doppler propre) → on garde le cas idéal du club, noté comme limite.
- **launch angle** : mesuré (point 3) — fit balistique du ridge post-impact (fréquence instantanée du lobe balle lissée, vol 2D traînée+gravité+Magnus, ψ = argmin SSE à échelle libre près) : ±1° sur captations synthétiques (16,5° → 17,5°, 16,0° → 17,0°) ; sur vraies captures le fit est plus bruité (confiance `launch_conf` exposée, parfois en butée de grille). Il contraint désormais le loft dynamique (shaft lean) via le modèle inverse.
- **courbure de trajectoire Δα** (mesurée stéréo, étape 1) : dépend directement du side spin via la force de Magnus latérale → c'est la contrainte retenue.

**Première version (modèle inverse par balayage).** Pour chaque `GamaFacePath` g ∈ [−15°, +15°] (pas 0,5°), on construit un `Ball` probe avec `ClubV0 = vclub`, `AlphaClubPath = α₀`, on rescale son spin à `spin_radar`, on lance `runSimu()`, et on mesure la courbure azimutale simulée `da = atan2(vy,vx)` sur la même fenêtre (~150 ms post-impact) que la mesure stéréo. On retient `g* = argmin |da(g) − Δα|`. (Variante "descente de gradient / ODE" envisagée puis écartée : le balayage 1D de 61 simulations est déterministe, ~5 s, et suffit ici.)

**Itération 1 — cohérence du vecteur spin.** Le premier jet affichait `backspin_est = spinY(g*)` *avant* rescale : sur cs104, backspin = 6381 rpm > spin total radar = 4927 rpm — absurde. Correction : le balayage ne sert qu'à trouver la **direction** du vecteur spin ; on rescale ensuite au total mesuré (`k = spin_radar / hypot(spinY,spinZ)`), d'où `backspin_est = spinY·k`, `sidespin_est = spinZ·k` et toujours `hypot(back, side) = spin_radar`. Corrigé dans le code et validé (cs104 : 4920/−246 → hypot = 4927 ✓).

**Itération 2 — signe de la courbure (testé puis reverti).** En comparant aux side spin de référence des noms de fichiers (toutes négatives, format `trm-…trm`), le signe semblant inversé entre Δα stéréo et le repère de la simu. Une version avec `err = |da + Δα|` et courbure mesurée jusqu'à la chute donnait de meilleurs minimums intérieurs (cs119 : g=+15° → −1,5°, side +700 → −73 rpm). Mais visuellement la trajectoire était déjà correcte (léger slice) → **revert** à la version `err = |da − Δα|`, fenêtre 150 ms, à la demande de l'utilisateur (commit revert). La question du signe reste ouverte (convention de repère stéréo vs simu à clarifier proprement).

**Résultat retenu.** Side spin estimé quand Δα ≠ 0 (cs104 : −246 rpm vs réf −630 ; cs124-2025 : +1241 vs +7102) ; quand Δα ≈ 0 (bruit < 0,3°) le balayage donne g ≈ 0. La trajectoire 3D résultante courbe du bon côté (vérifié visuellement sur cs119). Limites actées : amplitude du side limitée par le spin total radar (souvent sous-estimé), shaft lean au cas idéal ; launch angle désormais mesuré par fit du ridge (point 3, ±1° en synthétique).

**Schéma du workflow et des itérations (fonctions utilisées) :**

Légende : **bleu = partie réelle** (captation radar → mesures), **vert = partie simulée** (modèle inverse), **jaune = corrélation mesure ↔ simu**, **orange = itérations testées**, **rouge = testé puis reverti**, **vert foncé = version retenue**. **Flèches** : l'étiquette `-->|"donnée"|` = la donnée transportée sur la connexion (qui entre / qui sort de chaque nœud).

```mermaid
flowchart TD
    subgraph REAL["📡 PARTIE RÉELLE — captation radar → mesures"]
        direction TB
        R["Radar 10,525 GHz<br/>2 canaux R1 / R2"] -->|"écho radar brut"| W["Fichier WAV stéréo 44,1 kHz<br/>ou CSV 2 lignes ';'"]
        W -->|"fichier"| LC["load_capture()<br/>→ signaux bruts R1, R2"]
        LC -->|"samples R1, R2"| HP["FILTRAGE passe-haut ~50 km/h<br/>(élimine le bruit basse fréquence)"]
        HP -->|"signaux filtrés"| EST["estimate() — STFT 2048/1024, 2 passes<br/>1ʳᵉ pleine bande 1000–6000 Hz<br/>2ᵉ restreinte [0,85·f_D, 1,10·f_D] : rejette l'écho club<br/>→ vBall raffiné (±6 %), t0, R1 ∥ R2 : appariement stéréo<br/>→ α₀, Δα (courbure mesurée)"]
        EST -->|"vBall, t0"| VCLUB["estimate_club_speed()<br/>passe-bande autour de vBall/1.35, sur [t0−0,3 s, t0]<br/>→ V club mesurée, smash"]
        EST -->|"signal post-impact, f_D, t0"| S1 & S2 & S3 & S4
        S1["① voie patch — identification par les ALVÉOLES (AM)<br/>passe-bande f_D ± 1 kHz (rejette l'écho club)<br/>puis enveloppe de Hilbert 250 ms<br/>spectre 50–200 Hz, score d'harmoniques"]
        S2["② voie phase — démodulation FRÉQUENCE/PHASE<br/>estimate_spin_phase_spec()<br/>fi du signal analytique detrendée → pic f_rot"]
        S3["③ voie peigne — recherche de PÉRIODICITÉ micro-Doppler<br/>(PAS l'écho radar / vitesse) : autocorr. du spectre<br/>autour de f_D ± 2 kHz → lag = f_rot"]
        S4["④ voie excursion β — amplitude de l'excursion micro-Doppler<br/>fi lissée 300 ms, detrend linéaire → Δf/(2·KS)<br/>β SUPPOSÉ = 4πr/λ → norme du spin<br/>(l'angle/direction du vecteur spin = balayage SIMU ci-dessous)"]
        S1 & S2 & S3 & S4 -->|"rpm + confiance (pic/fond)"| FUS["fusion_spin() — 4 voies EN PARALLÈLE<br/>pondérée par confiance (conf excursion ×0,25 : biais β)<br/>réf = voie la + confiante, moyenne des voies à ±15 %<br/>→ spin_radar"]
        EST -->|"signal post-impact, f_D, t0, spin"| LAUNCH["⑤ launch angle ψ — fit balistique du ridge<br/>fi du signal analytique (passe-bande f_D ± 1 kHz)<br/>lissée ~1,5 T_rot, 12 bins sur 0,35 s → v_r(t)<br/>vol 2D (White + g + Magnus, comme flight_ode)<br/>ψ ∈ [4°,26°] pas 0,25°, c = least-squares<br/>ψ* = argmin SSE → launch_deg, launch_conf = 1 − SSE/SST"]
    end

    subgraph SIMU["💻 PARTIE SIMULÉE — modèle inverse + itérations"]
        direction TB
        SWEEP["🔁 BOUCLE de balayage<br/>GamaFacePath g ∈ [−15°, +15°], pas 0,5°<br/>(61 itérations en série)"]
        SWEEP -->|"g"| PROBE["Ball probe(ClubV0=vclub, α₀, g)<br/>rescaleSpin(spin_radar / hypot(spinY,spinZ))<br/>runSimu() → trajectoire mf (RK4, dt=0,01 s)"]
        PROBE -->|"mf [x,vx,y,vy,z,vz]"| DA["courbure azimutale SIMULÉE<br/>da = atan2(vy,vx) à ~150 ms post-impact"]
        DA -->|"da(g) simulé"| CORR["⚖️ CORRÉLATION mesure ↔ simu<br/>err(g) = |da_simulé − Δα_mesuré|"]
        CORR -->|"err(g)"| NEXT{"encore un g ?"}
        NEXT -->|"oui : g += 0,5°"| SWEEP
        NEXT -->|"non : les 61 err"| G["g* = argmin err(g)"]
        G --> IT1{"Itération 1 — cohérence<br/>backspin > spin_radar ?<br/>(cs104 : 6381 > 4927 → absurde)"}
        IT1 -->|"oui → k = spin_radar/hypot(spinY,spinZ)"| VEC["backspin_est = spinY·k, sidespin_est = spinZ·k<br/>hypot(back, side) = spin_radar ✓<br/>(cs104 : 4920/−246 → 4927 ✓)"]
        IT1 -->|"non"| VEC
        VEC --> IT2{"Itération 2 — signe de Δα<br/>test : err = |da + Δα|, fenêtre jusqu'à la chute<br/>(cs119 : g=+15° → −1,5°, side +700 → −73 rpm)"}
        IT2 -->|"version testée"| T["meilleurs minimums intérieurs"]
        T -->|"REVERT demandé (trajectoire déjà correcte)"| RET["✅ version retenue<br/>err = |da − Δα|, fenêtre 150 ms"]
        IT2 -->|"version initiale"| RET
        RET -->|"vclub, α₀, g*, spin_radar"| FINAL["Ball theBall(...) + rescaleSpin<br/>runSimu() → trajectoire finale mf"]
        FINAL -->|"mf + panneau d'infos + spectrogramme"| OUT["write_traj_3d_html() + spec_to_json()<br/>→ trajectoire_capture_3d.html<br/>write_traj_svg() → trajectoire_capture.svg"]
    end

    FUS -->|"spin_radar (rpm)"| SWEEP
    VCLUB -->|"vclub (km/h), smash"| SWEEP
    EST -->|"α₀, Δα (deg)"| SWEEP
    LAUNCH -->|"launch_deg = ψ mesuré (fit ridge)"| FINAL

    style REAL fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style SIMU fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style CORR fill:#fff9c4,stroke:#f9a825,stroke-width:2px
    style IT1 fill:#ffe0b2
    style IT2 fill:#ffe0b2
    style T fill:#ffcdd2
    style RET fill:#c8e6c9
```

### 4. De la mesure à la visualisation

- `Plot3D.h` : spectrogramme d'abord fixe en bas → **docké** (`left:250px; right:0`) et **redimensionnable** (drag), puis **zoom interactif** (drag = zone, molette = zoom au curseur, dblclick = reset) ; fairway passé de rectangle à **carré** (côté ≥ 1,1 × portée).
- `spectres_html.cpp` : outil de visu des 8 bandes/filtrages, qui réutilise finalement les analyseurs de `Capture.h` (pas de code dupliqué).

### 5. Validation : tableau de cohérence sur les 12 captations

Chaque capture a été relue (`spectres_*.html` + `--capture`) et comparée aux valeurs de référence du nom de fichier. Écarts moyens constatés : vBall ±8 %, smash 1,18–1,40 stable, spin total radar ±64 % (patch seul, avec 4 cas à 0) → le mode **fusion** ramène ces cas à une valeur non nulle (ex. cs118 : 0 → 5073 rpm). Bilan : V club et smash fiables ; le spin total reste le maillon faible, d'où les 4 voies. La **fusion pondérée par confiance** (point 9 : chaque voie sort pic/fond, la référence est la voie la plus confiante, moyenne pondérée des voies à ±15 % d'elle) fait passer l'erreur moyenne de spin de **44,4 % → 42,5 %** sur les 12 captations. Le **passe-bande autour de f_D appliqué à la voie patch** (vitesse à priori du ridge → rejet de l'écho club avant l'enveloppe de Hilbert) fait ensuite passer l'erreur moyenne de spin à **32,1 %** (cs99 +105 % → +4 %, cs124-2019 −41 % → −6 %). La **2ᵉ passe du ridge à bande restreinte** [0,85·f_D, 1,10·f_D] — l'écho club, très puissant et décroissant après l'impact, masquait le début du vol de la balle sur le ridge pleine bande — plus le **rabattement ×0,25 de la confiance de la voie excursion** (biais β systématique) donnent finalement **vBall 6,2 %, spin 30,0 %, portée Python 11,6 %** (cs118-2019 +1,2 %, cs119 −4,3 %, cs99 +1,0 % ; cs136 reste mauvais : spin 11624 tr/min, f_rot proche du bord de bande 200 Hz). La portée simulée colle à ±8 m quand le spin est bon.

### 6. Pièges rencontrés (et corrigés)

- backspin estimé > spin total (incohérent) → rescale du vecteur spin au total mesuré ;
- HTML à canvas blancs → apostrophe dans une chaîne JS + `FS` non défini côté JS (corrigés, `json_escape`, `var FS=44100`) ;
- peigne par autocorrélation : pic artificiel à lag~0 → normalisation par (nb−lag) + detrend glissant + score de fondamental (comme le patch) ;
- `head` en pipe tuait le process avant l'écriture du HTML (SIGPIPE) lors des tests.

## Tests

- `tests/test_synth_capture.py` — validation synthétique de bout en bout : génère des captations de vérité connue (`Python/gen_capture_modele.py`), les analyse avec `simu_headless --capture` (ligne `ESTIM`, via `GolfBall.analyze_capture_cpp`), compare vBall / spin / back-side / GamaFacePath / **launch angle (±2°)** aux vraies valeurs (tranche la convention de signe de Δα), puis rejoue la simu Python avec le side estimé. Lancer avec un Python numpy+scipy : `python3 tests/test_synth_capture.py` (code 0 si OK). Dernier run : launch 16,5° → 17,5° et 16,0° → 17,0° (conf 1,00).
- `sci/validate_all.py` — harnais de validation sur toutes les captations `sci/*-F7-*.csv` : analyse C++ (`ESTIM`) + rejoue la simu Python, compare à la référence du nom de fichier (portée, vBall, spin back/side) → table console + `sci/validation.html` coloré. `.venv/bin/python sci/validate_all.py` (code 1 si une captation échoue). Baseline courante (2ᵉ passe ridge à bande restreinte + passe-bande f_D sur la voie patch + fusion pondérée par confiance avec rabais excursion ×0,25 + fit launch angle du ridge post-impact) : **vBall 6,2 %, spin 30,0 %, portée Python 11,6 %** (le launch mesuré n'est pas encore réinjecté dans `validate_all.py`, qui garde 16,5° fixe).
