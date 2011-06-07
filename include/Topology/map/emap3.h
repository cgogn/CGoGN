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

#ifndef __EMAP3_H__
#define __EMAP3_H__

#include "Topology/map/map3.h"

namespace CGoGN
{

/**
* Class of 3-dimensional combinatorial maps
* with managed embeddings
* See Map3 class for comments..
*/
class EMap3: public Map3
{
public:
	/**
	* cut face in two faces between to vertices
	* @param d the dart of first vertex
	* @param e the dart of second vertex
	* @return a dart of the created edge
	*/
	Dart cutFace(Dart d, Dart e);

	/**
	* cut a face in n faces with a central point
	* @param d a dart of the face
	* @return a dart of central point
	*/
	Dart trianguleFace(Dart d);

	/**
	* quadrangule a face in n faces with a central point
	* WARNING: edges of faces must already be cutted
	* @param d a dart of the face
	* @return a dart of central point
	*/
	Dart quadranguleFace(Dart d);

	void embeddxxguledFace(Dart d, Dart n);

	/**
	* cut an edge  
	* @param d a dart of the edge
	* @return a dart of middle point
	*/
	Dart cutEdge(Dart d);

	/**Collapse an edge (that is deleted) possibly merging its vertices
	* If delDegenerateFaces is true, the method checks that no degenerate
	* faces are build (faces with less than 3 edges). If it occurs the faces
	* are deleted and the adjacencies are updated (see deleteIfDegenerated).
	* The new embedding is at the middle of the collapsed edge
	* \warning This may produce two distinct vertices if the edge
	* was the only link between two border faces
	* @param d a dart in the deleted edge
	* @param delDegenerateFaces a boolean (default to true)
	* @return a dart from the vertex at the middle of the edge
	*/
	Dart edgeCollapse(Dart d, bool delDegenerateFaces=true, bool delDegenerateVolumes=true);



	/**
	 * start from a volume composed only by triangles
	 * add tetrahedrons inside the volume
	 * @param a dart from the volume
	 * @return a dart from the new tetra (center of the old volume)
	 */
	Dart tetrahedrizeVolume(Dart d);

	bool check();

	/**
	*  Embed (vertex) the darts of the boundary
	* @param m the marker defining the boundary
	*/
	void embedBoundaryDarts(Marker m)
	{
		for( Dart d=this->begin(); d != this->end(); this->next(d))
		{
			if (isMarkedDart(d,m))
			{
				copyEmbedding(d, phi1(phi3(d)), VERTEX);
			}
		}
	}

	/** Flip the edge of d.
	 * WARNING : works only if the edge is owned by exactly two volumes
	 * @param d the edge to flip
	 */
	void flipEdge(Dart d);
};

} // namespace CGoGN

#endif
