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

#ifndef __LIGHTFIELD_APPROXIMATOR_H__
#define __LIGHTFIELD_APPROXIMATOR_H__

#include "Algo/Decimation/approximator.h"
#include "Topology/generic/mapBrowser.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

template <typename PFP>
class Approximator_FrameHalf : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	VertexAttribute<VEC3> *m_frameT ;
	VertexAttribute<VEC3> *m_frameB ;
	VertexAttribute<VEC3> *m_frameN ;

public:
	Approximator_FrameHalf(MAP& m, std::vector<VertexAttribute<VEC3>* >& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, attr, pred)
	{
		if (this->m_attrV.size() < 1)
			std::cerr << "Approximator_Frame: not enough attributes provided (only " << this->m_attrV.size() << " instead of 3)" << std::endl ;

		m_frameT = this->m_attrV[0] ;
		m_frameB = this->m_attrV[1] ;
		m_frameN = this->m_attrV[2] ;
		assert(m_frameT->isValid() || !"Approximator_FrameHalf: the first approximated attribute is not valid") ;
		assert(m_frameB->isValid() || !"Approximator_FrameHalf: the second approximated attribute is not valid") ;
		assert(m_frameN->isValid() || !"Approximator_FrameHalf: the third approximated attribute is not valid") ;
	}
	~Approximator_FrameHalf()
	{}

	ApproximatorType getType() const
	{
		return A_hLightfieldHalf ;
	}

	bool init()
	{
		return true ;
	}

	void approximate(Dart d) ;
} ;
/*
template <typename PFP>
class Approximator_LightfieldCoefsHalf : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

public:
	Approximator_LightfieldCoefsHalf(MAP& m, std::vector<VertexAttribute<VEC3> >& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, attr, pred)
	{}
	~Approximator_LightfieldCoefsHalf()
	{}

	ApproximatorType getType() const
	{
		return A_LightfieldHalf ;
	}

	bool init()
	{
		return true ;
	}

	void approximate(Dart d) ;
} ;
*/
} //namespace Decimation

} //namespace Algo

} //namespace CGoGN

#include "Algo/Decimation/lightfieldApproximator.hpp"

#endif
