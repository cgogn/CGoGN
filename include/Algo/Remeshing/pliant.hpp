/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Algo/Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Remeshing
{

template <typename PFP>
void pliantRemeshing(typename PFP::MAP& map, typename PFP::TVEC3& position)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	DartMarker m(map) ;

	REAL meanEdgeLength = 0 ;
	unsigned int nbEdges = 0 ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.markOrbit(EDGE_ORBIT, d) ;
			meanEdgeLength += Algo::Geometry::edgeLength<PFP>(map, d, position) ;
			++nbEdges ;
		}
	}
	meanEdgeLength /= REAL(nbEdges) ;

	REAL edgeLengthInf = REAL(4) / REAL(5) * meanEdgeLength ;
	REAL edgeLengthSup = REAL(4) / REAL(3) * meanEdgeLength ;

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(m.isMarked(d))
		{
			m.unmarkOrbit(EDGE_ORBIT, d) ;
			REAL length = Algo::Geometry::edgeLength<PFP>(map, d, position) ;
			if(length > edgeLengthSup)
			{
				Dart dd = map.phi2(d) ;
				VEC3 p = REAL(0.5) * (position[d] + position[dd]) ;
				map.cutEdge(d) ;
				position[map.phi1(d)] = p ;
				map.splitFace(map.phi1(d), map.phi_1(d)) ;
				map.splitFace(map.phi1(dd), map.phi_1(dd)) ;
			}
		}
	}

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.markOrbit(EDGE_ORBIT, d) ;
			REAL length = Algo::Geometry::edgeLength<PFP>(map, d, position) ;
			if(length < edgeLengthInf)
			{
				bool collapse = true ;
				Dart dd = map.phi2(d) ;
				VEC3 p = position[dd] ;
				Dart vit = map.alpha1(d) ;
				do
				{
					VEC3 vec = position[dd] - position[map.phi1(vit)] ;
					if(vec.norm() > edgeLengthSup)
						collapse = false ;
					vit = map.alpha1(vit) ;
				} while(vit != d && collapse) ;
				if(collapse)
				{
					Dart v = map.collapseEdge(d) ;
					position[v] = p ;
				}
			}
		}
	}
}

} // namespace Remeshing

} // namespace Algo

} // namespace CGoGN
