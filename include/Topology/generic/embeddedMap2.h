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

#ifndef __EMBEDDED_MAP2_H__
#define __EMBEDDED_MAP2_H__

#include "Topology/generic/genericmap.h"

namespace CGoGN
{

/**
* Class of 2-dimensional maps
* with lazily managed embeddings
*/
template <typename MAP2>
class EmbeddedMap2 : public MAP2
{
public:
	/**
	 *
	 */
	virtual void deleteFace(Dart d) ;

	/**
	 * The attributes attached to the old vertex are duplicated on both resulting vertices
	 * No attribute is attached to the new edge
	 * Nothing has to be done for the faces
	 */
	virtual void splitVertex(Dart d, Dart e) ;

	/**
	 * Nothing has to be done for the vertices
	 * Nothing has to be done for the edges
	 * The attributes attached to the face of d are kept on the resulting face
	 */
	virtual bool deleteVertex(Dart d) ;

	/**
	 * No attribute is attached to the new vertex
	 * The attributes attached to the old edge are duplicated on both resulting edges
	 * Nothing has to be done for the faces
	 */
	virtual void cutEdge(Dart d) ;

	/**
	 * Check if the edge of d can be collapsed or not based on some topological conditions
	 * @param d a dart of the edge to test
	 * @return true if the edge can be collapsed, false otherwise
	 */
	bool edgeCanCollapse(Dart d) ;

	/**
	 * The attributes attached to the vertex of dart d are kept on the resulting vertex
	 * See 'collapseDegeneratedFace' to see what can happen to edges attributes
	 * Nothing has to be done for the faces (some degenerate ones can be deleted)
	 */
	virtual Dart collapseEdge(Dart d, bool delDegenerateFaces = true) ;

	/**
	 * No cell is created or deleted
	 */
	virtual bool flipEdge(Dart d) ;

	/**
	 * No cell is created or deleted
	 */
	virtual bool flipBackEdge(Dart d) ;

	/**
	 * The attributes attached to the vertices of the edge of d are kept on the vertices of the resulting edge
	 * The attributes attached to the edge of d are kept on the resulting edge
	 * Nothing has to be done for the faces
	 */
	virtual void sewFaces(Dart d, Dart e) ;

	/**
	 * The attributes attached to the vertices of the old edge of d are duplicated on the vertices of both resulting edges
	 * The attributes attached to the old edge are duplicated on both resulting edges
	 * Nothing has to be done for the faces
	 */
	virtual void unsewFaces(Dart d) ;

	/**
	 * Nothing has to be done for the vertices
	 * The attributes attached to the edge of d are kept on the resulting edge
	 * Nothing has to be done for the faces
	 */
	virtual bool collapseDegeneratedFace(Dart d);

	/**
	 * Nothing has to be done for the vertices
	 * No attribute is attached to the new edge
	 * The attributes attached to the old face are duplicated on both resulting faces
	 */
	virtual void splitFace(Dart d, Dart e) ;

	/**
	 * Nothing has to be done for the vertices
	 * Nothing has to be done for the edges
	 * The attributes attached to the face of dart d are kept on the resulting face
	 */
	virtual bool mergeFaces(Dart d) ;

	/**
	 * The attributes attached to the vertices of the face of d are kept on the resulting vertices
	 * The attributes attached to the edges of the face of d are kept on the resulting edges
	 * Nothing has to be done for the faces
	 */
	virtual bool mergeVolumes(Dart d, Dart e) ;

	virtual bool check() ;
} ;

} // namespace CGoGN

#include "embeddedMap2.hpp"

#endif
