
Linux dependencies
==================
install the following packages:  
cmake cmake-curses-gui cmake-qt-gui libXi-dev libXmu-dev libglew-dev zlib1g-dev libqt4-dev qt4-dev-tools uuid-dev libgsl0-dev libsuitesparse-dev libqglviewer-dev

Mac Dependencies
=================
To compile CGoGN on MAC 

* install XCode and command line tools.
* install Qt4 (4.8) and QtCreator
* install (or build) libQGLViewer
* install cmake (native or with homebrew)
* install with homebrew : lzlib, glew, suite-sparse

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
