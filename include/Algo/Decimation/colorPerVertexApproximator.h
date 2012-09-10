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
class Approximator_Color : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	VertexAttribute<VEC3> *m_color ;

public:
	Approximator_Color(MAP& m, std::vector<VertexAttribute<VEC3>* >& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, attr, pred)
	{
		m_color = this->m_attrV[0] ;
		assert((m_color->isValid() && m_color->name() == "color") || !"Approximator_Color: the approximated attribute is not valid or not named 'color'") ;
	}
	~Approximator_Color()
	{}

	ApproximatorType getType() const
	{
		return A_hColor ;
	}

	bool init()
	{
		return true ;
	}

	void approximate(Dart d) ;
} ;

/*
template <typename PFP>
class Approximator_FrameHalf : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

public:
	Approximator_FrameHalf(MAP& m, std::vector<VertexAttribute<VEC3> >& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, attr, pred)
	{}
	~Approximator_FrameHalf()
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

#include "Algo/Decimation/colorPerVertexApproximator.hpp"

#endif
