Pour utiliser Eclipse (testé avec la version Helios & Galileo)

Faire un nouveau projet C++
Choisir Makefile project -> Empty Project
Choisir Comme "Location": la racine de NewCGoGN
Choisir Toolchains: Linux GCC

Dans les propriétés du projet: 
-> C/C++ Build:
	Dans la configuration Default | Active:
	ajouter au chemin de Build Directory /build
	
	on peut ensuite ajouter des configurations pour compiler ce que l'on veut
	en changeant juste le Build directory:
	CGoGNRelease:	build/Release		(compile la lib en release)
	CGoGNDebug:		build/Debug			(compile la lib en debug)
	Apps:			build/Apps			(compile les exemples et les tutos)
	Tuto:			build/Apps/Tuto		(compile les tutos)
	etc..

On peut alors compiler avec le petit marteau ;) (l'icone) et choisir avec le bouton droit ce que l'on 
veut compiler.

Ceci ajoute dans le repertoire de CGoGN un .project et un .cproject.

