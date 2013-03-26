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

#include "Algo/Decimation/predictor.h"

#ifndef __APPROXIMATOR_H__
#define __APPROXIMATOR_H__

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Decimation
{

enum ApproximatorType
{
	A_QEM = 0,
	A_MidEdge = 1,
	A_CornerCutting = 2,
	A_TangentPredict1 = 3,
	A_TangentPredict2 = 4,
	A_NormalArea = 5,
	A_ColorNaive = 6,
	A_ColorQEMext = 7,
	A_GeomColorOpt = 8,
	A_Lightfield = 9,
	// note: the following "h" prefix means that half-edges are prioritized instead of edges.
	A_hHalfCollapse = 10,
	A_hQEM = 11,
	A_hLightfieldHalf = 12
} ;

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
	virtual const std::string& getApproximatedAttributeName(unsigned int index = 0) const = 0 ;
//	virtual std::vector<std::string> getApproximatedAttributeNames() const = 0 ;
	virtual ApproximatorType getType() const = 0 ;
	virtual unsigned int getNbApproximated() const = 0 ;
	virtual bool init() = 0 ;
	virtual void approximate(Dart d) = 0 ;
	virtual void saveApprox(Dart d) = 0 ;
	virtual void affectApprox(Dart d) = 0 ;
	virtual const PredictorGen<PFP>* getPredictor() const = 0 ;
//	virtual REAL detailMagnitude(Dart d) = 0 ;
	virtual void addDetail(Dart d, double amount, bool sign, typename PFP::MATRIX33* detailTransform) = 0 ;
} ;


template <typename PFP, typename T, unsigned int ORBIT>
class Approximator : public ApproximatorGen<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	Predictor<PFP, T>* m_predictor ;

	std::vector<VertexAttribute<T>* > m_attrV ;	// vertex attributes to be approximated
	std::vector<AttributeHandler<T,ORBIT> > m_approx ;	// attributes to store approximation result
	std::vector<AttributeHandler<T,ORBIT> > m_detail ;	// attributes to store detail information for reconstruction
	std::vector<T> m_app ;

public:
	Approximator(MAP& m, std::vector<VertexAttribute<T>* > va, Predictor<PFP, T> * predictor) :
		ApproximatorGen<PFP>(m), m_predictor(predictor), m_attrV(va)
	{
		const unsigned int& size = m_attrV.size() ;
		assert(size > 0 || !"Approximator: no attributes provided") ;

		m_approx.resize(size) ;
		m_detail.resize(size) ;
		m_app.resize(size) ;

		for (unsigned int i = 0 ; i < size ; ++i)
		{
			if (!m_attrV[i]->isValid())
				std::cerr << "Approximator Warning: attribute number " << i << " is not valid" << std::endl ;

			std::stringstream aname ;
			aname << "approx_" << m_attrV[i]->name() ;
			m_approx[i] = this->m_map.template addAttribute<T, ORBIT>(aname.str()) ;

			if(m_predictor)	// if predictors are associated to the approximator
			{				// create attributes to store the details needed for reconstruction
				std::stringstream dname ;
				dname << "detail_" << m_attrV[i]->name() ;
				m_detail[i] = this->m_map.template addAttribute<T, ORBIT>(dname.str()) ;
			}
		}
	}

	virtual ~Approximator()
	{
		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
		{
			this->m_map.template removeAttribute(m_approx[i]) ;
			if(m_predictor)
				this->m_map.template removeAttribute(m_detail[i]) ;
		}
	}

	const std::string& getApproximatedAttributeName(unsigned int index = 0) const
	{
		return m_attrV[index]->name() ;
	}

//	std::vector<std::string> getApproximatedAttributeNames() const
//	{
//		std::vector<std::string> names ;
//		names.resize(m_attrV.size()) ;
//		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
//			names[i] = m_attrV[i]->name() ;
//
//		return names ;
//	}

	unsigned int getNbApproximated() const
	{
		return m_attrV.size() ;
	}

	void saveApprox(Dart d)
	{
		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
		{
			m_app[i] = m_approx[i][d] ;
		}
	}

	void affectApprox(Dart d)
	{
		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
		{
			m_attrV[i]->operator[](d) = m_app[i] ;
		}
	}

	const T& getApprox(Dart d, unsigned int index = 0) const
	{
		return m_approx[index][d] ;
	}

	const VertexAttribute<T>& getAttr(unsigned int index = 0) const
	{
		return *(m_attrV[index]) ;
	}

	std::vector<T> getAllApprox(Dart d) const
	{
		std::vector<T> res ;
		res.resize(m_attrV.size()) ;
		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
			res[i] = m_approx[i][d] ;

		return res ;
	}

	const Predictor<PFP, T>* getPredictor() const
	{
		return m_predictor ;
	}

	const T& getDetail(Dart d, unsigned int index = 0) const
	{
		assert(m_predictor || !"Trying to get detail on a non-predictive scheme") ;
		return m_detail[index][d] ;
	}

	std::vector<T> getAllDetail(Dart d) const
	{
		assert(m_predictor || !"Trying to get detail on a non-predictive scheme") ;

		std::vector<T> res ;
		res.resize(m_attrV.size()) ;
		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
			res[i] = m_detail[i][d] ;
		return res ;
	}

	void setDetail(Dart d, unsigned int index, T& val)
	{
		assert(m_predictor || !"Trying to set detail on a non-predictive scheme") ;
		m_detail[index][d] = val ;
	}

	void setDetail(Dart d, std::vector<T>& val)
	{
		assert(m_predictor || !"Trying to set detail on a non-predictive scheme") ;

		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
			m_detail[index][d] = val[i] ;
	}


//	// TODO works only for vector types !!
//	REAL detailMagnitude(Dart d)
//	{
//		assert(m_predictor || !"Trying to get detail magnitude on a non-predictive scheme") ;
//		return m_detail[d].norm2() ;
//	}

	void addDetail(Dart d, double amount, bool sign, typename PFP::MATRIX33* detailTransform)
	{
		assert(m_predictor || !"Trying to add detail on a non-predictive scheme") ;

		for (unsigned int i = 0 ; i < m_attrV.size() ; ++i)
		{
			T det = m_detail[i][d] ;
			if(detailTransform)
				det = (*detailTransform) * det ;
			det *= amount ;
			if(!sign)
				det *= REAL(-1) ;
			m_attrV[i]->operator[](d) += det ;
		}
	}
} ;

} //namespace Decimation

} // surface

} //namespace Algo

} //namespace CGoGN

#endif
