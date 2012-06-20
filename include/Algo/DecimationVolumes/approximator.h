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

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

enum ApproximatorType
{
	A_Centroid //barycenter of the n-cells
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
	virtual void approximate(Operator<PFP>* op) = 0 ;
	virtual void saveApprox(Operator<PFP>* op) = 0 ;
	virtual void affectApprox(Operator<PFP>* op) = 0 ;
} ;


template <typename PFP, typename T>
class Approximator :  public ApproximatorGen<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL;

protected:

	//TODO ajouter un predictor

	VertexAttribute<T>& m_attrV; // vertex attribute to be approximated

	//TODO Attribute to store approximation result
	//TODO attribute to store detail information for reconstruction

	T m_app;

public:
	Approximator(MAP& m, VertexAttribute<T>& a):
		ApproximatorGen<PFP>(m), m_attrV(a)
	{}

	//virtual ~Approximator();

	const std::string& getApproximatedAttributeName() const
	{
		return m_attrV.name() ;
	}

	void saveApprox(Operator<PFP>* op)
	{
		Dart d = op->getEdge();
		//m_app = m_approx[d] ;
	}

	void affectApprox(Operator<PFP>* op)
	{
		Dart d = op->getEdge();
		m_attrV[d] = m_app ;
	}
};


} // namespace DecimationVolumes

} // namespace Algo

} // namespace CGoGN

#endif
