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

namespace Geometry
{

template <typename PFP>
void computeLaplacianVertices(
		typename PFP::MAP& map,
		LaplacianType type,
		const typename PFP::TVEC3& position,
		typename PFP::TVEC3& laplacian,
		const FunctorSelect& select)
{
	CellMarker marker(map, VERTEX_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			switch(type)
			{
				case TOPOLOGICAL : {
					computeLaplacianVertex_Topo<PFP>(map, d, position, laplacian) ;
					break ; }
			}
		}
	}
}

template <typename PFP>
void computeLaplacianVertex_Topo(
	typename PFP::MAP& map,
	Dart d,
	const typename PFP::TVEC3& position,
	typename PFP::TVEC3& laplacian)
{
	typedef typename PFP::VEC3 VEC3 ;
	VEC3 l(0) ;
	unsigned int val = 0 ;
	Dart dd = d ;
	do
	{
		l += vectorOutOfDart<PFP>(map, dd, position) ;
		val++ ;
		dd = map.alpha1(dd) ;
	} while(dd != d) ;
	l /= val ;
	laplacian[d] = l ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
