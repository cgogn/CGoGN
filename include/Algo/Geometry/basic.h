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

#ifndef __ALGO_GEOMETRY_BASIC_H__
#define __ALGO_GEOMETRY_BASIC_H__

#include "Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

/**
 * vectorOutOfDart return a dart from the position of vertex attribute of d to the position of vertex attribute of phi1(d)
 */
template <typename PFP>
typename PFP::VEC3 vectorOutOfDart(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typename PFP::VEC3 vec = position[map.phi1(d)] ;
	vec -= position[d] ;
	return vec ;
}

template <typename PFP>
float angle(typename PFP::MAP& map, Dart d1, Dart d2, const typename PFP::TVEC3& position)
{
	typename PFP::VEC3 v1, v2 ;
	vectorOutOfDart<PFP>(map, d1, position, v1) ;
	vectorOutOfDart<PFP>(map, d2, position, v2) ;
	return Geom::angle(v1, v2) ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#endif
