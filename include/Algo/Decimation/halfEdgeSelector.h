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
#include "Utils/SphericalFunctionIntegratorCartesian.h"

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
	HalfEdgeSelector_QEMml(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
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
	HalfEdgeSelector_QEMextColor(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
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

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
		static std::string CGoGNnameOfType() { return "LightfieldHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

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
	HalfEdgeSelector_Lightfield(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
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

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
 *                                 HALF-EDGE LIGHTFIELD METRIC experimental                                      *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_LightfieldAvgColor : public EdgeSelector<PFP>
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
		static std::string CGoGNnameOfType() { return "LightfieldHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;

	VertexAttribute<Utils::Quadric<REAL> > m_quadricGeom ;
	DartAttribute<Utils::QuadricHF<REAL> > m_quadricHF ;

	VertexAttribute<VEC3> m_pos, m_frameT, m_frameB, m_frameN, m_avgColor ;
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
	HalfEdgeSelector_LightfieldAvgColor(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1),
		m_K(0)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadricGeom = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_quadricHF = m.template getAttribute<Utils::QuadricHF<REAL>, DART>("HFquadric") ;
		m_avgColor = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
		assert(m_avgColor.isValid()) ;
	}
	~HalfEdgeSelector_LightfieldAvgColor()
	{
		this->m_map.removeAttribute(m_quadricGeom) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hLightfieldAvgColor ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
 *                                 HALF-EDGE LIGHTFIELD METRIC KCL08-like                                        *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_LightfieldKCL : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LightfieldHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;

	VertexAttribute<Utils::Quadric<REAL> > m_quadricGeom ;

	VertexAttribute<REAL> m_visualImportance ;
	VertexAttribute<VEC3> m_avgColor ;

	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_FT, m_attrindex_FT ;
	int m_approxindex_FB, m_attrindex_FB ;
	int m_approxindex_FN, m_attrindex_FN ;
	std::vector<unsigned int> m_approxindex_HF, m_attrindex_HF ;
	unsigned int m_K ;

	REAL tmpVisualImportance ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d, bool recompute) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;

	REAL computeLightfieldError(Dart v0) const ;
	REAL computeSquaredLightfieldDifference(Dart d1, Dart d2) const ;

public:
	HalfEdgeSelector_LightfieldKCL(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FT(-1),
		m_attrindex_FT(-1),
		m_approxindex_FB(-1),
		m_attrindex_FB(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1),
		m_K(0)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadricGeom = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_visualImportance = m.template addAttribute<typename PFP::REAL, VERTEX>("VisualImportance") ;
		m_avgColor = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
				assert(m_avgColor.isValid()) ;
	}
	~HalfEdgeSelector_LightfieldKCL()
	{
		this->m_map.removeAttribute(m_quadricGeom) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
		this->m_map.removeAttribute(m_visualImportance) ;
	}
	SelectorType getType() { return S_hLightfieldKCL ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
 *                                 HALF-EDGE COLOR EXPERIMENTAL                                                  *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_ColorExperimental : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "ColorExperimentalHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;
	VertexAttribute<Utils::Quadric<REAL> > m_quadric ;

	VertexAttribute<VEC3> m_pos, m_color ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_color, m_attrindex_color ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	//void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;
	void recomputeQuadric(const Dart d) ;

	typename PFP::VEC3 computeExperimentalColorError(const Dart& v0, const Dart& v1) const ;


public:
	HalfEdgeSelector_ColorExperimental(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_color(-1),
		m_attrindex_color(-1)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
	}
	~HalfEdgeSelector_ColorExperimental()
	{
		this->m_map.removeAttribute(m_quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hColorExperimental ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
class HalfEdgeSelector_ColorGradient : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "ColorExperimentalHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;
	VertexAttribute<Utils::Quadric<REAL> > m_quadric ;

	VertexAttribute<VEC3> m_pos, m_color ;
	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_color, m_attrindex_color ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	//void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;
	void recomputeQuadric(const Dart d) ;

	typename PFP::VEC3 computeGradientColorError(const Dart& v0, const Dart& v1) const ;


public:
	HalfEdgeSelector_ColorGradient(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_color(-1),
		m_attrindex_color(-1)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
	}
	~HalfEdgeSelector_ColorGradient()
	{
		this->m_map.removeAttribute(m_quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hColorGradient ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
 *                                 HALF-EDGE LF EXPERIMENTAL METRIC                                              *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_LFexperimental : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LightfieldExpHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;

	VertexAttribute<Utils::Quadric<REAL> > m_quadric ;

	VertexAttribute<REAL> m_visualImportance ;
	VertexAttribute<VEC3> m_avgColor ;

	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_FT, m_attrindex_FT ;
	int m_approxindex_FB, m_attrindex_FB ;
	int m_approxindex_FN, m_attrindex_FN ;
	std::vector<unsigned int> m_approxindex_HF, m_attrindex_HF ;
	unsigned int m_K ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d) ;

	REAL computeLightfieldError(const Dart& v0, const Dart& v1) const ;
	REAL computeSquaredLightfieldDifference(const Dart& d1, const Dart& d2) const ;

public:
	HalfEdgeSelector_LFexperimental(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FT(-1),
		m_attrindex_FT(-1),
		m_approxindex_FB(-1),
		m_attrindex_FB(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1),
		m_K(0)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_avgColor = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
		assert(m_avgColor.isValid()) ;
	}
	~HalfEdgeSelector_LFexperimental()
	{
		this->m_map.removeAttribute(m_quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}

	SelectorType getType() { return S_hLFexperimental ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
 *                                 HALF-EDGE LF GRADIENT METRIC                                                  *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_LFgradient : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LightfieldGradHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;

	VertexAttribute<Utils::Quadric<REAL> > m_quadric ;

	VertexAttribute<VEC3> m_avgColor ;

	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_FT, m_attrindex_FT ;
	int m_approxindex_FB, m_attrindex_FB ;
	int m_approxindex_FN, m_attrindex_FN ;
	std::vector<unsigned int> m_approxindex_HF, m_attrindex_HF ;
	unsigned int m_K ;

	SphericalFunctionIntegratorCartesian m_integrator ;
	double *m_n ;
	double *m_workspace ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d) ;

	VEC3 computeGradientLFerror(const Dart& v0, const Dart& v1) const ;
	VEC3 computeSquaredLightfieldDifferenceAnalytical(const Dart& d1, const Dart& d2) const ;
	VEC3 computeSquaredLightfieldDifferenceNumerical(const Dart& d1, const Dart& d2) const ;
	VEC3 computeGradient(const VEC3& P0, const VEC3& Pi, const VEC3& Pj, const Dart& v0, const Dart& v1, const Dart& vi, const Dart& vj, unsigned int channel) const ;
	REAL computeIntegral(const double *avgi, const VEC3& ti, const VEC3& bi, const VEC3& ni, unsigned int nbCoefs, const std::vector<double>& coefs) const ;

	static bool isInDomain(double x, double y, double z, void *data) ;
	static double CartesianFunction (double x, double y, double z, void* data) ;
	static double SquaredDifferenceOfCartesianFunctions (double x, double y, double z, void* data) ;

public:
	HalfEdgeSelector_LFgradient(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FT(-1),
		m_attrindex_FT(-1),
		m_approxindex_FB(-1),
		m_attrindex_FB(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1),
		m_K(0),
		m_n(NULL),
		m_workspace(NULL)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_avgColor = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
		m_n = new double[3] ;
		assert(m_avgColor.isValid()) ;
	}
	~HalfEdgeSelector_LFgradient()
	{
		this->m_map.removeAttribute(m_quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
		m_integrator.Release() ;
		delete[] m_workspace ;
		delete[] m_n ;
	}

	SelectorType getType() { return S_hLFgradient ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"HalfEdgeSelector_LFgradient::getEdgeErrors requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "HalfEdgeSelector_LFgradient::getEdgeErrors requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
 *                                 HALF-EDGE COLOR PER FACE                                                      *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_ColorPerFace : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "ColorPerFaceHalfEdgeInfo" ; }
	} QEMextColorHalfEdgeInfo ;
	typedef NoMathIOAttribute<QEMextColorHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;
	VertexAttribute<Utils::Quadric<REAL> > m_quadric ;

	VertexAttribute<VEC3> m_pos ;
	FaceAttribute<VEC3> m_facecolor ;
	int m_approxindex_pos, m_attrindex_pos ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	//void recomputeQuadric(const Dart d, const bool recomputeNeighbors = false) ;
	void recomputeQuadric(const Dart d) ;

	typename PFP::REAL computeFaceColorError(const Dart& v0, const Dart& v1) ;


public:
	HalfEdgeSelector_ColorPerFace(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
	}
	~HalfEdgeSelector_ColorPerFace()
	{
		this->m_map.removeAttribute(m_quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}
	SelectorType getType() { return S_hColorPerFace ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
	{
		assert(errors != NULL || !"EdgeSelector::setColorMap requires non null vertexattribute argument") ;
		if (!errors->isValid())
			std::cerr << "EdgeSelector::setColorMap requires valid edgeattribute argument" << std::endl ;
		assert(halfEdgeInfo.isValid()) ;

		TraversorE<typename PFP::MAP> travE(this->m_map) ;
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
 *                                 HALF-EDGE LF PER FACE                                                         *
 *****************************************************************************************************************/
template <typename PFP>
class HalfEdgeSelector_LFperFace : public EdgeSelector<PFP>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;

private:
	typedef	struct
	{
		typename std::multimap<float,Dart>::iterator it ;
		bool valid ;
		static std::string CGoGNnameOfType() { return "LightfieldPerFaceHalfEdgeInfo" ; }
	} LightfieldHalfEdgeInfo ;
	typedef NoMathIOAttribute<LightfieldHalfEdgeInfo> HalfEdgeInfo ;

	DartAttribute<HalfEdgeInfo> halfEdgeInfo ;

	VertexAttribute<Utils::Quadric<REAL> > m_quadric ;

	VertexAttribute<REAL> m_visualImportance ;
	VertexAttribute<VEC3> m_avgColor ;

	int m_approxindex_pos, m_attrindex_pos ;
	int m_approxindex_FT, m_attrindex_FT ;
	int m_approxindex_FB, m_attrindex_FB ;
	int m_approxindex_FN, m_attrindex_FN ;
	std::vector<unsigned int> m_approxindex_HF, m_attrindex_HF ;
	unsigned int m_K ;

	std::vector<Approximator<PFP, typename PFP::VEC3,DART>* > m_approx ;

	std::multimap<float,Dart> halfEdges ;
	typename std::multimap<float,Dart>::iterator cur ;

	void initHalfEdgeInfo(Dart d) ;
	void updateHalfEdgeInfo(Dart d) ;
	void computeHalfEdgeInfo(Dart d, HalfEdgeInfo& einfo) ;
	void recomputeQuadric(const Dart d) ;

	REAL computeLightfieldError(const Dart& v0, const Dart& v1) const ;
	REAL computeSquaredLightfieldDifference(const Dart& d1, const Dart& d2) const ;

public:
	HalfEdgeSelector_LFperFace(MAP& m, VertexAttribute<typename PFP::VEC3>& pos, std::vector<ApproximatorGen<PFP>*>& approx) :
		EdgeSelector<PFP>(m, pos, approx),
		m_approxindex_pos(-1),
		m_attrindex_pos(-1),
		m_approxindex_FT(-1),
		m_attrindex_FT(-1),
		m_approxindex_FB(-1),
		m_attrindex_FB(-1),
		m_approxindex_FN(-1),
		m_attrindex_FN(-1),
		m_K(0)
	{
		halfEdgeInfo = m.template addAttribute<HalfEdgeInfo, DART>("halfEdgeInfo") ;
		m_quadric = m.template addAttribute<Utils::Quadric<REAL>, VERTEX>("QEMquadric") ;
		m_avgColor = m.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
		assert(m_avgColor.isValid()) ;
	}
	~HalfEdgeSelector_LFperFace()
	{
		this->m_map.removeAttribute(m_quadric) ;
		this->m_map.removeAttribute(halfEdgeInfo) ;
	}

	SelectorType getType() { return S_hLFperFace ; }
	bool init() ;
	bool nextEdge(Dart& d) ;
	void updateBeforeCollapse(Dart d) ;
	void updateAfterCollapse(Dart d2, Dart dd2) ;

	void updateWithoutCollapse() { }

	void getEdgeErrors(EdgeAttribute<typename PFP::REAL> *errors) const
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
			if (halfEdgeInfo[dd].valid && halfEdgeInfo[dd].it->first < (*errors)[d])
			{
				(*errors)[d] = halfEdgeInfo[dd].it->first ;
			}
		}
	}
} ;


} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Decimation/halfEdgeSelector.hpp"

#endif
