Numerical: diverses librariries de resolution de systemes lineaires et autres
Zinri: lecture/ecriture de fichier inr.gz (image3D inria)
Assimp: import de nombreux type de fichiers 3D (maillages)

Compilation:
============
cd build
cmake .. ou ccmake .. ou cmake-gui ..

options (et valeur par defaut)
	BUILD_SHARED_LIBS  (F)                                                                                                                                                           
	WITH_ASSIMP        (T)
	WITH_NUMERICAL     (T)
	WITH_ZINRI         (T)
	
Pour utiliser avec CMake -DOPTION=true/false 
