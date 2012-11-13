#ifndef PARTCELL2DANDHALFMEMO_H
#define PARTCELL2DANDHALFMEMO_H

#include "particle_cell_2DandHalf.h"

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
class ParticleCell2DAndHalfMemo : public ParticleCell2DAndHalf<PFP>
{

	public :
	typedef typename PFP::MAP Map;
	typedef typename PFP::VEC3 VEC3;
	typedef VertexAttribute<typename PFP::VEC3> TAB_POS;

	std::list<Dart> memo_cross;
	bool detect_vertex;
	bool detect_edge;
	bool detect_face;
	ParticleCell2DAndHalfMemo() {};

	ParticleCell2DAndHalfMemo(Map& map, Dart belonging_cell, VEC3 pos,const TAB_POS& tabPos) : ParticleCell2DAndHalf<PFP>(map,belonging_cell,pos,tabPos),detect_vertex(false),detect_edge(false),detect_face(true)
	{
//	 memo_cross.push_back(this->d);
	};

	void vertexState(VEC3 current);

	void edgeState(VEC3 current,Geom::Orientation3D sideOfEdge=Geom::ALIGNED);

	void faceState(VEC3 current);

	void move(const VEC3& newCurrent);
};

#include "particle_cell_2DandHalf_memo.hpp"
//namespace

}

}

}

#endif
