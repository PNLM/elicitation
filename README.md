

IHM destinée à l'élicitation de préférences.

Pour fonctionner, l'IHM a besoin de trois fichiers à placer dans le répertoire de l'exécutable :
- `data.ini` : contient les informations relatives aux Profils à comparer. Chaque ligne correspond à 1 profil Chaque caractère correspond, dans l'ordre : -La ligne de la grille où sera placé le profil -La colonne de la grille où sera placé le profil -Le numéro du profil (1 ou 2 chiffres) -Les chiffres restants correspondent à la valeur des critères présents dans criteria.ini Ainsi, par exemple, en lisant une ligne contenant 36201333 ("3 6 20 1 3 3 3"), le programme va placer un profil numéroté 20 à la troisième ligne, sixième colonne. Les 4 critères sur lesquels il est évalué valent respectivement 1, 3, 3 et 3.

- `criteria.ini` : chaque ligne correspond à un critère d'évaluation du profil (par exemple, "PHYSIQUE", "PSYCHOSOCIAL", "NUTRITIONNEL" et "COGNITIF").-

- `degrees.ini` : chaque ligne correspond à un degré de comparaison.
    
    
    Il est possible de renseigner jusqu'à 99 profils et autant de degrés de comparaison que souhaité.

