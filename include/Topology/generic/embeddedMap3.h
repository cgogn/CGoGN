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

#ifndef __EMBEDDED_MAP3_H__
#define __EMBEDDED_MAP3_H__

#include "Topology/generic/genericmap.h"

namespace CGoGN
{

/**
* Class of 3-dimensional maps
* with lazily managed embeddings
*/
template <typename MAP3>
class EmbeddedMap3 : public MAP3
{
public:

	//!
	/*!
	 *
	 */
	virtual void deleteOrientedVolume(Dart d);

	//!
	/*!
	 *
	 */
	virtual void sewVolumes(Dart d, Dart e);

	//!
	/*!
	 *
	 */
	virtual void unsewVolumes(Dart d);

	//!
	/*!
	 *
	 */
	virtual bool mergeVolumes(Dart d);

	//! Split a face inserting an edge between two vertices
	/*! \pre Dart d and e should belong to the same face and be distinct
	 *  @param d dart of first vertex
	 *  @param e dart of second vertex
	 *  @return the dart of the new edge lying in the vertex of d after the cut
	 */
	virtual void splitFace(Dart d, Dart e);

	//! Cut the edge of d
	/*! @param d a dart of the edge to cut
	 */
	virtual void cutEdge(Dart d);

	//!
	/*!
	 *
	 */
	virtual Dart cutSpike(Dart d);

	/**
	 * Flip the edge of d (rotation in phi1 order)
	 * WARNING : works only if the edge is owned by exactly one or two volumes
	 * @param d the edge to flip
	 */
	//virtual bool flipEdge(Dart d);

	/**
	 * Flip the edge of d (rotation in phi_1 order)
	 * WARNING : works only if the edge is owned by exactly one or two volumes
	 * @param d the edge to flip
	 */
	//virtual bool flipBackEdge(Dart d);

	//! Collapse an edge (that is deleted) possibly merging its vertices
	/*! If delDegenerateFaces is true, the method checks that no degenerate
	 *  faces are build (faces with less than 3 edges). If it occurs the faces
	 *  are deleted and the adjacencies are updated (see deleteIfDegenerated).
	 *  \warning This may produce two distinct vertices if the edge
	 *  was the only link between two border faces
	 *  @param d a dart in the deleted edge
	 *  @param delDegenerateFaces a boolean (default to true)
	 */
	virtual int collapseEdge(Dart d, bool delDegenerateFaces = true,
			bool delDegenerateVolumes = true);
	//!
	/*!
	 *
	 */
	virtual void collapseFace(Dart d, bool delDegenerateFaces = true,
			bool delDegenerateVolumes = true);
} ;

} // namespace CGoGN

#include "embeddedMap3.hpp"

#endif
