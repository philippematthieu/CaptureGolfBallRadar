# Repository branches and tags

Language: **EN** · [FR](BRANCHES.md)

This document describes the branch and tag layout of the GitHub repository `philippematthieu/CaptureGolfBallRadar`. It is part of the project's scientific record: explored dead ends are kept here, documented, and never merged into `main`.

## Policy

- `main` is the only active branch; it is always in sync with `origin/main` and the working tree is clean.
- Only **references already merged** into `main` are ever deleted (`git branch -d` refuses to delete an unmerged branch). No commit is ever lost: everything remains reachable from `main` or from a tag.
- **Exploration branches** documenting an investigation or a dead end are **preserved as-is** (local + origin), together with their anchor tag. Do not rename them, do not delete them, do not merge their code.

## Branches

| Branch | Status | Role |
|---|---|---|
| `main` | active, = `origin/main` | reference code (historical Scilab `sci/`, Python port `Python/`, C++ simulator `Cpp/`) |
| `hauts-spins` | **preserved — DO NOT merge** | exploration branch: documented "high-spin" dead end (experimental rule comb > 8000 rpm + "smallest lag at 85 %", rejected after testing). See tag `impasse/hauts-spins`. |

## Tags

| Tag | Commit | Meaning |
|---|---|---|
| `v1.0.0` | `f5b905b` | release 1.0.0 |
| `v1.1.0` | `3b4e62a` | release 1.1.0 — vegetated 3D scene (trees, bushes, hilly terrain) |
| `v1.2.0` | `f203858` | release 1.2.0 — bilingual FR/EN documentation, fixed file references, hardened equation/table rendering |
| `baseline-estimateur-sidebackspin-satisfaisant` | `1310cc0` | milestone: satisfactory validation baseline (reachable from `main`) |
| `launch-angle-estime-depuis-captation` | `9254f1a` | milestone: launch angle estimated from the radar capture (reachable from `main`) |
| `impasse/hauts-spins` | `24c50ef` | anchors the `hauts-spins` exploration branch (outside `main`) |

## Inspecting the `hauts-spins` dead-end branch

```bash
git log main..hauts-spins --oneline      # the 3 unique commits of the exploration
git diff main...hauts-spins --stat       # files touched
git show hauts-spins:Cpp/src/Capture.h   # the code as it stood at the dead end
```

Files involved in the exploration: `Cpp/src/Capture.h`, `Cpp/src/Simu_headless.cpp`, `Cpp/README.md` (Genesis §7: the "Scilab-style smoothed density" channel was kept, the high-spin rule was rejected).

## Cleanup performed on 2026-11-14

Feature branches **merged into `main`** then deleted (local and on `origin`): `LaunchAngle`, `clean-archi`, `densite-spectrale-lissee-scilab`, `vegetation-scene-3d`; the `modifications` branch (local only, already merged) deleted locally. `hauts-spins` and all tags are untouched.
