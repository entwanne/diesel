# Lab1 - Lexer

* Détection des entier, réels, identifiants, chaînes de caractères et commentaires
* Ajout des identifiants dans la string-pool


# Lab2 - Symtab

* Méthodes d'ouverture et de fermeture de scopes (open_scope, close_scope)
  * À l'ouverture incrémente le niveau de bloc et inscrit la position dans les table des blocs
  * À la fermeture supprime les références vers les symboles du bloc courant et décrémente le niveau de bloc
* Méthode de recherche d'un symbole (lookup_symbol): Suis les liens de la table de hash jusqu'à trouver un symbole du nom donné en paramètre
* Méthode d'installation d'un symbol (install_symbol): Crée un symbole du type indiqué en paramètre, lui attribue des valeurs par défaut, puis l'insère dans la symta et dans la table de hash
* Méthode d'entrée dans une procédure (enter_procedure): c/c de la méthode d'entrée dans une fonction


# Lab3 - Parser

* Générations et insértions dans l'AST des noeuds correspondant aux différentes opérations


# Lab4 - Sémantique

* Vérification et attribution des types pour les différents nœuds de l'AST, par c/c et appels récursifs


# Lab5 - Optimisation

* Parcours (récursif) de l'AST et optimisation des paramètres et opérations binaires:
  * Remplacement des constantes par leurs valeurs
  * Simplification des opérations entre deux valeurs constantes (calcul du résultat à la compilation)


# Lab6 - Quads

* Génération des quadruples associés aux nœuds de l'AST -> passage d'une arborescence à une structure linéaire
* Dans le cas des if/elsif/else, génération des labels et des sauts
* Création de variables temporaires dans la symtab pour stocker le résultat des opérations


# Lab7 - ASM

* Transformation un à un des quads en leur équivalent assembleur
* Ajout des prologues et epilogues autour des fonctions
* Méthodes de chargement et enregistrement des registres (fetch, store)
