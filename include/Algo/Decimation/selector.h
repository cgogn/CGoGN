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

#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include "Utils/qem.h"
#include "Utils/quadricRGBfunctions.h"
#include "Container/fakeAttribute.h"
#include "Algo/Geometry/curvature.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{


enum SelectorType
{
	S_MapOrder,
	S_Random,
	S_EdgeLength,
	S_QEM,
	S_QEMml,
	S_MinDetail,
	S_Curvature,
	S_Lightfield,
	S_LightfieldHalf
} ;


template <typename PFP> class ApproximatorGen ;
template <typename PFP, typename T> class Approximator ;


template <typename PFP>
class EdgeSelector
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	MAP& m_map ;
	typename PFP::TVEC3& m_position ;
	std::vector<ApproximatorGen<PFP>*>& m_approximators ;
	const FunctorSelect& m_select ;

public:
	EdgeSelector(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select) :
		m_map(m), m_position(pos), m_approximators(approx), m_select(select)
	{}
	virtual ~EdgeSelector()
	{}
	virtual SelectorType getType() = 0 ;
	virtual bool init() = 0 ;
	virtual bool nextEdge(Dart& d) = 0 ;
	virtual void updateBeforeCollapse(Dart d) = 0 ;
	virtual void updateAfterCollapse(Dart d2, Dart dd2) = 0 ;
} ;


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
	EdgeSelector_MapOrder(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
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
	EdgeSelector_Random(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
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

	AttributeHandler<EdgeInfo> edgeInfo ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Length(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE_ORBIT, "edgeInfo") ;
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

	AttributeHandler<EdgeInfo> edgeInfo ;
	AttributeHandler<Quadric<REAL> > quadric ;
	Quadric<REAL> tmpQ ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_QEM(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE_ORBIT, "edgeInfo") ;
		quadric = m.template addAttribute<Quadric<REAL> >(VERTEX_ORBIT, "QEMquadric") ;
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

	AttributeHandler<EdgeInfo> edgeInfo ;
	AttributeHandler<Quadric<REAL> > quadric ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

public:
	EdgeSelector_QEMml(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE_ORBIT, "edgeInfo") ;
		quadric = m.template addAttribute<Quadric<REAL> >(VERTEX_ORBIT, "QEMquadric") ;
	}
	~EdgeSelector_QEMml()
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
class HalfEdgeSelector_Lightfield : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "LightfieldHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	AttributeHandler<FRAME > m_frame ;

	AttributeHandler<HalfEdgeInfo> halfEdgeInfo ;
	AttributeHandler<Quadric<REAL> > quadric ;
	AttributeHandler<QuadricRGBfunctions<REAL> > quadricRGBfunctions ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, VEC3>* m_positionApproximator ;
	Approximator<PFP, FRAME >* m_frameApproximator ;
	Approximator<PFP, RGBFUNCTIONS >* m_RGBfunctionsApproximator ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors) ;

public:
	HalfEdgeSelector_Lightfield(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		m_frame = m.template getAttribute<FRAME>(VERTEX_ORBIT, "frame") ;

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
	SelectorType getType() { return S_Lightfield ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;
} ;

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

	typename PFP::TVEC3 normal ;
	AttributeHandler<EdgeInfo> edgeInfo ;
	typename PFP::TREAL k1 ;
	typename PFP::TREAL k2 ;
	typename PFP::TVEC3 K1 ;
	typename PFP::TVEC3 K2 ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_Curvature(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		normal = m.template getAttribute<VEC3>(VERTEX_ORBIT, "normal") ;
		if(!normal.isValid())
		{
			normal = m.template addAttribute<VEC3>(VERTEX_ORBIT, "normal") ;
			Algo::Geometry::computeNormalVertices<PFP>(m, pos, normal) ;
		}

		k1 = m.template getAttribute<REAL>(VERTEX_ORBIT, "k1") ;
		k2 = m.template getAttribute<REAL>(VERTEX_ORBIT, "k2") ;
		K1 = m.template getAttribute<VEC3>(VERTEX_ORBIT, "K1") ;
		K2 = m.template getAttribute<VEC3>(VERTEX_ORBIT, "K2") ;
		// as all these attributes are computed simultaneously by computeCurvatureVertices
		// one can assume that if one of them is not valid, the others must be created too
		if(!k1.isValid())
		{
			k1 = m.template addAttribute<REAL>(VERTEX_ORBIT, "k1") ;
			k2 = m.template addAttribute<REAL>(VERTEX_ORBIT, "k2") ;
			K1 = m.template addAttribute<VEC3>(VERTEX_ORBIT, "K1") ;
			K2 = m.template addAttribute<VEC3>(VERTEX_ORBIT, "K2") ;
			Algo::Geometry::computeCurvatureVertices<PFP>(m, this->m_position, normal, k1, k2, K1, K2) ;
		}

		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE_ORBIT, "edgeInfo") ;
	}
	~EdgeSelector_Curvature()
	{
		this->m_map.removeAttribute(k1) ;
		this->m_map.removeAttribute(k2) ;
		this->m_map.removeAttribute(K1) ;
		this->m_map.removeAttribute(K2) ;
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

	AttributeHandler<EdgeInfo> edgeInfo ;

	std::multimap<float,Dart> edges ;
	typename std::multimap<float,Dart>::iterator cur ;

	Approximator<PFP, typename PFP::VEC3>* m_positionApproximator ;

	void initEdgeInfo(Dart d) ;
	void updateEdgeInfo(Dart d, bool recompute) ;
	void computeEdgeInfo(Dart d, EdgeInfo& einfo) ;

public:
	EdgeSelector_MinDetail(MAP& m, typename PFP::TVEC3& pos, std::vector<ApproximatorGen<PFP>*>& approx, const FunctorSelect& select = SelectorTrue()) :
		EdgeSelector<PFP>(m, pos, approx, select)
	{
		edgeInfo = m.template addAttribute<EdgeInfo>(EDGE_ORBIT, "edgeInfo") ;
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

#include "Algo/Decimation/selector.hpp"

#endif
