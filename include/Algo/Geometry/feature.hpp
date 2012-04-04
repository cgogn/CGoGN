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
void computeTriangleType(typename PFP::MAP& map, const typename PFP::TVEC3& position, CellMarker& regularMarker, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorF<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		if(triangleType<PFP>(map, d, position))
			regularMarker.mark(d) ;
}

template <typename PFP>
void computeRidgeLines(typename PFP::MAP& map, const typename PFP::TVEC3& position, CellMarker& regularMarker, const typename PFP::TVEC3& faceGradient, const typename PFP::TREAL& area, typename PFP::TVEC3& k, AttributeHandler<ridgeSegment>& ridge_segments, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorF<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		if (regularMarker.isMarked(d))
		{
			ridgeLines<PFP>(map, d, position, regularMarker, k, faceGradient, area, ridge_segments) ;
		}
	}
}

template <typename PFP>
void initRidgeSegments(typename PFP::MAP& map, const typename PFP::TVEC3& position, AttributeHandler<ridgeSegment>& ridge_segments, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorF<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		ridge_segments[d].valid = EMPTY ;
	}
}

template <typename PFP>
void computeSingularTriangle(typename PFP::MAP& map, const typename PFP::TVEC3& position, CellMarker& regularMarker, AttributeHandler<ridgeSegment>& ridge_segments, const FunctorSelect& select = allDarts, unsigned int thread = 0)
{
	TraversorF<typename PFP::MAP> trav(map, select, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
	{
		if (! regularMarker.isMarked(d))
		{
			singularTriangle<PFP>(map, d, position, regularMarker, ridge_segments) ;
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

	G += k1 * ( ( n ^ ( pos3 - pos2 ) ) / ( 2 * a ) ) +
			k2 * ( ( n ^ ( pos1 - pos3 ) ) / ( 2 * a ) ) +
			k3 * ( ( n ^ ( pos2 - pos1 ) ) / ( 2 * a ) ) ;

	G.normalize() ;
	return G ;
}

template <typename PFP>
bool triangleType(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

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

	if ( isInSameOctant<PFP>(pos1,pos2) && isInSameOctant<PFP>(pos1,pos3) )
	{
		//regular triangle
		return true ;
	}
	else
	{
		//singular triangle
		return false ;
	}
}

template <typename PFP>
bool isInSameOctant(const typename PFP::VEC3& pos1, const typename PFP::VEC3& pos2)
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

template <typename PFP>
void ridgeLines(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, CellMarker& regularMarker, typename PFP::TVEC3& k, const typename PFP::TVEC3& faceGradient, const typename PFP::TREAL& area, AttributeHandler<ridgeSegment>& ridge_segments)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

	Traversor2FV<typename PFP::MAP> t(map, d) ;
	Dart v1 = t.begin() ;
	Dart v2 = t.next() ;
	Dart v3 = t.next() ;

	if((k[v1] * k[v2]) <= 0)
	{
		k[v2] *= -1 ;
	}

	if((k[v1] * k[v3]) <= 0)
	{
		k[v3] *= -1 ;
	}

	/* Calcul coefficient extremalite */

	REAL e1, e2, e3 ;

	e1 = extremality<PFP>(map, v1, position, k, faceGradient, area) ;
	e2 = extremality<PFP>(map, v2, position, k, faceGradient, area) ;
	e3 = extremality<PFP>(map, v3, position, k, faceGradient, area) ;

	/* Stockage des zeros */

	if( (e1 < 0 and e2 > 0) or (e1 > 0 and e2 < 0) )
	{
		REAL alpha = abs(e1) / ( abs(e1) + abs(e2) ) ;
		ridge_segments[d].valid = SEGMENT ;
		ridge_segments[d].p1.d = v1 ;
		ridge_segments[d].p1.w = alpha ;
	}
	if( (e1 < 0 and e3 > 0) or (e1 > 0 and e3 < 0) )
	{
		REAL alpha = abs(e1) / ( abs(e1) + abs(e2) ) ;
		if(ridge_segments[d].valid == EMPTY)
		{
			ridge_segments[d].valid = SEGMENT ;
			ridge_segments[d].p1.d = v2 ;
			ridge_segments[d].p1.w = alpha ;
		}
		else
		{
			ridge_segments[d].p2.d = v2 ;
			ridge_segments[d].p2.w = alpha ;
		}
	}
	if( (e2 < 0 and e3 > 0) or (e2 > 0 and e3 < 0) )
	{
		REAL alpha = abs(e1) / ( abs(e1) + abs(e2) ) ;
		ridge_segments[d].p2.d = v3 ;
		ridge_segments[d].p2.w = alpha ;
	}

	//stockage des zeros avec une autre methode pour le calcul du coefficient alpha
	/*if( (e1 < 0 and e3 > 0) )
		{
			REAL alpha = -e1 / ( e3 - e1 ) ;
			if(ridge_segments[d].valid == false)
			{
				ridge_segments[d].valid = true ;
				ridge_segments[d].p1.d = v2 ;
				ridge_segments[d].p1.w = alpha ;
			}
			else
			{
				ridge_segments[d].p2.d = v2 ;
				ridge_segments[d].p2.w = alpha ;
			}
		}
		else if(e1 > 0 and e3 < 0)
		{
			REAL alpha = -e3 / ( e1 - e3 ) ;
			if(ridge_segments[d].valid == false)
			{
				ridge_segments[d].valid = true ;
				ridge_segments[d].p1.d = v2 ;
				ridge_segments[d].p1.w = alpha ;
			}
			else
			{
				ridge_segments[d].p2.d = v2 ;
				ridge_segments[d].p2.w = alpha ;
			}
		}

		if( (e2 < 0 and e3 > 0) )
		{
			REAL alpha = -e2 / ( e3 - e2 ) ;
			ridge_segments[d].p2.d = v3 ;
			ridge_segments[d].p2.w = alpha ;
		}
		else if(e2 > 0 and e3 < 0)
		{
			REAL alpha = -e3 / ( e2 - e3 ) ;
			ridge_segments[d].p2.d = v3 ;
			ridge_segments[d].p2.w = alpha ;
		}*/
}

template <typename PFP>
typename PFP::REAL extremality(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, typename PFP::TVEC3& k, const typename PFP::TVEC3& faceGradient, const typename PFP::TREAL& area)
{
	typedef typename PFP::REAL REAL ;

	REAL a = 0 ;
	REAL e = 0 ;

	Traversor2VF<typename PFP::MAP> trav(map, d) ;
	for (Dart d2 = trav.begin(); d2 != trav.end(); d2 = trav.next())
	{
		a += area[d2] ;
		e += area[d2] * ( faceGradient[d2] * k[d] ) ;
	}

	return (e / a) ;
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

template <typename PFP>
void singularTriangle(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& position, CellMarker& regularMarker, AttributeHandler<ridgeSegment>& ridge_segments)
{
	int nbPoint = 0 ;

	Traversor2FFaE<typename PFP::MAP> f(map, d) ;
	for (Dart d2 = f.begin(); d2 != f.end(); d2 = f.next())
	{
		if(regularMarker.isMarked(d2) and ridge_segments[d2].valid == SEGMENT)
		{
			if(isEdgeInTriangle<PFP>(map, ridge_segments[d2].p1.d, d))
			{
				if(nbPoint == 0)
				{
					ridge_segments[d].p1.d = map.phi2(ridge_segments[d2].p1.d) ;
					ridge_segments[d].p1.w = 1 - ridge_segments[d2].p1.w ;
				}
				else
				{
					ridge_segments[d].valid ++ ;
					ridge_segments[d].p2.d = map.phi2(ridge_segments[d2].p1.d) ;
					ridge_segments[d].p2.w = 1 - ridge_segments[d2].p1.w ;
				}
				nbPoint ++ ;
			}
			else if(isEdgeInTriangle<PFP>(map, ridge_segments[d2].p1.d, d))
			{
				if(nbPoint == 0)
				{
					ridge_segments[d].p1.d = map.phi2(ridge_segments[d2].p2.d) ;
					ridge_segments[d].p1.w = 1 - ridge_segments[d2].p2.w ;
				}
				else
				{
					ridge_segments[d].valid ++ ;
					ridge_segments[d].p2.d = map.phi2(ridge_segments[d2].p2.d) ;
					ridge_segments[d].p2.w = 1 - ridge_segments[d2].p2.w ;
				}
				nbPoint ++ ;
			}
		}
	}
}

template <typename PFP>
bool isEdgeInTriangle(typename PFP::MAP& map, Dart edge, Dart triangle)
{
	bool inTriangle = false ;

	Traversor2FE<typename PFP::MAP> t(map, triangle) ;
	for (Dart e = t.begin(); e != t.end(); e = t.next())
	{
		if(map.phi2(e) == edge)
		{
			inTriangle = true ;
		}
	}

	return inTriangle ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
