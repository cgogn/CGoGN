#ifndef __SELECTORVOL_H__
#define __SELECTORVOL_H__

#include "Container/fakeAttribute.h"
#include "Algo/DecimationVolumique/operator.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumique
{

enum SelectorType
{
	S_MapOrder,
	S_Random,
	S_EdgeLength
} ;

template <typename PFP> class Approximator ;
//template <typename PFP> class Operator ;

/********************************************************************************
 *				 				Parent Selector									*
 ********************************************************************************/

//class du Selector de base
template <typename PFP>
class Selector
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

protected:
	/**
	 *
	 */
	MAP& m_map ;
	/**
	 *
	 */
	typename PFP::TVEC3& m_position ;
	/**
	 *
	 */
	Algo::DecimationVolumique::OperatorType nextOp;
	/**
	 * need a pointer to the current approximator if the current selector needs
	 * the future result of a collapse to estimate its cost
	 */
	Algo::DecimationVolumique::Approximator<PFP>* m_approximator ;

public:
	Selector(MAP& m, typename PFP::TVEC3& pos) :
		m_map(m), m_position(pos)
	{}

	/**
	 * Return the type of the selector
	 * @return the type of the selector
	 */
	virtual SelectorType getType() = 0 ;

	/**
	 *
	 */
	virtual Dart nextCell() = 0;

	/**
	 *
	 */
	virtual void init(Algo::DecimationVolumique::Approximator<PFP>* approx) = 0 ;

	/**
	 *
	 */
	virtual bool nextOperator() = 0 ;

	/**
	 *
	 */
	virtual void updateBeforeOperation(Algo::DecimationVolumique::Operator<PFP>* op) = 0 ;

	/**
	 *
	 */
	virtual void updateAfterOperation(Algo::DecimationVolumique::Operator<PFP>* op) = 0 ;

	/**
	 *
	 */
	virtual void finish() = 0 ;

	/**
	 *
	 */
	Algo::DecimationVolumique::Approximator<PFP>* getApproximator() { return m_approximator; }

	/**
	 *
	 */
	OperatorType nextOperatorType() { return nextOp; }

};

/********************************************************************************
 *			 				Parent Edge Selector								*
 ********************************************************************************/
template <typename PFP>
class EdgeSelector : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

protected:


public:
	EdgeSelector(MAP& m, typename PFP::TVEC3&  pos) :
		Selector<PFP>(m, pos)
	{}

	/*
	 * Initialiszation of the Edge selector
	 * TODO
	 */
	virtual void init(Algo::DecimationVolumique::Approximator<PFP>* approx);

//	/*
//	 * Select the next dart to collapse
//	 * TODO
//	 */
//	virtual bool nextOperator(Algo::DecimationVolumique::Operator<PFP>* op);
//	/*
//	 * TODO
//	 */
//	virtual void updateBeforeOperation(Algo::DecimationVolumique::Operator<PFP>* op);
//	/*
//	 * TODO
//	 */
//	virtual void updateAfterOperation(Algo::DecimationVolumique::Operator<PFP>* op);
//	/*
//	 * TODO
//	 */
//	virtual void finish();

};

/********************************************************************************
 *			 				Map Order Edge Selector								*
 ********************************************************************************/
template <typename PFP>
class EdgeSelector_MapOrder : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

private:
	Dart cur;

public:
	EdgeSelector_MapOrder(MAP& m, typename PFP::TVEC3&  pos) :
		EdgeSelector<PFP>(m, pos)
	{}

	~EdgeSelector_MapOrder()
	{}

	SelectorType getType() { return S_MapOrder; }
	Dart nextCell() { return cur; }

	void init(Algo::DecimationVolumique::Approximator<PFP>* approx);
	bool nextOperator();
	void updateBeforeOperation(Algo::DecimationVolumique::Operator<PFP>* op)
	{ }
	void updateAfterOperation(Algo::DecimationVolumique::Operator<PFP>* op);
	void finish()
	{ }

};

/********************************************************************************
 *				 				Random Selector									*
 ********************************************************************************/
template <typename PFP>
class EdgeSelector_Random : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	std::vector<Dart> darts ;
	unsigned int cur ;
	bool allSkipped ;

public:
	EdgeSelector_Random(MAP& m, typename PFP::TVEC3& pos) :
		EdgeSelector<PFP>(m, pos)
	{}

	~EdgeSelector_Random()
	{}

	SelectorType getType() { return S_Random; }
	Dart nextCell() { return darts[cur]; }

	void init(Algo::DecimationVolumique::Approximator<PFP>* approx);
	bool nextOperator();
	void updateBeforeOperation(Algo::DecimationVolumique::Operator<PFP>* op)
	{ }
	void updateAfterOperation(Algo::DecimationVolumique::Operator<PFP>* op);
	void finish()
	{ }

} ;

/********************************************************************************
 *			 				Length Edge Selector								*
 ********************************************************************************/
template <typename PFP>
class EdgeSelector_Length : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

protected:
	/*
	 * Update the Edge Informations
	 */
	void updateEdgeInfo(Dart d, typename PFP::VEC3 e);
	/*
	 * Init the Edge informations
	 */
	void initEdgeInfo(Dart d);
	/*
	 * Erase The Edge informations
	 */
	void eraseEdgeInfo(Dart d);

private:
	//New embedding type for storing informations
	typedef struct {
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
	} LengthEdgeInfo ;

	typedef NoMathIOAttribute<LengthEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;


public:
	EdgeSelector_Length(MAP& m, typename PFP::TVEC3&  pos) :
		EdgeSelector<PFP>(m, pos)
	{
		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE, "edgeInfo") ;
	}

	~EdgeSelector_Length()
	{
		this->m_map.removeAttribute(edgeInfo) ;
	}

	SelectorType getType() { return S_EdgeLength; }
	Dart nexCell() { return (*cur).second;  }

	void init(Algo::DecimationVolumique::Approximator<PFP>* approx);
	bool nextOperator();
	void updateBeforeOperation(Algo::DecimationVolumique::Operator<PFP>* op);
	void updateAfterOperation(Algo::DecimationVolumique::Operator<PFP>* op);
	void finish()
	{ }
};

} //end namespace DecimationVolumique
} //end namespace Algo
} //end namespace CGoGN


#include "Algo/DecimationVolumique/selector.hpp"

#endif
