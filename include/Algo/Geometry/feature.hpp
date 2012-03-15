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
#include "Topology/generic/traversorCell.h"

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

	TraversorE<typename PFP::MAP> t(map) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		if(!map.isBoundaryEdge(d) && Geom::angle(fNormal[d], fNormal[map.phi2(d)]) > M_PI / REAL(6))
			featureEdge.mark(d) ;
	}

//	map.template removeAttribute<VEC3>(fNormal) ;
}

template <typename PFP>
void computeArea(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TREAL& area, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorF<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		area[d] = faceArea<PFP>(map, d, position)[1] ;
}

template <typename PFP>
void computeFaceGradient(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& face_gradient, const typename PFP::TVEC3& face_normal, const typename PFP::TREAL& kmax, const typename PFP::TREAL& area, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorF<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		face_gradient[d] = faceGradient<PFP>(map, d, position, face_normal, kmax, area) ;
}

template <typename PFP>
void computeGradient(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& gradient, typename PFP::TVEC3& face_gradient, const typename PFP::TREAL& area, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorV<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		gradient[d] = vertexGradient<PFP>(map, d, position, face_gradient, area) ;
}

template <typename PFP>
void computeTriangleType(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& triangle_type, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorF<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		triangle_type[d] = triangleType<PFP>(map, d, position) ;
}

template <typename PFP>
void computeCurvatureSign(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& triangle_type, typename PFP::TVEC3& k, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorV<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		if (/*sommet non traité*/ true)
		{
			curvatureSign(map, d, position, triangle_type, k) ;
		}
	}
}

template <typename PFP>
std::vector<typename PFP::VEC3> occludingContoursDetection(typename PFP::MAP& map, const typename PFP::VEC3& cameraPosition, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	std::vector<VEC3> occludingContours ;

	TraversorF<typename PFP::MAP> t(map) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
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

	return occludingContours ;
}

template <typename PFP>
float faceArea(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

	float A [1] ;

	VEC3 pos1(0) ;
	VEC3 pos2(0) ;
	VEC3 pos3(0) ;

	Traversor2FV<typename PFP::MAP> t(map, d) ;
	Dart it = t.begin() ;

	pos1 += position[it] ;
	it = t.next ;
	pos2 += position[it] ;
	it = t.next ;
	pos3 += position[it] ;

	A = 1/2 * abs(
				( (pos2.x - pos1.x) * (pos3.y - pos1.y) )
				-
				( (pos3.x - pos1.x) * (pos2.y - pos1.y) )
				) ;

	return A ;
}

template <typename PFP>
typename PFP::VEC3 faceGradient(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, const typename PFP::TVEC3& face_normal, const typename PFP::TREAL& kmax, const typename PFP::TREAL& area)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

	VEC3 G(0) ;

	VEC3 pos1(0) ;
	VEC3 pos2(0) ;
	VEC3 pos3(0) ;

	VEC3 n = face_normal[d] ;

	REAL k1 ;
	REAL k2 ;
	REAL k3 ;

	REAL a = area[d] ;

	Traversor2FV<typename PFP::MAP> t(map, d) ;
	Dart it = t.begin() ;

	pos1 += position[it] ;
	k1 = kmax[it] ;
	it = t.next() ;
	pos2 += position[it] ;
	k2 = kmax[it] ;
	it = t.next() ;
	pos3 += position[it] ;
	k3 = kmax[it] ;

	G += k1 * ( ( n * ( pos3 - pos2 ) ) / ( 2 * a ) ) +
			k2 * ( ( n * ( pos1 - pos3 ) ) / ( 2 * a ) ) +
			k3 * ( ( n * ( pos2 - pos1 ) ) / ( 2 * a ) ) ;

	G.normalize() ;
	return G ;
}

template <typename PFP>
typename PFP::VEC3 triangleType(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

	VEC3 C(0) ;

	VEC3 pos1(0) ;
	VEC3 pos2(0) ;
	VEC3 pos3(0) ;

	Traversor2FV<typename PFP::MAP> t(map, d) ;
	Dart it = t.begin() ;

	pos1 += position[it] ;
	it = t.next() ;
	pos2 += position[it] ;
	it = t.next() ;
	pos3 += position[it] ;

	if ( isInSameOctant(pos1,pos2) && isInSameOctant(pos1,pos3) )
	{
		//regular triangle
		C = (1, 0, 0) ;
	}
	else
	{
		//singular triangle
		C = (0, 0, 1) ;
	}

	C.normalize() ;
	return C ;
}

bool isInSameOctant(const typename PFP::TVEC3& pos1, const typename PFP::TVEC3& pos2)
{
	return ( ( pos1[0] > 0 && pos1[1] > 0 && pos1[2] > 0 && //one of the eight octant
			( ( pos2[0] > 0 && pos2[1] > 0 && pos2[2] > 0 ) or //one choice of sign
			( pos2[0] < 0 && pos2[1] < 0 && pos2[2] < 0 ) ) ) or //the other choice

			( pos1[0] > 0 && pos1[1] > 0 && pos1[2] < 0 &&
			( ( pos2[0] > 0 && pos2[1] > 0 && pos2[2] < 0 ) or
			( pos2[0] < 0 && pos2[1] < 0 && pos2[2] > 0 ) ) ) or

			( pos1[0] > 0 && pos1[1] < 0 && pos1[2] > 0 &&
			( ( pos2[0] > 0 && pos2[1] < 0 && pos2[2] > 0 ) or
			( pos2[0] < 0 && pos2[1] > 0 && pos2[2] < 0 ) ) ) or

			( pos1[0] > 0 && pos1[1] < 0 && pos1[2] < 0 &&
			( ( pos2[0] > 0 && pos2[1] < 0 && pos2[2] < 0 ) or
			( pos2[0] < 0 && pos2[1] > 0 && pos2[2] > 0 ) ) ) or

			( pos1[0] < 0 && pos1[1] > 0 && pos1[2] > 0 &&
			( ( pos2[0] < 0 && pos2[1] > 0 && pos2[2] > 0 ) or
			( pos2[0] > 0 && pos2[1] < 0 && pos2[2] < 0 ) ) ) or

			( pos1[0] < 0 && pos1[1] > 0 && pos1[2] < 0 &&
			( ( pos2[0] < 0 && pos2[1] > 0 && pos2[2] < 0 ) or
			( pos2[0] > 0 && pos2[1] < 0 && pos2[2] > 0 ) ) ) or

			( pos1[0] < 0 && pos1[1] < 0 && pos1[2] > 0 &&
			( ( pos2[0] < 0 && pos2[1] < 0 && pos2[2] > 0 ) or
			( pos2[0] > 0 && pos2[1] > 0 && pos2[2] < 0 ) ) ) or

			( pos1[0] < 0 && pos1[1] < 0 && pos1[2] < 0 &&
			( ( pos2[0] < 0 && pos2[1] < 0 && pos2[2] < 0 ) or
			( pos2[0] > 0 && pos2[1] > 0 && pos2[2] > 0 ) ) ) ) ;
}

void curvatureSign(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, typename PFP::TVEC3& triangle_type, typename PFP::TVEC3& k)
{
	/*sommetTraité = true ;*/

	Traversor2VE<typename PFP::MAP> trav2(map, d) ;
	for (Dart d2 = trav2.begin(); d2 != trav2.end(); d2 = trav2.next())
	{
		if (/*arrete non traité*/ true)
		{
			if((k[d] * k[d2]) <= 0)
			{
				k[d2] *= -1 ;
			}
			/*arreteTraité = true ;*/

			/*meme chose avec sommet le long de l'arrete*/
			/*curvatureSign(sommet le long del'arrete) ;*/
		}
	}
}

template <typename PFP>
typename PFP::TVEC3 vertexGradient(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, const typename PFP::TVEC3& face_gradient, const typename PFP::TREAL& area)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

	VEC3 G(0) ;

	REAL A = 0 ;

	Traversor2VF<typename PFP::MAP> t(map, d) ;
	for (Dart d = t.begin(); d != t.end(); d = t.next())
	{
		G += area[d] * face_gradient[d] ;
		A += area[d] ;
	}

	G = G / A ;

	G.normalize() ;
	return G ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
