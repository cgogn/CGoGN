Pour utiliser Eclipse (testé avec la version Helios, Galileo & Indigo)

 * Configurer un projet dans Eclipse :
 - Faire un nouveau projet C++
 - Choisir Makefile project -> Empty Project
 - Choisir Comme "Location": la racine de NewCGoGN
 - Choisir Toolchains: Linux GCC

 * Compilation avec le petit marteau ;) (l'icone)
Dans les propriétés du projet: 
-> C/C++ Build:
	Dans la configuration Default | Active:
	ajouter au chemin de Build Directory /build
	
	on peut ensuite ajouter des configurations pour compiler ce que l'on veut
	en changeant juste le Build directory:
	CGoGNRelease:		build/Release		(compile la lib en release)
	CGoGNDebug:		build/Debug		(compile la lib en debug)
	Apps:			build/Apps		(compile les exemples et les tutos)
	Tuto:			build/Apps/Tuto		(compile les tutos)
	etc..

Ceci ajoute dans le repertoire de CGoGN un .project et un .cproject.

 * Dépendances inter-projets et autocomplétion
Si le projet X dépend d'un autre projet, alors :
Projet X -> Properties -> Project references : cocher le projet dont il dépend.

 * Dépendances de Qt :
Afin d'éviter qu'Eclipse ne rejette les syntaxes Qt (les includes de Qt, les Q_OBJECT, les callbacks) :
 - Télécharger le plugin Qt "eclipse-integration" http://qt.nokia.com/developer/eclipse-integration/
 - Décompresser l'archive et copier son contenu dans le répertoire d'installation d'Eclipse (plugins et features)
 - Relancer Eclipse au moins une fois avec l'option --clean (relecture des répertoires locaux)
Normalement, dans Window->Preferences de Eclipse, Qt apparait dans le menu.
 - Y ajouter le répertoire de Qt (/usr/bin sur Debian/Ubuntu) et le répertoire d'include (/usr/include/qt4).
Ensuite, pour chaque projet, ajouter les includes suivants dans Properties -> C/C++ general -> Paths and Symbols
parmi les includes de C++ :
 - /usr/include/qt4
 - /usr/include/qt4/Qt
 - /usr/include/qt4/QtCore
 - /usr/include/qt4/QtGui
 - /usr/include/qt4/QtOpenGL
REMARQUE : en bas de cette fenêtre, le bouton "Export Settings" permet d'exporter en XML la configuration afin 
de l'importer dans les autres projets.
 - Pour finir, un petit clic-droit sur le projet -> Index -> Freshen all files et le tour est joué.

 * Coloration syntaxique des shaders
 - Télécharger et décompresser EclipseShaders : http://sourceforge.net/projects/glshaders/
 - Copier les répertoires plugins et features dans le répertoire d'installation eclipse
 - Relancer Eclipse au moins une fois avec l'option --clean (relecture des répertoires locaux)
Eclipse reconnaîtra désormais les fichiers .frag et .vert.
Pour associer d'autres fichiers au programme GLSLeditor :
 - Preferences -> General -> Editors -> File Associations
Dans Preferences -> Shaders Preferences, on peut désormais configurer certaines options liées au shaders.
