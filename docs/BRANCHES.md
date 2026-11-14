# Branches et jalons du dépôt

Langue : **FR** · [EN](BRANCHES_EN.md)

Ce document décrit l'organisation des branches et tags du dépôt GitHub `philippematthieu/CaptureGolfBallRadar`. Il fait partie de la trace scientifique du projet : les impasses explorées y sont conservées, documentées, et ne sont jamais fusionnées dans `main`.

## Politique

- `main` est la seule branche active ; elle est toujours synchronisée avec `origin/main` et l'arbre de travail est propre.
- On ne supprime que des **références déjà mergées** dans `main` (`git branch -d` refuse de supprimer une branche non mergée). Aucun commit n'est jamais perdu : tout reste atteignable depuis `main` ou depuis un tag.
- Les **branches d'exploration** qui documentent une inspection ou une impasse sont **préservées telles quelles** (local + origin), avec leur tag d'ancrage. Ne pas les renommer, ne pas les supprimer, ne pas merger leur code.

## Branches

| Branche | État | Rôle |
|---|---|---|
| `main` | active, = `origin/main` | code de référence (Scilab historique `sci/`, port Python `Python/`, simulateur C++ `Cpp/`) |
| `hauts-spins` | **préservée — NE PAS merger** | branche d'exploration : impasse « hauts spins » documentée (règle expérimentale peigne > 8000 rpm + « plus petit lag à 85 % », écartée après essai). Voir tag `impasse/hauts-spins`. |

## Tags

| Tag | Commit | Signification |
|---|---|---|
| `v1.0.0` | `f5b905b` | release 1.0.0 |
| `v1.1.0` | `3b4e62a` | release 1.1.0 — scène 3D végétalisée (arbres, buissons, terrain valonné) |
| `v1.2.0` | `f203858` | release 1.2.0 — documentation bilingue FR/EN, références de fichiers corrigées, rendu équations/tableaux fiabilisé |
| `baseline-estimateur-sidebackspin-satisfaisant` | `1310cc0` | jalon : baseline de validation satisfaisante (atteignable depuis `main`) |
| `launch-angle-estime-depuis-captation` | `9254f1a` | jalon : launch angle estimé depuis la captation radar (atteignable depuis `main`) |
| `impasse/hauts-spins` | `24c50ef` | ancre la branche d'exploration `hauts-spins` (hors `main`) |

## Consulter la branche d'impasse `hauts-spins`

```bash
git log main..hauts-spins --oneline      # les 3 commits uniques de l'exploration
git diff main...hauts-spins --stat       # fichiers touchés
git show hauts-spins:Cpp/src/Capture.h   # le code à l'état de l'impasse
```

Fichiers concernés par l'exploration : `Cpp/src/Capture.h`, `Cpp/src/Simu_headless.cpp`, `Cpp/README.md` (section Genèse §7 : la voie « densité lissée façon Scilab » a été retenue, la règle hauts spins écartée).

## Nettoyage effectué le 2026-11-14

Branches de feature **mergées dans `main`** puis supprimées (en local et sur `origin`) : `LaunchAngle`, `clean-archi`, `densite-spectrale-lissee-scilab`, `vegetation-scene-3d` ; la branche `modifications` (locale seule, déjà mergée) supprimée en local. `hauts-spins` et tous les tags sont intacts.
