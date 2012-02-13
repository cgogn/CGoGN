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

namespace CGoGN
{

class Map2MR_Primal : public EmbeddedMap2
{
protected:
	bool shareVertexEmbeddings ;

public:
	Map2MR_Primal() ;

	std::string mapTypeName() { return "Map2MR_Primal";}

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

	/***************************************************
	 *           EMBEDDINGS MANAGEMENT                 *
	 ***************************************************/

	void copyVertexEmbeddings() ;

	/***************************************************
	 *               SUBDIVISION                       *
	 ***************************************************/

	/**
	 * add a new resolution level
	 */
	void addNewLevel() ;

	/**
	 * subdivide the edge of d to the next level
	 */
	void subdivideEdge(Dart d) ;

	/**
	 * subdivide the face of d to the next level
	 */
	void subdivideFace(Dart d) ;

	/**
	 * coarsen the edge of d from the next level
	 */
	void coarsenEdge(Dart d) ;

	/**
	 * coarsen the face of d from the next level
	 */
	void coarsenFace(Dart d) ;
} ;

} // namespace CGoGN

#endif
