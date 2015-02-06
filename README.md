French version bellow

Linux dependencies
==================
install the following packages:  
cmake cmake-curses-gui cmake-qt-gui libXi-dev libXmu-dev libglew-dev libboost-all-dev zlib1g-dev libqt4-dev qt4-dev-tools uuid-dev libgsl0-dev libsuitesparse-dev libqglviewer-dev

Mac Dependencies
=================
To compile CGoGN on MAC 

* install XCode and command line tools.
* install Qt4 (4.8) and QtCreator
* install (or build) libQGLViewer
* install cmake (native or with homebrew)
* install with homebrew : lzlib, glew, boost, suite-sparse

To install suite-sparse package, you must first
add the homebrew/science repository with the following command :
	brew tap homebrew/science

Compilation is the same as on Linux for CGoGN


To compile CGoGN
================
 * first compile third-party libraries:

	```
	cd CGoGN/ThirdParty/build  
	cmake ..  
	make (-j x if you have x cores)
	make install  
	```

	In debug mode:  
	```
	cd ../buildDebug  
	cmake ..  
	make  (-j x if you have x cores)
	make install
	```

	Some options are available to not compile everything:
	* WITH_ASSIMP  -> compile and use Assimp library 
	* WITH_ZINRI -> compile and use Zinri library 
	* WITH_PYTHONQT	-> needed by SCHNApps

* then compile CGoGN (the libs are generated in CGoGN/lib and some examples in CGoGN/bin):

	```
	cd CGoGN/build  
	cmake ..  
	make (-j x if you have x cores)  
	```

	in debug mode:  
	```
	cd ../buildDebug  
	cmake ..  
	make  
	```

	Some options are available:

	* BUILD_SHARED_LIBS  (F) experimental !
	* ONELIB	-> allow compilation in one lib (libcgogn.a) instead of four.
	* WITH_ASSIMP  -> use Assimp library (auto setted by third party compiling)
	* WITH_ZINRI -> use Zinri library (auto setted by third party compiling)
	* WITH_QT	-> to minimize dependencies and compilation time
	* WITH_GLEWMX  -> to use GLEWMX (for VRJuggler users)

* we can then compile SCHNApps and the provided plugins :
	
	```
	cd CGoGN/SCHNApps/build  
	cmake ..  
	make (-j x if you have x cores)  
	```

	in debug mode:
	```
	cd ../buildDebug  
	cmake ..  
	make  
	```

Libraries are generated in lib/Release and lib/Debug
Binaries are generated in bin/Release and bin/Debug


Examples of data (trianbgz, off, etc.. files) are available at the following address:
https://iggservis.u-strasbg.fr/Data/data.zip


FRENCH VERSION
==============

Dépendences Linux
=================
installer les paquets suivants:  
cmake cmake-curses-gui cmake-qt-gui libXi-dev libXmu-dev libglew-dev libboost-all-dev zlib1g-dev libqt4-dev qt4-dev-tools uuid-dev libgsl0-dev libsuitesparse-dev libqglviewer-dev


Mac Dependencies
=================
To compile CGoGN on MAC 

* installer XCode et les command line tools.
* installer Qt4 (4.8) et QtCreator
* installer (ou compiler) libQGLViewer
* installer cmake (native ou avec homebrew)
* installer with avec homebrew : lzlib, glew, boost, suite-sparse

Pour pouvoir installer le paquet suite-sparse, il faut avoir
prélablement ajouté le dépôt homebrew/science grâce à la commande suivante :
	brew tap homebrew/science
	
Pour le reste la compilation se passe de la meme manière


Pour compiler CGoGN
===================

* on compile tout d'abord les bibliothèques third-party :

``` 
cd CGoGN/ThirdParty/build  
cmake ..  
make (-j x si vous avez x cores)  
make install  
```

La même chose en debug:  
```
cd ../buildDebug  
cmake ..  
make (-j x si vous avez x cores) 
make install  
```

On peut fixer certaines option pour ne pas tout compiler:
	* WITH_ASSIMP  -> compile et utilise la librairie Assimp
	* WITH_ZINRI -> compile et utilise la librairie Zinri  
	* WITH_PYTHONQT	-> pour SCHNApps
	
* ensuite on compile CGoGN :  
	```
	cd CGoGN/build  
	cmake ..  
	make (-j x si vous avez x cores)  
	```

	Et en Debug avec:  
	```
	cd ../buildDebug  
	cmake ..  
	make (-j x si vous avez x cores)  
	```

    Certaines options sont disponibles :

	* BUILD_SHARED_LIBS -Q experimental !
	* ONELIB	-> compilation en une seule lib (libcgogn.a) au lieu de 4
	* WITH_ASSIMP  -> utilise Assimp (auto set par third party compil)
	* WITH_ZINRI -> utilise Zinri (auto set par third party compil)
	* WITH_QT	-> minimize les dependence si pas besoin de Qt (VRJuggler)
	* WITH_GLEWMX  -> pour utiliser GLEWMX a la place de GELW (pour VRJuggler)


	Les librairies sont generees dans lib/Release et lib/Debug  
	Les binaires sont generes dans bin/Release et bin/Debug


* on peut ensuite compiler SCHNApps ainsi que les plugins fournis :
	
	```
	cd CGoGN/SCHNApps/build  
	cmake ..  
	make (-j x si vous avez x cores)  
	```

	Et en Debug avec:  
	```
	cd ../buildDebug  
	cmake ..  
	make (-j x si vous avez x cores)  
	```


Des exemples de données (fichiers trianbgz, off, etc..) sont accessible à l'adresse suivante: 
https://iggservis.u-strasbg.fr/Data/data.zip
