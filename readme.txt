****************************************
********* PRINCIPE******* **************
****************************************

L'algorithme calcule le monoide de Markov associé à des matrices lettres données explicitement.

Pour cela l'algorithme calcule toutes les matrices du monoide associées,
ainsi qu'une #-expression canonique par matrice et
des règles de réécritures d'expressions qui réduisent la taille de l'expression.

Le système de réécriture est complet au sens où il permet de réécrire toute
#-expression en la #-expression canonique de la matrice sous-jacente.

****************************************
********* ALGORITHME **************
****************************************

(1) Fonction ProductClosure
File = S (ensemble des générateurs)
Soit u le premier élément dans la file
  Pour tout v dans S,
    Vérifier si uv se réduit. Il y a 4 cas, selon le premier opérateur de u et de v. Puisque u et v sont réduits, cela se simplifie à des questions de réducibilité pour une hauteur de # inférieure, qui peuvent être répondues en temps constant en lisant dans T.
    S'il ne se réduit pas, on le calcule (un produit entre u et v). On vérifie s'il apparaît déjà dans la table T. On l'ajoute dans la table T, avec le booléen correspondant. S'il n'apparaissaît pas pas dans T, on l'enfile.

(2) Fonction StabilizationClosure
Pour tout u dans S
  Calculer u^# (si applicable). On vérifie s'il apparaît déjà dans la table T. On l'ajoute dans la table T, avec le booléen correspondant.

Initialement S = {1}. On applique (1), puis (2), puis (1), etc...
Après avoir appliqué (1) et (2) k fois, on a un système de réécriture pour les #-expressions de hauteur au plus k.


****************************************
********* IMPLEMENTATION **************
****************************************

On construit incrémentalement un mapping des #-expressions
vers les matrices et réciproquement des matrices vers les #-expressions,
ainsi qu'un mapping de réécriture des #-expressions vers les #-expressions.
Des containeurs C++ "unordered_set" sont utilisés pour stocker toutes les #-expressions, matrices et vecteurs lignes et colonnes connus.
Il y a deux types de vecteurs sparse ou pas, sélectionné statiquement à la compilation.
