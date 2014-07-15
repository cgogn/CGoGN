English version bellow

Dépendences Linux
=================
installer les paquets suivants:  
cmake cmake-curses-gui cmake-qt-gui libXi-dev libXmu-dev libglew-dev libxml2-dev libboost-all-dev zlib1g-dev qt4-designer qt4-dev-tools uuid-dev libgsl0-dev libsuitesparse-dev


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

	* BUILD_SHARED_LIBS  (F) experimental !  
	* WITH_ASSIMP        (?)  
	* WITH_ZINRI ->
	* WITH_QT	 -> afin de limiter les dépendances et le temps de compilation  
	* ONELIB	-> permet de forcer la compilation en une seule lib (libcgogn.a) au lieu de quatre.

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


ENGLISH VERSION
===============

Linux dependencies
==================
install the following packages:  
cmake cmake-curses-gui cmake-qt-gui libXi-dev libXmu-dev libglew-dev libxml2-dev libboost-all-dev zlib1g-dev qt4-designer qt4-dev-tools uuid-dev libgsl0-dev libsuitesparse-dev

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
	make  
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
