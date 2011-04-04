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

#ifndef __IMPLICIT_HIERARCHICAL_MAP3__
#define __IMPLICIT_HIERARCHICAL_MAP3__

#include "Topology/map/map3.h"
#include "Topology/generic/embeddedMap3.h"

namespace CGoGN
{

namespace Algo
{

namespace IHM3
{

template<typename T> class AttributeHandler_IHM ;

class ImplicitHierarchicalMap3 : public EmbeddedMap3<Map3>
{
	template<typename T> friend class AttributeHandler_IHM ;

public:
	unsigned int m_curLevel ;
	unsigned int m_maxLevel ;
	unsigned int m_edgeIdCount ;
	unsigned int m_faceIdCount;

	AttributeHandler<unsigned int> m_dartLevel ;
	AttributeHandler<unsigned int> m_edgeId ;
	AttributeHandler<unsigned int> m_faceId ;

	AttribMultiVect<unsigned int>* m_nextLevelCell[NB_ORBITS] ;

public:
	ImplicitHierarchicalMap3() ;

	~ImplicitHierarchicalMap3() ;

	void init() ;

	/***************************************************
	 *             ATTRIBUTES MANAGEMENT               *
	 ***************************************************/

	template <typename T>
	AttributeHandler_IHM<T> addAttribute(unsigned int orbit, const std::string& nameAttr) ;

	/***************************************************
	 *                 MAP TRAVERSAL                   *
	 ***************************************************/

	virtual Dart newDart() ;

	Dart phi1(Dart d) ;

	Dart phi_1(Dart d) ;

private:
	Dart phi2bis(Dart d) ;

public:
	Dart phi2(Dart d) ;

	Dart phi3(Dart d);

	Dart alpha0(Dart d);

	Dart alpha1(Dart d);

	Dart alpha2(Dart d);

	Dart alpha_2(Dart d);

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

	void setEdgeId(Dart d, unsigned int i, unsigned int orbit);

	/***************************************************
	 *             FACE ID MANAGEMENT                  *
	 ***************************************************/
	void initFaceId() ;

	/**
	 * Return the next available face id
	 */
	unsigned int getNewFaceId() ;

	unsigned int getFaceId(Dart d) ;

	void setFaceId(Dart d, unsigned int i, unsigned int orbit);

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
	 *
	 */
	unsigned int faceLevel(Dart d);

	/**
	 *
	 */
	unsigned int volumeLevel(Dart d);

	/**
	 *
	 */
	Dart faceOldestDart(Dart d);

	/**
	 *
	 */
	Dart volumeOldestDart(Dart d);

	/**
	 * Return true if the edge of d in the current level map
	 * has already been subdivided to the next level
	 */
	bool edgeIsSubdivided(Dart d) ;

	/**
	 *
	 */
	bool faceIsSubdivided(Dart d) ;

	/**
	 *
	 */
	bool volumeIsSubdivided(Dart d);
} ;

template <typename T>
class AttributeHandler_IHM : public AttributeHandler<T>
{
public:
	typedef T DATA_TYPE ;

	AttributeHandler_IHM() : AttributeHandler<T>()
	{}

	AttributeHandler_IHM(GenericMap* m, unsigned int idAttr) : AttributeHandler<T>(m, idAttr)
	{}

	AttribMultiVect<T>* getDataVector() const
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

#include "Algo/ImplicitHierarchicalMesh/ihm3.hpp"

#endif
