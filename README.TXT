*** english version below ***

Dépendences Linux:
installer les paquets suivants:
cmake  cmake-curses-gui libXi-dev libXmu-dev libglew-dev libxml2-dev libboost-all-dev zlib1g-dev qt4-designer qt4-dev-tools uuid-dev libgsl0-dev libsuitesparse-dev

Pour compiler CGoGN:

- aller dans ThirdParty, cd build, taper "cmake .", puis make ( avec -j x si vous avez x core sur votre machine) suivi de make install

	On peut fixer certaines option pour ne pas tout compiler (cf le README.TXT dans ThirdParty)

  - ensuite on compile CGoGN (compile les libs dans CGoGN/lib et des exemples dans CGoGN/bin) :

	cd CGoGN/build
	cmake ..
	make (-j x si vous avez x cores)

	Certaines options (booléennes) sont ici aussi disponible:
	BUILD_SHARED_LIBS  (F) experimental !
	WITH_ASSIMP        (T)
	WITH_NUMERICAL     (T)
	WITH_ZINRI         (T)
	WITH_QT	           (T)
	afin de limiter les dépendances et le temps de compilation
	FORCE_MR           (2)
	permet de compiler dans 3 modes: mono-resolution (0), multi-resolution (1)
	ONELIB             (F)
	permet de forcer la compilation en une seule lib (libcgogn.a) au lieu de quatre.
	
  - on peut ensuite compiler SCHNApps ainsi que les plugins fournis :

	cd CGoGN/SCHNApps/build
	cmake ..
	make (-j x si vous avez x cores)


Des exemples de données:
- fichiers trianbgz, off, etc
sont accessible à l'adresse suivante: 
https://iggservis.u-strasbg.fr/Data/data.zip

=========================================================================================
                                ENGLISH VERSION
=========================================================================================

Linux dependencies:
install the following packages:
cmake cmake-curses-gui libXi-dev libXmu-dev libglew-dev libxml2-dev libboost-all-dev zlib1g-dev qt4-designer qt4-dev-tools uuid-dev libgsl0-dev libsuitesparse-dev

To compile CGoGN:
  - first compile third-party libraries:

	cd CGoGN/ThirdParty/build
	cmake ..
	make (-j x if you have x cores)
	make install

	Some options are available to not compile everything (see the README.TXT in ThirdParty)

  - then compile CGoGN (the libs are generated in CGoGN/lib and some examples in CGoGN/bin):

	cd CGoGN/build
	cmake ..
	make (-j x if you have x cores)

	Some options are available:
	BUILD_SHARED_LIBS  (F) experimental !
	WITH_ASSIMP        (T)
	WITH_NUMERICAL     (T)
	WITH_ZINRI         (T)
	WITH_QT	           (T)
	to minimize dependencies and compilation time
	FORCE_MR           (2)
	allows the compilation in 3 modes: mono-resolution (0), multi-resolution (1)
	ONELIB             (F)
	allowss to force the compilation in one lib (libcgogn.a) instead of four.

  - we can then compile SCHNApps and the provided plugins :

	cd CGoGN/SCHNApps/build
	cmake ..
	make (-j x if you have x cores)


Examples of data:
- Trianbgz files, off, etc.
are accessible at the following address:
https://iggservis.u-strasbg.fr/Data/data.zip
