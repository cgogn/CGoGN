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

#ifndef __GMAP0_H__
#define __GMAP0_H__

#include "Topology/generic/attribmap.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{

/**
* The class of 0-GMap
* Warning here we use beta instead of classic alpha
*/
class GMap0 : public AttribMap
{
protected:
	AttributeMultiVector<Dart>* m_beta0 ;

	void init() ;

public:
	GMap0();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

	virtual void clear(bool removeAttrib);

	/*! @name Basic Topological Operators
	 * Access and Modification
	 *************************************************************************/

	virtual Dart newDart();

	Dart beta0(const Dart d);

	void beta0sew(Dart d, Dart e);

	void beta0unsew(Dart d);

	/*! @name Constructors and Destructors
	 *  To generate or delete cells in a 0-G-map
	 *************************************************************************/

	/**
	* create an edge
	* @return a dart of the edge
	*/
	Dart newEdge();

	/**
	* delete an edge
	* @param d a dart of the edge
	*/
	void deleteEdge(Dart d);

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
	bool foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread=0);

	//! Apply a functor on every dart of a face
	/*! @param d a dart of the face
	 *  @param f the functor to apply
	 */
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
};

} // namespace CGoGN

#include "Topology/gmap/gmap0.hpp"

#endif
