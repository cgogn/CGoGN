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

/// INLINE FUNCTIONS

template <class MAP>
inline void Map2<MAP>::init()
{
	MAP::addInvolution() ;
}

template <class MAP>
inline Map2<MAP>::Map2() : Map1<MAP>()
{
	init() ;
}

template <class MAP>
inline std::string Map2<MAP>::mapTypeName() const
{
	return "Map2" ;
}

template <class MAP>
inline unsigned int Map2<MAP>::dimension() const
{
	return 2 ;
}

template <class MAP>
inline void Map2<MAP>::clear(bool removeAttrib)
{
	ParentMap::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

template <class MAP>
inline void Map2<MAP>::update_topo_shortcuts()
{
	ParentMap::update_topo_shortcuts();
	m_phi2 = getRelation("phi2");
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

template <class MAP>
inline Dart Map2<MAP>::newDart()
{
	Dart d = Map1::newDart() ;
	(*m_phi2)[dartIndex(d)] = d ;
//	if(m_isMultiRes)
//	{
//		pushLevel() ;
//		for(unsigned int i = m_mrCurrentLevel + 1;  i < m_mrDarts.size(); ++i)
//		{
//			setCurrentLevel(i) ;
//			(*m_phi2)[dartIndex(d)] = d ;
//		}
//		popLevel() ;
//	}
	return d ;
}

template <class MAP>
inline Dart Map2<MAP>::phi2(Dart d) const
{
	return MAP::getInvolution<0>(d);
}

template <class MAP>
template <int N>
inline Dart Map2<MAP>::phi(Dart d) const
{
	assert( (N > 0) || !"negative parameters not allowed in template multi-phi");
	if (N < 10)
	{
		switch(N)
		{
			case 1 : return ParentMap::phi1(d) ;
			case 2 : return phi2(d) ;
			default : assert(!"Wrong multi-phi relation value") ; return d ;
		}
	}
	switch(N%10)
	{
		case 1 : return phi1(phi<N/10>(d)) ;
		case 2 : return phi2(phi<N/10>(d)) ;
		default : assert(!"Wrong multi-phi relation value") ; return d ;
	}
}

template <class MAP>
inline Dart Map2<MAP>::alpha0(Dart d) const
{
	return phi2(d) ;
}

template <class MAP>
inline Dart Map2<MAP>::alpha1(Dart d) const
{
	return phi2(phi_1(d)) ;
}

template <class MAP>
inline Dart Map2<MAP>::alpha_1(Dart d) const
{
	return phi1(phi2(d)) ;
}

template <class MAP>
inline Dart Map2<MAP>::phi2_1(Dart d) const
{
	return phi2(phi_1(d)) ;
}

template <class MAP>
inline Dart Map2<MAP>::phi12(Dart d) const
{
	return phi1(phi2(d)) ;
}

template <class MAP>
inline void Map2<MAP>::phi2sew(Dart d, Dart e)
{
	MAP::involutionSew<0>(d,e);
}

template <class MAP>
inline void Map2<MAP>::phi2unsew(Dart d)
{
	MAP::involutionUnsew<0>(d);
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

inline bool Map2::sameVertex(Dart d, Dart e) const
{
	return sameOrientedVertex(d, e) ;
}

inline bool Map2::sameEdge(Dart d, Dart e) const
{
	return d == e || phi2(d) == e ;
}

inline bool Map2::isBoundaryEdge(Dart d) const
{
	return isBoundaryMarked2(d) || isBoundaryMarked2(phi2(d));
}

inline bool Map2::sameOrientedFace(Dart d, Dart e) const
{
	return Map1::sameCycle(d, e) ;
}

inline bool Map2::sameFace(Dart d, Dart e) const
{
	return sameOrientedFace(d, e) ;
}

inline unsigned int Map2::faceDegree(Dart d) const
{
	return Map1::cycleDegree(d) ;
}


inline int Map2::checkFaceDegree(Dart d, unsigned int le) const
{
	return Map1::checkCycleDegree(d,le) ;
}


inline bool Map2::sameVolume(Dart d, Dart e) const
{
	return sameOrientedVolume(d, e) ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

inline bool Map2::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread) const
{
	return Map1::foreach_dart_of_cc(d, f, thread);
}

inline bool Map2::foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread) const
{
	return foreach_dart_of_cc(d, f, thread);
}

inline bool Map2::foreach_dart_of_vertex1(Dart d, FunctorType& f, unsigned int thread) const
{
	return Map1::foreach_dart_of_vertex(d,f,thread);
}

inline bool Map2::foreach_dart_of_edge1(Dart d, FunctorType& f, unsigned int thread) const
{
	return Map1::foreach_dart_of_edge(d,f,thread);
}

} // namespace CGoGN
