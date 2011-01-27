Pour compiler CGoGN sur MAC (testé sur OS X 10.6)

1) créer un lien de sys/malloc vers malloc.h dans usr/include:
    cd /usr/include; sudo ln -s sys/malloc.h . 
   Ceci permet d'éviter pas mal de pb de compilation de source linux.

2) installer avec macports (http://www.macports.org)
    zlib, glut, gmp, glew (sudo port install xxxx)

3) installer libDevil 1.7.8 (libIL) à partir des sources du site:
    http://openil.sourceforge.net/download.php
    ./configure --enable-ILU=yes --enable-ILUT=yes 
    make install

4) Ajouter (si ce n'est déjà fait) dans bash_profile:
    export LD_LIBRARY_PATH=/opt/local/lib
    export C_INCLUDE_PATH=/opt/local/include:/usr/X11/include
    export CPLUS_INCLUDE_PATH=/opt/local/include:/usr/X11/include
    export LIBRARY_PATH=/opt/local/lib:/usr/X11/lib


Remarques pour developpements:
- inclure glew.h à la place de gl.h
- pour utiliser GLUT avec GLEW, ajouter devant include glut.h:
	#define __X_GL_H
	#define GLAPIENTRY
  et plus de pb de compilation

