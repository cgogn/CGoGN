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

#ifndef __EDGESELECTOR_H__
#define __EDGESELECTOR_H__

#include "Algo/Decimation/selector.h"
#include "Algo/Geometry/boundingbox.h"

#include "Container/fakeAttribute.h"
#include "Utils/qem.h"
#include "Utils/quadricRGBfunctions.h"
#include "Algo/Geometry/curvature.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

template <typename PFP>
class EdgeSelector_MapOrder : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	Dart cur ;

public:
	EdgeSelector_MapOrder(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{}
	~EdgeSelector_MapOrder()
	{}
	SelectorType getType() { return S_MapOrder ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d)
	{}
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

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
	EdgeSelector_Random(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select),
		cur(0),
		allSkipped(false)
	{}
	~EdgeSelector_Random()
	{}
	SelectorType getType() { return S_Random ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d2)
	{}
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

template <typename PFP>
class EdgeSelector_Length : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	typedef struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LengthEdgeInfo" ; }
	} LengthEdgeInfo ;
	typedef NoMathIOAttribute<LengthEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Length(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
	}
	~EdgeSelector_Length()
	{
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_EdgeLength ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

template <typename PFP>
class EdgeSelector_QEM : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "QEMedgeInfo" ; }
	} QEMedgeInfo ;
	typedef NoMathIOAttribute<QEMedgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;
	VertexAttribute<Quadric<REAL> > quadric ;
	Quadric<REAL> tmpQ ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_QEM(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_positionApproximator(NULL)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
		quadric = m.template addAttribute<Quadric<REAL>, VERTEX>("QEMquadric") ;
	}
	~EdgeSelector_QEM()
	{
		this->m_map.removeAttribute(quadric) ;
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_QEM ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse();
} ;

template <typename PFP>
class EdgeSelector_QEMml : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "QEMedgeInfo" ; }
	} QEMedgeInfo ;
	typedef NoMathIOAttribute<QEMedgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;
	VertexAttribute<Quadric<REAL> > quadric ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_QEMml(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_positionApproximator(NULL)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
		quadric = m.template addAttribute<Quadric<REAL>, VERTEX>("QEMquadric") ;
	}
	~EdgeSelector_QEMml()
	{
		this->m_map.removeAttribute(quadric) ;
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_QEMml ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

template <typename PFP>
class EdgeSelector_Curvature : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "CurvatureEdgeInfo" ; }
	} CurvatureEdgeInfo ;
	typedef NoMathIOAttribute<CurvatureEdgeInfo> EdgeInfo ;

	Geom::BoundingBox<VEC3> bb ;
	REAL radius ;

	VertexAttribute<VEC3> normal ;
	EdgeAttribute<EdgeInfo> edgeInfo ;
	EdgeAttribute<REAL> edgeangle ;
	VertexAttribute<REAL> kmax ;
	VertexAttribute<REAL> kmin ;
	VertexAttribute<VEC3> Kmax ;
	VertexAttribute<VEC3> Kmin ;
	VertexAttribute<VEC3> Knormal ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Curvature(MAP& m, VertexAttribute<VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_positionApproximator(NULL)
	{
		bb = Algo::Geometry::computeBoundingBox<PFP>(m, pos) ;
		radius = bb.diagSize() * 0.003 ;

		normal = m.template getAttribute<VEC3, VERTEX>("normal") ;
		if(!normal.isValid())
		{
			normal = m.template addAttribute<VEC3, VERTEX>("normal") ;
			Algo::Geometry::computeNormalVertices<PFP>(m, pos, normal) ;
		}

		edgeangle = m.template getAttribute<REAL, EDGE>("edgeangle") ;
		if(!edgeangle.isValid())
		{
			edgeangle = m.template addAttribute<REAL, EDGE>("edgeangle") ;
			Algo::Geometry::computeAnglesBetweenNormalsOnEdges<PFP>(m, pos, edgeangle) ;
		}

		kmax = m.template getAttribute<REAL, VERTEX>("kmax") ;
		kmin = m.template getAttribute<REAL, VERTEX>("kmin") ;
		Kmax = m.template getAttribute<VEC3, VERTEX>("Kmax") ;
		Kmin = m.template getAttribute<VEC3, VERTEX>("Kmin") ;
		Knormal = m.template getAttribute<VEC3, VERTEX>("Knormal") ;
		// as all these attributes are computed simultaneously by computeCurvatureVertices
		// one can assume that if one of them is not valid, the others must be created too
		if(!kmax.isValid())
		{
			kmax = m.template addAttribute<REAL, VERTEX>("kmax") ;
			kmin = m.template addAttribute<REAL, VERTEX>("kmin") ;
			Kmax = m.template addAttribute<VEC3, VERTEX>("Kmax") ;
			Kmin = m.template addAttribute<VEC3, VERTEX>("Kmin") ;
			Knormal = m.template addAttribute<VEC3, VERTEX>("Knormal") ;
			Algo::Geometry::computeCurvatureVertices_NormalCycles<PFP>(m, radius, pos, normal, edgeangle, kmax, kmin, Kmax, Kmin, Knormal) ;
		}

		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
	}
	~EdgeSelector_Curvature()
	{
		this->m_map.removeAttribute(edgeangle) ;
		this->m_map.removeAttribute(kmax) ;
		this->m_map.removeAttribute(kmin) ;
		this->m_map.removeAttribute(Kmax) ;
		this->m_map.removeAttribute(Kmin) ;
		this->m_map.removeAttribute(Knormal) ;
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_Curvature ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

template <typename PFP>
class EdgeSelector_MinDetail : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "MinDetailEdgeInfo" ; }
	} MinDetailEdgeInfo ;
	typedef NoMathIOAttribute<MinDetailEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_MinDetail(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_positionApproximator(NULL)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
	}
	~EdgeSelector_MinDetail()
	{
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_MinDetail ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

/*****************************************************************************************************************
 *                                      EDGE NAIVE COLOR METRIC (using QEMml)                                    *
 *****************************************************************************************************************/
template <typename PFP>
class EdgeSelector_ColorNaive : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "ColorNaiveEdgeInfo" ; }
	} ColorNaiveedgeInfo ;
	typedef NoMathIOAttribute<ColorNaiveedgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;
	VertexAttribute<Quadric<REAL> > m_quadric ;

	VertexAttribute<VEC3> m_pos, m_color ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_color, m_attrindex_color ;

	std::vector<Approximator<PFP, typename PFP::VEC3>* > m_approx ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d,EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_ColorNaive(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_color(-1),
		m_attrindex_color(-1)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
		m_quadric = m.template addAttribute<Quadric<REAL>, VERTEX>("QEMquadric") ;
	}
	~EdgeSelector_ColorNaive()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_ColorNaive ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;
} ;

/*****************************************************************************************************************
 *                                 QEM extended to color metric                                                  *
 *****************************************************************************************************************/
template <typename PFP>
class EdgeSelector_QEMextColor : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "QEMextColorEdgeInfo" ; }
	} QEMextColorEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;
	VertexAttribute<QuadricNd<REAL,6> > m_quadric ;

	VertexAttribute<VEC3> m_pos, m_color ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_color, m_attrindex_color ;

	std::vector<Approximator<PFP, typename PFP::VEC3>* > m_approx ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d,EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_QEMextColor(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_color(-1),
		m_attrindex_color(-1)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
		m_quadric = m.template addAttribute<QuadricNd<REAL,6>, VERTEX>("QEMext-quadric") ;
	}
	~EdgeSelector_QEMextColor()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_QEMextColor ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;
} ;

/*****************************************************************************************************************
 *                                 LIGHTFIELD QUADRIC ERROR METRIC                                               *
 *****************************************************************************************************************/
template <typename PFP>
class EdgeSelector_Lightfield : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "QEMextColorEdgeInfo" ; }
	} QEMextColorEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo> edgeInfo ;

	VertexAttribute<VEC3> m_pos, m_frameT, m_frameB, m_frameN ;
	//VertexAttribute<VEC3> *m_HF ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_FN, m_attrindex_FN ;

	VertexAttribute<Quadric<REAL> > m_quadricGeom ;
	VertexAttribute<QuadricHF<REAL> > m_quadricHF ;

	std::vector<Approximator<PFP, typename PFP::VEC3>* > m_approx ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d,EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_Lightfield(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = allDarts) :
		EdgeSelector<PFP>(m, pos, approx, select),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE>("edgeInfo") ;
		m_quadricGeom = m.template addAttribute<Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_quadricHF = m.template addAttribute<QuadricHF<REAL>, VERTEX>("HFquadric") ;
	}
	~EdgeSelector_Lightfield()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(m_quadricGeom) ;
		this->m_map.removeAttribute(m_quadricHF) ;
	}
	SelectorType getType() { return S_Lightfield ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;
} ;

} // namespace Decimation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/edgeSelector.hpp"

#endif
