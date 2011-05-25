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

#include "Algo/Geometry/localFrame.h"
#include "Geometry/matrix.h"
#include "Topology/generic/cellmarker.h"

extern "C"
{
	#include "C_BLAS_LAPACK/INCLUDE/f2c.h"
	#include "C_BLAS_LAPACK/INCLUDE/clapack.h"
}
#undef max
#undef min

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
void computeCurvatureVertices(
		typename PFP::MAP& map,
		const typename PFP::TVEC3& position,
		const typename PFP::TVEC3& normal,
		typename PFP::TREAL& k1,
		typename PFP::TREAL& k2,
		typename PFP::TVEC3& K1,
		typename PFP::TVEC3& K2,
		const FunctorSelect& select)
{
	CellMarker marker(map, VERTEX_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			computeCurvatureVertex<PFP>(map, d, position, normal, k1, k2, K1, K2) ;
		}
	}
}

template <typename PFP>
void computeCurvatureVertex(
		typename PFP::MAP& map,
		Dart dart,
		const typename PFP::TVEC3& position,
		const typename PFP::TVEC3& normal,
		typename PFP::TREAL& k1,
		typename PFP::TREAL& k2,
		typename PFP::TVEC3& K1,
		typename PFP::TVEC3& K2)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::MATRIX33 MATRIX33 ;

	VEC3 n = normal[dart] ;
	MATRIX33 localFrame = Algo::Geometry::vertexLocalFrame<PFP>(map, dart, position, n) ;
	MATRIX33 invLocalFrame ;
	localFrame.invert(invLocalFrame) ;

	REAL a, b, c, d, e;
	//vertexCubicFitting(map,dart,localFrame,a,b,c,d,e,f,g,h,i) ;
	vertexQuadricFitting<PFP>(map, dart, localFrame, position, normal, a, b, c, d, e) ;

	REAL k1_v, k2_v, K1x, K1y ;
	//int res = slaev2_(&e, &f, &g, &maxC, &minC, &dirX, &dirY) ;
	/*int res = */slaev2_(&a, &b, &c, &k1_v, &k2_v, &K1x, &K1y) ;

	VEC3 K1_v(K1x, K1y, 0.0f) ;
	K1_v = invLocalFrame * K1_v ;
	VEC3 K2_v = n ^ K1_v ;

	if (k1_v < k2_v)
	{
		k1[dart] = -k1_v ;
		k2[dart] = -k2_v ;
		K1[dart] = K1_v ;
		K2[dart] = K2_v ;
	}
	else
	{
		k1[dart] = -k2_v ;
		k2[dart] = -k1_v ;
		K1[dart] = K2_v ;
		K2[dart] = K1_v ;
	}
}

template <typename PFP>
void vertexQuadricFitting(
		typename PFP::MAP& map,
		Dart dart,
		typename PFP::MATRIX33& localFrame,
		const typename PFP::TVEC3& position,
		const typename PFP::TVEC3& normal,
		float& a, float& b, float& c, float& d, float& e)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::MATRIX33 MATRIX33 ;

	VEC3 p = position[dart] ;

	LinearSolver<CPUSolverTraits> solver(5) ;
	solver.set_least_squares(true) ;
	solver.begin_system() ;
	Dart it = dart ;
	do
	{
		// 1-ring vertices
		VEC3 v = position[map.phi2(it)] ;
		quadricFittingAddVertexPos<PFP>(v, p, localFrame, solver) ;
		VEC3 n = normal[map.phi2(it)] ;
		quadricFittingAddVertexNormal<PFP>(v, n, p, localFrame, solver) ;
		// 2-ring vertices
//		Dart d2 = map.phi1(map.phi1(map.phi2(map.phi1(it)))) ;
//		VEC3 v2 = position[d2] ;
//		quadricFittingAddVertexPos(v2, p, localFrame, solver) ;
//		VEC3 n2 = normal[d2] ;
//		quadricFittingAddVertexNormal(v2, n2, p, localFrame, solver) ;
		it = map.phi1( map.phi2(it)) ;
	} while (it != dart) ;
	solver.end_system() ;
	solver.solve() ;

	a = solver.variable(0).value() ;
	b = solver.variable(1).value() ;
	c = solver.variable(2).value() ;
	d = solver.variable(3).value() ;
	e = solver.variable(4).value() ;
}

template <typename PFP>
void quadricFittingAddVertexPos(typename PFP::VEC3& v, typename PFP::VEC3& p, typename PFP::MATRIX33& localFrame, LinearSolver<CPUSolverTraits>& solver)
{
	typename PFP::VEC3 vec = v - p ;
	vec = localFrame * vec ;
	solver.begin_row() ;

	solver.add_coefficient(0, vec[0]*vec[0]) ;
	solver.add_coefficient(1, vec[0]*vec[1]) ;
	solver.add_coefficient(2, vec[1]*vec[1]) ;
	solver.add_coefficient(3, vec[0]) ;
	solver.add_coefficient(4, vec[1]) ;

	solver.set_right_hand_side(vec[2]) ;
	solver.end_row() ;
}

template <typename PFP>
void quadricFittingAddVertexNormal(typename PFP::VEC3& v, typename PFP::VEC3& n, typename PFP::VEC3& p, typename PFP::MATRIX33& localFrame, LinearSolver<CPUSolverTraits>& solver)
{
	typename PFP::VEC3 vec = v - p ;
	vec = localFrame * vec ;
	typename PFP::VEC3 norm = localFrame * n ;
	solver.begin_row() ;

	solver.add_coefficient(0, 2.0f*vec[0]) ;
	solver.add_coefficient(1, vec[1]) ;
	solver.add_coefficient(2, 0) ;
	solver.add_coefficient(3, 1.0f) ;
	solver.add_coefficient(4, 0) ;
	solver.set_right_hand_side(-1.0f*norm[0]/norm[2]) ;
	solver.end_row() ;

	solver.begin_row() ;
	solver.add_coefficient(0, 0) ;
	solver.add_coefficient(1, vec[0]) ;
	solver.add_coefficient(2, 2.0f*vec[1]) ;
	solver.add_coefficient(3, 0) ;
	solver.add_coefficient(4, 1.0f) ;
	solver.set_right_hand_side(-1.0f*norm[1]/norm[2]) ;
	solver.end_row() ;
}
/*
template <typename PFP>
void DifferentialProperties<PFP>::vertexCubicFitting(Dart dart, gmtl::Vec3f& normal, float& a, float& b, float& c, float& d, float& e, float& f, float& g, float& h, float& i)
{
	gmtl::Matrix33f localFrame, invLocalFrame ;
	Algo::Geometry::vertexLocalFrame<PFP>(m_map,dart,normal,localFrame) ;
	gmtl::invertFull(invLocalFrame, localFrame) ;
	gmtl::Vec3f p = m_map.getVertexEmb(dart)->getPosition() ;
	solverC->reset(false) ;
	solverC->set_least_squares(true) ;
	solverC->begin_system() ;
	Dart it = dart ;
	do
	{
		// 1-ring vertices
		gmtl::Vec3f v = m_map.getVertexEmb(m_map.phi2(it))->getPosition() ;
		cubicFittingAddVertexPos(v,p,localFrame) ;
		gmtl::Vec3f n = m_normalsV[m_map.getVertexEmb(m_map.phi2(it))->getLabel()] ;
		cubicFittingAddVertexNormal(v,n,p,localFrame) ;
		// 2-ring vertices
//		Dart d2 = m_map.phi1(m_map.phi1(m_map.phi2(m_map.phi1(it)))) ;
//		gmtl::Vec3f v2 = m_map.getVertexEmb(d2)->getPosition() ;
//		cubicFittingAddVertexPos(v2,p,localFrame) ;
//		gmtl::Vec3f n2 = m_normalsV[m_map.getVertexEmb(d2)->getLabel()] ;
//		cubicFittingAddVertexNormal(v2,n2,p,localFrame) ;

		it = m_map.phi1( m_map.phi2(it)) ;
	} while (it != dart) ;
	solverC->end_system() ;
	solverC->solve() ;
	a = solverC->variable(0).value() ;
	b = solverC->variable(1).value() ;
	c = solverC->variable(2).value() ;
	d = solverC->variable(3).value() ;
	e = solverC->variable(4).value() ;
	f = solverC->variable(5).value() ;
	g = solverC->variable(6).value() ;
	h = solverC->variable(7).value() ;
	i = solverC->variable(8).value() ;

//	normal = gmtl::Vec3f(-h, -i, 1.0f) ;
//	gmtl::normalize(normal) ;
//	normal = invLocalFrame * normal ;
}

template <typename PFP>
void DifferentialProperties<PFP>::cubicFittingAddVertexPos(gmtl::Vec3f& v, gmtl::Vec3f& p, gmtl::Matrix33f& localFrame)
{
	gmtl::Vec3f vec = v - p ;
	vec = localFrame * vec ;
	solverC->begin_row() ;

	solverC->add_coefficient(0, vec[0]*vec[0]*vec[0]) ;
	solverC->add_coefficient(1, vec[0]*vec[0]*vec[1]) ;
	solverC->add_coefficient(2, vec[0]*vec[1]*vec[1]) ;
	solverC->add_coefficient(3, vec[1]*vec[1]*vec[1]) ;
	solverC->add_coefficient(4, vec[0]*vec[0]) ;
	solverC->add_coefficient(5, vec[0]*vec[1]) ;
	solverC->add_coefficient(6, vec[1]*vec[1]) ;
	solverC->add_coefficient(7, vec[0]) ;
	solverC->add_coefficient(8, vec[1]) ;

	solverC->set_right_hand_side(vec[2]) ;
	solverC->end_row() ;
}

template <typename PFP>
void DifferentialProperties<PFP>::cubicFittingAddVertexNormal(gmtl::Vec3f& v, gmtl::Vec3f& n, gmtl::Vec3f& p, gmtl::Matrix33f& localFrame)
{
	gmtl::Vec3f vec = v - p ;
	vec = localFrame * vec ;
	gmtl::Vec3f norm = localFrame * n ;
	solverC->begin_row() ;

	solverC->add_coefficient(0, 3.0f*vec[0]*vec[0]) ;
	solverC->add_coefficient(1, 2.0f*vec[0]*vec[1]) ;
	solverC->add_coefficient(2, vec[1]*vec[1]) ;
	solverC->add_coefficient(3, 0) ;
	solverC->add_coefficient(4, 2.0f*vec[0]) ;
	solverC->add_coefficient(5, vec[1]) ;
	solverC->add_coefficient(6, 0) ;
	solverC->add_coefficient(7, 1.0f) ;
	solverC->add_coefficient(8, 0) ;
	solverC->set_right_hand_side(-1.0f*norm[0]/norm[2]) ;
	solverC->end_row() ;

	solverC->begin_row() ;
	solverC->add_coefficient(0, 0) ;
	solverC->add_coefficient(1, vec[0]*vec[0]) ;
	solverC->add_coefficient(2, 2.0f*vec[0]*vec[1]) ;
	solverC->add_coefficient(3, 3.0f*vec[1]*vec[1]) ;
	solverC->add_coefficient(4, 0) ;
	solverC->add_coefficient(5, vec[0]) ;
	solverC->add_coefficient(6, 2.0f*vec[1]) ;
	solverC->add_coefficient(7, 0) ;
	solverC->add_coefficient(8, 1.0f) ;
	solverC->set_right_hand_side(-1.0f*norm[1]/norm[2]) ;
	solverC->end_row() ;
}
*/
} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
