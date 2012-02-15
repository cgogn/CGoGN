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

#ifndef __MAP2MR_PRIMAL__
#define __MAP2MR_PRIMAL__

#include "Topology/map/embeddedMap2.h"
#include "Topology/generic/traversor2.h"

#include <cmath>

namespace CGoGN
{

class Map2MR_PrimalAdapt : public EmbeddedMap2
{
protected:
	bool shareVertexEmbeddings ;

	FunctorType* vertexVertexFunctor ;
	FunctorType* edgeVertexFunctor ;
	FunctorType* faceVertexFunctor ;

public:
	Map2MR_PrimalAdapt() ;

	std::string mapTypeName() { return "Map2MR_PrimalAdapt" ; }

	/***************************************************
	 *               CELLS INFORMATION                 *
	 ***************************************************/

	/**
	 * Return the level of the edge of d in the current level map
	 */
	unsigned int edgeLevel(Dart d) ;

	/**
	 * Return the level of the face of d in the current level map
	 */
	unsigned int faceLevel(Dart d) ;

	/**
	 * Given the face of d in the current level map,
	 * return a level 0 dart of its origin face
	 */
	Dart faceOrigin(Dart d) ;

	/**
	 * Return the oldest dart of the face of d in the current level map
	 */
	Dart faceOldestDart(Dart d) ;

	/**
	 * Return true if the edge of d in the current level map
	 * has already been subdivided to the next level
	 */
	bool edgeIsSubdivided(Dart d) ;

	/**
	 * Return true if the edge of d in the current level map
	 * is subdivided to the next level,
	 * none of its resulting edges is in turn subdivided to the next level
	 * and the middle vertex is of degree 2
	 */
	bool edgeCanBeCoarsened(Dart d) ;

	/**
	 * Return true if the face of d in the current level map
	 * has already been subdivided to the next level
	 */
	bool faceIsSubdivided(Dart d) ;

	/**
	 * Return true if the face of d in the current level map
	 * is subdivided to the next level
	 * and none of its resulting faces is in turn subdivided to the next level
	 */
	bool faceIsSubdividedOnce(Dart d) ;

	/***************************************************
	 *               SUBDIVISION                       *
	 ***************************************************/

protected:
//	void propagatePhi1(Dart d) ;
//	void propagatePhi_1(Dart d) ;

	/**
	 * add a new resolution level
	 */
	void addNewLevel() ;

	/**
	 * subdivide the edge of d to the next level
	 */
	void subdivideEdge(Dart d) ;

	/**
	 * coarsen the edge of d from the next level
	 */
	void coarsenEdge(Dart d) ;

public:
	/**
	 * subdivide the face of d to the next level
	 */
	unsigned int subdivideFace(Dart d) ;

	/**
	 * coarsen the face of d from the next level
	 */
	void coarsenFace(Dart d) ;

	/**
	 * vertices attributes management
	 */
	void setVertexVertexFunctor(FunctorType* f) { vertexVertexFunctor = f ; }

	void setEdgeVertexFunctor(FunctorType* f) { edgeVertexFunctor = f ; }

	void setFaceVertexFunctor(FunctorType* f) { faceVertexFunctor = f ; }
} ;


class Map2MR_PrimalRegular : public EmbeddedMap2
{
protected:
	bool shareVertexEmbeddings ;

public:
	Map2MR_PrimalRegular() ;

	std::string mapTypeName() { return "Map2MR_PrimalRegular" ; }

	void addNewLevel() ;
} ;




/*********************************************************************************
 *                           VERTEX FUNCTORS
 *********************************************************************************/


template <typename PFP>
class PipoVertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	PipoVertexVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;
		typename PFP::VEC3 p = m_position[d] ;
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

template <typename PFP>
class PipoEdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	PipoEdgeVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d1 = m_map.phi2(d) ;

		m_map.decCurrentLevel() ;
		Dart d2 = m_map.phi2(d1) ;
		typename PFP::VEC3 p = (m_position[d1] + m_position[d2]) / 2.0 ;
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

template <typename PFP>
class PipoFaceVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	PipoFaceVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi1(m_map.phi1(d)) ;

		m_map.decCurrentLevel() ;

		typename PFP::VEC3 p(0) ;
		unsigned int degree = 0 ;
		Traversor2FV<typename PFP::MAP> trav(m_map, df) ;
		for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
		{
			++degree ;
			p += m_position[it] ;
		}
		p /= degree ;

		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

template <typename PFP>
class LoopVertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LoopVertexVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;

		typename PFP::VEC3 np(0) ;
		unsigned int degree = 0 ;
		Traversor2VVaE<typename PFP::MAP> trav(m_map, d) ;
		for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
		{
			++degree ;
			np += m_position[it] ;
		}
		float tmp = 3.0 + 2.0 * cos(2.0 * M_PI / degree) ;
		float beta = (5.0 / 8.0) - ( tmp * tmp / 64.0 ) ;
		np *= beta / degree ;

		typename PFP::VEC3 vp = m_position[d] ;
		vp *= 1.0 - beta ;

		m_map.incCurrentLevel() ;

		m_position[d] = np + vp ;

		return false ;
	}
} ;

template <typename PFP>
class LoopEdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LoopEdgeVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d1 = m_map.phi2(d) ;

		m_map.decCurrentLevel() ;

		Dart d2 = m_map.phi2(d1) ;
		Dart d3 = m_map.phi_1(d1) ;
		Dart d4 = m_map.phi_1(d2) ;

		typename PFP::VEC3 p1 = m_position[d1] ;
		typename PFP::VEC3 p2 = m_position[d2] ;
		typename PFP::VEC3 p3 = m_position[d3] ;
		typename PFP::VEC3 p4 = m_position[d4] ;

		p1 *= 3.0 / 8.0 ;
		p2 *= 3.0 / 8.0 ;
		p3 *= 1.0 / 8.0 ;
		p4 *= 1.0 / 8.0 ;

		m_map.incCurrentLevel() ;

		m_position[d] = p1 + p2 + p3 + p4 ;

		return false ;
	}
} ;

template <typename PFP>
class CCVertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	CCVertexVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;

		typename PFP::VEC3 np1(0) ;
		typename PFP::VEC3 np2(0) ;
		unsigned int degree1 = 0 ;
		unsigned int degree2 = 0 ;
		Dart it = d ;
		do
		{
			++degree1 ;
			Dart dd = m_map.phi1(it) ;
			np1 += m_position[dd] ;
			Dart end = m_map.phi_1(it) ;
			dd = m_map.phi1(dd) ;
			do
			{
				++degree2 ;
				np2 += m_position[dd] ;
				dd = m_map.phi1(dd) ;
			} while(dd != end) ;
			it = m_map.alpha1(it) ;
		} while(it != d) ;

		float beta = 3.0 / (2.0 * degree1) ;
		float gamma = 1.0 / (4.0 * degree2) ;
		np1 *= beta / degree1 ;
		np2 *= gamma / degree2 ;

		typename PFP::VEC3 vp = m_position[d] ;
		vp *= 1.0 - beta - gamma ;

		m_map.incCurrentLevel() ;

		m_position[d] = np1 + np2 + vp ;

		return false ;
	}
} ;

template <typename PFP>
class CCEdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	CCEdgeVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d1 = m_map.phi2(d) ;

		m_map.decCurrentLevel() ;

		Dart d2 = m_map.phi2(d1) ;
		Dart d3 = m_map.phi_1(d1) ;
		Dart d4 = m_map.phi_1(d2) ;
		Dart d5 = m_map.phi1(m_map.phi1(d1)) ;
		Dart d6 = m_map.phi1(m_map.phi1(d2)) ;

		typename PFP::VEC3 p1 = m_position[d1] ;
		typename PFP::VEC3 p2 = m_position[d2] ;
		typename PFP::VEC3 p3 = m_position[d3] ;
		typename PFP::VEC3 p4 = m_position[d4] ;
		typename PFP::VEC3 p5 = m_position[d5] ;
		typename PFP::VEC3 p6 = m_position[d6] ;

		p1 *= 3.0 / 8.0 ;
		p2 *= 3.0 / 8.0 ;
		p3 *= 1.0 / 16.0 ;
		p4 *= 1.0 / 16.0 ;
		p5 *= 1.0 / 16.0 ;
		p6 *= 1.0 / 16.0 ;

		m_map.incCurrentLevel() ;

		m_position[d] = p1 + p2 + p3 + p4 + p5 + p6 ;

		return false ;
	}
} ;

template <typename PFP>
class CCFaceVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	CCFaceVertexFunctor(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi1(m_map.phi1(d)) ;

		m_map.decCurrentLevel() ;

		typename PFP::VEC3 p(0) ;
		unsigned int degree = 0 ;
		Traversor2FV<typename PFP::MAP> trav(m_map, df) ;
		for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
		{
			++degree ;
			p += m_position[it] ;
		}
		p /= degree ;

		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

} // namespace CGoGN

#endif
