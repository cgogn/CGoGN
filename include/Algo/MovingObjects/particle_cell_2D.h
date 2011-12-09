#ifndef PARTCELL_H
#define PARTCELL_H

#include "particle_base.h"

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

enum {
	NO_CROSS,
	CROSS_EDGE,
	CROSS_OTHER
};

template <typename PFP>
class ParticleCell2D : public ParticleBase
{
public :
	typedef typename PFP::MAP Map;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::TVEC3 TAB_POS;

	Map& m;

	const TAB_POS& m_positions;
	
	Dart d;
	Dart lastCrossed;

	unsigned int state;

	unsigned int crossCell ;

	ParticleCell2D(Map& map) : m(map)
	{}

	ParticleCell2D(Map& map, Dart belonging_cell, VEC3 pos, const TAB_POS& tabPos) :
		ParticleBase(pos), m(map), m_positions(tabPos), d(belonging_cell), lastCrossed(belonging_cell), state(FACE), crossCell(NO_CROSS)
	{}

	Dart getCell() { return d; }

	Geom::Orientation2D getOrientationEdge(const VEC3& point, Dart d);

	void display();

// 	template <unsigned int DD, typename TT>
// 	friend std::istream& operator>> (std::istream& in, Vector<DD,TT>& v) ;

	VEC3 pointInFace(Dart d);

	VEC3 intersectLineEdge(const VEC3& pA, const VEC3& pB, Dart d);

	Geom::Orientation2D getOrientationFace(VEC3 sourcePoint, VEC3 point, Dart d);

	void vertexState(const VEC3& current);

	void edgeState(const VEC3& current, Geom::Orientation2D sideOfEdge=Geom::ALIGNED);

	//just an orientation test : check which dart is aimed to leave the current face to reach an other position
	Dart faceOrientationState(const VEC3& toward);

	void faceState(const VEC3& current);

	virtual unsigned int getState()
	{
		return state;
	}

	void move(const VEC3& newCurrent)
	{
		crossCell = NO_CROSS ;
		if(!Geom::arePointsEquals(newCurrent, m_position))
		{
			switch(state) {
			case VERTEX : vertexState(newCurrent); break;
			case EDGE : 	edgeState(newCurrent);   break;
			case FACE : 	faceState(newCurrent);   break;
			}

			display();
		}
		else
			m_position = newCurrent;
	}
};

#include "particle_cell_2D.hpp"

}

}

}

#endif
