Prérequis:
Visual Studio ou C++ Express (au moins 2005)
CMake (au moins 2.6)


Lancer CMake
Mettre le chemin de la racine de CGoGN + /build pour les sources
Mettre le chemin de la racine de CGoGN + /Visual (par exemple) pour le build
Cliquer sur Configure
Choisir le bon compilateur (celui de votre version de Visual C++)
Recliquer sur Configure
Cliquer sur Generate
Unziper le dependencies.zip (http://iggservis.u-strasbg.fr/Data/dependencies.zip) dans le repertoire Visual
Aller dans Visual et double-cliquer sur CGoGN.sln !!

Pour que les executables fonctionnent ajouter ....\Visual\dependencies\dll dans la variable d'environnement path
(ou copier les dll dans un répertoire déjà dans le path)

Remarque: uniquement compilation 32bits testée pour le moment.



Compilation 64bits:

La libraries et les exemples sont compilables en 64bit sous Windows.
Testé avec Visual Express C++ 2010 + Windows SDK 7.1
Les dépendances sont accessibles sous http://iggservis.u-strasbg.fr/Data/dependencies64.zip

Attention il faut modifer les projets et choisir Windows7.1SDK sous outils de plateforme
dans configuration/Général, pour utiliser les bons compilateurs et lib associées.

Remarque: gain de performance non négligeable en 64 bits (20/25%)

(Merci à Olivier Génevaux pour son expertise en compilation sous Windows !)

