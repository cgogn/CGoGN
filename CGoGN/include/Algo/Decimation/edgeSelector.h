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

#ifndef __EDGESELECTOR_H__
#define __EDGESELECTOR_H__

#include "Algo/Decimation/selector.h"
#include "Algo/Decimation/approximator.h"
#include "Algo/Geometry/boundingbox.h"
#include "Utils/qem.h"
#include "Algo/Geometry/normal.h"
#include "Algo/Selection/collector.h"
#include "Algo/Geometry/curvature.h"
#include "Algo/Geometry/area.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Decimation
{

template <typename PFP>
class EdgeSelector_MapOrder : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP;

private:
	Dart cur ;

public:
	EdgeSelector_MapOrder(MAP& m) :
		Selector<PFP>(m)
	{}
	~EdgeSelector_MapOrder()
	{}
	SelectorType getType() { return S_MapOrder ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart /*d*/) {}
	void updateAfterCollapse(Dart d2, Dart dd2) ;
	void updateWithoutCollapse() {}
} ;

template <typename PFP>
class EdgeSelector_Random : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP;

private:
	std::vector<Dart> darts ;
	unsigned int cur ;
	bool allSkipped ;

public:
	EdgeSelector_Random(MAP& m) :
		Selector<PFP>(m),
		cur(0),
		allSkipped(false)
	{}
	~EdgeSelector_Random()
	{}
	SelectorType getType() { return S_Random ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart /*d2*/) {}
	void updateAfterCollapse(Dart d2, Dart dd2) ;
	void updateWithoutCollapse();
} ;

template <typename PFP>
class EdgeSelector_Length : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& position ;

	typedef struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LengthEdgeInfo" ; }
	} LengthEdgeInfo ;
	typedef NoTypeNameAttribute<LengthEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Length(MAP& m, const VertexAttribute<VEC3, MAP>& pos) :
		Selector<PFP>(m),
		position(pos)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
	}
	~EdgeSelector_Length()
	{
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_EdgeLength ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse();

	void getEdgeErrors(EdgeAttribute<REAL, MAP> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(edgeInfo.isValid()) ;

		TraversorE<MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			(*errors)[d] = -1 ;
			if (edgeInfo[d].valid)
			{
				(*errors)[d] = edgeInfo[d].it->first ;
			}
		}
	}
} ;

template <typename PFP>
class EdgeSelector_QEM : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator ;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMedgeInfo" ; }
	} QEMedgeInfo ;
	typedef NoTypeNameAttribute<QEMedgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	VertexAttribute<Utils::Quadric<REAL>, MAP> quadric ;
	Utils::Quadric<REAL> tmpQ ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_QEM(MAP& m, const VertexAttribute<VEC3, MAP>& pos, Approximator<PFP, VEC3, EDGE>& posApprox) :
		Selector<PFP>(m),
		m_position(pos),
		m_positionApproximator(posApprox)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
		std::string attrName = m_position.name();
		attrName += "_QEM";
		quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX, MAP>(attrName) ;
	}
	~EdgeSelector_QEM()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(quadric) ;
	}
	SelectorType getType() { return S_QEM ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse();
} ;

template <typename PFP>
class EdgeSelector_QEMml : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator ;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMedgeInfo" ; }
	} QEMedgeInfo ;
	typedef NoTypeNameAttribute<QEMedgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	VertexAttribute<Utils::Quadric<REAL>, MAP> quadric ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_QEMml(MAP& m, VertexAttribute<VEC3, MAP>& pos, Approximator<PFP, VEC3, EDGE>& posApprox) :
		Selector<PFP>(m),
		m_position(pos),
		m_positionApproximator(posApprox)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
		std::string attrName = m_position.name();
		attrName += "_QEM";
		quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX, MAP>(attrName) ;
	}
	~EdgeSelector_QEMml()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(quadric) ;
	}
	SelectorType getType() { return S_QEMml ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse();
} ;

template <typename PFP>
class EdgeSelector_NormalArea : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator ;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "NormalAreaEdgeInfo" ; }
	} NormalAreaEdgeInfo ;
	typedef NoTypeNameAttribute<NormalAreaEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	EdgeAttribute<Geom::Matrix<3,3,REAL>, MAP> edgeMatrix ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;
	void computeEdgeMatrix(Dart d) ;

public:
	EdgeSelector_NormalArea(MAP& m, VertexAttribute<VEC3, MAP>& pos, Approximator<PFP, VEC3, EDGE>& posApprox) :
		Selector<PFP>(m),
		m_position(pos),
		m_positionApproximator(posApprox)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
		edgeMatrix = m.template addAttribute<Geom::Matrix<3,3,REAL>, EDGE, MAP>("NormalAreaMatrix") ;
	}
	~EdgeSelector_NormalArea()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(edgeMatrix) ;
	}
	SelectorType getType() { return S_NormalArea ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

template <typename PFP>
class EdgeSelector_Curvature : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	VertexAttribute<VEC3, MAP>& m_position ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator ;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "CurvatureEdgeInfo" ; }
	} CurvatureEdgeInfo ;
	typedef NoTypeNameAttribute<CurvatureEdgeInfo> EdgeInfo ;

	Geom::BoundingBox<VEC3> bb ;
	REAL radius ;

	VertexAttribute<VEC3, MAP> normal ;
	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	EdgeAttribute<REAL, MAP> edgeangle ;
	EdgeAttribute<REAL, MAP> edgearea ;
	VertexAttribute<REAL, MAP> kmax ;
	VertexAttribute<REAL, MAP> kmin ;
	VertexAttribute<VEC3, MAP> Kmax ;
	VertexAttribute<VEC3, MAP> Kmin ;
	VertexAttribute<VEC3, MAP> Knormal ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Curvature(MAP& m, VertexAttribute<VEC3, MAP>& pos, Approximator<PFP, VEC3, EDGE>& posApprox) :
		Selector<PFP>(m),
		m_position(pos),
		m_positionApproximator(posApprox)
	{
		bb = Algo::Geometry::computeBoundingBox<PFP>(m, pos) ;
		radius = bb.diagSize() * 0.003f ;

		normal = m.template getAttribute<VEC3, VERTEX, MAP>("normal") ;
		if(!normal.isValid())
		{
			normal = m.template addAttribute<VEC3, VERTEX, MAP>("normal") ;
			Algo::Surface::Geometry::computeNormalVertices<PFP>(m, pos, normal) ;
		}

		edgeangle = m.template getAttribute<REAL, EDGE, MAP>("edgeangle") ;
		if(!edgeangle.isValid())
		{
			edgeangle = m.template addAttribute<REAL, EDGE, MAP>("edgeangle") ;
			Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP>(m, pos, edgeangle) ;
		}

		edgearea = m.template getAttribute<REAL, EDGE, MAP>("edgearea") ;
		if(!edgearea.isValid())
		{
			edgearea = m.template addAttribute<REAL, EDGE, MAP>("edgearea") ;
			Algo::Surface::Geometry::computeAreaEdges<PFP>(m, pos, edgearea) ;
		}

		kmax = m.template getAttribute<REAL, VERTEX, MAP>("kmax") ;
		kmin = m.template getAttribute<REAL, VERTEX, MAP>("kmin") ;
		Kmax = m.template getAttribute<VEC3, VERTEX, MAP>("Kmax") ;
		Kmin = m.template getAttribute<VEC3, VERTEX, MAP>("Kmin") ;
		Knormal = m.template getAttribute<VEC3, VERTEX, MAP>("Knormal") ;
		// as all these attributes are computed simultaneously by computeCurvatureVertices
		// one can assume that if one of them is not valid, the others must be created too
		if(!kmax.isValid())
		{
			kmax = m.template addAttribute<REAL, VERTEX, MAP>("kmax") ;
			kmin = m.template addAttribute<REAL, VERTEX, MAP>("kmin") ;
			Kmax = m.template addAttribute<VEC3, VERTEX, MAP>("Kmax") ;
			Kmin = m.template addAttribute<VEC3, VERTEX, MAP>("Kmin") ;
			Knormal = m.template addAttribute<VEC3, VERTEX, MAP>("Knormal") ;
			Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles<PFP>(m, radius, pos, normal, edgeangle, edgearea, kmax, kmin, Kmax, Kmin, Knormal) ;
		}

		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
	}
	~EdgeSelector_Curvature()
	{
//		this->m_map.removeAttribute(edgeangle) ;
//		this->m_map.removeAttribute(edgearea) ;
//		this->m_map.removeAttribute(kmax) ;
//		this->m_map.removeAttribute(kmin) ;
//		this->m_map.removeAttribute(Kmax) ;
//		this->m_map.removeAttribute(Kmin) ;
//		this->m_map.removeAttribute(Knormal) ;
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_Curvature ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse();
} ;


template <typename PFP>
class EdgeSelector_CurvatureTensor : public Selector<PFP>
{
	// TODO : this selector still needs to be tested
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	VertexAttribute<VEC3, MAP>& m_position ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator ;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "CurvatureTensorEdgeInfo" ; }
	} CurvatureTensorEdgeInfo ;
	typedef NoTypeNameAttribute<CurvatureTensorEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	EdgeAttribute<REAL, MAP> edgeangle ;
	EdgeAttribute<REAL, MAP> edgearea ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d) ; // TODO : usually has a 2nd arg (, bool recompute) : why ??
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_CurvatureTensor(MAP& m, VertexAttribute<VEC3, MAP>& pos, Approximator<PFP, VEC3, EDGE>& posApprox) :
		Selector<PFP>(m),
		m_position(pos),
		m_positionApproximator(posApprox)
	{
		edgeangle = m.template getAttribute<REAL, EDGE, MAP>("edgeangle") ;
		if(!edgeangle.isValid())
		{
			edgeangle = m.template addAttribute<REAL, EDGE, MAP>("edgeangle") ;
			Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP>(m, pos, edgeangle) ;
		}

		edgearea = m.template getAttribute<REAL, EDGE, MAP>("edgearea") ;
		if(!edgearea.isValid())
		{
			edgearea = m.template addAttribute<REAL, EDGE, MAP>("edgearea") ;
			Algo::Surface::Geometry::computeAreaEdges<PFP>(m, pos, edgearea) ;
		}

		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
	}
	~EdgeSelector_CurvatureTensor()
	{
//		this->m_map.removeAttribute(edgeangle) ;
//		this->m_map.removeAttribute(edgearea) ;
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_CurvatureTensor ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() {};
} ;

template <typename PFP>
class EdgeSelector_MinDetail : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator ;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "MinDetailEdgeInfo" ; }
	} MinDetailEdgeInfo ;
	typedef NoTypeNameAttribute<MinDetailEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_MinDetail(MAP& m, VertexAttribute<VEC3, MAP>& pos, Approximator<PFP, VEC3, EDGE>& posApprox) :
		Selector<PFP>(m),
		m_position(pos),
		m_positionApproximator(posApprox)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
	}
	~EdgeSelector_MinDetail()
	{
		this->m_map.removeAttribute(edgeInfo) ;
	}
	SelectorType getType() { return S_MinDetail ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse();
} ;

/*****************************************************************************************************************
 *                                      EDGE NAIVE COLOR METRIC (using QEMml)                                    *
 *****************************************************************************************************************/

template <typename PFP>
class EdgeSelector_ColorNaive : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	const VertexAttribute<VEC3, MAP>& m_color ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator;
	Approximator<PFP, VEC3, EDGE>& m_colorApproximator;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "ColorNaiveEdgeInfo" ; }
	} ColorNaiveedgeInfo ;
	typedef NoTypeNameAttribute<ColorNaiveedgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	VertexAttribute<Utils::Quadric<REAL>, MAP> m_quadric ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d,EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_ColorNaive(
		MAP& m,
		VertexAttribute<VEC3, MAP>& pos,
		VertexAttribute<VEC3, MAP>& col,
		Approximator<PFP, VEC3, EDGE>& posApprox,
		Approximator<PFP, VEC3, EDGE>& colApprox
	) :
		Selector<PFP>(m),
		m_position(pos),
		m_color(col),
		m_positionApproximator(posApprox),
		m_colorApproximator(colApprox)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX, MAP>("QEMquadric") ;
	}
	~EdgeSelector_ColorNaive()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_ColorNaive ; }
	bool init() ;
	bool nextEdge(Dart& d) const ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }
} ;

/*****************************************************************************************************************
 *                                  EDGE GEOMETRY+COLOR METRIC (using QEMml and Gradient norm)                   *
 *****************************************************************************************************************/

template <typename PFP>
class EdgeSelector_GeomColOptGradient : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	const VertexAttribute<VEC3, MAP>& m_color ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator;
	Approximator<PFP, VEC3, EDGE>& m_colorApproximator;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "GeomColOptGradEdgeInfo" ; }
	} ColorNaiveedgeInfo ;
	typedef NoTypeNameAttribute<ColorNaiveedgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	VertexAttribute<Utils::Quadric<REAL>, MAP> m_quadric ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d) ;
	void computeEdgeInfo(Dart d,EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;
	VEC3 computeEdgeGradientColorError(const Dart& v0, const VEC3& p, const VEC3& c) ;

public:
	EdgeSelector_GeomColOptGradient(
			MAP& m,
			VertexAttribute<VEC3, MAP>& pos,
			VertexAttribute<VEC3, MAP>& col,
			Approximator<PFP, VEC3, EDGE>& posApprox,
			Approximator<PFP, VEC3, EDGE>& colApprox
		) :
			Selector<PFP>(m),
			m_position(pos),
			m_color(col),
			m_positionApproximator(posApprox),
			m_colorApproximator(colApprox)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
		std::string attrName = m_position.name();
		attrName += "_QEM";
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX, MAP>(attrName) ;
	}
	~EdgeSelector_GeomColOptGradient()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_GeomColOptGrad ; }
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
		assert(edgeInfo.isValid()) ;

		TraversorE<MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			(*errors)[d] = -1 ;
			if (edgeInfo[d].valid)
			{
				(*errors)[d] = edgeInfo[d].it->first ;
			}
		}
	}
} ;

/*****************************************************************************************************************
 *                                 QEM extended to color metric                                                  *
 *****************************************************************************************************************/

template <typename PFP>
class EdgeSelector_QEMextColor : public Selector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename Geom::Vector<6,REAL> VEC6 ;

private:
	const VertexAttribute<VEC3, MAP>& m_position ;
	const VertexAttribute<VEC3, MAP>& m_color ;
	Approximator<PFP, VEC3, EDGE>& m_positionApproximator;
	Approximator<PFP, VEC3, EDGE>& m_colorApproximator;

	typedef	struct
	{
		typename std::multimap<REAL,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "QEMextColorEdgeInfo" ; }
	} QEMextColorEdgeInfo ;
	typedef NoTypeNameAttribute<QEMextColorEdgeInfo> EdgeInfo ;

	EdgeAttribute<EdgeInfo, MAP> edgeInfo ;
	VertexAttribute<Utils::QuadricNd<REAL,6>, MAP> m_quadric ;

	std::multimap<REAL,Dart> edges ;
	typename std::multimap<REAL,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_QEMextColor(
			MAP& m,
			VertexAttribute<VEC3, MAP>& pos,
			VertexAttribute<VEC3, MAP>& col,
			Approximator<PFP, VEC3, EDGE>& posApprox,
			Approximator<PFP, VEC3, EDGE>& colApprox
		) :
			Selector<PFP>(m),
			m_position(pos),
			m_color(col),
			m_positionApproximator(posApprox),
			m_colorApproximator(colApprox)
	{
		edgeInfo = m.template addAttribute<EdgeInfo, EDGE, MAP>("edgeInfo") ;
		m_quadric = m.template addAttribute<Utils::QuadricNd<REAL,6>, VERTEX, MAP>("QEMext-quadric") ;
	}
	~EdgeSelector_QEMextColor()
	{
		this->m_map.removeAttribute(edgeInfo) ;
		this->m_map.removeAttribute(m_quadric) ;
	}
	SelectorType getType() { return S_QEMextColor ; }
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
		assert(edgeInfo.isValid()) ;

		TraversorE<MAP> travE(this->m_map) ;
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			(*errors)[d] = -1 ;
			if (edgeInfo[d].valid)
			{
				(*errors)[d] = edgeInfo[d].it->first ;
			}
		}
	}
} ;

} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/edgeSelector.hpp"

#endif
