#ifndef ENVMap_H
#define ENVMap_H

#include <iostream>

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
	typedef std::vector<Agent * > AGENTS;
	typedef NoMathIONameAttribute<AGENTS > AGENTVECT;

	typedef AttributeHandler<VEC3> TVEC3;

	typedef AttributeHandler<AGENTVECT> TAB_AGENTVECT;
};

typedef PFP::VEC3 VEC3 ;

class EnvMap
{
	public :
		PFP::MAP map;

		//identifier for the position attribute
		PFP::TVEC3 position;
		//identifier for the normal attribute
		PFP::TVEC3 normal;

		PFP::TAB_AGENTVECT agentvect;

		float sideSize;

		DartMarker closeMark;

		std::vector<Dart> newBuildings;

		Dart dSimplify;
		Dart dSubdivide;
		std::list<Dart> filledFaces;

		static const unsigned int maxNbAgentInFace=3;

		EnvMap();

		bool simplifiable(Dart d);
		bool simplifiableDart(Dart d);
		bool collapsableEdge(Dart& d,Dart& dStop);
		bool simplifyVertex(Dart& d);
		bool simplifyEdge(Dart& d);
		void simplify();

		unsigned int getNbFaces();

		Dart getDart(unsigned int i);

		Dart getBelongingCell(const PFP::VEC3& pos);
		
		bool noAgentSurroundingVertex(Dart d);

		void findAgentContactInFace(Dart d, PFP::VEC3 pos, float radius, std::vector<Dart>& l_contact_Dart,DartMarker& m);

		void insertObstacleOfFace(PFP::AGENTS agents,Dart d);
		
		void getAllFacesOfAgents(Dart d);

		void linkAgentWithCells(Agent* agent);

		void addObstacle(std::vector<VEC3> points);

		void resetAgentInFace(Agent * agent);

		void updateMap();

		void subdivideFaces();

		void pushFaceToSubdivide(const Dart& d);

		void popFaceToSubdivide(const Dart& d);

		void pushAgentInCell(Agent * agent, Dart d);

		void popAgentInCell(Agent * agent, Dart d);

// 		void addNeighborAgents(size_t agentno, Agent * agent);
		void addNeighborAgents(PFP::AGENTS agentsFrom,PFP::AGENTS agentsTo);
};

#endif
