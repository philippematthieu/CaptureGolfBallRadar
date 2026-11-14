# Technical documentation

Language: **EN** · [FR](README.md)

> This document synthesizes the **hypotheses, models, trials, errors encountered**, experimental results, conclusions and remaining work around the golf launch monitor based on 10.525 GHz Doppler radars (44.1 kHz audio acquisition, aluminium-tape patched golf ball, Scilab frequency processing, 3D flight model).

## Fundamental technical documents

- **[Golf Radar_EN.md](Golf%20Radar_EN.md)** — reference document: hypotheses, physical models (Doppler, patch micro-Doppler, spin, Magnus), trials and errors encountered, results on real captures, limits and remaining work. The central rule: distinguish what is **measured**, **estimated**, **deduced**, and **not robustly identified**.
- **[Cpp/README_EN.md](../Cpp/README_EN.md)** — documentation of the C++ simulator (architecture, multi-channel radar analysis, measurement ↔ inverse-model workflow, genesis and quantified validation).
- **[sci/](../sci/)** — historical Scilab scripts (`Golfball.sci`, `FFT_Mat.sce`, `testsFFT.sci`, `comet3d2.sci`…) and real radar captures (`sci/*.csv`, `*.wav`).
- **[tex/](../tex/)** — LaTeX documents (`VolBalleGolf.tex`: golf ball flight theory).
