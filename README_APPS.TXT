Principe des applications dans CGoGN_Apps (english version below)

Faire un repertoire CGoGN_Apps ou on veut. Y mettre des sous répertoires
contenant chacun une application/

Le CMakeLists.txt a la racine de l'application doit commencer par

cmake_minimum_required(VERSION 2.8)

project(APPLICATION_NAME)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/../../CGoGN CACHE STRING "CGoGN root dir")
include(${CGoGN_ROOT_DIR}/apps_cmake.txt)

include_directories(
	${CGoGN_ROOT_DIR}/include
	${COMMON_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)


Faire un repertoire build et buildDebug
Pour compiler en release
	cd build
	cmake ..
	make 

Pour compiler en debug:
	cd buildDebug
	cmake ..
	make 

=========================================================================================
                                ENGLISH VERSION
=========================================================================================

Create a directory CGoGN_Apps where you want. Inside create subdirectories, one for each application.

The CMakeLists.txt to the root of the application must begin with

cmake_minimum_required(VERSION 2.8)

project(APPLICATION_NAME)

SET(CGoGN_ROOT_DIR ${CMAKE_SOURCE_DIR}/../../CGoGN CACHE STRING "CGoGN root dir")
include(${CGoGN_ROOT_DIR}/apps_cmake.txt)

include_directories(
	${CGoGN_ROOT_DIR}/include
	${COMMON_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

The .. / .. / CGoGN corresponds to the relative path to your version of CGoGN
(here placed at the same level as CGoGN_Apps)


Create directories build and buildDebug
To compile in release mode:
	cd build
	cmake ..
	make 

To compile in debug mode:
	cd buildDebug
	cmake ..
	make 





