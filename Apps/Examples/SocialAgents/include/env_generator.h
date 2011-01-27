#ifndef ENV_GENERATOR
#define ENV_GENERATOR

namespace CGoGN
{

namespace CityGenerator
{

template <typename PFP>
void generateAbsolutSpiralOfDeath(typename PFP::MAP& map,typename PFP::TVEC3& position, DartMarker& closeMark, unsigned int side, float rMin, float rMax,float nbTurns);

template <typename PFP>
void generateToboggan(typename PFP::MAP& map,typename PFP::TVEC3& position, DartMarker& closeMark, unsigned int side, float rMin, float rMax,float nbTurns);

template <typename PFP, typename EMBV>
void generateSmallCity(typename PFP::MAP& map, EMBV& position, DartMarker& closeMark, float sideSize);

template <typename PFP, typename EMBV>
void generateCity(typename PFP::MAP& map, EMBV& position, DartMarker& closeMark, float sideSize);

template <typename PFP, typename EMBV, typename EMBAGENT>
void animateCity(typename PFP::MAP& map, EMBV& position, EMBAGENT& agents, DartMarker& closeMark, std::vector<Dart>& newBuilding);

template <typename PFP, typename EMBV>
bool notEightAdjacentToAnObstacle(typename PFP::MAP& map, Dart d, DartMarker& closeMark);

template <typename PFP, typename EMBV>
void generateGrid(typename PFP::MAP& map, EMBV& position, unsigned int cX,unsigned int cY,float sideLength, DartMarker& closeMark);

template <typename PFP, typename EMBV>
void markObstacleSteepFaces(typename PFP::MAP& map, EMBV& position, float steepness, DartMarker& closeMark);

template <typename PFP, typename EMBV>
void generatePathToUpperStair(typename PFP::MAP& map, EMBV& position, Dart dLower, Dart dUpper, DartMarker& closeMark);

template <typename PFP, typename EMBV>
void generateBuilding(typename PFP::MAP& map, EMBV& position, Dart d,DartMarkerStore& build, float height, float side, unsigned int buildingType, DartMarker& closeMark);

template <typename PFP, typename EMBV>
static void generateBridge(typename PFP::MAP& map, EMBV& position, Dart dStart, Dart dStop,float bridgeHeight, DartMarker& closeMark);

template <typename PFP, typename EMBV>
void generateBridge(typename PFP::MAP& map, EMBV& position, unsigned int cX, unsigned int cY,float sideLength, DartMarker& closeMark);

}

}

#include "env_generator.hpp"
#endif