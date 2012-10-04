#ifndef __EDGE_SELECTOR_VOLUMES_H__
#define __EDGE_SELECTOR_VOLUMES_H__

#include "Container/fakeAttribute.h"
#include "Algo/DecimationVolumes/selector.h"
#include "Algo/DecimationVolumes/operator.h"
#include "Utils/qem.h"
#include "Topology/generic/traversorCell.h"

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
	EdgeSelector(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		Selector<PFP>(m, pos, approx, select)
	{}

};

template <typename PFP>
class EdgeSelector_QEM : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

private:
	typedef struct
	{
		typename std::multimap<float, Dart>::iterator it;
		bool valid;
		static std::string CGoGNnameOfType() { return "QEMedgeInfo" ; }
	} QEMedgeInfo;

	typedef NoMathIOAttribute<QEMedgeInfo> EdgeInfo;

	EdgeAttribute<EdgeInfo> edgeInfo;
	VertexAttribute<Quadric<REAL> > quadric;
	Quadric<REAL> tmpQ;

	std::multimap<float, Dart> edges;
	typename std::multimap<float, Dart>::iterator cur;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator;

	void initEdgeInfo(Dart d);
	void updateEdgeInfo(Dart d, bool recompute);
	void computeEdgeInfo(Dart d, EdgeInfo& einfo);

public:
	EdgeSelector_QEM(MAP& m, VertexAttribute<typename PFP::VEC3>& pos,
			std::vector<ApproximatorGen<PFP>* >& approx, const FunctorSelect& select) :
				EdgeSelector<PFP>(m, pos, approx, select)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo");
		quadric = m.template addAttribute<Quadric<REAL>, VERTEX>("QEMquadric");
	}

	~EdgeSelector_QEM()
	{
		this->m_map.removeAttribute(quadric);
		this->m_map.removeAttribute(edgeInfo);
	}

	SelectorType getType() { return S_QEM; }
	bool init();
	Operator<PFP>* nextOperator();
	bool updateBeforeOperation(Operator<PFP>* op);
	void updateAfterOperation(Operator<PFP>* op);
	void finish() { }
};




} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN


#include "Algo/DecimationVolumes/edgeSelector.hpp"

#endif
