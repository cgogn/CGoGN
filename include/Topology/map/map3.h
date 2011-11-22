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

#ifndef __MAP3_H__
#define __MAP3_H__

#include "Topology/map/map2.h"

namespace CGoGN
{

/*! \brief The class of dual 3-dimensional combinatorial maps:
 *  set of oriented volumes pairwise sewed by an adjacency relation.
 *  A dual 3-map represents close or open oriented 3-manifolds (volume subdivisions).
 *  - A dual 3-map is made of darts linked by the phi1 permutation
 * 	and/or the phi2 and phi3 one-to-one relation.
 *  - In this class darts are interpreted as oriented edges.
 *  - The phi1 relation defines oriented faces (see tMap1)
 *  and faces may have arbitrary size (degenerated faces are accepted).
 *  - The phi2 relation links oriented faces along oriented edges building
 *  oriented surfaces. A close oriented surface define an oriented volume.
 *  - Volume are linked along whole faces with the phi3 relation 
 *  - Faces that have no phi3-link are border faces. If there exists
 *  such edges the maps is open.
 *  - When every face is phi3-linked, the map is close. In this case
 *  some optimizations are enable that speed up the processing of cells.
 *  @param DART the type of dart used in the class
 */
class Map3 : public Map2
{
protected:
	AttributeMultiVector<Dart>* m_phi3 ;

	void init() ;

public:
	typedef Map2 ParentMap;

	Map3();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

	virtual void clear(bool removeAttrib);

	/*! @name Basic Topological Operators
	 * Access and Modification
	 *************************************************************************/

	virtual Dart newDart();

	Dart phi3(Dart d);

	template <int N>
	Dart phi(Dart d);

	Dart alpha0(Dart d);

	Dart alpha1(Dart d);

	Dart alpha2(Dart d);

	Dart alpha_2(Dart d);

protected:
	//! Link dart d with dart e by an involution
	/*! @param d,e the darts to link
	 *	- Before:	d->d and e->e
	 *	- After:	d->e and e->d
	 */
	void phi3sew(Dart d, Dart e);

	//! Unlink the current dart by an involution
	/*! @param d the dart to unlink
	 * - Before:	d->e and e->d
	 * - After:		d->d and e->e
	 */
	void phi3unsew(Dart d);

public:
	/*! @name Generator and Deletor
	 *  To generate or delete volumes in a 3-map
	 *************************************************************************/

	//@{
	//! Delete a volume erasing all its darts.
	/*! The phi3-links around the volume are removed
	 *  @param d a dart of the volume
	 */
	virtual void deleteVolume(Dart d);
	//@}

	/*! @name Topological Operators
	 *  Topological operations on 3-maps
	 *************************************************************************/

	//@{
	//TODO deleteVertex : works only for boundary vertices
	//! Delete the vertex of d
	/*! All the faces around the vertex are merged into one face
	 *  @param d a dart of the vertex to delete
	 *  @return true if the deletion has been executed, false otherwise
	 */
	virtual bool deleteVertex(Dart d);

	//! Cut the edge of d (all darts around edge orbit are cutted)
	/*! @param d a dart of the edge to cut
	 */
	virtual void cutEdge(Dart d);

	//! Uncut the edge of d (all darts around edge orbit are uncutted)
	/*! @param d a dart of the edge to uncut
	 */
	virtual void uncutEdge(Dart d);

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

	//! Uncut the edge of d
	/*! @param d a dart of the edge to uncut
	 */
	virtual bool uncutEdge(Dart d);

	//! Sew two oriented volumes along their faces. 
	/*! The oriented faces should not be phi3-linked and have the same length
	 *  @param d a dart of the first volume
	 *  @param e a dart of the second volume
	 */
	virtual void sewVolumes(Dart d, Dart e);

	//! Unsew two oriented volumes along their faces.
	/*! @param d a dart of one volume
	 */
	virtual void unsewVolumes(Dart d);

	//! Merge to volume along their common oriented face
	/*! @param d a dart of common face
	 */
	virtual bool mergeVolumes(Dart d);

	//! Split a volume into two volumes along a edge path
	/*! @param vd a vector of darts
	 */
	virtual void splitVolume(std::vector<Dart>& vd);

	//@}

	/*! @name Topological Queries
	 *  Return or set various topological information
	 *************************************************************************/

	//@{
	//! Test if dart d and e belong to the same oriented vertex
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameOrientedVertex(Dart d, Dart e) ;

	//! Test if dart d and e belong to the same vertex
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameVertex(Dart d, Dart e) ;

	//! Compute the number of edges of the vertex of d
	/*! @param d a dart
	 */
	unsigned int vertexDegree(Dart d) ;

	//! Tell if the vertex of d is on the boundary
	/*! @param d a dart
	 */
	bool isBoundaryVertex(Dart d);

	//! Test if dart d and e belong to the same oriented edge
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameOrientedEdge(Dart d, Dart e) ;

	//! Test if dart d and e belong to the same edge
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameEdge(Dart d, Dart e) ;

	//! Compute the number of volumes around the edge of d
	/*! @param d a dart
	 */
	unsigned int edgeDegree(Dart d);

	//! Test if dart d and e belong to the same oriented face
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameFace(Dart d, Dart e);

	//! Test if the face is on the boundary
	/*! @param d a dart from the face
	 */
	bool isBoundaryFace(Dart d);

	//! Tell if a face of the volume is on the boundary
	/*  @param d a dart
	 */
	bool isBoundaryVolume(Dart d);

	// TODO a mettre dans algo ?
	virtual bool check();
	//@}

	/*! @name Cell Functors
	 *  Apply functors to all darts of a cell
	 *************************************************************************/

	//@{
	//! Apply a functor on each dart of a vertex
	/*! @param d a dart of the vertex
	 *  @param fonct the functor
	 */
	bool foreach_dart_of_oriented_vertex(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on each dart of a vertex
	/*! @param d a dart of the vertex
	 *  @param fonct the functor
	 */
	bool foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on each dart of an edge
	/*! @param d a dart of the oriented edge
	 *  @param fonct the functor
	 */
	bool foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread=0);

	bool foreach_dart_of_open_edge(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on each dart of an oriented face
	/*! @param d a dart of the oriented face
	 *  @param fonct the functor
	 */
	bool foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread=0);

	// TODO change to oriented volume to handle higher dimension ?
	//! Apply a functor on each dart of an volume
	/*! @param d a dart of the volume
	 *  @param fonct the functor
	 */
	bool foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread=0)
	{
		return foreach_dart_of_oriented_volume(d,f);
	}

	//! Apply a functor on each dart of a cc
	/*! @param d a dart of the cc
	 *  @param fonct the functor
	 */
	bool foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread=0);
	//@}

};

} // namespace CGoGN

#include "Topology/map/map3.hpp"

#endif
