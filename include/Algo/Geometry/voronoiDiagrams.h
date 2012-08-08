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
private :
	typedef typename PFP::REAL REAL;
	typedef typename PFP::VEC3 VEC3;

	typedef struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		unsigned int region;
		static std::string CGoGNnameOfType() { return "VoronoiVertexInfo" ; }
	} VoronoiVertexInfo ;
	typedef NoMathIOAttribute<VoronoiVertexInfo> VertexInfo ;

	typename PFP::MAP& map;
	const EdgeAttribute<REAL>& edgeCost; // weights on the graph edges
//	const VertexAttribute<VEC3>& position; // positions
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

private :
	void clear ();
	void initFrontWithSeeds();
};

}// end namespace Geometry
}// end namespace Algo
}// end namespace CGoGN

#include "voronoiDiagrams.hpp"

#endif
