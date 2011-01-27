#ifndef ENVMap_H
#define ENVMap_H

#include <iostream>
// #include "Definitions.h"

#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Algo/ImplicitHierarchicalMesh/ihm.h"
#include "Algo/ImplicitHierarchicalMesh/subdivision.h"

#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Modelisation/extrusion.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Geometry/centroid.h"

#include "Container/fakeAttribute.h"

#include "env_generator.h"

using namespace CGoGN;

class Agent;

struct PFP {
	// definition de la carte
// 	typedef EmbeddedMap2<Map2> MAP;
 	typedef Algo::IHM::ImplicitHierarchicalMap MAP;
	// definition du type de reel utilise
	typedef float REAL;
	// definition du type de vecteur (point) utilise
	typedef Geom::Vec3f VEC3;
	// definition des listes d'agent
// 	typedef std::vector<Agent *> AGENTS;
	typedef std::vector<Agent * > AGENTS;
	typedef NoMathIONameAttribute<AGENTS > AGENTVECT;

	typedef AttributeHandler<VEC3> TVEC3;

	typedef AttributeHandler<AGENTVECT> TAB_AGENTVECT;

	// definition des cellules abstraites
// 	typedef MAP::EVertex EVERTEX;
// 	typedef MAP::EEdge EEDGE;
// 	typedef MAP::EFace EFACE;
//	typedef MAP::EVolume EVOLUME;
};

typedef PFP::VEC3 VEC3 ;

class EnvMap
{
	public :
		PFP::MAP map;
		//SelectorTrue<PFP::MAP::Dart> allDarts;

		//identifier for the position attribute
// 		unsigned int idPos;
		PFP::TVEC3 position;
		//identifier for the normal attribute
// 		unsigned int idNorm;
		PFP::TVEC3 normal;

// 		unsigned int idAgent;
		PFP::TAB_AGENTVECT agentvect;

		float sideSize;

// 		PFP::POSITION tablePosition;
// 		PFP::TAB_AGENTVECT tableAgent;

		DartMarker closeMark;
// 		DartMarker subdivMarker;

		std::vector<Dart> newBuildings;

// 		std::vector<Dart> subdividedDarts;
		Dart dSimplify;
		Dart dSubdivide;

		EnvMap();

		bool simplifiable(Dart d);
		bool simplifiableDart(Dart d);
		bool collapsableEdge(Dart& d,Dart& dStop);
		bool simplifyVertex(Dart& d);
		bool simplifyEdge(Dart& d);
		void simplify();

		VEC3 faceCenter(Dart d);

// 		const PFP::VEC3& getPosition(const Dart& d);
// 		RVO::Vector2 getPosition2(Dart d);

		unsigned int getNbFaces();

		Dart getDart(unsigned int i);

		Dart getBelongingCell(const PFP::VEC3& pos);
		
		bool noAgentSurroundingVertex(Dart d);

		void findAgentContactInFace(Dart d, PFP::VEC3 pos, float radius, std::vector<Dart>& l_contact_Dart,DartMarker& m);

// 		void insertObstacleOfFace(Agent * agent, const Dart d);
		void insertObstacleOfFace(PFP::AGENTS agents,const Dart d);
		
// 		void getAllFacesOfAgent(Agent * agent, Dart d, PFP::VEC3 pos, float radius);
		void getAllFacesOfAgents(Dart d);

		void linkAgentWithCells(Agent* agent);

		void addObstacle(std::vector<VEC3> points);

		bool subdivideSquare(Dart d);

		bool subdivideFace(Dart d);

		void resetAgentInFace(Agent * agent);

		bool checkForSubdivision(Dart d);

		void updateMap();

		void subdivideFaces();

		void simplifyFaces();

		void pushAgentInCell(Agent * agent, Dart d);

		void popAgentInCell(Agent * agent, Dart d);

// 		void addNeighborAgents(size_t agentno, Agent * agent);
		void addNeighborAgents(PFP::AGENTS agentsFrom,PFP::AGENTS agentsTo);
};

#endif
