# Simulateur de vol de balle de golf — C++ (`Cpp/src/`)

Simulation complète : vol (traînée de White, Magnus/Smits & Smith, gravité) + 3 rebonds (Penner/Roh & Lee) + roulement, avec sorties texte, image 2D (SVG / PNG-CImg) et **scène 3D interactive HTML** (three.js, rotation souris).

## Compilation (Mac / Linux, g++ ou clang++)

```bash
cd Cpp/src

# version de base (console + SVG + HTML 3D, aucune dépendance)
g++ -O2 -std=c++11 -o simu_headless \
    Simu_headless.cpp Ball.cpp Club.cpp EquationODE.cpp EquationODEFlight.cpp \
    EquationODERoll.cpp EquationODEEventFlight.cpp EquationODEEventRoll.cpp SolverODE.cpp

# version avec image PNG via CImg (headless : -Dcimg_display=0 ; PNG si libpng, sinon BMP)
# optionnel : brew install libpng   (pour le PNG),  brew install --cask xquartz (pour display_graph en fenêtre)
g++ -O2 -std=c++11 -Dcimg_display=0 -DUSE_CIMG -I. -o simu_headless_cimg \
    Simu_headless.cpp Ball.cpp Club.cpp EquationODE.cpp EquationODEFlight.cpp \
    EquationODERoll.cpp EquationODEEventFlight.cpp EquationODEEventRoll.cpp SolverODE.cpp
```

L'ancien `Simu.cpp` (avec `CImg::display_graph` en fenêtre) se compile sur PC/avec XQuartz : remplacer `Simu_headless.cpp` par `Simu.cpp` et ajouter `-Dcimg_display=1` + les libs d'affichage.

## Commandes

```bash
# simulation "club" : Type ClubV0(km/h) AlphaClubPath(deg) GamaFacePath(deg) ShaftLean(deg) [spinScale]
./simu_headless F7 122.3 0 0 -6.5          # fer 7 à 122,3 km/h
./simu_headless Dr 170.6 -8 2 6            # driver 170,6 km/h
./simu_headless F7 122.3 0 0 -6.5 0        # spin nul (test Magnus : apex baisse)

# depuis une captation radar (CSV 2 lignes ';' ou WAV stéréo 44,1 kHz)
./simu_headless --capture ../../sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.wav
./simu_headless --capture ../../sci/2025-11-08-F7-131m-162kmh-9418trm-7102trm-cs124.csv F7
```

Clubs disponibles : `Dr B5 H3 F5 F6 F7 F8 F9 PW AW SW LW Pt`.

## Options

| Argument | Défaut | Rôle |
|---|---|---|
| `Type` | — | club (voir liste) |
| `ClubV0` | config club | vitesse club en km/h |
| `AlphaClubPath` | 0 | direction du chemin de club (deg) → azimut |
| `GamaFacePath` | 0 | angle face/trajectoire (deg) → side spin |
| `ShaftLeanImp` | config | shaft lean à l'impact (deg) → loft dynamique |
| `spinScale` | 1.0 | échelle du spin (0 = pas de spin) |
| `--capture f.csv\|f.wav [club] [spinScale]` | — | mesure vBall/spin/α₀ sur la captation (Capture.h : FFT maison) puis simule |

## Sorties produites (dans `Cpp/src/`)

- console : `RESULT portee=... m  apex=... m  z_final=... m  temps=... s`
- `trajectoire_<club>.svg` — image 2D vectorielle (profil + dessus) → `open ...svg`
- `trajectoire_<club>.png` / `.bmp` — image 2D raster (CImg, compile `-DUSE_CIMG`) → `open ...png`
- `trajectoire_3d.html` — **scène 3D interactive** (three.js embarqué dans `lib/`) → `open trajectoire_3d.html` ; souris : glisser = tourner, molette = zoom ; slider = vitesse de l'animation ; projection de la trajectoire au sol en pointillés
- en mode `--capture` : `trajectoire_capture.svg`, `trajectoire_capture_3d.html`, etc.

## Fichiers clés

- `Ball.{h,cpp}` modèle balle + rebonds/roulement, `Club.{h,cpp}` club, `EquationODE*.{h,cpp}` EDO vol/roulement/événements, `SolverODE.{h,cpp}` RK4 + zero-crossing
- `Simu_headless.cpp` main console, `Capture.h` lecture CSV/WAV + estimation radar, `PlotSVG.h`/`PlotCImg.h`/`Plot3D.h` sorties graphiques, `lib/` three.js local
