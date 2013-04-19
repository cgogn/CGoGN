/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2013, IGG Team, ICube, University of Strasbourg           *
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
	// EDGE selectors
	// Topo criteria
	S_MapOrder = 0,
	S_Random = 1,
	// Geom criteria
	S_EdgeLength = 2,
	S_QEM = 3,
	S_QEMml = 4,
	S_MinDetail = 5,
	S_Curvature = 6,
	S_NormalArea = 7,
	S_CurvatureTensor = 8,
	// Geom + color criteria
	S_ColorNaive = 9,
	S_QEMextColor = 10,
	S_GeomColOptGrad = 11,

	// HALF-EDGE selectors
	// Geom criteria
	S_hQEMml = 12,
	// Geom + color criteria
	S_hQEMextColor = 13,
	S_hColorGradient = 14,

	S_OTHER // for extensions use this type
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

public:
	EdgeSelector(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		m_map(m), m_position(pos), m_approximators(approx)
	{}
	virtual ~EdgeSelector()
	{}
	virtual SelectorType getType() = 0 ;
	virtual bool init() = 0 ;
	virtual bool nextEdge(Dart& d) const = 0 ;
	virtual void updateBeforeCollapse(Dart d) = 0 ;
	virtual void updateAfterCollapse(Dart d2, Dart dd2) = 0 ;
	virtual void updateWithoutCollapse() = 0;

	virtual void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		std::cout << "WARNING:: getEdgeErrors was not overridden" << std::endl ;
	}
} ;

} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#endif
