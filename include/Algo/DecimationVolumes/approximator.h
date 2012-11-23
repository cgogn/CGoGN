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

#ifndef __APPROXIMATOR_VOLUMES_H__
#define __APPROXIMATOR_VOLUMES_H__

#include "Algo/DecimationVolumes/operator.h"
#include "Algo/DecimationVolumes/predictor.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

enum ApproximatorType
{
	A_QEM,
	A_MidEdge,
	A_hHalfCollapse
};

template <typename PFP>
class ApproximatorGen
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	MAP& m_map ;

public:
	ApproximatorGen(MAP& m) : m_map(m)
	{}
	virtual ~ApproximatorGen()
	{}
	virtual const std::string& getApproximatedAttributeName() const = 0 ;
	virtual ApproximatorType getType() const = 0 ;
	virtual bool init() = 0 ;
	virtual void approximate(Dart d) = 0 ;
	virtual void saveApprox(Dart d) = 0 ;
	virtual void affectApprox(Dart d) = 0 ;
	virtual const PredictorGen<PFP>* getPredictor() const = 0 ;
} ;


template <typename PFP, typename T>
class Approximator :  public ApproximatorGen<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL;

protected:
	Predictor<PFP, T>* m_predictor ;

	//Vertex attribute to be approximated
	VertexAttribute<T>& m_attrV;
	//Attribute to store approximation result
	EdgeAttribute<T> m_approx;
	// attribute to store detail information for reconstruction
	EdgeAttribute<T> m_detail ;

	T m_app;

public:
	Approximator(MAP& m, VertexAttribute<T>& a, Predictor<PFP, T>* predictor) :
		ApproximatorGen<PFP>(m), m_predictor(predictor), m_attrV(a)
	{
		std::stringstream aname ;
		aname << "approx_" << m_attrV.name() ;
		m_approx = this->m_map.template addAttribute<T, EDGE>(aname.str()) ;

		if(m_predictor)	// if a predictor is associated to the approximator
		{				// create an attribute to store the detail needed for reconstruction
			std::stringstream dname ;
			dname << "detail_" << m_attrV.name() ;
			m_detail = this->m_map.template addAttribute<T, EDGE>(dname.str()) ;
		}
	}

	virtual ~Approximator()
	{
		this->m_map.template removeAttribute(m_approx) ;

		if(m_predictor)
			this->m_map.template removeAttribute(m_detail) ;
	}

	const std::string& getApproximatedAttributeName() const
	{
		return m_attrV.name() ;
	}

	const T& getApprox(Dart d) const
	{
		return m_approx[d] ;
	}

	void saveApprox(Dart d)
	{
		m_app = m_approx[d] ;
	}

	void affectApprox(Dart d)
	{
		m_attrV[d] = m_app ;
	}

	const Predictor<PFP, T>* getPredictor() const
	{
		return m_predictor ;
	}

};


} // namespace DecimationVolumes

} // namespace Algo

} // namespace CGoGN

#endif
