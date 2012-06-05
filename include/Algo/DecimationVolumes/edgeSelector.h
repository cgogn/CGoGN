#ifndef __EDGE_SELECTOR_VOLUMES_H__
#define __EDGE_SELECTOR_VOLUMES_H__

#include "Container/fakeAttribute.h"
#include "Algo/DecimationVolumes/selector.h"
#include "Algo/DecimationVolumes/operator.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

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
	EdgeSelector(MAP& m, VertexAttribute<typename PFP::VEC3>&  pos) :
		Selector<PFP>(m, pos)
	{}

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
	EdgeSelector_MapOrder(MAP& m, VertexAttribute<typename PFP::VEC3>&  pos) :
		EdgeSelector<PFP>(m, pos)
	{}

	~EdgeSelector_MapOrder()
	{}

	SelectorType getType() { return S_MapOrder; }
	Dart nextCell() { return cur; }

	void init();
	bool nextOperator(Operator<PFP>* op);
	void updateBeforeOperation(Operator<PFP>* op)
	{ }
	void updateAfterOperation(Operator<PFP>* op);
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
	EdgeSelector_Random(MAP& m, VertexAttribute<typename PFP::VEC3>& pos) :
		EdgeSelector<PFP>(m, pos)
	{}

	~EdgeSelector_Random()
	{}

	SelectorType getType() { return S_Random; }
//	Dart nextCell() { return darts[cur]; }

	void init();
	bool nextOperator(Operator<PFP>* op);
	void updateBeforeOperation(Operator<PFP>* op)
	{ }
	void updateAfterOperation(Operator<PFP>* op);
	void finish()
	{ }

} ;

///********************************************************************************
// *			 				Length Edge Selector								*
// ********************************************************************************/
//template <typename PFP>
//class EdgeSelector_Length : public EdgeSelector<PFP>
//{
//public:
//	typedef typename PFP::MAP MAP;
//	typedef typename PFP::VEC3 VEC3;
//	typedef typename PFP::REAL REAL;
//
//protected:
//	/*
//	 * Update the Edge Informations
//	 */
//	void updateEdgeInfo(Dart d, typename PFP::VEC3 e);
//	/*
//	 * Init the Edge informations
//	 */
//	void initEdgeInfo(Dart d);
//	/*
//	 * Erase The Edge informations
//	 */
//	void eraseEdgeInfo(Dart d);
//
//private:
//	//New embedding type for storing informations
//	typedef struct {
//		typename std::multimap<float,Dart>::iterator it ;
//		bool valid ;
//	} LengthEdgeInfo ;
//
//	typedef NoMathIOAttribute<LengthEdgeInfo> EdgeInfo ;
//
//	EdgeAttribute<EdgeInfo> edgeInfo ;
//
//	std::multimap<float,Dart> edges ;
//	typename std::multimap<float,Dart>::iterator cur ;
//
//
//public:
//	EdgeSelector_Length(MAP& m, VertexAttribute<typename PFP::VEC3>&  pos) :
//		EdgeSelector<PFP>(m, pos)
//	{
//		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE, "edgeInfo") ;
//	}
//
//	~EdgeSelector_Length()
//	{
//		this->m_map.removeAttribute(edgeInfo) ;
//	}
//
//	SelectorType getType() { return S_EdgeLength; }
//	Dart nexCell() { return (*cur).second;  }
//
//	void init(Algo::DecimationVolumique::Approximator<PFP>* approx);
//	bool nextOperator();
//	void updateBeforeOperation(Algo::DecimationVolumique::Operator<PFP>* op);
//	void updateAfterOperation(Algo::DecimationVolumique::Operator<PFP>* op);
//	void finish()
//	{ }
//};

} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN


#include "Algo/DecimationVolumes/selector.hpp"

#endif
