#ifndef __OPERATORVOL_H__
#define __OPERATORVOL_H__

#include "Algo/Geometry/volume.h"


namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumique
{

enum OperatorType
{
	O_CEdge,
	O_CFace,
	O_CVolume
} ;


template <typename PFP> class Approximator ;

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
	Algo::DecimationVolumique::Approximator<PFP>* m_approximator ;

public:
	Operator(Dart d, Algo::DecimationVolumique::Approximator<PFP>* approx) :
		m_edge(d), m_approximator(approx)
	{}

	Operator() {}

	~Operator() {};

	Dart getEdge() {return m_edge;}
	void setEdge(Dart d) { m_edge = d; }
	OperatorType getType() {return O_CVolume;};

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
	CollapseOperator(Dart d, Algo::DecimationVolumique::Approximator<PFP>* approx) :
		Operator<PFP>(d, approx)
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
	CollapseEdgeOperator(Dart d, Algo::DecimationVolumique::Approximator<PFP>* approx) :
		CollapseOperator<PFP>(d, approx)
	{}

	~CollapseEdgeOperator()
	{ }

	OperatorType getType() { return O_CEdge; }
	unsigned int perform(MAP &m, VertexAttribute<typename PFP::VEC3>& position);
	bool canPerform(MAP &m ,Dart d, VertexAttribute<typename PFP::VEC3>& position);
};


} //end namespace DecimationVolumique
} //end namespace Algo
} //end namespace CGoGN

#include "Algo/DecimationVolumique/operator.hpp"

#endif
