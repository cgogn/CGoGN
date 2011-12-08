Dépendences Linux:
installer les paquets suivants:
cmake libXi-dev libXmu-dev libglew-dev libxml2-dev libboost-all-dev zlib1g-dev qt4-designer qt4-dev-tools uuid-dev

Pour compiler CGoGN:
- aller dans ThirdParty, cd build, taper "cmake ..", puis make ( avec -j x si vous avez x core sur votre machine)
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
	

- la compilation des sous-parties se fait en allant dans les sous-repertoires de build

Les applications (Examples et Tutos) sont dans le répertoire Apps

Les binaires sont générés dans le repertoire bin

Des exemples de données:
- fichiers trianbgz
- fichiers png (contenant des images voxel 3D)
sont accessible à l'adresse suivante: 
https://iggservis.u-strasbg.fr/Data/data.zip
