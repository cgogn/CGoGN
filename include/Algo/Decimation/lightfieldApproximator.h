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

/************************************************************************************
 *                      FULL CONTRACTION LF APPROXIMATORS                           *
 ************************************************************************************/
template <typename PFP>
class Approximator_FrameInterpolation : public Approximator<PFP, typename PFP::VEC3, EDGE>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	VertexAttribute<VEC3> m_position ;
	EdgeAttribute<VEC3> m_approxposition ;

	VertexAttribute<VEC3> *m_frameT ;
	VertexAttribute<VEC3> *m_frameB ;
	VertexAttribute<VEC3> *m_frameN ;

public:
	Approximator_FrameInterpolation(MAP& m, std::vector<VertexAttribute<VEC3>* >& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3, EDGE>(m, attr, pred),
		m_frameT(NULL),
		m_frameB(NULL),
		m_frameN(NULL)
	{
		if (this->m_attrV.size() < 3)
			std::cerr << "Approximator_FrameInterpolation: not enough attributes provided (only " << this->m_attrV.size() << " instead of 3)" << std::endl ;

		m_frameT = this->m_attrV[0] ;
		m_frameB = this->m_attrV[1] ;
		m_frameN = this->m_attrV[2] ;
	}

	~Approximator_FrameInterpolation()
	{}

	ApproximatorType getType() const
	{
		return A_Lightfield ;
	}

	bool init()
	{
		assert(m_frameT->isValid() || !"Approximator_FrameInterpolation: the first approximated attribute is not valid") ;
		assert(m_frameB->isValid() || !"Approximator_FrameInterpolation: the second approximated attribute is not valid") ;
		assert(m_frameN->isValid() || !"Approximator_FrameInterpolation: the third approximated attribute is not valid") ;

		m_position = this->m_map.template getAttribute<VEC3, VERTEX>("position") ;
		assert(m_position.isValid() || !"Approximator_FrameInterpolation::init: the position attribute is not valid") ;

		m_approxposition = this->m_map.template getAttribute<VEC3, EDGE>("approx_position") ;
		assert(m_approxposition.isValid() || !"Approximator_FrameInterpolation::init: the approx_position attribute is not valid") ;

		return m_frameT->isValid() && m_frameB->isValid() && m_frameN->isValid() && m_position.isValid() && m_approxposition.isValid() ;
	}

	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_HemiFuncCoefs: public Approximator<PFP, typename PFP::VEC3, EDGE>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	unsigned int m_nbCoefs ;
	unsigned int m_HFtype ;

	VertexAttribute<VEC3 > m_frameT ;
	VertexAttribute<VEC3 > m_frameB ;
	VertexAttribute<VEC3 > m_frameN ;

	EdgeAttribute<VEC3 > m_newFrameT ;
	EdgeAttribute<VEC3 > m_newFrameB ;
	EdgeAttribute<VEC3 > m_newFrameN ;

	std::vector<VertexAttribute<VEC3>* > m_coefs ;

	EdgeAttribute<Utils::QuadricHF<REAL> > m_quadricHF ;

public:
	Approximator_HemiFuncCoefs(MAP& m, std::vector<VertexAttribute<VEC3>* >& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3, EDGE>(m, attr, pred),
		m_nbCoefs(0),
		m_HFtype(0) // SH = 0
	{
		unsigned int i ;
		for (i = 0 ; i < 200 ; ++i)
		{
			// check if number i is present
			if ((this->m_attrV.size() <= i) || this->m_attrV[i]->name().find("coefs") == std::string::npos)
				break ;

			m_coefs.push_back(this->m_attrV[i]) ;
		}

		// check name of last valid
		if (this->m_attrV[i-1]->name().find("PB") != std::string::npos)
			m_HFtype = 1 ;

		m_nbCoefs = i ;

		// set quadric
		m_quadricHF = this->m_map.template addAttribute<Utils::QuadricHF<REAL>, EDGE>("HFquadric") ;
	}
	~Approximator_HemiFuncCoefs()
	{
		this->m_map.removeAttribute(m_quadricHF) ;
	}

	ApproximatorType getType() const
	{
		return A_Lightfield ;
	}

	bool init() ;

	void approximate(Dart d) ;
} ;

// TODO : merge Approximator_FrameInterpolationHalfEdge and FullEdge ? By using template ?

/************************************************************************************
 *                 HALF EDGE CONTRACTION LF APPROXIMATORS                           *
 ************************************************************************************/
template <typename PFP>
class Approximator_FrameInterpolationHalfEdge : public Approximator<PFP, typename PFP::VEC3, DART>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	VertexAttribute<VEC3> m_position ;
	DartAttribute<VEC3> m_approxposition ;

	VertexAttribute<VEC3> *m_frameT ;
	VertexAttribute<VEC3> *m_frameB ;
	VertexAttribute<VEC3> *m_frameN ;

public:
	Approximator_FrameInterpolationHalfEdge(MAP& m, std::vector<VertexAttribute<VEC3>* >& attr, Predictor<PFP, VEC3>* pred = NULL):
		Approximator<PFP, VEC3, DART>(m, attr, pred),
		m_frameT(NULL),
		m_frameB(NULL),
		m_frameN(NULL)
	{
		if (this->m_attrV.size() < 3)
			std::cerr << "Approximator_FrameInterpolation: not enough attributes provided (only " << this->m_attrV.size() << " instead of 3)" << std::endl ;

		m_frameT = this->m_attrV[0] ;
		m_frameB = this->m_attrV[1] ;
		m_frameN = this->m_attrV[2] ;
	}

	~Approximator_FrameInterpolationHalfEdge()
	{}

	ApproximatorType getType() const
	{
		return A_hLightfieldHalf ;
	}

	bool init()
	{
		assert(m_frameT->isValid() || !"Approximator_FrameInterpolation: the first approximated attribute is not valid") ;
		assert(m_frameB->isValid() || !"Approximator_FrameInterpolation: the second approximated attribute is not valid") ;
		assert(m_frameN->isValid() || !"Approximator_FrameInterpolation: the third approximated attribute is not valid") ;

		m_position = this->m_map.template getAttribute<VEC3, VERTEX>("position") ;
		assert(m_position.isValid() || !"Approximator_FrameInterpolation::init: the position attribute is not valid") ;

		m_approxposition = this->m_map.template getAttribute<VEC3, DART>("approx_position") ;
		assert(m_approxposition.isValid() || !"Approximator_FrameInterpolation::init: the approx_position attribute is not valid") ;

		return m_frameT->isValid() && m_frameB->isValid() && m_frameN->isValid() && m_position.isValid() && m_approxposition.isValid() ;
	}

	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_HemiFuncCoefsHalfEdge: public Approximator<PFP, typename PFP::VEC3, DART>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	unsigned int m_nbCoefs ;
	unsigned int m_HFtype ;

	VertexAttribute<VEC3 > m_frameT ;
	VertexAttribute<VEC3 > m_frameB ;
	VertexAttribute<VEC3 > m_frameN ;

	DartAttribute<VEC3 > m_newFrameT ;
	DartAttribute<VEC3 > m_newFrameB ;
	DartAttribute<VEC3 > m_newFrameN ;

	std::vector<VertexAttribute<VEC3>* > m_coefs ;

	DartAttribute<Utils::QuadricHF<REAL> > m_quadricHF ;

public:
	Approximator_HemiFuncCoefsHalfEdge(MAP& m, std::vector<VertexAttribute<VEC3>* >& attr, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3, DART>(m, attr, pred),
		m_nbCoefs(0),
		m_HFtype(0) // SH = 0
	{
		unsigned int i ;
		for (i = 0 ; i < 200 ; ++i)
		{
			// check if number i is present
			if ((this->m_attrV.size() <= i) || this->m_attrV[i]->name().find("coefs") == std::string::npos)
				break ;

			m_coefs.push_back(this->m_attrV[i]) ;
		}

		// check name of last valid
		if (this->m_attrV[i-1]->name().find("PB") != std::string::npos)
			m_HFtype = 1 ;

		m_nbCoefs = i ;

		// set quadric
		m_quadricHF = this->m_map.template addAttribute<Utils::QuadricHF<REAL>, DART>("HFquadric") ;
	}
	~Approximator_HemiFuncCoefsHalfEdge()
	{
		this->m_map.removeAttribute(m_quadricHF) ;
	}

	ApproximatorType getType() const
	{
		return A_hLightfieldHalf ;
	}

	bool init() ;

	void approximate(Dart d) ;
} ;

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN

#include "Algo/Decimation/lightfieldApproximator.hpp"

#endif
