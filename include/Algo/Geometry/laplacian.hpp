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

#include "Algo/Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP, typename ATTR_TYPE>
ATTR_TYPE computeLaplacianTopoVertex(
	typename PFP::MAP& map,
	Dart d,
	const AttributeHandler<ATTR_TYPE>& attr)
{
	ATTR_TYPE l(0) ;
	unsigned int val = 0 ;
	Dart it = d ;
	do
	{
		l += attr[map.phi1(it)] - attr[it] ;
		val++ ;
		it = map.alpha1(it) ;
	} while(it != d) ;
	l /= val ;
	return l ;
}

template <typename PFP, typename ATTR_TYPE>
ATTR_TYPE computeLaplacianCotanVertex(
	typename PFP::MAP& map,
	Dart d,
	const typename PFP::TREAL& edgeWeight,
	const typename PFP::TREAL& vertexArea,
	const AttributeHandler<ATTR_TYPE>& attr)
{
	typedef typename PFP::REAL REAL ;
	ATTR_TYPE l(0) ;
	Dart it = d ;
	REAL vArea = vertexArea[d] ;
	REAL val = 0 ;
	do
	{
		REAL w = edgeWeight[it] / vArea ;
		l += (attr[map.phi1(it)] - attr[it]) * w ;
		val += w ;
		it = map.alpha1(it) ;
	} while(it != d) ;
	l /= val ;
	return l ;
}

template <typename PFP, typename ATTR_TYPE>
void computeLaplacianTopoVertices(
	typename PFP::MAP& map,
	const AttributeHandler<ATTR_TYPE>& attr,
	AttributeHandler<ATTR_TYPE>& laplacian,
	const FunctorSelect& select)
{
	CellMarker marker(map, VERTEX);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			laplacian[d] = computeLaplacianTopoVertex<PFP, ATTR_TYPE>(map, d, attr) ;
		}
	}
}

template <typename PFP, typename ATTR_TYPE>
void computeLaplacianCotanVertices(
	typename PFP::MAP& map,
	const typename PFP::TREAL& edgeWeight,
	const typename PFP::TREAL& vertexArea,
	const AttributeHandler<ATTR_TYPE>& attr,
	AttributeHandler<ATTR_TYPE>& laplacian,
	const FunctorSelect& select)
{
	CellMarker marker(map, VERTEX);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			laplacian[d] = computeLaplacianCotanVertex<PFP, ATTR_TYPE>(map, d, edgeWeight, vertexArea, attr) ;
		}
	}
}

template <typename PFP>
typename PFP::REAL computeCotanWeightEdge(
	typename PFP::MAP& map,
	Dart d,
	const typename PFP::TVEC3& position)
{
	typename PFP::REAL cot_alpha = 1 / tan(angle<PFP>(map, map.phi_1(d), map.phi2(map.phi1(d)), position)) ;
	typename PFP::REAL cot_beta = 0 ;
	Dart dd = map.phi2(d) ;
	if(dd != d)
		cot_beta = 1 / tan(angle<PFP>(map, map.phi_1(dd), map.phi2(map.phi1(dd)), position)) ;
	return 0.5 * ( cot_alpha + cot_beta ) ;
}

template <typename PFP>
void computeCotanWeightEdges(
	typename PFP::MAP& map,
	const typename PFP::TVEC3& position,
	typename PFP::TREAL& edgeWeight,
	const FunctorSelect& select = SelectorTrue())
{
	CellMarker marker(map, EDGE);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			edgeWeight[d] = computeCotanWeightEdge<PFP>(map, d, position) ;
		}
	}
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
