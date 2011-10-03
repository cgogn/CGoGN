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

namespace CGoGN
{

inline void GMap1::init()
{
	m_beta1 = addRelation("beta1") ;
}

inline GMap1::GMap1() : GMap0()
{
	init() ;
}

inline std::string GMap1::mapTypeName()
{
	return "GMap1";
}

inline unsigned int GMap1::dimension()
{
	return 1;
}

inline void GMap1::clear(bool removeAttrib)
{
	GMap0::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

inline int GMap1::getDartsPerTriangle()
{
	return 6 ;
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

inline Dart GMap1::newDart()
{
	Dart d = GMap0::newDart() ;
	(*m_beta1)[d.index] = d ;
	return d ;
}

inline Dart GMap1::beta1(Dart d)
{
	return (*m_beta1)[d.index] ;
}

template <int N>
inline Dart GMap1::beta(const Dart d)
{
	assert( (N > 0) || !"negative parameters not allowed in template multi-beta");
	if (N<10)
	{
		switch(N)
		{
		case 0 : return beta0(d) ;
		case 1 : return beta1(d) ;
		default : assert(!"Wrong multi-beta relation value") ;
		}
	}
	switch(N%10)
	{
	case 0 : return beta0(beta<N/10>(d)) ;
	case 1 : return beta1(beta<N/10>(d)) ;
	default : assert(!"Wrong multi-beta relation value") ;
	}
}

inline Dart GMap1::phi1(Dart d)
{
	Dart e = beta0(d) ;
	Dart f = beta1(e) ;
	if(f == e)
		return d ;
	else
		return f ;
}

inline Dart GMap1::phi_1(Dart d)
{
	Dart e = beta1(d) ;
	if(e == d)
		return d ;
	else
		return beta0(e) ;
}

template <int N>
inline Dart GMap1::phi(Dart d)
{
	assert((N > 0) || !"negative parameters not allowed in template multi-phi");
	if (N < 10)
	{
		switch(N)
		{
		case 1 : return phi1(d) ;
		default : assert(!"Wrong multi-phi relation value") ; return d ;
		}
	}
	switch(N%10)
	{
	case 1 : return phi1(phi<N/10>(d)) ;
	default : assert(!"Wrong multi-phi relation value") ; return d ;
	}
}

inline Dart GMap1::alpha1(Dart d)
{
	return phi1(d) ;
}

inline Dart GMap1::alpha_1(Dart d)
{
	return phi_1(d) ;
}

inline void GMap1::beta1sew(Dart d, Dart e)
{
	assert((*m_beta1)[d.index] == d) ;
	assert((*m_beta1)[e.index] == e) ;
	(*m_beta1)[d.index] = e ;
	(*m_beta1)[e.index] = d ;
}

inline void GMap1::beta1unsew(Dart d)
{
	Dart e = (*m_beta1)[d.index] ;
	(*m_beta1)[d.index] = d ;
	(*m_beta1)[e.index] = e ;
}

inline void GMap1::phi1sew(Dart d, Dart e)
{
	Dart f = phi1(d) ;
	Dart g = phi1(e) ;
	beta1unsew(f) ;
	beta1unsew(g) ;
	beta1sew(beta0(d), g) ;
	beta1sew(beta0(e), f) ;
}

inline void GMap1::phi1unsew(Dart d)
{
	Dart e = phi1(d) ;
	Dart f = phi1(e) ;
	beta1unsew(e) ;
	beta1unsew(f) ;
	beta1sew(beta0(d), f) ;
}

/*! @name Topological Operators
 *  Topological operations on 1-maps
 *************************************************************************/

inline void GMap1::cutEdge(Dart d)
{
	Dart e = newEdge() ;	// Create a new edge
	Dart f = phi1(d) ;
	beta1unsew(f) ;
	beta1sew(beta0(d), e) ;
	beta1sew(beta0(e), f) ;
}

inline void GMap1::collapseEdge(Dart d)
{
	Dart d1 = beta1(d) ;
	Dart dd = beta0(d) ;
	Dart dd1 = beta1(dd) ;
	beta1unsew(d) ;
	beta1unsew(dd) ;
	beta1sew(d1, dd1) ;
	deleteEdge(d) ;
}

inline void GMap1::splitFace(Dart d, Dart e)
{
	assert(d != e && sameOrientedFace(d, e)) ;
	Dart d1 = beta1(d) ;
	Dart e1 = beta1(e) ;
	Dart dd = newEdge() ;
	Dart ee = newEdge() ;
	beta1unsew(d) ;
	beta1unsew(e) ;
	beta1sew(d, dd) ;
	beta1sew(beta0(dd), e1) ;
	beta1sew(e, ee) ;
	beta1sew(beta0(ee), d1) ;
}

inline void GMap1::mergeFaces(Dart d, Dart e)
{
	assert(!sameFace(d, e)) ;
	Dart dd = beta0(d) ;
	Dart d1 = beta1(d) ;
	Dart dd1 = beta1(dd) ;
	Dart ee = beta0(e) ;
	Dart e1 = beta1(e) ;
	Dart ee1 = beta1(ee) ;
	beta1unsew(d) ;
	beta1unsew(dd) ;
	beta1unsew(e) ;
	beta1unsew(ee) ;
	beta1sew(d1, ee1) ;
	beta1sew(e1, dd1) ;
	deleteEdge(d) ;
	deleteEdge(e) ;
}

inline void GMap1::linkVertices(Dart d, Dart e)
{
	assert(d != e && !sameOrientedFace(d, e)) ;
	GMap1::cutEdge(phi_1(d));		// cut the edge before d (insert a new dart before d)
	GMap1::cutEdge(phi_1(e));		// cut the edge before e (insert a new dart before e)
	phi1sew(phi_1(d), phi_1(e)) ;	// phi1sew between the 2 new inserted darts
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

inline bool GMap1::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	if (f(d)) return true;
	Dart d1 = beta1(d);
	if (d1 != d) return f(d1);
	return false;
}

inline bool GMap1::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	if (f(d)) return true;
	Dart d1 = beta0(d);
	if (d1 != d) return f(d1);
	return false;
}

inline bool GMap1::foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread)
{
	return foreach_dart_of_face(d, f, thread) ;
}

} // namespace CGoGN
