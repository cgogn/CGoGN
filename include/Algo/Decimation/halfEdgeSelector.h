/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
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

	AttributeHandler<HalfEdgeInfo> halfEdgeInfo ;
	AttributeHandler<Quadric<REAL> > quadric ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	HalfEdgeSelector_QEMml(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo>(DART_ORBIT, "halfEdgeInfo") ;
		quadric = m.template addAttribute<Quadric<REAL> >(VERTEX_ORBIT, "QEMquadric") ;
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
} ;

template <typename PFP>
class HalfEdgeSelector_Lightfield : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	typedef Geom::Matrix<3,3,REAL> MATRIX33 ;
	typedef Geom::Matrix<3,6,REAL> MATRIX36 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LightfieldHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	AttributeHandler<MATRIX33 > m_frame ;

	AttributeHandler<HalfEdgeInfo> halfEdgeInfo ;
	AttributeHandler<Quadric<REAL> > quadric ;
	AttributeHandler<QuadricRGBfunctions<REAL> > quadricRGBfunctions ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, VEC3>* m_positionApproximator ;
	Approximator<PFP, MATRIX33 >* m_frameApproximator ;
	Approximator<PFP, MATRIX36 >* m_RGBfunctionsApproximator ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors) ;

public:
	HalfEdgeSelector_Lightfield(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		m_frame = m.template getAttribute<MATRIX33>(VERTEX_ORBIT, "frame") ;

		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo>(DART_ORBIT, "halfEdgeInfo") ;
		quadric = m.template addAttribute<Quadric<REAL> >(VERTEX_ORBIT, "QEMquadric") ;
		quadricRGBfunctions = m.template addAttribute<QuadricRGBfunctions<REAL> >(EDGE_ORBIT, "quadricRGBfunctions") ;
	}
	~HalfEdgeSelector_Lightfield()
	{
		this->m_map.removeAttribute(quadric) ;
		this->m_map.removeAttribute(quadricRGBfunctions) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hLightfield ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;
} ;

/*
template <typename PFP>
class EdgeSelector_Lightfield : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	typedef Geom::Matrix<3,3,REAL> MATRIX33 ;
	typedef MATRIX33 FRAME ;
	typedef Geom::Matrix<3,6,REAL> MATRIX36 ;
	typedef MATRIX36 RGBFUNCTIONS ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LightfieldEdgeInfo" ; }
	} LightfieldEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldEdgeInfo> EdgeInfo ;

	AttributeHandler<FRAME > m_frame ;

	AttributeHandler<EdgeInfo> edgeInfo ;
	AttributeHandler<Quadric<REAL> > quadric ;
	AttributeHandler<QuadricRGBfunctions<REAL> > quadricRGBfunctions ;
	Quadric<REAL> tmpQ ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, VEC3>* m_positionApproximator ;
	Approximator<PFP, FRAME >* m_frameApproximator ;
	Approximator<PFP, RGBFUNCTIONS >* m_RGBfunctionsApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Lightfield(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		m_frame = m.template getAttribute<FRAME>(VERTEX_ORBIT, "frame") ;

		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE_ORBIT, "edgeInfo") ;
		quadric = m.template addAttribute<Quadric<REAL> >(VERTEX_ORBIT, "QEMquadric") ;
		quadricRGBfunctions = m.template addAttribute<QuadricRGBfunctions<REAL> >(EDGE_ORBIT, "quadricRGBfunctions") ;
	}
	~EdgeSelector_Lightfield()
	{
		this->m_map.removeAttribute(quadric) ;
		this->m_map.removeAttribute(quadricRGBfunctions) ;
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_Lightfield ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;
} ;*/

} // namespace Decimation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/halfEdgeSelector.hpp"

#endif
