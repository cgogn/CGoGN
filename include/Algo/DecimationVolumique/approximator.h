#ifndef __APPROXIMATORVOL_H__
#define __APPROXIMATORVOL_H__

#include "operator.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumique
{

enum ApproximatorType
{
	A_Centroid //barycenter of the n-cells
};

//template <typename PFP> class Selector ;


template <typename PFP>
class Approximator
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

protected:
	MAP& m_map ;
	VertexAttribute<VEC3>& m_position;
	CGoGN::Algo::DecimationVolumique::Selector<PFP>* m_selector;
	//
	VEC3 m_approx;

public:
	Approximator(MAP& m, VertexAttribute<VEC3>& pos):
		m_map(m), m_position(pos)
	{}

	VEC3 getApproximation() { return m_approx; }
	void setApproximation(VEC3 approx) { m_approx = approx; }

	virtual ApproximatorType getType() = 0;
	virtual void init(CGoGN::Algo::DecimationVolumique::Selector<PFP>* selector) = 0;
	virtual void approximate(CGoGN::Algo::DecimationVolumique::Operator<PFP> *op) = 0;
};

template <typename PFP>
class Approximator_Centroid : public Approximator<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

	Approximator_Centroid(MAP& m, VertexAttribute<VEC3>& pos):
		Approximator<PFP>(m, pos)
	{}

	ApproximatorType getType() { return A_Centroid; }
	void init(CGoGN::Algo::DecimationVolumique::Selector<PFP>* selector);
	void approximate(CGoGN::Algo::DecimationVolumique::Operator<PFP> *op);

} ;

} // namespace DecimationVolumique

} // namespace Algo

} // namespace CGoGN

#include "Algo/DecimationVolumique/approximator.hpp"

#endif
