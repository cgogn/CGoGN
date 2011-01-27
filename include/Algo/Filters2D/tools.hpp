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

#include "Geometry/distances.h"

#include <time.h>
#include <stdlib.h>
#include <limits>

namespace CGoGN
{

namespace Algo
{

namespace Filters2D
{

template <typename PFP>
float computeHaussdorf(typename PFP::MAP& map, const typename PFP::TVEC3& originalPosition, const typename PFP::TVEC3& position2, const FunctorSelect& select)
{
	typedef typename PFP::VEC3 VEC3 ;

	float dist_o = 0.0f ;
	float dist_f = 0.0f ;

	CellMarker mv(map, VERTEX_CELL) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !mv.isMarked(d))
		{
			mv.mark(d);

			const VEC3& posO = originalPosition[d] ;
			const VEC3& posF = position2[d] ;

			float dv_o = std::numeric_limits<float>::max();
			float dv_f = std::numeric_limits<float>::max();

			// just test the faces around the vertex => warning not real haussdorff distance!
			Dart dd = d ;
			do
			{
				Dart e = map.phi1(dd) ;
				const VEC3& Bo = originalPosition[e] ;
				const VEC3& Bf = position2[e] ;
				e = map.phi1(e) ;
				const VEC3& Co = originalPosition[e] ;
				const VEC3& Cf = position2[e] ;

				float d = Geom::squaredDistancePoint2Triangle(posO, posF, Bf, Cf) ;
				if(d < dv_o)
					dv_o = d ;
				d = Geom::squaredDistancePoint2Triangle(posF, posO, Bo, Co) ;
				if(d < dv_f)
					dv_f = d ;

				dd = map.phi2(e) ;
			} while(dd != d) ;

			if(dv_o > dist_o)
				dist_o = dv_o ;
			if(dv_f > dist_f)
				dist_f = dv_f ;
		}
	}

	if (dist_f > dist_o)
		return sqrtf(dist_f) ;
	return sqrtf(dist_o) ;
}

template <typename PFP>
void computeNoise(typename PFP::MAP& map, long amount, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select)
{
	typedef typename PFP::VEC3 VEC3 ;

	// init the seed for random
	srand(time(NULL)) ;

	// apply noise on each vertex
	CellMarker mv(map, VERTEX_CELL) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !mv.isMarked(d))
		{
			mv.mark(d);

			const VEC3& pos = position[d] ;
			VEC3 norm = normal[d] ;

			float r1 = float(rand() % amount) / 100.0f ;
			float r2 = 0 ;
			if (amount >= 5)
				r2 = float(rand() % (amount/5)) / 100.0f ;

			long sign = rand() % 2 ;
			if (sign == 1) norm *= -1.0f ;
			float avEL = 0.0f ;
			VEC3 td(0) ;

			Dart dd = d ;
			long nbE = 0 ;
			do	// turn around vertex and get each neighbor vertex
			{
				Dart e = map.phi2(dd) ;
				const VEC3& p = position[e] ;
				VEC3 vec = p - pos ;
				float el = vec.norm() ;
				vec *= r2 ;
				td += vec ;
				avEL += el ;
				nbE++ ;
				dd = map.phi1(e) ;
			} while (dd != d) ;

			avEL /= float(nbE) ;
			norm *= avEL * r1 ;
			norm += td ;
			position2[d] = pos + norm ;
		}
	}
}

} //namespace Filters2D

} //namespace Algo

} //namespace CGoGN
