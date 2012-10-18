#ifndef PARTCELL2DMEMO_H
#define PARTCELL2DMEMO_H

#include "particle_cell_2D.h"

#include "Algo/Geometry/inclusion.h"
#include "Geometry/intersection.h"
#include "Geometry/orientation.h"
#include <iostream>

/* A particle cell is a particle base within a map, within a precise cell, the displacement function should indicate
 after each displacement wherein lies the new position of the particle */

namespace CGoGN
{

namespace Algo
{

namespace MovingObjects
{

template <typename PFP>
class ParticleCell2DMemo : public ParticleCell2D<PFP>
{

public:
	typedef typename PFP::MAP MAP ;
	typedef VertexAttribute<typename PFP::VEC3> TAB_POS ;

	std::list<Dart> memo_cross ;

private:
	ParticleCell2DMemo()
	{
	}

public:
	ParticleCell2DMemo(MAP& map, Dart belonging_cell, const VEC3& pos, const TAB_POS& tabPos,
	                   DartMarker& obst) :
		ParticleCell2D<PFP>(map, belonging_cell, pos, tabPos, obst)
	{
	}

	void vertexState(const VEC3& current) ;

	void edgeState(const VEC3& current, Geom::Orientation2D sideOfEdge = Geom::ALIGNED) ;

	void faceState(const VEC3& current) ;

	void move(const VEC3& goal) ;
} ;

#include "particle_cell_2D_memo.hpp"
//namespace

}

}

}

#endif
