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

#ifndef __MAP1_H__
#define __MAP1_H__

#include "Topology/generic/attribmap.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{

/*! \brief The class of dual 1-dimensional combinatorial maps: set of oriented faces.
 *  - A dual 1-map is made of darts linked by the phi1 permutation.
 *  - In this class darts are interpreted as oriented edges.
 *  - The phi1 relation defines cycles of darts or (oriented) faces.
 *    - Faces may have abitrary size.
 *    - Faces with only one edge (sometime called loops) are accepted.
 *    - Degenerated faces with only two edges are accepted.
 *  @param DART the type of dart used in the class
 */
class Map1 : public AttribMap
{
protected:
	AttributeMultiVector<Dart>* m_phi1 ;
	AttributeMultiVector<Dart>* m_phi_1 ;

public:
	Map1();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

	/*! @name Basic Topological Operators
	 * Access and Modification
	 *************************************************************************/

	virtual Dart newDart();

	Dart phi1(Dart d);

	Dart phi_1(Dart d);

	template <int N>
	Dart phi(Dart d);

	Dart alpha1(Dart d);

	Dart alpha_1(Dart d);

protected:
	//! Link the current dart to dart d with a permutation
	/*! @param d the dart to which the current is linked
	 * - Before:	d->f and e->g
	 * - After:		d->g and e->f
	 * Join the permutations cycles of dart d and e
	 * - Starting from two cycles : d->f->...->d and e->g->...->e
	 * - It makes one cycle d->g->...->e->f->...->d
	 * If e = g then insert e in the cycle of d : d->e->f->...->d
	 */
	void phi1sew(Dart d, Dart e);

	//! Unlink the successor of a given dart in a permutation
	/*!	@param d a dart
	 * - Before:	d->e->f
	 * - After:		d->f and e->e
	 */
	void phi1unsew(Dart d);

public:
	virtual int getDartsPerTriangle();

	/*! @name Generator and Deletor
	 *  To generate or delete faces in a 1-map
	 *************************************************************************/

	//@{
	//! Create an new face made of nbEdges linked darts.
	/*! @param nbEdges the number of edges
	 *  @return return a dart of the face
	 */
	Dart newOrientedFace(unsigned int nbEdges) ;

	//! Create an new face made of nbEdges linked darts.
	/*! @param nbEdges the number of edges
	 *  @return return a dart of the face
	 */
	Dart newFace(unsigned int nbEdges) ;

	//! Delete an oriented face erasing all its darts
	/*! @param d a dart of the face
	 */
	void deleteOrientedFace(Dart d) ;

	//! Delete a face erasing all its darts
	/*! @param d a dart of the face
	 */
	void deleteFace(Dart d) ;
	//@}

	/*! @name Topological Operators
	 *  Topological operations on 1-maps
	 *************************************************************************/

	//@{
	//! Cut an edge inserting a new dart between d and its successor in the face
	/*! @param d the edge to cut
	 * \image hmtl map1_cutEdge.png
	 */
	void cutEdge(Dart d);

	//! Collapse an edge of a face
	/*!  \warning Dart d no longer exists after the call
	 *  @param d the edge
	 */
	void collapseEdge(Dart d);

	//! Split a face between vertices d and e
	/*! \pre Dart d and e MUST be different and belong to the same face
	 *  @param d first dart in the face
	 *  @param e second dart in the face
	 */
	void splitFace(Dart d, Dart e);

	//! Link two vertices belonging to distinct faces (add an edge between the two vertices)
	/*! \pre Dart d and e MUST be different and belong to distinct face
	 *  @param d first dart in the face
	 *  @param e second dart in the face
	 */
	void linkVertices(Dart d, Dart e);

	//! Merge the two faces of d and e, darts d & e disappear
	/*! \pre Dart d and e MUST belong to distinct faces
	 *  \warning Darts d and e no longer exist after the call
	 *  @param d a dart in the first face
	 *  @param e a dart in the second face
	 */
	void mergeFaces(Dart d, Dart e);

	//! reverse a face (phi1 become phi_1 and ...)
	/*! @param d a dart of face
	 */
	void reverseFace(Dart d) ;
	//@}

	/*! @name Topological Queries
	 *  Return or set various topological information
	 *************************************************************************/

	//@{
	//! Test if dart d and e belong to the same oriented face
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameOrientedFace(Dart d, Dart e) ;

	//! Test if dart d and e belong to the same face
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameFace(Dart d, Dart e) ;

	//! Length of a face (its number of oriented edges)
	/*! @param d a dart of the face
	 *  @return the length of the face
	 */
	unsigned int faceDegree(Dart d) ;

	/**
	 * check if the face of d is a triangle
	 * @return a boolean indicating if the face is a triangle
	 */
	bool isFaceTriangle(Dart d);
	//@}

	/*! @name Cell Functors
	 *  Apply functors to all darts of a cell
	 *************************************************************************/

	//@{
	//! Apply a functor on every dart of a vertex
	/*! @param d a dart of the vertex
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on every dart of an edge
	/*! @param d a dart of the edge
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on every dart of an oriented face
	/*! @param d a dart of the face
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread=0) ;

	bool foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on every dart of a volume
	/*! @param d a dart of the volume
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on every dart of a connected component
	/*! @param d a dart of the onnected component
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread=0);
	//@}
} ;

} // namespace CGoGN

#include "Topology/map/map1.hpp"

#endif
