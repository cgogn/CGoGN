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
inline void GMap2<MAP>::init()
{
	MAP::addInvolution() ;
}

template <class MAP>
inline GMap2<MAP>::GMap2() : GMap1<MAP>()
{
	init() ;
}

template <class MAP>
inline std::string GMap2<MAP>::mapTypeName() const
{
	return "GMap2";
}

template <class MAP>
inline unsigned int GMap2<MAP>::dimension() const
{
	return 2;
}

template <class MAP>
inline void GMap2<MAP>::clear(bool removeAttrib)
{
	ParentMap::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

template <class MAP>
inline void GMap2<MAP>::update_topo_shortcuts()
{
	ParentMap::update_topo_shortcuts();
//	m_beta2 = getRelation("beta2");
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

template <class MAP>
inline Dart GMap2<MAP>::beta2(Dart d) const
{
	return MAP::getInvolution<2>(d);
}

template <class MAP>
template <int N>
inline Dart GMap2<MAP>::beta(const Dart d) const
{
	assert( (N > 0) || !"negative parameters not allowed in template multi-beta");
	if (N<10)
	{
		switch(N)
		{
			case 0 : return this->beta0(d) ;
			case 1 : return this->beta1(d) ;
			case 2 : return beta2(d) ;
			default : assert(!"Wrong multi-beta relation value") ;
		}
	}
	switch(N%10)
	{
		case 0 : return beta0(beta<N/10>(d)) ;
		case 1 : return beta1(beta<N/10>(d)) ;
		case 2 : return beta2(beta<N/10>(d)) ;
		default : assert(!"Wrong multi-beta relation value") ;
	}
}

template <class MAP>
inline Dart GMap2<MAP>::phi2(Dart d) const
{
	return beta2(this->beta0(d)) ;
}

template <class MAP>
template <int N>
inline Dart GMap2<MAP>::phi(Dart d) const
{
	assert( (N >0) || !"negative parameters not allowed in template multi-phi");
	if (N<10)
	{
		switch(N)
		{
			case 1 : return this->phi1(d) ;
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
inline Dart GMap2<MAP>::alpha0(Dart d) const
{
	return beta2(this->beta0(d)) ;
}

template <class MAP>
inline Dart GMap2<MAP>::alpha1(Dart d) const
{
	return beta2(this->beta1(d)) ;
}

template <class MAP>
inline Dart GMap2<MAP>::alpha_1(Dart d) const
{
	return beta1(beta2(d)) ;
}

template <class MAP>
inline void GMap2<MAP>::beta2sew(Dart d, Dart e)
{
	MAP::involutionSew<2>(d,e);
}

template <class MAP>
inline void GMap2<MAP>::beta2unsew(Dart d)
{
	MAP::involutionUnsew<2>(d);
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

template <class MAP>
inline bool GMap2<MAP>::sameVertex(Dart d, Dart e) const
{
	return sameOrientedVertex(d, e) || sameOrientedVertex(beta2(d), e) ;
}

template <class MAP>
inline bool GMap2<MAP>::sameEdge(Dart d, Dart e) const
{
	return d == e || beta2(d) == e || this->beta0(d) == e || beta2(this->beta0(d)) == e ;
}

template <class MAP>
inline bool GMap2<MAP>::isBoundaryEdge(Dart d) const
{
	return this->isBoundaryMarked2(d) || this->isBoundaryMarked2(beta2(d));
}

template <class MAP>
inline bool GMap2<MAP>::sameOrientedFace(Dart d, Dart e) const
{
	return ParentMap::sameOrientedCycle(d, e) ;
}

template <class MAP>
inline bool GMap2<MAP>::sameFace(Dart d, Dart e) const
{
	return ParentMap::sameCycle(d, e) ;
}

template <class MAP>
inline unsigned int GMap2<MAP>::faceDegree(Dart d) const
{
	return ParentMap::cycleDegree(d) ;
}

template <class MAP>
inline int GMap2<MAP>::checkFaceDegree(Dart d, unsigned int le) const
{
	return ParentMap::checkCycleDegree(d,le) ;
}

template <class MAP>
inline bool GMap2<MAP>::sameVolume(Dart d, Dart e) const
{
	return sameOrientedVolume(d, e) || sameOrientedVolume(beta2(d), e) ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread) const
{
	return GMap2<MAP>::foreach_dart_of_oriented_vertex(d, f, thread) || GMap2<MAP>::foreach_dart_of_oriented_vertex(this->beta1(d), f, thread) ;
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread) const
{
	return GMap2<MAP>::foreach_dart_of_oriented_cc(d, f, thread) || GMap2<MAP>::foreach_dart_of_oriented_cc(this->beta0(d), f, thread) ;
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_cc(d, f, thread);
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_oriented_cc(d, f, thread);
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_vertex1(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_vertex(d,f,thread);
}

template <class MAP>
inline bool GMap2<MAP>::foreach_dart_of_edge1(Dart d, FunctorType& f, unsigned int thread) const
{
	return ParentMap::foreach_dart_of_edge(d,f,thread);
}

} // namespace CGoGN
