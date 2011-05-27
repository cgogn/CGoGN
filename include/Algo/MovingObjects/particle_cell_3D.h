#ifndef PARTCELL_H
#define PARTCELL_H

#include "particle_base.h"

#include "Algo/Geometry/inclusion.h"
#include "Geometry/intersection.h"
#include "Geometry/orientation.h"
#include "Geometry/plane_3d.h"
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
class ParticleCell3D : public ParticleBase
{
public :
	typedef typename PFP::MAP Map;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::TVEC3 TAB_POS;

	Map& m;

	const TAB_POS& position;

	Dart d;
	Dart lastCrossed;

	VEC3 m_positionFace;

	unsigned int state;

	unsigned int crossCell ;

	ParticleCell3D(Map& map) : m(map)
	{}

	ParticleCell3D(Map& map, Dart belonging_cell, VEC3 pos, const TAB_POS& tabPos) :
		ParticleBase(pos), m(map), position(tabPos), d(belonging_cell), state(3)
	{
		m_positionFace = pointInFace(d);
	}

	void display();

	Dart getCell() { return d; }

	VEC3 pointInFace(Dart d);

	Geom::Orientation3D isLeftENextVertex(VEC3 c, Dart d, VEC3 base);

	bool isRightVertex(VEC3 c, Dart d, VEC3 base);

	Geom::Orientation3D whichSideOfFace(VEC3 c, Dart d);

	Geom::Orientation3D isLeftL1DVol(VEC3 c, Dart d, VEC3 base, VEC3 top);

	Geom::Orientation3D isRightDVol(VEC3 c, Dart d, VEC3 base, VEC3 top);

	Geom::Orientation3D isAbove(VEC3 c, Dart d, VEC3 top);

	int isLeftL1DFace(VEC3 c, Dart d, VEC3 base, VEC3 normal);

	bool isRightDFace(VEC3 c, Dart d, VEC3 base, VEC3 normal);

	Dart nextDartOfVertexNotMarked(Dart d, CellMarker& mark);

	Dart nextNonPlanar(Dart d);

	Dart nextFaceNotMarked(Dart d,CellMarker& mark);

	Geom::Orientation3D whichSideOfEdge(VEC3 c, Dart d);

	bool isOnHalfEdge(VEC3 c, Dart d);

	void vertexState(const VEC3& current);

	void edgeState(const VEC3& current);

	void faceState(const VEC3& current, Geom::Orientation3D sideOfFace=Geom::ON);

	void volumeState(const VEC3& current);

	void volumeSpecialCase(const VEC3& current);


	void move(const VEC3& newCurrent)
	{
		if(!Geom::arePointsEquals(newCurrent, m_position))
		{
			switch(state) {
			case VERTEX_ORBIT : vertexState(newCurrent); break;
			case EDGE_ORBIT : 	edgeState(newCurrent);   break;
			case FACE_ORBIT : 	faceState(newCurrent);   break;
			case VOLUME_ORBIT : volumeState(newCurrent);   break;
			}

			display();
		}
	}
};

#include "particle_cell_3D.hpp"

}

}

}

#endif
