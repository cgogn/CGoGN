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

namespace CGoGN
{

namespace Algo
{

namespace BooleanOperator
{

template <typename PFP>
bool isBetween(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, Dart d, Dart e, Dart f)
{
	return CGoGN::Geom::isBetween(positions[map.phi1(d)]-positions[d],
	                 positions[map.phi1(e)]-positions[e],
	                 positions[map.phi1(f)]-positions[f]);
}

template <typename PFP>
void mergeVertex(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, Dart d, Dart e)
{
	assert(Geom::arePointsEquals(positions[d],positions[e]) && !map.sameVertex(d,e));
  // d1 traverses the vertex of d (following the alpha1 permutation)
  // y is a temporay buffer to stop the loop
  Dart d1=d;
  // e1 traverses the vertex of e (following the alpha1 permutation)
  Dart e1=e;
  bool notempty = true;
  do {
	if (map.phi2_1(e1) == e1) notempty = false;
	// detach z from its vertex
	map.removeEdgeFromVertex(e1);
	// Searchs the dart of the vertex of x where tz may be inserted
	Dart nd1 = d1;
	do {
	  if (CGoGN::Algo::BooleanOperator::isBetween<PFP>(map,positions,e1,d1,map.phi2_1(d1))) break;
	  d1 = map.phi2_1(d1);
	} while (d1 != nd1);
	map.insertEdgeInVertex(d1,e1);
	d1 = e1;
  } while (notempty);

  // 0-embed z on the vertex of x without copy of the vertex
//  positions[d] = ;
}



template <typename PFP>
void mergeVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions)
{
	// TODO optimiser en triant les sommets
	for(Dart d = map.begin() ; d != map.end() ; map.next(d))
	{
		CellMarker<VERTEX> vM(map);
		vM.mark(d);
		std::cout << "." ; std::cout.flush() ;
		for(Dart dd = map.begin() ; dd != map.end() ; map.next(dd))
		{
			if(!vM.isMarked(dd))
			{
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
