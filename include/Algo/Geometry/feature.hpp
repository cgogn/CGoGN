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
std::vector<typename PFP::VEC3> occludingContoursDetection(typename PFP::MAP& map, const typename PFP::VEC3& cameraPosition, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal)
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

			VEC3 p1 = position[d] ;
			VEC3 p2 = position[map.phi1(d)] ;
			VEC3 p3 = position[map.phi_1(d)] ;

			REAL dp1 = (p1 - cameraPosition) * normal[d] ;
			REAL dp2 = (p2 - cameraPosition) * normal[map.phi1(d)] ;
			REAL dp3 = (p3 - cameraPosition) * normal[map.phi_1(d)] ;

			if(dp1 < 0 && dp2 > 0)
			{
				REAL alpha = -dp1 / (-dp1 + dp2) ;
				occludingContours.push_back(alpha * p1 + (1 - alpha) * p2) ;
			}
			if(dp2 < 0 && dp1 > 0)
			{
				REAL alpha = dp1 / (dp1 - dp2) ;
				occludingContours.push_back(alpha * p1 + (1 - alpha) * p2) ;
			}
			if(dp1 < 0 && dp3 > 0)
			{
				REAL alpha = -dp1 / (-dp1 + dp3) ;
				occludingContours.push_back(alpha * p1 + (1 - alpha) * p3) ;
			}
			if(dp3 < 0 && dp1 > 0)
			{
				REAL alpha = dp1 / (dp1 - dp3) ;
				occludingContours.push_back(alpha * p1 + (1 - alpha) * p3) ;
			}
			if(dp2 < 0 && dp3 > 0)
			{
				REAL alpha = -dp2 / (-dp2 + dp3) ;
				occludingContours.push_back(alpha * p2 + (1 - alpha) * p3) ;
			}
			if(dp3 < 0 && dp2 > 0)
			{
				REAL alpha = dp2 / (dp2 - dp3) ;
				occludingContours.push_back(alpha * p2 + (1 - alpha) * p3) ;
			}
		}
	}

	return occludingContours ;
}

template <typename PFP>
std::vector<typename PFP::VEC3> featureLinesDetection(typename PFP::MAP& map, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	std::vector<VEC3> featureLines ;

	Geom::BoundingBox<VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(map, position) ;

	AttributeHandler<REAL> edgeangle = map.template getAttribute<REAL>(EDGE, "edgeangle") ;
	if(!edgeangle.isValid())
	{
		edgeangle = map.template addAttribute<REAL>(EDGE, "edgeangle") ;
		Algo::Geometry::computeAnglesBetweenNormalsOnEdges<PFP>(map, position, edgeangle) ;
	}

	AttributeHandler<REAL> kmax = map.template getAttribute<REAL>(VERTEX, "kmax") ;
	AttributeHandler<REAL> kmin = map.template getAttribute<REAL>(VERTEX, "kmin") ;
	AttributeHandler<VEC3> Kmax = map.template getAttribute<VEC3>(VERTEX, "Kmax") ;
	AttributeHandler<VEC3> Kmin = map.template getAttribute<VEC3>(VERTEX, "Kmin") ;
	AttributeHandler<VEC3> Knormal = map.template getAttribute<VEC3>(VERTEX, "Knormal") ;
	// as all these attributes are computed simultaneously by computeCurvatureVertices
	// one can assume that if one of them is not valid, the others must be created too
	if(!kmax.isValid())
	{
		kmax = map.template addAttribute<REAL>(VERTEX, "kmax") ;
		kmin = map.template addAttribute<REAL>(VERTEX, "kmin") ;
		Kmax = map.template addAttribute<VEC3>(VERTEX, "Kmax") ;
		Kmin = map.template addAttribute<VEC3>(VERTEX, "Kmin") ;
		Knormal = map.template addAttribute<VEC3>(VERTEX, "Knormal") ;
		Algo::Geometry::computeCurvatureVertices_NormalCycles<PFP>(map, 0.01f * bb.diagSize(), position, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;
	}

	AttributeHandler<VEC3> kmaxGrad = map.template addAttribute<VEC3>(FACE, "kmaxGrad") ;
	AttributeHandler<VEC3> kminGrad = map.template addAttribute<VEC3>(FACE, "kminGrad") ;

	CellMarker mf(map, FACE) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!mf.isMarked(d))
		{
			mf.mark(d) ;
			Dart v1 = d ;
			Dart v2 = myMap.phi1(d) ;
			Dart v3 = myMap.phi_1(d) ;
			VEC3 v3v2 = position[v3] - position[v2] ;
			VEC3 v1v3 = position[v1] - position[v3] ;
			VEC3 v2v1 = position[v2] - position[v1] ;
			VEC3 n = Geom::triangleNormal(position[v1], position[v2], position[v3]) ;
			n.normalize() ;
			VEC3 g1 = (n^v3v2 * kmax[v1]) + (n^v1v3 * kmax[v2]) + (n^v2v1 * kmax[v3]) ;
			VEC3 g2 = (n^v3v2 * kmin[v1]) + (n^v1v3 * kmin[v2]) + (n^v2v1 * kmin[v3]) ;
			g1.normalize() ;
			kmaxGrad[d] = g1 ;
			kminGrad[d] = g2 ;
		}
	}
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
