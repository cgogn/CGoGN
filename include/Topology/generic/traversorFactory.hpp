/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Topology/generic/traversor1.h"
#include "Topology/generic/traversor2.h"
#include "Topology/generic/traversor3.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversorDoO.h"

namespace CGoGN
{

template<typename MAP>
Traversor<MAP>* TraversorFactory<MAP>::createIncident(MAP& map, Dart dart, unsigned int dim, unsigned int orbX, unsigned int orbY)
{
	int code = 0x100*dim + 0x10*(orbX-VERTEX) + orbY-VERTEX;

	switch(code)
	{
		case 0x301:
			return new Traversor3XY<MAP, VERTEX, EDGE>(map,dart);
			break;
		case 0x302:
			return new Traversor3XY<MAP, VERTEX, FACE>(map,dart);
			break;
		case 0x303:
			return new Traversor3XY<MAP, VERTEX, VOLUME>(map,dart);
			break;

		case 0x310:
			return new Traversor3XY<MAP, EDGE, VERTEX>(map,dart);
			break;
		case 0x312:
			return new Traversor3XY<MAP, EDGE, FACE>(map,dart);
			break;
		case 0x313:
			return new Traversor3XY<MAP, EDGE, VOLUME>(map,dart);
			break;

		case 0x320:
			return new Traversor3XY<MAP, FACE, VERTEX>(map,dart);
			break;
		case 0x321:
			return new Traversor3XY<MAP, FACE, EDGE>(map,dart);
			break;
		case 0x323:
			return new Traversor3XY<MAP, FACE, VOLUME>(map,dart);
			break;

		case 0x330:
			return new Traversor3XY<MAP, VOLUME, VERTEX>(map,dart);
			break;
		case 0x331:
			return new Traversor3XY<MAP, VOLUME, EDGE>(map,dart);
			break;
		case 0x332:
			return new Traversor3XY<MAP, VOLUME, FACE>(map,dart);
			break;

		case 0x201:
			return new Traversor2VE<MAP>(map,dart);
			break;
		case 0x202:
			return new Traversor2VF<MAP>(map,dart);
			break;
		case 0x210:
			return new Traversor2EV<MAP>(map,dart);
			break;
		case 0x212:
			return new Traversor2EF<MAP>(map,dart);
			break;
		case 0x220:
			return new Traversor2FV<MAP>(map,dart);
			break;
		case 0x221:
			return new Traversor2FE<MAP>(map,dart);
			break;


		case 0x101:
			return new Traversor1VE<MAP>(map,dart);
			break;
		case 0x110:
			return new Traversor1EV<MAP>(map,dart);
			break;
		default:
			return NULL;
			break;
	}
	return NULL;
}

template<typename MAP>
Traversor<MAP>* TraversorFactory<MAP>::createAdjacent(MAP& map, Dart dart, unsigned int dim, unsigned int orbX, unsigned int orbY)
{
	int code = 0x100*dim + 0x10*(orbX-VERTEX) + orbY-VERTEX;

	switch(code)
	{
		case 0x301:
			return new Traversor3XXaY<MAP, VERTEX, EDGE>(map,dart);
			break;
		case 0x302:
			return new Traversor3XXaY<MAP, VERTEX, FACE>(map,dart);
			break;
		case 0x303:
			return new Traversor3XXaY<MAP, VERTEX, VOLUME>(map,dart);
			break;

		case 0x310:
			return new Traversor3XXaY<MAP, EDGE, VERTEX>(map,dart);
			break;
		case 0x312:
			return new Traversor3XXaY<MAP, EDGE, FACE>(map,dart);
			break;
		case 0x313:
			return new Traversor3XXaY<MAP, EDGE, VOLUME>(map,dart);
			break;

		case 0x320:
			return new Traversor3XXaY<MAP, FACE, VERTEX>(map,dart);
			break;
		case 0x321:
			return new Traversor3XXaY<MAP, FACE, EDGE>(map,dart);
			break;
		case 0x323:
			return new Traversor3XXaY<MAP, FACE, VOLUME>(map,dart);
			break;

		case 0x330:
			return new Traversor3XXaY<MAP, VOLUME, VERTEX>(map,dart);
			break;
		case 0x331:
			return new Traversor3XXaY<MAP, VOLUME, EDGE>(map,dart);
			break;
		case 0x332:
			return new Traversor3XXaY<MAP, VOLUME, FACE>(map,dart);
			break;

		case 0x201:
			return new Traversor2VVaE<MAP>(map,dart);
			break;
		case 0x202:
			return new Traversor2VVaF<MAP>(map,dart);
			break;
		case 0x210:
			return new Traversor2EEaV<MAP>(map,dart);
			break;
		case 0x212:
			return new Traversor2EEaF<MAP>(map,dart);
			break;
		case 0x220:
			return new Traversor2FFaV<MAP>(map,dart);
			break;
		case 0x221:
			return new Traversor2FFaE<MAP>(map,dart);
			break;

		case 0x101:
			return new Traversor1VVaE<MAP>(map,dart);
			break;
		case 0x110:
			return new Traversor1EEaV<MAP>(map,dart);
			break;
		default:
			return NULL;
			break;
	}

	return NULL;
}

template<typename MAP>
Traversor<MAP>* TraversorFactory<MAP>::createCell(MAP& map, unsigned int orb, bool forceDartMarker, unsigned int thread)
{
	switch(orb)
	{
		case VERTEX:
			return new TraversorCell<MAP,VERTEX>(map,forceDartMarker,thread);
			break;
		case EDGE:
			return new TraversorCell<MAP,EDGE>(map,forceDartMarker,thread);
			break;
		case FACE:
			return new TraversorCell<MAP,FACE>(map,forceDartMarker,thread);
			break;
		case VOLUME:
			return new TraversorCell<MAP,VOLUME>(map,forceDartMarker,thread);
			break;
		case CC:
			return new TraversorCell<MAP,CC>(map,forceDartMarker,thread);
			break;
		case VERTEX1:
			return new TraversorCell<MAP,VERTEX1>(map,forceDartMarker,thread);
			break;
		case EDGE1:
			return new TraversorCell<MAP,EDGE1>(map,forceDartMarker,thread);
			break;
		case VERTEX2:
			return new TraversorCell<MAP,VERTEX2>(map,forceDartMarker,thread);
			break;
		case EDGE2:
			return new TraversorCell<MAP,EDGE2>(map,forceDartMarker,thread);
			break;
		case FACE2:
			return new TraversorCell<MAP,FACE2>(map,forceDartMarker,thread);
			break;
		default:
			return NULL;
			break;
	}
}

template<typename MAP>
Traversor<MAP>* TraversorFactory<MAP>::createDartsOfOrbits(MAP& map, Dart dart, unsigned int orb)
{
	switch(orb)
	{
		case VERTEX:
			return new TraversorDartsOfOrbit<MAP,VERTEX>(map,dart);
			break;
		case EDGE:
			return new TraversorDartsOfOrbit<MAP,EDGE>(map,dart);
			break;
		case FACE:
			return new TraversorDartsOfOrbit<MAP,FACE>(map,dart);
			break;
		case VOLUME:
			return new TraversorDartsOfOrbit<MAP,VOLUME>(map,dart);
			break;
		case CC:
			return new TraversorDartsOfOrbit<MAP,CC>(map,dart);
			break;
		case VERTEX1:
			return new TraversorDartsOfOrbit<MAP,VERTEX1>(map,dart);
			break;
		case EDGE1:
			return new TraversorDartsOfOrbit<MAP,EDGE1>(map,dart);
			break;
		case VERTEX2:
			return new TraversorDartsOfOrbit<MAP,VERTEX2>(map,dart);
			break;
		case EDGE2:
			return new TraversorDartsOfOrbit<MAP,EDGE2>(map,dart);
			break;
		case FACE2:
			return new TraversorDartsOfOrbit<MAP,FACE2>(map,dart);
			break;
		default:
			return NULL;
			break;
	}
}

}
