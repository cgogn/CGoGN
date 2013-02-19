Dépendences Linux:
installer les paquets suivants:
cmake libXi-dev libXmu-dev libglew-dev libxml2-dev libboost-all-dev zlib1g-dev qt4-designer qt4-dev-tools uuid-dev libgsl0-dev

Pour compiler CGoGN:
- aller dans ThirdParty, cd build, taper "cmake .", puis make ( avec -j x si vous avez x core sur votre machine) suivi de make install
	On peut fixer certaines option pour ne pas tout compiler (cf le README.TXT dans ThirdParty)
	On peut aussi compiler en debug en créant un répertoir qui fini par Debug à la place de build
	
- aller dans build et taper "cmake ..", puis make
	Des options (booléennes) sont ici aussi disponible:
	BUILD_SHARED_LIBS  (F) experimental !
	WITH_ASSIMP        (T)
	WITH_NUMERICAL     (T)
	WITH_ZINRI         (T)
	WITH_QT	           (T)
	afin de limiter les dépendances et le temps de compilation/
	
	FORCE_MR           (2)
	permet de compiler dans 3 modes: mixte (0), multi-resolution(1), mono-resolution(2).
	
	ONELIB             (F)
	permet de forcer la compilation en une seule lib (libcgogn.a) au lieu de quatre.
	
	

- la compilation des sous-parties se fait en allant dans les sous-repertoires de build

Les applications (Examples et Tutos) sont dans le répertoire Apps

Les binaires sont générés dans le repertoire bin

Des exemples de données:
- fichiers trianbgz, off, etc
sont accessible à l'adresse suivante: 
https://iggservis.u-strasbg.fr/Data/data.zip

=========================================================================================
                                ENGLISH VERSION
=========================================================================================

Linux dependencies:
install the following packages:
ccmake libXi-dev libXmu-dev libglew-dev libxml2-dev libboost-all-dev zlib1g-dev qt4-designer qt4-dev-tools uuid-dev

To compile CGoGN:
- Go ThirdParty, cd build, type "cmake .." and then make (with -j x if you have x core on your machine) followed by make install
We can set some option to not compile everything (see the README.TXT in ThirdParty)
You can also compile debug by creating a directory that ended Debug build instead

- Go to build and type "cmake .." and then make
	Options (Boolean) are also available here:
	BUILD_SHARED_LIBS (F) experimental!
	WITH_ASSIMP (T)
	WITH_NUMERICAL (T)
	WITH_ZINRI (T)
	WITH_QT (T)
	to minimize dependencies and compilation time /
	
	FORCE_MR           (2)
	allow the compilation in 3 mode: mixed(0), multi-resolution(1), mono-resolution(2).
	
	ONELIB             (F)
	allow compilation in one lib (libcgogn.a) instead of four.


- the compilation of each sub-parts is done by going in the subdirectories of build

Applications (examples and tutorials) are in the Apps directory

The binaries are generated in the bin directory

Examples of data:
- Trianbgz files, off, etc.
are accessible at the following address:
https://iggservis.u-strasbg.fr/Data/data.zip
