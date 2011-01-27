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

#ifndef __LINEAR_SOLVING_LAPLACIAN_SETUP__
#define __LINEAR_SOLVING_LAPLACIAN_SETUP__

namespace CGoGN
{

namespace LinearSolving
{

enum LaplacianType
{
	TOPOLOGICAL,
	COTWEIGHT
};

template<typename PFP, class SOLVER_TRAITS>
class LaplacianTopo : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;
	const AttributeHandler<unsigned int>& indexTable ;

public:
	typedef typename PFP::MAP MAP ;
	

	LaplacianTopo(MAP& m, LinearSolver<SOLVER_TRAITS>* s, const AttributeHandler<unsigned int> index) :
		FunctorMap<MAP>(m), solver(s), indexTable(index)
	{}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		Dart dd = d ;
		float aii = 0.0f ;
		do
		{
			float aij = 1.0f ;
			aii += aij ;
			solver->add_coefficient(indexTable[this->m_map.phi2(dd)], aij) ;
			dd = this->m_map.phi1(this->m_map.phi2(dd)) ;
		} while(dd != d) ;
		solver->add_coefficient(indexTable[d], -aii) ;
		solver->set_right_hand_side(0.0f) ;
		solver->normalize_row() ;
		solver->end_row() ;
		return false ;
	}
} ;

/*
template<typename PFP, class SOLVER_TRAITS>
class LaplacianCotWeight : public FunctorMap<typename PFP::MAP>
{
protected:
	LinearSolver<SOLVER_TRAITS>* solver ;

public:
	LaplacianCotWeight(MAP& m, LinearSolver<SOLVER_TRAITS>* s) :
		FunctorMap<MAP>(m), solver(s)
	{}

//	float half_w(Dart d)
//	{
//		Dart d1 = this->m_map.phi2( this->m_map.phi1( d)) ;
//		gmtl::Vec3f V1 = this->m_map.getVertexEmb(d1)->getPosition() ;
//		V1 -= this->m_map.getVertexEmb(this->m_map.phi2(d1))->getPosition() ;
//		Dart d2 = this->m_map.phi_1( d) ;
//		gmtl::Vec3f V2 = this->m_map.getVertexEmb(d2)->getPosition() ;
//		V1 -= this->m_map.getVertexEmb(this->m_map.phi2(d2))->getPosition() ;
//		float nV1 = gmtl::lengthSquared(V1) ;
//		float nV2 = gmtl::lengthSquared(V2) ;
//		float cos_angle = gmtl::dot(V1, V2) / sqrt(nV1 * nV2) ;
//		float angle = acos(cos_angle) ;
//		return 1.0 / tan(angle) ;
//	}
//
//	float w(Dart d)
//	{
//		float result = 0.0f ;
//		Dart dd = this->m_map.phi2(d) ;
//		result += half_w(d) ;
//		result += half_w(dd) ;
//		gmtl::Vec3f vec = this->m_map.getVertexEmb(d)->getPosition() ;
//		vec -= this->m_map.getVertexEmb(dd)->getPosition() ;
//		result *= gmtl::lengthSquared(vec) ;
//		return result ;
//	}
//
//	float one_ring_area(Dart d)
//	{
//		float result = 0.0 ;
//		Dart dd = d ;
//		do
//		{
//			gmtl::Vec3f n ;
//			Algo::Geometry::faceNormal<PFP>(this->m_map, d, n) ;
//			result += 0.5f * gmtl::length(n) ;
//			dd = this->m_map.phi1(this->m_map.phi2(dd)) ;
//		} while(dd != d) ;
//		return result ;
//	}

	bool operator()(Dart d)
	{
		solver->begin_row() ;
		Dart dd = d ;
		EMB* e = reinterpret_cast<EMB*>(this->m_map.getVertexEmb(d)) ;
		unsigned int idx = e->getIndex() ;
		float area = one_ring_area(d) ;
		float aii = 0.0f ;
		do
		{
			float aij = w(dd) / area ;
			aii += aij ;
			EMB* ne = reinterpret_cast<EMB*>(this->m_map.getVertexEmb(this->m_map.phi2(dd))) ;
			unsigned int neighbour_idx = ne->getIndex() ;
			this->solver->add_coefficient(neighbour_idx, aij) ;
			dd = this->m_map.phi1(this->m_map.phi2(dd)) ;
		} while(dd != d) ;
		solver->add_coefficient(idx, -aii) ;
		solver->set_right_hand_side(0.0f) ;
		solver->normalize_row() ;
		solver->end_row() ;
		return false ;
	}
} ;
*/

template <typename PFP, class SOLVER_TRAITS>
void setupLaplacianMatrix(typename PFP::MAP& m, LinearSolver<SOLVER_TRAITS>* s, LaplacianType lt, const AttributeHandler<unsigned int> index)
{
	switch(lt)
	{
	case TOPOLOGICAL : {
		LaplacianTopo<PFP, SOLVER_TRAITS> lt(m, s, index) ;
		m.foreach_orbit(VERTEX_ORBIT, lt) ;
		break ; }
	case COTWEIGHT : {
		LaplacianTopo<PFP, SOLVER_TRAITS> lt(m, s, index) ;
		m.foreach_orbit(VERTEX_ORBIT, lt) ;
		break ; }
	}
}

}

}

#endif
