Prérequis:
Visual Studio ou C++ Express (au moins 2005, si possible 2010)
CMake (au moins 2.8)

Unziper le windows_dependencies.zip (http://iggservis.u-strasbg.fr/Data/) a la racine de CGoGN
Installer boost et Qt

Attention installer les binaires de Qt dans une version de compilateur differente (VS2008/VS2010)
permet d'executer mais pas de debugger !
Si vous utiliser BoostPro (http://www.boostpro.com) installer toutes les versions multithread dll
pour eviter les pb de compilation.

Le plus sur mais plus long est de compiler soi-même Qt et Boost (bon courage)

Lancer CMake
Mettre le chemin de la racine de CGoGN + /ThirdParty pour les sources
Mettre le chemin de la racine de CGoGN + /ThirdParty/buildRelease pour le build en release
Mettre le chemin de la racine de CGoGN + /ThirdParty/buildDebug pour le build en debug
La suite vaut pour les 2 répertoires:
Cliquer sur Configure
Choisir le bon compilateur (celui de votre version de Visual C++)
Modifier les variables en fonctions de ce que vous voulez compiler (README.TXT de ThirdParty)
Modifier le CMAKE_PREFIX_PATH si nécessaire (cf remarque dépendances)
Recliquer sur Configure
Cliquer sur Generate
Aller compiler sous visual(en release)

Relancer CMake
Mettre le chemin de la racine de CGoGN + /build pour les sources
Mettre le chemin de la racine de CGoGN + /Visual (par exemple) pour le build
Cliquer sur Configure
Choisir le bon compilateur (celui de votre version de Visual C++)
Modifier les variables comme pour ThirdParty (et CMAKE_PREFIX_PATH si nécessaire)
Recliquer sur Configure
Cliquer sur Generate

Aller dans Visual et double-cliquer sur CGoGN.sln !!

Pour que les executables fonctionnent ajouter le chemin vers les dll des dépendances dans la variable
d'environnement path (ou copier les dll dans un répertoire déjà dans le path)


Remarque dépendances:
Si vous voulez utiliser vos propre versions des dépendance (Zlib / glew / Boost / Qt)
changer le CMAKE_PREFIX_PATH (voir ci dessous)
Attention pour Boost le prefix path ne marche pas (utiliser BOOST_ROOT et d'autres si nécessaire)


Compilation 64bits:

La libraries et les exemples sont compilables en 64bit sous Windows.
Testé avec Visual Express C++ 2010 + Windows SDK 7.1
Les dépendances sont accessibles sous http://iggservis.u-strasbg.fr/Data/dependencies64.zip

Attention il faut modifer les projets et choisir Windows7.1SDK sous outils de plateforme
dans configuration/Général, pour utiliser les bons compilateurs et lib associées.

Remarque: gain de performance non négligeable en 64 bits (20/25%)

(Merci à Olivier Génevaux pour son expertise en compilation sous Windows !)

