
# Golf Ball Simulator in Scilab (Scilab, Python and cpp)

> **Abstract — principe directeur.** *Mesurer directement ce que les radars savent réellement mesurer, puis utiliser le modèle physique pour identifier les grandeurs qui ne sont pas directement observables.* Un radar Doppler 10,525 GHz mesure des fréquences (vitesses radiales, micro-Doppler du patch, courbure stéréo R1/R2) — pas des angles de club ni un vecteur spin 3D. On estime donc d'abord, aussi précisément que possible, les observables radar (vBall, V club, smash, spin total par fusion multi-voies, α₀/Δα, launch angle ψ par fit balistique) ; puis le **modèle physique** (impact club→balle + vol traînée/Magnus/gravité) sert d'inverse : balayage de `GamaFacePath` contraint par la courbure mesurée pour décomposer back/side spin, loft dynamique déduit de ψ, etc. Chaque grandeur non observable est ainsi identifiée par cohérence mesure ↔ simulation, jamais inventée. Bilan sur 12 captations réelles : vBall 6,3 %, spin 20,5 %, portée 10,4 % d'erreur moyenne (détails dans `Cpp/README.md`).

## Index

- **[Simulateur de vol de balle de golf — C++](Cpp/README.md)** — architecture, compilation, commandes, analyse radar d'une captation (vBall, V club, smash, spin 7 voies, launch angle ψ, back/side spin), sorties HTML 3D / SVG / PNG, genèse et validation chiffrée.
- **[Documentation technique — hypothèses, modèles, essais, erreurs rencontrées](docs/README.md)** — document qui synthétise les hypothèses, modèles, essais et erreurs rencontrées du projet, en mettant en évidence les documents techniques fondamentaux.

## Simulateur Scilab
<img src="sci/img/Pasted image 20250120171635.png"  width="560" height="380">



## Simulateur CPP
Application `simu_headless --capture` (scène 3D interactive + panneau mesures radar + spectrogramme docké/zoomable) :
```shell
cd Cpp/build
./simu_headless --capture ../../sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.csv F7 1.0 --caseangle_deg 11 --open
```
Le **launch angle ψ est estimé directement depuis la captation** (fit balistique du ridge post-impact : fréquence instantanée du lobe balle lissée, vol 2D traînée de White + gravité + Magnus, ψ = argmin SSE à échelle libre — ±0,75° sur captations synthétiques de vérité connue). Options : `--launch_deg 16` pour forcer ψ, `--caseangle_deg 11` si la valise incline le radar vers le haut (v_r = V·cos(ψ−ε), ε rajouté au fit — les deux options sont exclusives), `--live 2.0` pour enregistrer la carte son (radar en entrée audio, sauve `live_capture.wav` puis analyse). ψ mesuré pilote la simu quand conf ≥ 0,5 (ou `--caseangle_deg` renseigné), sinon repli loft du club.

<img src="sci/img/capture_simu_headless_20261110.png" width="980">

**Spin total : fusion de 7 voies** (détail dans `Cpp/README.md` Genèse §3/§7) : ① patch (enveloppe de Hilbert), ② phase (fréquence instantanée), ③ peigne (autocorrélation du spectre), ④ excursion β (confiance ×0,25), ⑤ sideband micro-Doppler frame par frame (interpolation parabolique, gating IC), ⑥ densité spectrale lissée « façon Scilab » (rebasée par frame, chirp-aware, conf ×0,25), ⑦ loft(ψ) (spin modèle `V0·cosψ·sin(loft_dyn)·CoeffBackSpin`, active seulement si loft_dyn ∈ 12°–35°). La référence est la voie la plus confiante, moyenne pondérée des voies à ±15 %. **Bilan 12 captations** (`Python/validate_all.py`) : vBall 6,3 %, **spin 20,5 %** (18,2 % avec `--caseangle_deg 11`), portée 10,4 %. L'animation 3D montre à l'impact une **équerre** : tube orange = plan de la face (perpendiculaire au lancement), tube rouge + flèche = normale de la face, orientée du côté de la courbure.

## Simulateur Python
Animation 3D matplotlib `Python/GolfBall.py [capture.csv|wav]` (vol + 3 rebonds + roulement ; panneau d'infos : club, V club/smash, vBall radar, **spin total fusion 7 voies** (patch/phase/peigne/exc/sideband/dens/loft affichés) + back/side estimés, **launch angle ψ mesuré sur la captation**, portée, apex, déviation latérale z final) :

```python
python Python/GolfBall.py sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.csv --club F7 --caseangle_deg 11
# options : --launch_deg 16 (ψ forcé, exclusif avec --caseangle_deg), --caseangle_deg 11 (valise inclinée vers le haut)
```

<img src="sci/img/Simulateur_Python.png" width="980">

### Spectres multi-panneaux d'une captation — `spectres_html` (C++)
Générés par `cd Cpp/build && ./spectres_html ../../sci/2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.csv` (8 panneaux : spectrogramme R1, zoom lobe balle, peigne de raies micro-Doppler, lobe club, patch Hilbert, autocorrélation du spectre, spectre de la fréquence instantanée, barre d'infos) — capture PNG pleine page ci-dessous :

<img src="sci/img/spectres_html_F7-123m-cs111.png" width="980">

[version interactive HTML](Cpp/build/spectres_2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.html) · [index de tous les spectres](Cpp/build/spectres_index.html)



## Radar setup — geometry, swing line and shot angles

<img src="sci/img/setup_radar.png" width="980">

Vue de dessus : les deux radars Doppler 10,525 GHz (baseline b = 0,30 m) visent la balle patchée (L₀ ≈ 1,50 m).
`f_C = (f₁+f₂)/2` → vitesse radiale (`V_r = f_D / 19,49` km/h) ; `f_Δ = (f₁−f₂)/2` → azimut `α(t) = arctan[(2L/b)·(f_Δ/f_C)]`.
Au shoot : l'axe de spin ω est incliné de β → backspin = ω·cos β, sidespin = ω·sin β (draw/slice, courbure Δα).

## Flight Analyzer captured by little Radars
<img src="sci/img/Pasted image 20250122082849.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122082902.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122082913.png"  width="280" height="190">
<img src="sci/img/Pasted image 20250122082924.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122082939.png"  width="280" height="190"><img src="sci/img/Pasted image 20250122082958.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122083320.png"  width="280" height="190">
<img src="sci/img/Pasted image 20250122083456.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122083851.png"  width="280" height="190">

### Densités spectrales régénérées en Python — 2 voies R1/R2, axe Y en km/h (f/19,49), ridge balle en cyan
Commande : `python3 Python/plot_spectro.py sci/<capture.csv|wav> [sortie.png] [--nfft 512 --hop 128] [--hp 50] [--zoom 256] [--open]` — vue pleine 0–250 km/h par défaut (fenêtre STFT 512 = 11,6 ms, ridge cyan bien résolu en temps, impact pointillé, passe-haut 50 km/h qui rejette le lobe du club), `--hp 0` sans filtrage, `--zoom N` pour zoomer ±N Hz autour de vBall
(ex. `sci/img/spectro_F7-*.png` ci-dessous, régénérés avec `Python/pipe_validation.py` ; équivalent C++ sans dépendance : `Cpp/src/spectro_png.cpp` (binaire `Cpp/build/spectro_png`), image `sci/img/spectro_CPP_F7-137m-*.png`)
<img src="sci/img/spectro_F7-113m-156kmh-11624trm-999trmcs-136.png"  width="300" height="236">
<img src="sci/img/spectro_F7-123m-150kmh-5274trm-210trm-cs111.png"  width="300" height="236">
<img src="sci/img/spectro_F7-124m-156kmh-5948trm-597trm-cs119.png"  width="300" height="236">
<img src="sci/img/spectro_F7-128m-151kmh-3978trm-630trm-cs104.png"  width="300" height="236">
<img src="sci/img/spectro_F7-131m-162kmh-6185trm-7102trm-cs124.png"  width="300" height="236">
<img src="sci/img/spectro_F7-133m-160kmh-5432trm-311trm-cs117.png"  width="300" height="236">
<img src="sci/img/spectro_F7-134m-151kmh-3037trm-393trm-cs99.png"  width="300" height="236">
<img src="sci/img/spectro_F7-134m-159kmh-3648trm-1926trm-cs107.png"  width="300" height="236">
<img src="sci/img/spectro_F7-137m-166kmh-4990trm-1312trm-cs118.png"  width="300" height="236">

### Référence théorique : captation modèle générée (idéale)

Les deux images ci-dessous ne sont **pas des mesures** : c'est une captation **générée** par `Python/gen_capture_modele.py`
pour être optimale en temps et en signal — c'est ce que le traitement *devrait* obtenir théoriquement.
Config : fer 7, V₀ = 151 km/h, launch = 16,5°, spin total = 4000 rpm, side spin = 700 rpm (slice droite → β = 10,1°), cs = 115 km/h.
Validation par `Python/pipe_validation.py` : spin retrouvé à −0,1 % (3996 vs 4000 rpm).

Densité spectrale de la captation modèle à swing complet (config du coup réel 137m : 166 km/h, 4990/1312 rpm, cs 118) :
<img src="sci/img/spectro_MODELE_137m-166kmh.png"  width="430" height="339">

Trajectoire modèle (vol + déviation latérale du slice) :
<img src="sci/img/traj_MODELE_2025-11-08.png"  width="480" height="267">

# Find backspin

<img src="sci/img/Pasted image 20250120172502.png"  width="560" height="380">

## Data catched with SkyTrak

Club : iron 7

Club Speed : 111kh

Ball Speed : 150kmh

Length : 123m

Backspin : 5274trm

Sidespin : 210trm

## Backspin estimated with the radar
Given d = 0.04267m

 V = F / 19,48
 
rpm = 1000*(F_sommet - F_balle)/(19.48\*60\*%pi*d)

As mesured and as seen above, we can see that the ball speed is around 160km/h and the maximum echo is 191km/h.
So that the backspin could be estimated as 5097trm.

## The HW
### The suitecase
<img src="sci/img/Pasted image 20250123184355.png"  width="380" height="560">

### Internal with Arduino, Radar, Amplifier, Audio Boards
<img src="sci/img/20250123_132345.jpg"  width="190" height="280"> <img src="sci/img/20250123_132310.jpg"  width="190" height="280"> <img src="sci/img/20250123_132304.jpg"  width="190" height="280">

### Aluminium Tape
Place aluminum tape to increase the surface area of ​​the radar echo.

<img src="sci/img/PatchedBall.png"  width="345" height="191">

### Arduino Tennsy 3.6

<img src="sci/img/IMG_2427.jpeg"  width="400" height="114">

see here : [Arduino Tennsy 3.6](https://www.pjrc.com/store/teensy36.html)

### Radar HB100

<img src="sci/img/IMG_2429.png"  width="400" height="400">

see here : [Radar HB100](https://www.otronic.nl/fr/capteur-radar-doppler-hyperfrequence.html)

### Amplification Stage Outputs

<img src="sci/img/PCB_Back_s.jpg"  width="350" height="244">

see here : [Amplification Stage Outputs](https://www.limpkin.fr/index.php?post/2013/08/09/Making-the-electronics-for-a-%247-USD-doppler-motion-sensor)

### Audio Board

<img src="sci/img/IMG_2428.jpeg"  width="485" height="364">

see here : [Audio Board](https://www.pjrc.com/store/teensy3_audio.html)

## Simples Scilab Commands
exec('GolfBall-master\sci\comet3d2.sci', -1)
exec('GolfBall-master\sci\FFT_Mat.sce', -1)
exec('GolfBall-master\sci\Golfball.sci', -1)

fichier = uigetfile("*.wav", "Golf\Radar\");[x,Fs,bits]=wavread(fichier);
[x,Fs,bits]=wavread(fichier); 

Or 

[x,Fs,bits]=wavread('2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.wav');

Then

xg = x(1,1:$);
xd = x(2,1:$);

[xgCentre, xgf] = plotFFT(xg,Fs);
[xdCentre, xdf] = plotFFT(xd,Fs);

[tt5,f5,M5]=animFFT(xgf,44100, 128 ,2 , 2,0);
[tt5,f5,M5]=animFFT(xdf,44100, 128 ,2 , 2,0);

[M5,tt5,f5]=animDensite(xgf,44100, 512 ,10 , 1, 0.0010,1);
[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,Club)

[M5,tt5,f5]=animDensite(xdf,44100, 512 ,10 , 1, 0.0010,1);
[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,Club)

[M5,tt5,f5]=animDensite3(xgf,44100, 512 ,10 , 1, 100,1);

[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info2((xgf),'7',44100)
[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info2((xdf),'7',44100)

alphaClubPath=0;
gamaFacePath=0;
[t,VOL,Res] = Golfball(18, vClub, '7', alphaClubPath,gamaFacePath,ShafLeanImp ,1);
