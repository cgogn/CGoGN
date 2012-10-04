#ifndef __SELECTORVOL_H__
#define __SELECTORVOL_H__

#include "Container/fakeAttribute.h"
#include "Algo/DecimationVolumes/operator.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

enum SelectorType
{
	S_QEM
} ;

template <typename PFP> class ApproximatorGen ;
template <typename PFP, typename T> class Approximator ;
template <typename PFP> class Operator ;

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
	VertexAttribute<typename PFP::VEC3>& m_position ;
//	/**
//	 *
//	 */
//	OperatorType nextOp;
	/**
	 * need a pointer to the current approximator if the current selector needs
	 * the future result of a collapse to estimate its cost
	 */
	std::vector<ApproximatorGen<PFP>*>& m_approximators ;

	const FunctorSelect& m_select ;

public:
	Selector(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		m_map(m), m_position(pos), m_approximators(approx), m_select(select)
	{}

	virtual ~Selector()
	{}

	/**
	 * Return the type of the selector
	 * @return the type of the selector
	 */
	virtual SelectorType getType() = 0 ;

//	/**
//	 *
//	 */
//	virtual Dart nextCell() = 0;

	/**
	 *
	 */
	virtual bool init() = 0 ;

	/**
	 *
	 */
	virtual Operator<PFP>* nextOperator() = 0 ;

	/**
	 *
	 */
	virtual bool updateBeforeOperation(Operator<PFP>* op) = 0 ;

	/**
	 *
	 */
	virtual void updateAfterOperation(Operator<PFP>* op) = 0 ;

	/**
	 *
	 */
	virtual void finish() = 0 ;

//	/**
//	 *
//	 */
//	OperatorType nextOperatorType() { return nextOp; }

};

} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN


#endif
