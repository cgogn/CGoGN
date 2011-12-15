Pour compiler CGoGN sur MAC (testé sur OS X 10.6)

1) créer un lien de sys/malloc vers malloc.h dans usr/include:
    cd /usr/include; sudo ln -s sys/malloc.h . 
   Ceci permet d'éviter pas mal de pb de compilation de source linux.

2) installer avec macports  ou homebrew
    zlib,  glew (sudo port install xxxx)

3) installer Qt (http://qt.nokia.com/downloads/)

4) Ajouter (si ce n'est déjà fait) dans bash_profile:
    export LD_LIBRARY_PATH=/opt/local/lib 
    export C_INCLUDE_PATH=/opt/local/include:/usr/X11/include
    export CPLUS_INCLUDE_PATH=/opt/local/include:/usr/X11/include
    export LIBRARY_PATH=/opt/local/lib:/usr/X11/lib


=========================================================================================
                                ENGLISH VERSION
=========================================================================================
To compile CGoGN on MAC (tested on OSX 10.6 and OSX 10.7.2)

1) create a link from sys/malloc to malloc.h in usr/include:
    cd /usr/include; sudo ln -s sys/malloc.h . 
	This solve compilation problems of some linux codes on mac

2) install with macports or homebrew
    zlib,  glew, boost (sudo port install xxxx)

3) install Qt (http://qt.nokia.com/downloads/)

4) Add if not already done (by homebrew) in bash_profile:
    export LD_LIBRARY_PATH=/opt/local/lib 
    export C_INCLUDE_PATH=/opt/local/include:/usr/X11/include
    export CPLUS_INCLUDE_PATH=/opt/local/include:/usr/X11/include
    export LIBRARY_PATH=/opt/local/lib:/usr/X11/lib
