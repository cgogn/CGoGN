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
} ;

/*****************************************************************************************************************
 *                                 HALF-EDGE QEMextColor METRIC                                                  *
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
} ;

///*****************************************************************************************************************
// *                                 HALF-EDGE LIGHTFIELD METRIC                                                   *
// *****************************************************************************************************************/
//template <typename PFP>
//class HalfEdgeSelector_Lightfield : public EdgeSelector<PFP>
//{
//public:
//	typedef typename PFP::MAP MAP ;
//	typedef typename PFP::VEC3 VEC3 ;
//	typedef typename PFP::REAL REAL ;
//
//private:
//	typedef	struct
//	{
//		typename std::multimap<float,Dart>::iterator it ;
//		bool valid ;
//		static std::string CGoGNnameOfType() { return "LightfieldHalfEdgeInfo" ; }
//	} QEMhalfEdgeInfo ;
//	typedef NoMathIOAttribute<QEMhalfEdgeInfo> HalfEdgeInfo ;
//
//	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;
//
//	VertexAttribute<VEC3> m_pos, m_frameT, m_frameB, m_frameN ;
//	//VertexAttribute<VEC3> *m_HF ;
//	int m_approxindex_pos, m_attrindex_pos ;
//	int m_approxindex_FN, m_attrindex_FN ;
//
//	VertexAttribute<Utils::Quadric<REAL> > m_quadricGeom ;
//	VertexAttribute<Utils::QuadricHF<REAL> > m_quadricHF ;
//
//	std::multimap<float,Dart> halfEdges ;
//	typename std::multimap<float,Dart>::iterator cur ;
//
//	std::vector<Approximator<PFP, typename PFP::VEC3>* > m_approx ;
//
//	void initHalfEdgeInfo(Dart d) ;
//	void updateHalfEdgeInfo(Dart d, bool recompute) ;
//	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
//	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;
//
//public:
//	HalfEdgeSelector_Lightfield(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
//		EdgeSelector<PFP>(m, pos, approx, select),
////		m_positionApproximator(NULL),
////		m_frameApproximator(NULL),
////		m_hfcoefsApproximator(NULL),
////		m_pos(NULL),
////		m_frameB(NULL),
////		m_frameN(NULL),
////		m_frameT(NULL),
//		m_approxindex_pos(-1),
//		m_attrindex_pos(-1),
//		m_approxindex_FN(-1),
//		m_attrindex_FN(-1)
//	{
//		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
//		m_quadricGeom = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
//		m_quadricHF = m.template addAttribute<Utils::QuadricHF<REAL>, VERTEX>("HFquadric") ;
//	}
//	~HalfEdgeSelector_Lightfield()
//	{
//		this->m_map.removeAttribute(halfEdgeInfo) ;
//		this->m_map.removeAttribute(m_quadricGeom) ;
//		this->m_map.removeAttribute(m_quadricHF) ;
//	}
//	SelectorType getType() { return S_hLightfield ; }
//	bool init() ;
//	bool nextEdge(Dart& d) ;
//	void updateBeforeCollapse(Dart d) ;
//	void updateAfterCollapse(Dart d2, Dart dd2) ;
//} ;

/*template <typename PFP>
class HalfEdgeSelector_Lightfield_deprecated : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "LightfieldHalfEdgeInfo_deprecated" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	VertexAttribute<MATRIX33> m_frame ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;
	VertexAttribute<Utils::Quadric<REAL> > quadric ;
	EdgeAttribute<Utils::QuadricRGBfunctions<REAL> > quadricRGBfunctions ;

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
	HalfEdgeSelector_Lightfield_deprecated(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		m_frame = m.template getAttribute<MATRIX33, VERTEX>("frame") ;

		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		quadricRGBfunctions = m.template addAttribute<Utils::QuadricRGBfunctions<REAL>, EDGE>("quadricRGBfunctions") ;
	}
	~HalfEdgeSelector_Lightfield_deprecated()
	{
		this->m_map.removeAttribute(quadric) ;
		this->m_map.removeAttribute(quadricRGBfunctions) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hLightfield_deprecated ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;
} ;

/ *
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

	AttributeHandler<EdgeInfo, EDGE> edgeInfo ;
	AttributeHandler<Utils::Quadric<REAL>, VERTEX> quadric ;
	AttributeHandler<Utils::QuadricRGBfunctions<REAL>, EDGE> quadricRGBfunctions ;
	Utils::Quadric<REAL> tmpQ ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, VEC3>* m_positionApproximator ;
	Approximator<PFP, FRAME >* m_frameApproximator ;
	Approximator<PFP, RGBFUNCTIONS >* m_RGBfunctionsApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Lightfield(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		m_frame = m.template getAttribute<FRAME, VERTEX>("frame") ;

		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
		quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		quadricRGBfunctions = m.template addAttribute<Utils::QuadricRGBfunctions<REAL>, EDGE>("quadricRGBfunctions") ;
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
