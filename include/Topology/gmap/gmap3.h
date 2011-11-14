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

#ifndef __GMAP3_H__
#define __GMAP3_H__

#include "Topology/gmap/gmap2.h"

namespace CGoGN
{

/**
* The class of 2-GMap
*/
class GMap3 : public GMap2
{
protected:
	AttributeMultiVector<Dart>* m_beta3 ;

	void init() ;

public:
	typedef GMap2 ParentMap;

	GMap3();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

	virtual void clear(bool removeAttrib);

	/*! @name Basic Topological Operators
	 * Access and Modification
	 *************************************************************************/

	virtual Dart newDart();

	Dart beta3(Dart d);

	template <int N>
	Dart beta(const Dart d);

	Dart phi3(Dart d);

	template <int N>
	Dart phi(const Dart d);

	Dart alpha0(Dart d);

	Dart alpha1(Dart d);

	Dart alpha_1(Dart d);

	Dart alpha2(Dart d);

	Dart alpha_2(Dart d);

	void beta3sew(Dart d, Dart e);

	void beta3unsew(Dart d);

	void phi3sew(Dart d, Dart e);

	void phi3unsew(Dart d);

	/*! @name Generator and Deletor
	 *  To generate or delete volumes in a 3-map
	 *************************************************************************/

	//@{
	//! Delete a volume erasing all its darts.
	/*! The phi3-links around the volume are removed
	 *  @param d a dart of the volume
	 */
	void deleteVolume(Dart d);
	//@}

	/*! @name Topological Operators
	 *  Topological operations on 3-maps
	 *************************************************************************/

	//@{
	//! Cut the edge of d
	/*! @param d a dart of the edge to cut
	 */
	virtual void cutEdge(Dart d);

	//! Split a face inserting an edge between two vertices
	/*! \pre Dart d and e should belong to the same face and be distinct
	 *  @param d dart of first vertex
	 *  @param e dart of second vertex
	 *  @return the dart of the new edge lying in the vertex of d after the cut
	 */
	virtual void splitFace(Dart d, Dart e);

	//! Sew two oriented volumes along their faces.
	/*! The oriented faces should not be phi3-linked and have the same length
	 *  @param d a dart of the first volume
	 *  @param e a dart of the second volume
	 */
	void sewVolumes(Dart d, Dart e);

	//! unsew two oriented volumes along their faces.
	/*! @param d a dart of one volume
	 */
	void unsewVolumes(Dart d);

	//! merge to volume sewed by one face
	/*! @param d a dart of common face
	 */
	bool mergeVolumes(Dart d);
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
	virtual bool isBoundaryVertex(Dart d);

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

	//!Test if dart d and e belong to the same oriented face
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameOrientedFace(Dart d, Dart e);

	//!Test if dart d and e belong to the same oriented face
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameFace(Dart d, Dart e);

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
	bool foreach_dart_of_vertex(Dart d, FunctorType& fonct, unsigned int thread=0);

	/**
	* Apply a functor on each dart of an edge
	* @param d a dart of the oriented face
	* @param fonct functor obj ref
	*/
	bool foreach_dart_of_edge(Dart d, FunctorType& fonct, unsigned int thread=0);

	//! Apply a functor on every dart of a face
	/*! @param d a dart of the volume
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_face(Dart d, FunctorType& fonct, unsigned int thread=0);


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

#include "Topology/gmap/gmap3.hpp"

#endif
