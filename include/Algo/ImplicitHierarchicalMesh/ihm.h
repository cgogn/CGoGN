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

#ifndef __IMPLICIT_HIERARCHICAL_MAP__
#define __IMPLICIT_HIERARCHICAL_MAP__

#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

namespace CGoGN
{

namespace Algo
{

namespace IHM
{

template<typename T> class AttributeHandler_IHM ;

class ImplicitHierarchicalMap : public EmbeddedMap2<Map2>
{
	template<typename T> friend class AttributeHandler_IHM ;

private:
	unsigned int m_curLevel ;
	unsigned int m_maxLevel ;
	unsigned int m_idCount ;

	AttributeHandler<unsigned int> m_dartLevel ;
	AttributeHandler<unsigned int> m_edgeId ;

	AttributeMultiVector<unsigned int>* m_nextLevelCell[NB_ORBITS] ;

public:
	ImplicitHierarchicalMap() ;

	~ImplicitHierarchicalMap() ;

	void init() ;

	/***************************************************
	 *             ATTRIBUTES MANAGEMENT               *
	 ***************************************************/

	template <typename T>
	AttributeHandler_IHM<T> addAttribute(unsigned int orbit, const std::string& nameAttr) ;

	template <typename T>
	AttributeHandler_IHM<T> getAttribute(unsigned int orbit, const std::string& nameAttr) ;

	/***************************************************
	 *                 MAP TRAVERSAL                   *
	 ***************************************************/

	virtual Dart newDart() ;

	Dart phi1(Dart d) ;

	Dart phi_1(Dart d) ;

	Dart phi2(Dart d) ;

	Dart alpha0(Dart d);

	Dart alpha1(Dart d);

	Dart alpha_1(Dart d);

	virtual Dart begin() ;

	virtual Dart end() ;

	virtual void next(Dart& d) ;

	virtual bool foreach_dart_of_vertex(Dart d, FunctorType& f) ;

	virtual bool foreach_dart_of_edge(Dart d, FunctorType& f) ;

	bool foreach_dart_of_oriented_face(Dart d, FunctorType& f);
	virtual bool foreach_dart_of_face(Dart d, FunctorType& f) ;

	bool foreach_dart_of_oriented_volume(Dart d, FunctorType& f);
	virtual bool foreach_dart_of_volume(Dart d, FunctorType& f) ;

	virtual bool foreach_dart_of_cc(Dart d, FunctorType& f) ;

	/***************************************************
	 *               MAP MANIPULATION                  *
	 ***************************************************/

	void splitFace(Dart d, Dart e) ;

	/***************************************************
	 *              LEVELS MANAGEMENT                  *
	 ***************************************************/

	unsigned int getCurrentLevel() ;

	void setCurrentLevel(unsigned int l) ;

	unsigned int getMaxLevel() ;

	unsigned int getDartLevel(Dart d) ;

	void setDartLevel(Dart d, unsigned int i) ;

	/***************************************************
	 *             EDGE ID MANAGEMENT                  *
	 ***************************************************/

	/**
	 * Give a new unique id to all the edges of the map
	 */
	void initEdgeId() ;

	/**
	 * Return the next available edge id
	 */
	unsigned int getNewEdgeId() ;

	unsigned int getEdgeId(Dart d) ;

	void setEdgeId(Dart d, unsigned int i) ;

	/***************************************************
	 *               CELLS INFORMATION                 *
	 ***************************************************/

	/**
	 * Return the level of insertion of the vertex of d
	 */
	unsigned int vertexInsertionLevel(Dart d) ;

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
} ;

template <typename T>
class AttributeHandler_IHM : public AttributeHandler<T>
{
public:
	typedef T DATA_TYPE ;

	AttributeHandler_IHM() : AttributeHandler<T>()
	{}

	AttributeHandler_IHM(GenericMap* m, unsigned int orbit, unsigned int index) : AttributeHandler<T>(m, orbit, index)
	{}

	AttributeMultiVector<T>* getDataVector() const
	{
		return AttributeHandler<T>::getDataVector() ;
	}

	bool isValid() const
	{
		return AttributeHandler<T>::isValid() ;
	}

	T& operator[](Dart d) ;

	const T& operator[](Dart d) const ;

	T& operator[](unsigned int a)
	{
		return AttributeHandler<T>::operator[](a) ;
	}

	const T& operator[](unsigned int a) const
	{
		return AttributeHandler<T>::operator[](a) ;
	}
} ;

} //namespace IHM

} //namespace Algo

} //namespace CGoGN

#include "Algo/ImplicitHierarchicalMesh/ihm.hpp"

#endif
