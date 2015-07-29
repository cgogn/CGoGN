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

#ifndef __HALFEDGESELECTOR_H__
#define __HALFEDGESELECTOR_H__

#include "Algo/Decimation/selector.h"
#include "Algo/Decimation/approximator.h"
#include "Utils/qem.h"
#include "Topology/generic/dart.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Decimation
{

/*****************************************************************************************************************
 *                                 HALF-EDGE MEMORYLESS QEM METRIC                                               *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_QEMml : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	Approximator<PFP, VEC3, DART>& m_positionApproximator ;

	typedef	struct
	{
		typename std::multimap<REAL, Dart>::iterator it;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMhalfEdgeInfo" ; }
	} QEMhalfEdgeInfo ;
	typedef NoTypeNameAttribute<QEMhalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo, MAP> halfEdgeInfo ;
	VertexAttribute<Utils::Quadric<REAL>, MAP> m_quadric ;

	std::multimap<REAL,Dart> halfEdges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_QEMml(MAP& m, VertexAttribute<VEC3, MAP>& pos, Approximator<PFP, VEC3, DART>& posApprox) :
		Selector<PFP>(m),
		m_position(pos),
		m_positionApproximator(posApprox)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART, MAP>("halfEdgeInfo") ;
		std::string attrName = m_position.name();
		attrName += "_QEM";
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX, MAP>(attrName) ;
	}
	~HalfEdgeSelector_QEMml()
	{
		this->m_map.removeAttribute(halfEdgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_hQEMml ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

/*****************************************************************************************************************
 *                                 HALF-EDGE QEMextColor METRIC                                                  *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_QEMextColor : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename Geom::Vector<6,REAL> VEC6 ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	const VertexAttribute<VEC3, MAP>& m_color ;
	Approximator<PFP, VEC3, DART>& m_positionApproximator;
	Approximator<PFP, VEC3, DART>& m_colorApproximator;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMextColorHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoTypeNameAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo, MAP> halfEdgeInfo ;
	VertexAttribute<Utils::QuadricNd<REAL,6>, MAP> m_quadric ;

	std::multimap<REAL,Dart> halfEdges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_QEMextColor(
			MAP& m,
			VertexAttribute<VEC3, MAP>& pos,
			VertexAttribute<VEC3, MAP>& col,
			Approximator<PFP, VEC3, DART>& posApprox,
			Approximator<PFP, VEC3, DART>& colApprox
		) :
			Selector<PFP>(m),
			m_position(pos),
			m_color(col),
			m_positionApproximator(posApprox),
			m_colorApproximator(colApprox)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART, MAP>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::QuadricNd<REAL,6>, VERTEX, MAP>("hQEMext-quadric") ;
	}
	~HalfEdgeSelector_QEMextColor()
	{
		this->m_map.removeAttribute(halfEdgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_hQEMextColor ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<REAL, MAP> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			Dart dd = this->m_map.phi2(d) ;
			if (halfEdgeInfo[d].valid)
			{
				(*errors)[d] = halfEdgeInfo[d].it->first ;
			}
			if (halfEdgeInfo[dd].valid && halfEdgeInfo[dd].it->first < (*errors)[d])
			{
				(*errors)[d] = halfEdgeInfo[dd].it->first ;
			}
			if (!(halfEdgeInfo[d].valid || halfEdgeInfo[dd].valid))
				(*errors)[d] = -1 ;
		}
	}
} ;

/*****************************************************************************************************************
 *                           HALF-EDGE QEMextColorNormal METRIC                                                  *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_QEMextColorNormal : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename Geom::Vector<9,REAL> VEC9 ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	const VertexAttribute<VEC3, MAP>& m_color ;
	const VertexAttribute<VEC3, MAP>& m_normal ;
	Approximator<PFP, VEC3, DART>& m_positionApproximator;
	Approximator<PFP, VEC3, DART>& m_colorApproximator;
	Approximator<PFP, VEC3, DART>& m_normalApproximator;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMextColorNormalHalfEdgeInfo" ; }
	} QEMextColorNormalHalfEdgeInfo ;
	typedef NoTypeNameAttribute<QEMextColorNormalHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo, MAP> halfEdgeInfo ;
	VertexAttribute<Utils::QuadricNd<REAL,9>, MAP> m_quadric ;

	std::multimap<REAL,Dart> halfEdges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_QEMextColorNormal(
			MAP& m,
			VertexAttribute<VEC3, MAP>& pos,
			VertexAttribute<VEC3, MAP>& col,
			VertexAttribute<VEC3, MAP>& norm,
			Approximator<PFP, VEC3, DART>& posApprox,
			Approximator<PFP, VEC3, DART>& colApprox,
			Approximator<PFP, VEC3, DART>& normApprox
		) :
			Selector<PFP>(m),
			m_position(pos),
			m_color(col),
			m_normal(norm),
			m_positionApproximator(posApprox),
			m_colorApproximator(colApprox),
			m_normalApproximator(normApprox)
	{
        halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART, MAP>("halfEdgeInfo") ;
        m_quadric = m.template addAttribute<Utils::QuadricNd<REAL,9>, VERTEX, MAP>("hQEMextNormal-quadric") ;
	}
	~HalfEdgeSelector_QEMextColorNormal()
	{
		this->m_map.removeAttribute(halfEdgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_hQEMextColorNormal ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<REAL, MAP> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			Dart dd = this->m_map.phi2(d) ;
			if (halfEdgeInfo[d].valid)
			{
				(*errors)[d] = halfEdgeInfo[d].it->first ;
			}
			if (halfEdgeInfo[dd].valid && halfEdgeInfo[dd].it->first < (*errors)[d])
			{
				(*errors)[d] = halfEdgeInfo[dd].it->first ;
			}
			if (!(halfEdgeInfo[d].valid || halfEdgeInfo[dd].valid))
				(*errors)[d] = -1 ;
		}
	}
} ;

/*****************************************************************************************************************
 *                                 HALF-EDGE COLOR GRADIENT ERR                                                  *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_ColorGradient : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	const VertexAttribute<VEC3, MAP>& m_color ;
	Approximator<PFP, VEC3, DART>& m_positionApproximator;
	Approximator<PFP, VEC3, DART>& m_colorApproximator;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "ColorExperimentalHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoTypeNameAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo, MAP> halfEdgeInfo ;
	VertexAttribute<Utils::Quadric<REAL>, MAP> m_quadric ;

	std::multimap<REAL,Dart> halfEdges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	//void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;
	void recomputeQuadric(const Dart d) ;

	VEC3 computeGradientColorError(const Dart& v0, const Dart& v1) const ;

public:
	HalfEdgeSelector_ColorGradient(
			MAP& m,
			VertexAttribute<VEC3, MAP>& pos,
			VertexAttribute<VEC3, MAP>& col,
			Approximator<PFP, VEC3, DART>& posApprox,
			Approximator<PFP, VEC3, DART>& colApprox
		) :
			Selector<PFP>(m),
			m_position(pos),
			m_color(col),
			m_positionApproximator(posApprox),
			m_colorApproximator(colApprox)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART, MAP>("halfEdgeInfo") ;
		std::string attrName = m_position.name();
		attrName += "_QEM";
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX, MAP>(attrName) ;
	}
	~HalfEdgeSelector_ColorGradient()
	{
		this->m_map.removeAttribute(halfEdgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_hColorGradient ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<REAL, MAP> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			Dart dd = this->m_map.phi2(d) ;
			if (halfEdgeInfo[d].valid)
			{
				(*errors)[d] = halfEdgeInfo[d].it->first ;
			}
			if (halfEdgeInfo[dd].valid && halfEdgeInfo[dd].it->first < (*errors)[d])
			{
				(*errors)[d] = halfEdgeInfo[dd].it->first ;
			}
			if (!(halfEdgeInfo[d].valid || halfEdgeInfo[dd].valid))
				(*errors)[d] = -1 ;

		}
	}
} ;

} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/halfEdgeSelector.hpp"

#endif
