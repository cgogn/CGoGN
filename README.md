
See the [CGoGN Web site](cgogn.unistra.fr) for more informations.

We provide:
* CGoGN the core library
* tutorials and examples
* SCHNApps an integrated interface with plugin system.


CGoGN and SCHNApps are compatible with Linux, Mac and Windows
The usage of C++11 enforce the usage of recent compilers (g++4.8, clang3.0, cl18)

Linux
=====
Dependencies
------------
* install for following package cmake cmake-curses-gui cmake-qt-gui libXi-dev libXmu-dev libglew-dev zlib1g-dev uuid-dev libgsl0-dev libsuitesparse-dev python2.7-dev
* for Qt4: libqt4-dev qt4-dev-tools qtcreator
* for Qt5: qt5-default qtbase5-dev-tools qtcreator libqt5svg5-dev libqt5opengl5-dev

**Supported compilers: g++ 4.8 or greater**

Compiling
---------
* Launch cmake in a build directory with path to the CGoGN root directory as source parameter
* If directory name finish by 'Debug' compilation is done in Debug mode, else in Release mode
* Go in build directory and make (-j4 in your machine is 4 core)

CMake options (using gui or ccmake)
-----------------------------------
* CGoGN_WITH_ASSIMP: build CGoGN with Assimp (or not)
* CGoGN_WITH_ZINRI: build CGoGN with Zinri lib (or not)
* CGoGN_WITH_QT: build CGoGN with Qt (or not)
* CGoGN_DESIRED_QT_VERSION:  Choose 4 or 5" 
* CGoGN_WITH_GLEWMX: use (or not) multi-contex GLEW (for VRJuggler).
* CGoGN_USE_OGL_CORE_PROFILE: use (or not) OpenGL 3.3 core profile.
* CGoGN_COMPILE_EXAMPLES: compile the examples.
* CGoGN_COMPILE_TUTOS: compile the tutorials.
* CGoGN_COMPILE_TESTS: compile the tests.
* CGoGN_COMPILE_BENCHES: compile the benches.
* CGoGN_COMPILE_SANDBOX: compile all in sandbox
* CGoGN_ONELIB: build CGoGN in one lib instead of 4 separated libs

By default libraries are generated in dynamic version.

You can ideally do the same with QtCreator

Same compilation system for SCHNApps.


MacOS
=====
Dependencies
------------
* XCode and command line tools.
* Qt4 or Qt5 and QtCreator (native or with homebrew)
* cmake (native or with homebrew)
* install with homebrew : lzlib, glew, suite-sparse (you must first add the homebrew/science repository with the following command : brew tap homebrew/science)
* for macport user installation of suite sparse must be done with "+metis" (sudo port install SuiteSparse +metis)

**Supported compilers: clang 3.0 or greater**

Compiling
---------
As on linux

CMake Qt detection problem
--------------------------
If Qt is not found, add path to Qt to CMAKE_PREFIX_PATH variable.

Executables generation
----------------------
For best system compatibility executables are generated in bin/Release/Contents/MacOS

Window 
======
Dependencies
------------
* install (or compile) Qt (choose 32/64 bits)
* install python 2.7 (choose 32/64 bits) (Needed for SCHNApps)
* Other Windows dependencies are furnished at:
  * http://cgogn.unistra.fr/Dependencies/dependencies.txt
  * http://cgogn.unistra.fr/Dependencies/windows_dependencies32.zip
  * http://cgogn.unistra.fr/Dependencies/windows_dependencies64.zip

**Supported compilers: Visual C++ 2013 (12.0) or greater**

Environment variables
---------------------
* set PYTHON_PATH to your python installation directory
* set PYTHON_LIB to your python installation directory + \lib
* add xxx\windows_dependencies\lib to PATH
* add path to bin of Qt to PATH

Compiling
---------
* Use CMake with a build directory and path to the CGoGN root directory as source parameter
* for CMake options see Linux compiling
* generated visual solution (choose right compiler type)
* launch Visual Studio !
* build solution after choosing Debug/Release

CMake Qt detection problem
--------------------------
If Qt is not found set the QT_QMAKE_EXECUTABLE cmake variable.

DLL/Executables generation
--------------------------
To avoid dll loading problem, all dll and executable (even SCHNApps) are generated in bin/Release
or bin/Debug of CGoGN root directory.

Same compilation system for SCHNApps.

Compiling simple application
============================

To compile a Qt application using Qt, you just need to include *apps_cmake.txt* after setting
the CGoGN_ROOT_DIR variable. See example provided in *apps_cmake.txt* comments.


