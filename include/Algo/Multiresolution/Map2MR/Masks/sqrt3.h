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

#ifndef __MR_SQRT3_MASK__
#define __MR_SQRT3_MASK__

#include <cmath>

namespace CGoGN
{

namespace Algo
{

namespace MR
{

namespace Primal
{

namespace Masks
{


template <typename PFP>
class Sqrt3VertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Sqrt3VertexVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;

		typename PFP::VEC3 np(0) ;
		unsigned int degree = 0 ;
		Traversor2VVaE<typename PFP::MAP> trav(m_map, d) ;
		for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
		{
			++degree ;
			np += m_position[it] ;
		}
		float alpha = 1.0/9.0 * ( 4.0 - 2.0 * cos(2.0 * M_PI / degree));
		np *= alpha / degree ;

		typename PFP::VEC3 vp = m_position[d] ;
		vp *= 1.0 - alpha ;

		m_map.incCurrentLevel() ;

		m_position[d] = np + vp ;

		return false ;
	}
} ;

template <typename PFP>
class Sqrt3FaceVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Sqrt3FaceVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d0 = m_map.phi1(d) ;

		m_map.decCurrentLevel() ;

		Dart d1 = m_map.phi1(d1) ;
		Dart d2 = m_map.phi1(d2) ;

		typename PFP::VEC3 p0 = m_position[d0] ;
		typename PFP::VEC3 p1 = m_position[d1] ;
		typename PFP::VEC3 p2 = m_position[d2] ;

		p0 *= 1.0 / 3.0 ;
		p1 *= 1.0 / 3.0 ;
		p2 *= 1.0 / 3.0 ;

		m_map.incCurrentLevel() ;

		m_position[d] = p0 + p1 + p2;

		return false ;
	}
} ;

} // namespace Masks

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN

#endif

