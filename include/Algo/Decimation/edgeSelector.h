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
	EdgeSelector_MapOrder(MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
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
	EdgeSelector_Random(MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{}
	~EdgeSelector_Random()
	{}
	SelectorType getType() { return S_Random ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d2)
	{}
	void updateAfterCollapse(Dart d2, Dart dd2) ;
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

	AttributeHandler<EdgeInfo, EDGE> edgeInfo ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Length(MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
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

	AttributeHandler<EdgeInfo, EDGE> edgeInfo ;
	AttributeHandler<Quadric<REAL>, VERTEX> quadric ;
	Quadric<REAL> tmpQ ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_QEM(MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select)
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

	AttributeHandler<EdgeInfo, EDGE> edgeInfo ;
	AttributeHandler<Quadric<REAL>, VERTEX> quadric ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_QEMml(MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select)
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

	AttributeHandler<VEC3, VERTEX> normal ;
	AttributeHandler<EdgeInfo, EDGE> edgeInfo ;
	AttributeHandler<REAL, EDGE> edgeangle ;
	AttributeHandler<REAL, VERTEX> kmax ;
	AttributeHandler<REAL, VERTEX> kmin ;
	AttributeHandler<VEC3, VERTEX> Kmax ;
	AttributeHandler<VEC3, VERTEX> Kmin ;
	AttributeHandler<VEC3, VERTEX> Knormal ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Curvature(MAP& m, AttributeHandler<VEC3, VERTEX>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select)
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

	AttributeHandler<EdgeInfo, EDGE> edgeInfo ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_MinDetail(MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		EdgeSelector<PFP>(m, pos, approx, select)
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
} ;

} // namespace Decimation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/edgeSelector.hpp"

#endif
