/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN
{

namespace Algo
{

namespace BooleanOperator
{

template <typename PFP>
void mergeVertex(typename PFP::MAP& map, const typename PFP::TVEC3& positions, Dart d, Dart e)
{
	assert(Geom::arePointsEquals(positions[d],positions[e]) && !map.sameVertex(d,e));
	Dart dd;
	do
	{
		dd = map.alpha1(d);
		map.removeEdgeFromVertex(dd);
		Dart ee = e;
		do
		{
			if(Geom::testOrientation2D(positions[map.phi1(dd)],positions[ee],positions[map.phi1(ee)])!=Geom::RIGHT
					&& Geom::testOrientation2D(positions[map.phi1(dd)],positions[ee],positions[map.phi1(map.alpha1(ee))])==Geom::RIGHT)
			{
				break;
			}
			ee = map.alpha1(ee);
		} while(ee != e);
		map.insertEdgeInVertex(ee,dd);
	} while(dd!=d);
}

template <typename PFP>
void mergeVertices(typename PFP::MAP& map, const typename PFP::TVEC3& positions)
{
	for(Dart d = map.begin() ; d != map.end() ; map.next(d))
	{
		CellMarker vM(map,VERTEX);
		vM.mark(d);
		for(Dart dd = map.begin() ; dd != map.end() ; map.next(dd))
		{
			if(!vM.isMarked(dd))
			{
				vM.mark(dd);
				if(Geom::arePointsEquals(positions[d],positions[dd]))
				{
					mergeVertex<PFP>(map,positions,d,dd);
				}
			}
		}
	}
}

}

}

}
