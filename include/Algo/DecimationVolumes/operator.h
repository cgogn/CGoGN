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

#ifndef __OPERATOR_VOLUMES_H__
#define __OPERATOR_VOLUMES_H__

#include "Algo/Geometry/volume.h"


namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

enum OperatorType
{
	O_CEdge,
	O_CFace,
	O_CVolume
} ;


//template <typename PFP, typename T> class Approximator ;
//template <typename PFP> class ApproximatorGen ;

template <typename PFP>
class Operator
{

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

protected:
	//
	Dart m_edge;

	/**
	 * need a pointer to the current approximator if the current selector needs
	 * the future result of a collapse to estimate its cost
	 */
	//ApproximatorGen<PFP>* m_approximator ;

public:
	Operator(Dart d) ://, ApproximatorGen<PFP>* approx) :
		m_edge(d)//, m_approximator(approx)
	{}

	Operator() {}

	~Operator() {};

	Dart getEdge() {return m_edge;}
	void setEdge(Dart d) { m_edge = d; }
	virtual OperatorType getType() = 0;

	virtual unsigned int perform(MAP& m, VertexAttribute<typename PFP::VEC3>& position) = 0;
	virtual bool canPerform(MAP &m ,Dart d, VertexAttribute<typename PFP::VEC3>& position) = 0;
};

template <typename PFP>
class CollapseOperator : public Operator<PFP>
{

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;
protected:


public:
	CollapseOperator(Dart d) : //, ApproximatorGen<PFP>* approx) :
		Operator<PFP>(d)//, approx)
	{}

	~CollapseOperator() {};

};

template <typename PFP>
class CollapseEdgeOperator : public CollapseOperator<PFP>
{

public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

protected:


public:
	CollapseEdgeOperator(Dart d) ://,  ApproximatorGen<PFP>* approx) :
		CollapseOperator<PFP>(d)//, approx)
	{}

	~CollapseEdgeOperator()
	{ }

	OperatorType getType() { return O_CEdge; }
	unsigned int perform(MAP &m, VertexAttribute<typename PFP::VEC3>& position);
	bool canPerform(MAP &m ,Dart d, VertexAttribute<typename PFP::VEC3>& position);
};


} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN

#include "Algo/DecimationVolumes/operator.hpp"

#endif
