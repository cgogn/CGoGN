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

#include <iostream>
#include <Geometry/inclusion.h>

using namespace CGoGN;

int main()
{
	typedef Geom::Vec3f VEC3;
	CGoGNout << "Check Geometry/inclusion.h" << CGoGNendl;
	CGoGNout << "Check Status : PARTIAL" << CGoGNendl;
	
	CGoGNout << "Check isPointInTriangle : Start" << CGoGNendl;
	VEC3 ta(0,0,0);
	VEC3 tb(5,0,0);
	VEC3 tc(0,5,0);
	VEC3 pIn1(1,1,0);
	VEC3 pIn2(2,0,0);
	VEC3 pIn3 = (tb+tc)/2.0f;
	VEC3 pIn4 = (tc+ta)/2.0f;
	VEC3 pOut(6,0,0);

	if(Geom::isPointInTriangle<VEC3>(pIn1,ta,tb,tc)!=Geom::FACE_INCLUSION)
	{
		CGoGNout << "ERROR : isPointInTriangle : face inclusion" << CGoGNendl;
	}

	if(Geom::isPointInTriangle<VEC3>(pIn2,ta,tb,tc)!=Geom::EDGE_INCLUSION)
	{
		CGoGNout << "ERROR : isPointInTriangle : edge inclusion" << CGoGNendl;
	}

	if(Geom::isPointInTriangle<VEC3>(pIn3,ta,tb,tc)!=Geom::EDGE_INCLUSION)
	{
		CGoGNout << "ERROR : isPointInTriangle : edge inclusion" << CGoGNendl;
	}

	if(Geom::isPointInTriangle<VEC3>(pIn4,ta,tb,tc)!=Geom::EDGE_INCLUSION)
	{
		CGoGNout << "ERROR : isPointInTriangle : edge inclusion" << CGoGNendl;
	}

	if(Geom::isPointInTriangle<VEC3>(ta,ta,tb,tc)!=Geom::VERTEX_INCLUSION)
	{
		CGoGNout << "ERROR : isPointInTriangle : vertex inclusion" << CGoGNendl;
	}

	if(Geom::isPointInTriangle<VEC3>(pOut,ta,tb,tc)!=Geom::NO_INCLUSION)
	{
		CGoGNout << "ERROR : isPointInTriangle : no inclusion" << CGoGNendl;
	}

	CGoGNout << "Check isPointInTriangle : Done" << CGoGNendl;

// 	Inclusion isSegmentInTriangle(const VEC3& P1, const VEC3& P2, const VEC3& Ta, const VEC3& Tb, const VEC3& Tc, const VEC3& N) ;
// 	bool isPointInTetrahedron(VEC3 points[4], VEC3& point, bool CCW) ;
// 	bool isEdgeInOrIntersectingTetrahedron(VEC3 points[4], VEC3& point1, VEC3& point2, bool CCW) ;

	CGoGNout << "Check arePointsEquals : Start" << CGoGNendl;

	VEC3 p1(0,0,0);
	VEC3 p2(0.1,0.1,0.1);
	if(!Geom::arePointsEquals<VEC3>(p1,p1) || !Geom::arePointsEquals<VEC3>(p2,p2) 
		|| Geom::arePointsEquals<VEC3>(p1,p2) || Geom::arePointsEquals<VEC3>(p2,p1))
	{
		CGoGNout << "ERROR : arePointsEquals" << CGoGNendl;
	}

	CGoGNout << "Check arePointsEquals : Done" << CGoGNendl;

	return 0;
}
