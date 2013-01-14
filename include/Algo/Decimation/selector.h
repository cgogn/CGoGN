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

#ifndef __SELECTOR_H__
#define __SELECTOR_H__

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Decimation
{

enum SelectorType
{
	S_MapOrder = 0,
	S_Random,
	S_EdgeLength,
	S_QEM,
	S_QEMml,
	S_MinDetail,
	S_Curvature,
	S_ColorNaive,
	S_QEMextColor,
	S_Lightfield,
	// note: the following "h" prefix means that half-edges are prioritized instead of edges.
	S_hQEMextColor,
	S_hQEMml,
	S_hLightfield
} ;

template <typename PFP> class ApproximatorGen ;
template <typename PFP, typename T, unsigned int ORBIT> class Approximator ;

template <typename PFP>
class EdgeSelector
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;
	std::vector<ApproximatorGen<PFP>*>& m_approximators ;
	const FunctorSelect& m_select ;

public:
	EdgeSelector(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		m_map(m), m_position(pos), m_approximators(approx), m_select(select)
	{}
	virtual ~EdgeSelector()
	{}
	virtual SelectorType getType() = 0 ;
	virtual bool init() = 0 ;
	virtual bool nextEdge(Dart& d) = 0 ;
	virtual void updateBeforeCollapse(Dart d) = 0 ;
	virtual void updateAfterCollapse(Dart d2, Dart dd2) = 0 ;
	virtual void updateWithoutCollapse() = 0;

	virtual void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors)
	{
		std::cout << "WARNING:: getEdgeErrors was not overridden" << std::endl ;
	}
} ;

} // namespace Decimation

}

} // namespace Algo

} // namespace CGoGN

#endif
