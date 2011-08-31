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

#ifndef __GMAP2_H__
#define __GMAP2_H__

#include "Topology/gmap/gmap1.h"

namespace CGoGN
{

/**
* The class of 2-GMap
*/
class GMap2 : public GMap1
{
protected:
	AttributeMultiVector<Dart>* m_beta2 ;

	void init() ;

public:
	GMap2();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

	virtual void clear(bool removeAttrib);

	virtual int getDartsPerTriangle();

	/*! @name Basic Topological Operators
	 * Access and Modification
	 *************************************************************************/

	virtual Dart newDart();

	Dart beta2(Dart d);

	template <int N>
	Dart beta(const Dart d);

	Dart phi2(Dart d);

	template <int N>
	Dart phi(const Dart d);

	Dart alpha0(Dart d);

	Dart alpha1(Dart d);

	Dart alpha_1(Dart d);

	void beta2sew(Dart d, Dart e);

	void beta2unsew(Dart d);

	void phi2sew(Dart d, Dart e);

	void phi2unsew(Dart d);

	/*! @name Constructors and Destructors
	 *  To generate or delete cells in a 2-G-map
	 *************************************************************************/

	//@{
	//! Delete a face erasing all its darts
	/*! @param d a dart of the face
	 */
	virtual void deleteFace(Dart d);
	//@}

	/*! @name Topological Operators
	 *  Topological operations on 2-G-maps
	 *************************************************************************/

	//@{
	//! Split a vertex v between d and e
	/*! \pre Darts d & e MUST belong to the same oriented vertex
	 *  @param d first dart in vertex v
	 *  @param e second dart in vertex v
	 */
	virtual void splitVertex(Dart d, Dart e);

	//! Delete the vertex of d (works only for internal vertices)
	/*! All the faces around the vertex are merged into one face
	 *  @param d a dart of the vertex to delete
	 * @return true if the deletion has been executed, false otherwise
	 */
	virtual bool deleteVertex(Dart d) ;

	//! Cut the edge of d and its opposite edge if it exists
	/*! @param d a dart of the edge to cut
	 */
	virtual void cutEdge(Dart d);

	//! Collapse an edge (that is deleted) possibly merging its vertices
	/*! If delDegenerateFaces is true, the method checks that no degenerate
	 *  faces are build (faces with less than 3 edges). If it occurs the faces
	 *  are deleted and the adjacencies are upadted (see deleteIfDegenerated).
	 *  \warning This may produce two distinct vertices if the edge
	 *  was the only link between two border faces
	 *  @param d a dart in the deleted edge
	 *  @param delDegenerateFaces a boolean (default to true)
	 *  @return a dart of the resulting vertex
	 */
	virtual Dart collapseEdge(Dart d, bool delDegenerateFaces);

	/**
	 * Flip the edge of d. (rotation in phi1 order)
	 * Works only for non-border edges.
	 * @param d a dart of the edge to flip
	 * @return true if the flip has been executed, false otherwise
	 */
	virtual bool flipEdge(Dart d);

	/**
	 * Flip the edge of d. (rotation in phi_1 order)
	 * Works only for non-border edges.
	 * @param d a dart of the edge to flip
	 * @return true if the flip has been executed, false otherwise
	 */
	virtual bool flipBackEdge(Dart d);

	//! Sew two oriented faces along oriented edges (pay attention to the orientation !)
	/*! \pre Darts d & e MUST be fixed point of phi2 relation
	 *  @param d a dart of the first face
	 *  @param e a dart of the second face
	 */
	virtual void sewFaces(Dart d, Dart e);

	//! Unsew two oriented faces along oriented edges
	 /*! @param d a dart of one face
	 */
	virtual void unsewFaces(Dart d);

	//! Delete an oriented face if and only if it has one or two edges
	/*! If the face is phi2-linked to two distinct adjacent faces,
	 *  then those two faces are phi2-linked
	 *  @param d a dart of the face
	 *  @return true if the collapse has been executed, false otherwise
	 */
	virtual bool collapseDegeneratedFace(Dart d);

	//! Split a face f between d and e inserting an edge between vertices d & e
	/*! \pre Darts d & e MUST belong to the same face
	 *  @param d first dart in face f
	 *  @param e second dart in face f
	 */
	virtual void splitFace(Dart d, Dart e);

	//! Merge the two faces incident to the edge of d.
	/*! Works only for non-border edges.
	 *  \warning Darts d & phi2(d) no longer exist after the call
	 *  @param d a dart in the first face
	 *  @return true if the merge has been executed, false otherwise
	 */
	virtual bool mergeFaces(Dart d);

	/**
	 * Extract a pair of sewed triangles and sew their adjacent faces
	 * d is a dart of the common edge of the pair of triangles
	 */
	void extractTrianglePair(Dart d) ;

	/**
	 * Insert a pair of sewed triangles in a vertex by exploding the edges of d1 and d2
	 * d1 and d2 belong to the same vertex
	 * d is a dart of the common edge of the pair of triangles
	 */
	void insertTrianglePair(Dart d, Dart v1, Dart v2) ;

	//! Merge two volumes along two faces.
	/*! Works only if the two faces have the same number of edges.
	 *  The faces adjacent to the two given faces are pairwise phi2-linked
	 *  then the 2 faces are deleted.
	 *  If the two faces belong to different surfaces, the surfaces are merged,
	 *  else a handle is created that increases the genus of the surface.
	 *  @param d a dart of the first face
	 *  @param e a dart of the second face
	 *  @return true if the merge has been executed, false otherwise
	 */
	virtual bool mergeVolumes(Dart d, Dart e);

	//! Close a topological hole (a sequence of connected fixed point of phi2).
	/*! \pre dart d MUST be fixed point of phi2 relation
	 *  Add a face to the map that closes the hole.
	 *  The darts of this face are marked with holeMarker.
	 *  @param d a dart of the hole (with phi2(d)==d)
	 *  @return the degree of the created face
	 */
	virtual unsigned int closeHole(Dart d);

//	/**
//	* Mark half of dart defining an orientation
//	* @param d a dart that belong to the orientation we want to mark
//	* @return the marker which mark th orientation
//	*/
//	DartMarker markCCOrientation(Dart d);

	void closeMap(DartMarker& marker);
	//@}

	/*! @name Topological Queries
	 *  Return or set various topological information
	 *************************************************************************/

	//@{
	//! Test if dart d and e belong to the same oriented vertex
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameOrientedVertex(Dart d, Dart e);

	//! Test if dart d and e belong to the same vertex
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameVertex(Dart d, Dart e);

	/**
	 * compute the number of edges of the vertex of d
	 */
	unsigned int vertexDegree(Dart d) ;

	/**
	 * tell if the vertex of d is on the boundary of the map
	 */
	bool isBoundaryVertex(Dart d) ;

	/**
	 * Check if map is complete
	 */
	/* TODO devrait disparaitre :
	 * Le test doit être fait à l'import. Ensuite closeMap doit être
	 * utilisé pour fermer la carte si des faces ouvertes sont générées
	 */
	virtual bool check();
	//@}

	/*! @name Cell Functors
	 *  Apply functors to all darts of a cell
	 *************************************************************************/

	//@{
	/**
	* Apply a functor on each dart of a face
	* @param d a dart of the face
	* @param fonct functor obj ref
	*/
	bool foreach_dart_of_oriented_vertex(Dart d, FunctorType& fonct, unsigned int thread=0);

	/**
	* Apply a functor on each dart of a face
	* @param d a dart of the face
	* @param fonct functor obj ref
	*/
	bool foreach_dart_of_vertex(Dart d, FunctorType& fonct, unsigned int thread=0);

	/**
	* Apply a functor on each dart of an edge
	* @param d a dart of the oriented face
	* @param fonct functor obj ref
	*/
	bool foreach_dart_of_edge(Dart d, FunctorType& fonct, unsigned int thread=0);

	//! Apply a functor on every dart of a volume
	/*! @param d a dart of the volume
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_oriented_volume(Dart d, FunctorType& fonct, unsigned int thread=0);

	//! Apply a functor on every dart of a volume
	/*! @param d a dart of the volume
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_volume(Dart d, FunctorType& fonct, unsigned int thread=0);

	/**
	* Apply a functor on each dart of a cc
	* @param d a dart of the cc
	* @param fonct functor obj ref
	*/
	bool foreach_dart_of_cc(Dart d, FunctorType& fonct, unsigned int thread=0);
	//@}
};

} // namespace CGoGN

#include "Topology/gmap/gmap2.hpp"

#endif
