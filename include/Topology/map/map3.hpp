/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN
{

template <class MAP>
inline void Map3<MAP>::init()
{
	MAP::addInvolution() ;
}

template <class MAP>
inline Map3<MAP>::Map3() : Map2<MAP>()
{
	init() ;
}

template <class MAP>
inline std::string Map3<MAP>::mapTypeName() const
{
	return "Map3";
}

template <class MAP>
inline unsigned int Map3<MAP>::dimension() const
{
	return 3;
}

template <class MAP>
inline void Map3<MAP>::clear(bool removeAttrib)
{
	ParentMap::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

template <class MAP>
inline void Map3<MAP>::update_topo_shortcuts()
{
	ParentMap::update_topo_shortcuts();
//	m_phi3 = getRelation("phi3");
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

template <class MAP>
inline Dart Map3<MAP>::phi3(Dart d) const
{
	return MAP::getInvolution<1>(d);
}

template <class MAP>
template <int N>
inline Dart Map3<MAP>::phi(Dart d) const
{
	assert( (N >0) || !"negative parameters not allowed in template multi-phi");
	if (N<10)
	{
		switch(N)
		{
			case 1 : return this->phi1(d) ;
			case 2 : return this->phi2(d) ;
			case 3 : return phi3(d) ;
			default : assert(!"Wrong multi-phi relation value") ; return d ;
		}
	}
	switch(N%10)
	{
		case 1 : return phi1(phi<N/10>(d)) ;
		case 2 : return phi2(phi<N/10>(d)) ;
		case 3 : return phi3(phi<N/10>(d)) ;
		default : assert(!"Wrong multi-phi relation value") ; return d ;
	}
}

template <class MAP>
inline Dart Map3<MAP>::alpha0(Dart d) const
{
	return phi3(d) ;
}

template <class MAP>
inline Dart Map3<MAP>::alpha1(Dart d) const
{
	return phi3(this->phi_1(d)) ;
}

template <class MAP>
inline Dart Map3<MAP>::alpha2(Dart d) const
{
	return phi3(this->phi2(d));
}

template <class MAP>
inline Dart Map3<MAP>::alpha_2(Dart d) const
{
	return phi2(phi3(d));
}

template <class MAP>
inline void Map3<MAP>::phi3sew(Dart d, Dart e)
{
	MAP::involutionSew<1>(d,e);
}

template <class MAP>
inline void Map3<MAP>::phi3unsew(Dart d)
{
	MAP::involutionUnsew<1>(d);
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

template <class MAP>
inline bool Map3<MAP>::sameEdge(Dart d, Dart e) const
{
	return ParentMap::sameOrientedEdge(d, e) || ParentMap::sameOrientedEdge(this->phi2(d), e) ;
}

template <class MAP>
inline bool Map3<MAP>::sameFace(Dart d, Dart e) const
{
	return ParentMap::sameOrientedFace(d, e) || ParentMap::sameOrientedFace(phi3(d), e) ;
}

template <class MAP>
inline bool Map3<MAP>::isBoundaryFace(Dart d) const
{
	return this->isBoundaryMarked3(d) || this->isBoundaryMarked3(phi3(d));
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

template <class MAP>
inline bool Map3<MAP>::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_face(d, f, thread) || ParentMap::foreach_dart_of_face(phi3(d), f, thread);
}

template <class MAP>
inline bool Map3<MAP>::foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_cc(d, f, thread);
}

template <class MAP>
inline bool Map3<MAP>::foreach_dart_of_vertex2(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_vertex(d, f, thread);
}

template <class MAP>
inline bool Map3<MAP>::foreach_dart_of_edge2(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_edge(d, f, thread);
}

template <class MAP>
inline bool Map3<MAP>::foreach_dart_of_face2(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_face(d, f, thread);
}

} // namespace CGoGN
