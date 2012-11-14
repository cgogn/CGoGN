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

#include <limits>

namespace CGoGN
{

namespace Geom
{

template <typename VEC3>
Orientation2D testOrientation2D(const VEC3& P, const VEC3& Pa, const VEC3& Pb)
{

	typedef typename VEC3::DATA_TYPE T ;
//	const T min = std::numeric_limits<T>::min()*T(100);
	const T min = 0.0001;


//	T wsof = (Pa[0]-P[0])*(P[1]-Pb[1])-(P[0]-Pb[0])*(Pa[1]-P[1]);
	T wsof = (P[0]-Pa[0])*(Pb[1]-Pa[1])-(Pb[0]-Pa[0])*(P[1]-Pa[1]);

	if(wsof>min)
		return RIGHT ;
	else if(fabs(wsof)>min)
		return LEFT;

	return ALIGNED;

// 	VEC3 dir = Pb - Pa ;
// 	VEC3 Np = dir ^ N ;
// 	int o3d = testOrientation3D(P, Np, Pa) ;
// 	switch(o3d)
// 	{
// 		case ON : return ALIGNED ;
// 		case OVER : return RIGHT ;
// 		case UNDER : return LEFT ;
// 	}
}

template <typename VEC3>
bool isTetrahedronWellOriented(const VEC3 points[4], bool CCW)
{
	typedef typename VEC3::DATA_TYPE T ;

	VEC3 AB = points[1] - points[0] ;
	VEC3 AC = points[2] - points[0] ;
	VEC3 AD = points[3] - points[0] ;

	VEC3 N = AB ^ AC ;

	T dot = N * AD ;
	if(CCW)
		return dot <= 0 ;
	else
		return dot >= 0 ;
}

}

}
