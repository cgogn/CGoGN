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

public:
	typedef Map2 ParentMap;

	Map3();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

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
	/*  @param d,e the darts to link
	 *	- Before:	d->d and e->e
	 *	- After:	d->e and e->d
	 */
	void phi3sew(Dart d, Dart e);

	//! Unlink the current dart by an involution
	/*  @param d the dart to unlink
	 * - Before:	d->e and e->d
	 * - After:		d->d and e->e
	 */
	void phi3unsew(Dart d);

	/*! @name Generator and Deletor
	 *  To generate or delete volumes in a 3-map
	 *************************************************************************/

public:

	//@{
	//! Delete an oriented volume
	/*! @param d a dart of the volume
	 */
	virtual void deleteOrientedVolume(Dart d);
	//@}

	/*! @name Topological Operators
	 *  Topological operations on 3-maps
	 *************************************************************************/

	//@{
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

	//! Merge to volume sewed by one face
	/*! @param d a dart of common face
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

	//! Uncut the edge of d
	/*! @param d a dart of the edge to uncut
	 */
	virtual void uncutEdge(Dart d);

	//!
	/*!
	 *
	 */
	virtual bool deleteVertex(Dart d);

	//!
	/*! Flip the edge of d (rotation in phi1 order)
	 *  WARNING : works only if the edge is owned by exactly one or two volumes
	 *  @param d the edge to flip
	 */
	//virtual bool flipEdge(Dart d);

	//!
	/*! Flip the edge of d (rotation in phi_1 order)
	 *  WARNING : works only if the edge is owned by exactly one or two volumes
	 *  @param d the edge to flip
	 */
	//virtual bool flipBackEdge(Dart d);

	//!
	/*!
	 *
	 */
	//virtual bool flipFace(Dart d);

    //! Insert a face in phi2 along the orbit phi<-1,2,-1>
    /*! The cardinality of <phi1>(nF) must be equal to the cardinality of <phi(-1,2,-1)>(d)
     *  @param d a dart where the face should be inserted
     *  @param e a dart from the face to insert
     */
	//virtual void insertFace(Dart d, Dart e);

	//! Collapse an edge (that is deleted) possibly merging its vertices
	/*! If delDegenerateFaces is true, the method checks that no degenerate
	 *  faces are build (faces with less than 3 edges). If it occurs the faces
	 *  are deleted and the adjacencies are updated (see deleteIfDegenerated).
	 *  \warning This may produce two distinct vertices if the edge
	 *  was the only link between two border faces
	 *  @param d a dart in the deleted edge
	 *  @param delDegenerateFaces a boolean (default to true)
	 */
	virtual int collapseEdge(Dart d, bool delDegenerateFaces = true, bool delDegenerateVolumes = true);

	//!
	/*!
	 *
	 * @param
	 * @param
	 * @param
	 */
	virtual void collapseFace(Dart d, bool delDegenerateFaces = true, bool delDegenerateVolumes = true);

	//!
	/*!
	 *
	 * @param
	 * @param
	 * @param
	 */
	virtual void collapseVolume(Dart d, bool delDegenerateFaces = true, bool delDegenerateVolumes = true);

	/** Fusion two faces deleting the two edges after d and e
	 *  and their respective phi3 faces
	 *! \pre Dart d and e should belong to distinct faces
	 *  \pre Dart d and e should not be phi2-linked
	 *  \pre Dart d and e should not be phi3-linked
	 *  @param d a dart in the first face
	 *  @param e a dart in the second face
	 */
	virtual void mergeFaces(Dart d, Dart e);

	//!
	/*!
	 *
	 */
	virtual Dart cutSpike(Dart d);


	//!
	/*
	 *
	 */
	virtual int edgeDegree(Dart d);

	/**
	 * compute the number of edges of the vertex of d
	 */
	unsigned int vertexDegree(Dart d) ;

	//!
	/* Tell if a face of the volume is on the boundary
	 *
	 */
	virtual bool isBoundaryVolume(Dart d);

	/**
	 *
	 */
	virtual bool isBoundaryVertex(Dart d);

	//@}

	/*! @name Cell Functors
	 *  Apply functors to all darts of a cell
	 *************************************************************************/

	//@{
	/**
	* Apply a functor on each dart of a vertex
	* @param d a dart of the vertex
	* @param fonct the functor
	*/
	bool foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread=0);

	/**
	* Apply a functor on each dart of an edge
	* @param d a dart of the oriented edge
	* @param fonct the functor
	*/
	bool foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread=0);

	bool foreach_dart_of_open_edge(Dart d, FunctorType& f, unsigned int thread=0);

	/**
	* Apply a functor on each dart of an oriented face
	* @param d a dart of the oriented face
	* @param fonct the functor
	*/
	bool foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread=0);

	/**
	* Apply a functor on each dart of an volume
	* @param d a dart of the volume
	* @param fonct the functor
	*/
	// TODO change to oriented volume to handle higher dimension ?
	bool foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread=0)
	{
		return foreach_dart_of_oriented_volume(d,f);
	}

//	template<typename TMAP>
//	bool foreach_dart_of_parent_cell(int dim, Dart d, FunctorType& f);

	/**
	* Apply a functor on each dart of a cc
	* @param d a dart of the cc
	* @param fonct the functor
	*/
	bool foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread=0);

	/**
	* Search one dart per volumes degree-connex to the volume of d
	* and apply a functor on this dart of a cc if selected by the functorselect
	* @param d a dart of the volume
	* @param degree degree of connexity : 0=vertex-connexity, 1=edge-connexity, 2=face-connexity
	* @param f the functor
	* @param s the selector
	*/
	 bool foreach_connex_volume(Dart d, int degree, FunctorType& f, FunctorSelect& s, unsigned int thread=0);
	//@}

//	template<typename TMAP>
//	bool markOrbitOf(int dim, Dart d, Marker m);
//
//	template<typename TMAP>
//	bool unmarkOrbitOf(int dim, Dart d, Marker m);



	//TODO devrait disparaitre
	// pour les mêmes raisons que dans map2
	/**
	* Close map: add external faces to map which contain
	* dart with phi2 fixed point. Only topological operation
	* embeddings are not updated.
	* WARNING: all added dart are marked !!
	* @ return index of the marker which with addes darts are marked
	*/
	virtual void closeMap(DartMarker& marker);

	//TODO a mettre en algo
	/**
	* Reverse orientation of map (reverse orientation of all faces)
	* no dart created, each dart keeps its embedding, only changing phi1, phi2 and phi3
	*/
	void reverseOrientation();


	/*! @name Topological Queries
	 *  Return or set various topological information
	 *************************************************************************/

	//@{
	// TODO devrait disparaitre
	// pour les mêmes raisons que dans map2
	virtual bool check();
	//@}
};

} // namespace CGoGN

#include "Topology/map/map3.hpp"

#endif
