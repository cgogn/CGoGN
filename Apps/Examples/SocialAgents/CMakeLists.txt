cmake_minimum_required(VERSION 2.6)

project(SocialAgents)

SET(CMAKE_BUILD_TYPE Debug)

link_directories(
	${CGoGN_ROOT_DIR}/lib/Debug/
	${CGoGN_ROOT_DIR}/lib/Release/)

# define includes path
include_directories(
	/usr/include/libxml2/
	${CGoGN_ROOT_DIR}/Apps/Examples/SocialAgents/include
	${CGoGN_ROOT_DIR}/include
	${CGoGN_ROOT_DIR}/ThirdParty/AntTweakBar/include
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical
	${CGoGN_ROOT_DIR}/ThirdParty/Numerical/UFconfig
	${CGoGN_ROOT_DIR}/ThirdParty/gzstream
	${CGoGN_ROOT_DIR}/ThirdParty/Zinri
)

#define exec to compile
add_executable(socialAgents ./src/viewer.cpp ./src/env_map.cpp ./src/agent.cpp ./src/simulator.cpp  )
target_link_libraries( socialAgents container topology utils algo ${COMMON_LIBS} )

add_executable(socialAgentsD ./src/viewer.cpp ./src/env_map.cpp ./src/agent.cpp ./src/simulator.cpp  )
target_link_libraries( socialAgentsD containerD topologyD utilsD algoD ${COMMON_LIBS} )
