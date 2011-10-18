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

#include "Geometry/basic.h"
#include "Algo/Geometry/normal.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
void featureEdgeDetection(typename PFP::MAP& map, typename PFP::TVEC3& position, CellMarker& featureEdge)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	featureEdge.unmarkAll() ;

	AttributeHandler<VEC3> fNormal = map.template getAttribute<VEC3>(FACE, "normal") ;
	if(!fNormal.isValid())
		fNormal = map.template addAttribute<VEC3>(FACE, "normal") ;
	Algo::Geometry::computeNormalFaces<PFP>(map, position, fNormal) ;

	CellMarker m(map, EDGE) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.mark(d) ;
			if(Geom::angle(fNormal[d], fNormal[map.phi2(d)]) > M_PI / REAL(6))
				featureEdge.mark(d) ;
		}
	}

//	map.template removeAttribute<VEC3>(fNormal) ;
}

template <typename PFP>
std::vector<typename PFP::VEC3> occludingContoursDetection(typename PFP::MAP& map, const typename PFP::VEC3& viewDir, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	std::vector<VEC3> occludingContours ;

	CellMarker m(map, FACE) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.mark(d) ;
			REAL dp1 = viewDir * normal[d] ;
			REAL dp2 = viewDir * normal[map.phi1(d)] ;
			REAL dp3 = viewDir * normal[map.phi_1(d)] ;
			if(dp1 < 0 && dp2 > 0)
			{
				REAL alpha = -dp1 / (-dp1 + dp2) ;
				occludingContours.push_back(alpha * position[d] + (1 - alpha) * position[map.phi1(d)]) ;
			}
			if(dp2 < 0 && dp1 > 0)
			{
				REAL alpha = dp1 / (dp1 - dp2) ;
				occludingContours.push_back(alpha * position[d] + (1 - alpha) * position[map.phi1(d)]) ;
			}
			if(dp1 < 0 && dp3 > 0)
			{
				REAL alpha = -dp1 / (-dp1 + dp3) ;
				occludingContours.push_back(alpha * position[d] + (1 - alpha) * position[map.phi_1(d)]) ;
			}
			if(dp3 < 0 && dp1 > 0)
			{
				REAL alpha = dp1 / (dp1 - dp3) ;
				occludingContours.push_back(alpha * position[d] + (1 - alpha) * position[map.phi_1(d)]) ;
			}
			if(dp2 < 0 && dp3 > 0)
			{
				REAL alpha = -dp2 / (-dp2 + dp3) ;
				occludingContours.push_back(alpha * position[map.phi1(d)] + (1 - alpha) * position[map.phi_1(d)]) ;
			}
			if(dp3 < 0 && dp2 > 0)
			{
				REAL alpha = dp2 / (dp2 - dp3) ;
				occludingContours.push_back(alpha * position[map.phi1(d)] + (1 - alpha) * position[map.phi_1(d)]) ;
			}
		}
	}

	return occludingContours ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
