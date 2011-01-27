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

	public :
	typedef typename PFP::MAP Map;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::TVEC3 TAB_POS;

	std::list<Dart> memo_cross;

	ParticleCell2DMemo() {};

	ParticleCell2DMemo(Map& map, Dart belonging_cell, VEC3 pos, TAB_POS tabPos, DartMarker& obst) : ParticleCell2D<PFP>(map,belonging_cell,pos,tabPos,obst)
	{};

	void vertexState(const VEC3& current);

	void edgeState(const VEC3& current,Geom::Orientation2D sideOfEdge=Geom::ALIGNED);

	void faceState(const VEC3& current);

	void move(const VEC3& newCurrent);
};

#include "particle_cell_2D_memo.hpp"
//namespace

}

}

}

#endif
