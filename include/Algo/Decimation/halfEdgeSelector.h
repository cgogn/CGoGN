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

#ifndef __HALFEDGESELECTOR_H__
#define __HALFEDGESELECTOR_H__

#include "Algo/Decimation/selector.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

/*****************************************************************************************************************
 *                                 HALF-EDGE MEMORYLESS QEM METRIC                                               *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_QEMml : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMhalfEdgeInfo" ; }
	} QEMhalfEdgeInfo ;
	typedef NoMathIOAttribute<QEMhalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;
	VertexAttribute<Utils::Quadric<REAL> > quadric ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3, DART>* m_positionApproximator ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_QEMml(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_positionApproximator(NULL)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
	}
	~HalfEdgeSelector_QEMml()
	{
		this->m_map.removeAttribute(quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hQEMml ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

/*****************************************************************************************************************
 *                                 HALF-EDGE QEMextColor METRIC                                                  *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_QEMextColor : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename Geom::Vector<6,REAL> VEC6 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMextColorHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;
	VertexAttribute<Utils::QuadricNd<REAL,6> > m_quadric ;

	VertexAttribute<VEC3> m_pos, m_color ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_color, m_attrindex_color ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_QEMextColor(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_color(-1),
		m_attrindex_color(-1)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::QuadricNd<REAL,6>, VERTEX>("hQEMext-quadric") ;
	}
	~HalfEdgeSelector_QEMextColor()
	{
		this->m_map.removeAttribute(m_quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hQEMextColor ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors)
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			if (halfEdgeInfo[d].valid)
			{
				(*errors)[d] = halfEdgeInfo[d].it->first ;
			}
			else
				(*errors)[d] = -1 ;
		}
	}
} ;

/*****************************************************************************************************************
 *                                 HALF-EDGE LIGHTFIELD METRIC                                                   *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_Lightfield : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename Geom::Vector<6,REAL> VEC6 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMextColorHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;

	VertexAttribute<Utils::Quadric<REAL> > m_quadricGeom ;
	DartAttribute<Utils::QuadricHF<REAL> > m_quadricHF ;

	VertexAttribute<VEC3> m_pos, m_frameT, m_frameB, m_frameN ;
	std::vector<VertexAttribute<VEC3> > m_HF ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_FN, m_attrindex_FN ;
	std::vector<unsigned int> m_approxindex_HF, m_attrindex_HF ;
	unsigned int m_K ;

	std::vector<Approximator<PFP, typename PFP::VEC3, DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_Lightfield(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1),
		m_K(0)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadricGeom = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_quadricHF = m.template getAttribute<Utils::QuadricHF<REAL>, DART>("HFquadric") ;
	}
	~HalfEdgeSelector_Lightfield()
	{
		this->m_map.removeAttribute(m_quadricGeom) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hLightfield ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors)
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			(*errors)[d] = -1 ;
			if (halfEdgeInfo[d].valid)
			{
				(*errors)[d] = halfEdgeInfo[d].it->first ;
			}
			Dart dd = this->m_map.phi2(d) ;
			if (halfEdgeInfo[dd].valid && halfEdgeInfo[d].it->first < (*errors)[d])
			{
				(*errors)[d] = halfEdgeInfo[dd].it->first ;
			}
		}
	}
} ;

/*****************************************************************************************************************
 *                                 HALF-EDGE LIGHTFIELD METRIC experimental                                      *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_LightfieldExp : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename Geom::Vector<6,REAL> VEC6 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMextColorHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;

	VertexAttribute<Utils::Quadric<REAL> > m_quadricGeom ;
	DartAttribute<Utils::QuadricHF<REAL> > m_quadricHF ;

	VertexAttribute<VEC3> m_pos, m_frameT, m_frameB, m_frameN, m_avgColor ;
	std::vector<VertexAttribute<VEC3> > m_HF ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_FN, m_attrindex_FN ;
	std::vector<unsigned int> m_approxindex_HF, m_attrindex_HF ;
	unsigned int m_K ;
	int m_approxindex_color, m_attrindex_color ;

	std::vector<Approximator<PFP, typename PFP::VEC3, DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_LightfieldExp(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1),
		m_K(0),
		m_approxindex_color(-1),
		m_attrindex_color(-1)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadricGeom = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_quadricHF = m.template getAttribute<Utils::QuadricHF<REAL>, DART>("HFquadric") ;
		m_avgColor = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
		assert(m_avgColor.isValid()) ;
	}
	~HalfEdgeSelector_LightfieldExp()
	{
		this->m_map.removeAttribute(m_quadricGeom) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hLightfield ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors)
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			(*errors)[d] = -1 ;
			if (halfEdgeInfo[d].valid)
			{
				(*errors)[d] = halfEdgeInfo[d].it->first ;
			}
			Dart dd = this->m_map.phi2(d) ;
			if (halfEdgeInfo[dd].valid && halfEdgeInfo[d].it->first < (*errors)[d])
			{
				(*errors)[d] = halfEdgeInfo[dd].it->first ;
			}
		}
	}
} ;


} // namespace Decimation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/halfEdgeSelector.hpp"

#endif
