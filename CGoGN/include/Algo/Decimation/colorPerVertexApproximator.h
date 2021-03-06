/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2013, IGG Team, ICube, University of Strasbourg           *
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

#ifndef __COLOR_APPROXIMATOR_H__
#define __COLOR_APPROXIMATOR_H__

#include "Algo/Decimation/approximator.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Decimation
{

template <typename PFP>
class Approximator_ColorNaive : public Approximator<PFP, typename PFP::VEC3, EDGE>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	const VertexAttribute<VEC3, MAP>& m_position ;
	const EdgeAttribute<VEC3, MAP>& m_approximatedPosition ;

public:
	Approximator_ColorNaive(MAP& m, VertexAttribute<VEC3, MAP>& color, const VertexAttribute<VEC3, MAP>& position, const EdgeAttribute<VEC3, MAP>& approximatedPosition, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3, EDGE>(m, color, pred),
		m_position(position),
		m_approximatedPosition(approximatedPosition)
	{}
	~Approximator_ColorNaive()
	{}
	ApproximatorType getType() const { return A_ColorNaive ; }
	bool init();
	void approximate(Dart d) ;
} ;
/*
template <typename PFP>
class Approximator_ColorQEMext : public Approximator<PFP, typename PFP::VEC3, EDGE>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef Geom::Vector<6, REAL> VEC6 ;

protected:
	VertexAttribute<Utils::QuadricNd<REAL,6>, MAP> m_quadric ;
	VertexAttribute<VEC3, MAP>* m_position ;
	VertexAttribute<VEC3, MAP>* m_color ;

public:
	Approximator_ColorQEMext(MAP& m, VertexAttribute<VEC3, MAP>& position, VertexAttribute<VEC3, MAP>& color, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3, EDGE>(m, position, pred),
		m_position(position),
		m_color(color)
	{}
	~Approximator_ColorQEMext()
	{}
	ApproximatorType getType() const { return A_ColorQEMext ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_GeomColOpt : public Approximator<PFP, typename PFP::VEC3, EDGE>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

protected:
	VertexAttribute<Utils::Quadric<REAL>, MAP> m_quadric ;
	VertexAttribute<VEC3, MAP>* m_position ;
	VertexAttribute<VEC3, MAP>* m_color ;

public:
	Approximator_GeomColOpt(MAP& m, std::vector<VertexAttribute<VEC3, MAP>*>& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3, EDGE>(m, attr, pred)
	{
		assert(attr.size() > 1 || !"Approximator_GeomColOpt: there are not sufficient attributes provided") ;

		m_position = this->m_attrV[0] ;
		m_color = this->m_attrV[1] ;
	}

	~Approximator_GeomColOpt()
	{}

	ApproximatorType getType() const
	{
		return A_GeomColorOpt ;
	}

	bool init() ;

	void approximate(Dart d) ;
} ;
*/
} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/colorPerVertexApproximator.hpp"

#endif
