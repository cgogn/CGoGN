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

#ifndef __GMAP1_H__
#define __GMAP1_H__

#include "Topology/gmap/gmap0.h"

namespace CGoGN
{

/**
* The class of 1-GMap
*/
class GMap1 : public GMap0
{
protected:
	AttribMultiVect<Dart>* m_beta1 ;

public:
	GMap1();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

	virtual int getDartsPerTriangle();

	/*! @name Basic Topological Operators
	 * Access and Modification
	 *************************************************************************/

	virtual Dart newDart();

	Dart beta1(const Dart d);

	template <int N>
	Dart beta(const Dart d);

	Dart phi1(const Dart d);

	Dart phi_1(const Dart d);

	template <int N>
	Dart phi(const Dart d);

	Dart alpha1(const Dart d);

	Dart alpha_1(const Dart d);

	void beta1sew(Dart d, Dart e);

	void beta1unsew(Dart d);

	void phi1sew(Dart d, Dart e);

	void phi1unsew(Dart d);

	/*! @name Constructors and Destructors
	 *  To generate or delete cells in a 1-G-map
	 *************************************************************************/

	//@{
	/**
	* create a face
	* @param n the number of sides of face
	* @return a dart of the edge
	*/
	Dart newFace(unsigned int n);

	//! Delete a face erasing all its darts
	/*! @param d a dart of the face
	 */
	void deleteFace(Dart d) ;

	/**
	 * destroy an edge
	 * @param d a dart of the edge
	 */
	void deleteEdge(Dart d);
	//@}

	/*! @name Topological Operators
	 *  Topological operations on 1-G-maps
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

	//! Merge the two faces of d and e, darts d & e disappear
	/*! \pre Dart d and e MUST belong to distinct faces
	 *  \warning Darts d and e no longer exist after the call
	 *  @param d a dart in the first face
	 *  @param e a dart in the second face
	 */
	void mergeFaces(Dart d, Dart e);
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
	unsigned int faceDegree(Dart d);

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

	/**
	* Apply a functor on each dart of an oriented face
	* @param d a dart of the oriented face
	* @param fonct functor obj ref
	*/
	bool foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on every dart of a face
	/*! @param d a dart of the face
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

#include "Topology/gmap/gmap1.hpp"

#endif
