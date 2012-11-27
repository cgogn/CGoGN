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
void mergeVertex(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& positions, Dart d, Dart e)
{
	assert(Geom::arePointsEquals(positions[d], positions[e]) && !map.sameVertex(d, e)) ;

	typedef typename PFP::VEC3 VEC3;
	VEC3 p = positions[d] ;

	bool notempty = true ;
	do // while vertex of e contains more than one dart
	{
		Dart e1 = map.alpha1(e) ;			// e1 stores next dart of vertex of e
		if (e1 == e)
			notempty = false ;				// last dart of vertex of e
		else {
			map.removeEdgeFromVertex(e) ;	// detach e from its vertex
		}
		// Searchs where e may be inserted in the vertex of d
		Dart d1 = d ;
		do
		{
			if (CGoGN::Algo::BooleanOperator::isBetween<PFP>(map, positions, e, d,
			                                                 map.alpha1(d))) break ;
			d = map.alpha1(d) ;
		} while (d != d1) ;

		// Inserted e in the correct place (after d)
		map.insertEdgeInVertex(d, e) ;

		// Go on with next darts
		d = e ;
		e = e1 ;
	} while (notempty) ;

	// 0-embed z on the merged vertex
	positions[d] = p ;
}

template <typename PFP>
void mergeVertices(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& positions)
{
	// TODO optimiser en triant les sommets
	for(Dart d = map.begin() ; d != map.end() ; map.next(d))
	{
		CellMarker<VERTEX> vM(map);
		vM.mark(d);
		for(Dart dd = map.begin() ; dd != map.end() ; map.next(dd))
		{
			if(!vM.isMarked(dd))
			{
				if(Geom::arePointsEquals(positions[d],positions[dd]))
				{
					if (map.sameVertex(d,dd)) std::cout << "fusion: sameVertex" << std::endl ;
					if (!map.sameVertex(d,dd)) mergeVertex<PFP>(map,positions,d,dd);
//					vM.mark(d);
				}
			}
		}
	}
}

}

}

}
