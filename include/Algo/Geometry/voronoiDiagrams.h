#ifndef __VORONOI_DIAGRAMS_H__
#define __VORONOI_DIAGRAMS_H__

#include <vector>
#include <map>

#include "Topology/generic/traversor2.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
class VoronoiDiagram {
protected :
	typedef typename PFP::REAL REAL;

	typedef struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
//		unsigned int region;
		Dart pathOrigin;
		static std::string CGoGNnameOfType() { return "VoronoiVertexInfo" ; }
	} VoronoiVertexInfo ;
	typedef NoMathIOAttribute<VoronoiVertexInfo> VertexInfo ;

	typename PFP::MAP& map;
	const EdgeAttribute<REAL>& edgeCost; // weights on the graph edges
	VertexAttribute<unsigned int>& regions; // region labels
	std::vector<Dart> border;
	std::vector<Dart> seeds;

	VertexAttribute<VertexInfo> vertexInfo;
	std::multimap<float,Dart> front ;
	CellMarkerStore<VERTEX> vmReached;

public :
	VoronoiDiagram (typename PFP::MAP& m, const EdgeAttribute<REAL>& c, VertexAttribute<unsigned int>& r);
	~VoronoiDiagram ();

	const std::vector<Dart>& getSeeds (){return seeds;}
	void setSeeds (const std::vector<Dart>&);
	void setRandomSeeds (unsigned int nbseeds);
	const std::vector<Dart>& getBorder (){return border;}
	void setCost (const EdgeAttribute<REAL>& c);

	void computeDiagram ();
	void computeDistancesWithinRegion (Dart seed);

protected :
	virtual void clear ();
	void initFrontWithSeeds();
	virtual void collectVertexFromFront(Dart e);
	void addVertexToFront(Dart f, float d);
	void updateVertexInFront(Dart f, float d);
};


template <typename PFP>
class CentroidalVoronoiDiagram : public VoronoiDiagram<PFP> {
private :
	typedef typename PFP::REAL REAL;
	typedef typename PFP::VEC3 VEC3;

	double globalEnergy;

	VertexAttribute<REAL>& distances; // distances from the seed
	VertexAttribute<Dart>& pathOrigins; // previous vertex on the shortest path from origin
	VertexAttribute<REAL>& areaElts; // area element attached to each vertex

public :
	CentroidalVoronoiDiagram (typename PFP::MAP& m,
			const EdgeAttribute<REAL>& c,
			VertexAttribute<unsigned int>& r,
			VertexAttribute<REAL>& d,
			VertexAttribute<Dart>& o,
			VertexAttribute<REAL>& a);
	~CentroidalVoronoiDiagram ();

	void cumulateEnergyOnPaths();
	unsigned int moveSeeds(); // returns the number of seeds that did move
	unsigned int moveSeeds2(); // returns the number of seeds that did move
	REAL getGlobalEnergy() {return globalEnergy;}

protected :
	void clear();
	void collectVertexFromFront(Dart e);
	REAL cumulateEnergyFromRoot(Dart e);
	unsigned int moveSeed(unsigned int numSeed);
};


}// end namespace Geometry
}// end namespace Algo
}// end namespace CGoGN

#include "voronoiDiagrams.hpp"

#endif
