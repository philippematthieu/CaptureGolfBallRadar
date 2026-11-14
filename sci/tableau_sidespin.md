# Tableau SideSpin — point délicat (identification des captations CSV)

Généré par `Python/pipe_validation.py` (mesures radar : α₀, Δα) + `Python/GolfBall.py` (Δα_sim avec β_ref).
Données : `sci/2019-02-24-F7-*.csv` (références dans le nom de fichier).

| coup | ssRef (trm) | bsRef (trm) | β_ref | α₀ mes (°) | Δα mes (°) | signe mes | Δα_sim(β_ref) (°) | signe sim | verdict |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 113m-156-11624-999-cs136 | 999 | 11624 | +4,9 | −3,07 | +0,00 | 0 | −0,65 | − | Δα≈0 non résolu |
| 123m-150-5274-210-cs111 | 210 | 5274 | +2,3 | −0,63 | −0,38 | − | −0,09 | − | KO vs ssRef |
| 124m-156-5948-597-cs119 | 597 | 5948 | +5,7 | −0,17 | −0,45 | − | −0,27 | − | KO vs ssRef |
| 128m-151-3978-630-cs104 | 630 | 3978 | +9,0 | −0,63 | −0,38 | − | −0,22 | − | KO vs ssRef |
| 131m-162-6185-7102-cs124 | 7102 | 6185 | +48,9 | −1,13 | +0,00 | 0 | −3,92 | − | Δα≈0 non résolu |
| 133m-160-5432-311-cs117 | 311 | 5432 | +3,3 | −0,91 | +0,00 | 0 | −0,13 | − | non résolu |
| 134m-151-3037-393-cs99 | 393 | 3037 | +7,4 | −1,92 | +0,00 | 0 | −0,12 | − | non résolu |
| 134m-159-3648-1926-cs107 | 1926 | 3648 | +27,8 | −0,24 | −0,43 | − | −0,69 | − | KO vs ssRef |
| 137m-166-4990-1312-cs118 | 1312 | 4990 | +14,7 | −3,07 | +0,00 | 0 | −0,55 | − | non résolu |

**Concordance signe(Δα_mes) = signe(ssRef) : 0/9 coups.**

## Lecture

1. **Le modèle répond correctement au SideSpin** : |Δα_sim| croît avec β_ref (−0,09° à β=2,3° → −3,92° à β=48,9°). La chaîne Magnus est monotone et cohérente.
2. **Radar et modèle cohérents en signe** : là où Δα est résolu (4 coups), signe mesuré = signe simulé.
3. **Le signe de ssRef (2ᵉ `trm` du nom) est opposé** → ce sont des valeurs absolues dont le signe n'est pas confirmé (doc §35).
4. **Fenêtres courtes** (~150 ms de vol utile) : Δα_mes ≈ 0 sur 5/9 coups → courbure « non résolue » (doc §24/H4 ; effet Magnus ∝ t², doc §36).

## Conclusion

Le SideSpin n'est **pas identifiable en signe** avec ce dataset (noms non signés + fenêtre courte).
Le modèle prédit correctement la courbure. Pour lever le doute : dataset signé gauche/droite (priorité 6) + durée d'observation portée à 300 ms (effet Magnus ×25).

## Bloc LaTeX (booktabs)

```latex
\begin{table}[htb]\centering
\caption{SideSpin : référence (nom CSV) vs mesures radar ($\alpha_0$, $\Delta\alpha$) vs modèle ($\Delta\alpha_{\rm sim}(\beta_{\rm ref})$).}
\label{tab:sidespin}
\begin{tabular}{lrrrrrrrrr}
\toprule
Coup & ss$_{\rm ref}$ & bs$_{\rm ref}$ & $\beta_{\rm ref}$ & $\alpha_0$ & $\Delta\alpha_{\rm mes}$ & sgn & $\Delta\alpha_{\rm sim}$ & sgn$_{\rm sim}$ & verdict \\
 & rpm & rpm & ° & ° & ° & & ° & & \\
\midrule
113m-156-11624-999 & 999 & 11624 & +4,9 & -3,07 & +0,00 & 0 & -0,65 & - & non résolu \\
123m-150-5274-210 & 210 & 5274 & +2,3 & -0,63 & -0,38 & - & -0,09 & - & KO vs ssRef \\
124m-156-5948-597 & 597 & 5948 & +5,7 & -0,17 & -0,45 & - & -0,27 & - & KO vs ssRef \\
128m-151-3978-630 & 630 & 3978 & +9,0 & -0,63 & -0,38 & - & -0,22 & - & KO vs ssRef \\
131m-162-6185-7102 & 7102 & 6185 & +48,9 & -1,13 & +0,00 & 0 & -3,92 & - & non résolu \\
133m-160-5432-311 & 311 & 5432 & +3,3 & -0,91 & +0,00 & 0 & -0,13 & - & non résolu \\
134m-151-3037-393 & 393 & 3037 & +7,4 & -1,92 & +0,00 & 0 & -0,12 & - & non résolu \\
134m-159-3648-1926 & 1926 & 3648 & +27,8 & -0,24 & -0,43 & - & -0,69 & - & KO vs ssRef \\
137m-166-4990-1312 & 1312 & 4990 & +14,7 & -3,07 & +0,00 & 0 & -0,55 & - & non résolu \\
\bottomrule
\end{tabular}
\end{table}
```
