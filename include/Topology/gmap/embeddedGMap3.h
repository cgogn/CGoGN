/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __EMBEDDED_GMAP3_H__
#define __EMBEDDED_GMAP3_H__

#include "Topology/gmap/gmap3.h"

namespace CGoGN
{

/**
* Class of 3-dimensional G-maps
* with managed embeddings
*/
class EmbeddedGMap3 : public GMap3
{
public:
	typedef GMap3 TOPO_MAP;

	/**
	 * The attributes attached to the vertices of the edge of d are kept on the vertices of the resulting edge
	 * The attributes attached to the edge of d are kept on the resulting edge
	 * The attributes attached to the volume of d are kept on the resulting volume
	 */
	void sewFaces(Dart d, Dart e);

	/**
	 * The attributes attached to the vertices of the old edge of d are duplicated on the vertices of both resulting edges
	 * The attributes attached to the old edge are duplicated on both resulting edges
	 * The attributes attached to the old volume are duplicated on both volume if a new one is created
	 */
	virtual void unsewFaces(Dart d);

	/**
	 * The attributes attached to the vertices of the vertices of the face of d are kept on the vertices of the resulting face
	 * The attributes attached to the edges of the face of d are kept on the resulting face
	 * The attributes attached to the face of d are kept on the resulting face
	 */
	virtual void sewVolumes(Dart d, Dart e);

	/**
	 * The attributes attached to the vertices of the old face of d are duplicated on the vertices of both resulting faces
	 * The attributes attached to the old edges of the old face of d are duplicated on the edges of both resulting faces
	 * The attributes attached to the old face d are duplicated on both resulting faces
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

//	//!
//	/*!
//	 *
//	 */
//	virtual Dart cutSpike(Dart d);

//	//! Collapse an edge (that is deleted) possibly merging its vertices
//	/*! If delDegenerateFaces is true, the method checks that no degenerate
//	 *  faces are build (faces with less than 3 edges). If it occurs the faces
//	 *  are deleted and the adjacencies are updated (see deleteIfDegenerated).
//	 *  \warning This may produce two distinct vertices if the edge
//	 *  was the only link between two border faces
//	 *  @param d a dart in the deleted edge
//	 *  @param delDegenerateFaces a boolean (default to true)
//	 */
//	virtual int collapseEdge(Dart d, bool delDegenerateFaces = true,
//			bool delDegenerateVolumes = true);

//	//!
//	/*!
//	 *
//	 */
//	virtual void collapseFace(Dart d, bool delDegenerateFaces = true,
//			bool delDegenerateVolumes = true);

	virtual unsigned int closeHole(Dart d);

	virtual void closeMap(DartMarker &marker);


	virtual bool check();
} ;

} // namespace CGoGN

#endif
