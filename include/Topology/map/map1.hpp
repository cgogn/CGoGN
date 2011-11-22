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

/// INLINE FUNCTIONS

inline void Map1::init()
{
	m_phi1 = addRelation("phi1") ;
	m_phi_1 = addRelation("phi_1") ;
}

inline Map1::Map1() : AttribMap()
{
	init() ;
}

inline std::string Map1::mapTypeName()
{
	return "Map1" ;
}

inline unsigned int Map1::dimension()
{
	return 1 ;
}

inline void Map1::clear(bool removeAttrib)
{
	AttribMap::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

inline Dart Map1::newDart()
{
	Dart d = GenericMap::newDart() ;
	(*m_phi1)[d.index] = d ;
	(*m_phi_1)[d.index] = d ;
	return d ;
}

inline Dart Map1::phi1(Dart d)
{
	return (*m_phi1)[d.index] ;
}

inline Dart Map1::phi_1(Dart d)
{
	return (*m_phi_1)[d.index] ;
}

template <int N>
inline Dart Map1::phi(Dart d)
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

inline Dart Map1::alpha1(Dart d)
{
	return phi1(d) ;
}

inline Dart Map1::alpha_1(Dart d)
{
	return phi_1(d) ;
}

inline void Map1::phi1sew(Dart d, Dart e)
{
	Dart f = (*m_phi1)[d.index] ;
	Dart g = (*m_phi1)[e.index] ;
	(*m_phi1)[d.index] = g ;
	(*m_phi1)[e.index] = f ;
	(*m_phi_1)[g.index] = d ;
	(*m_phi_1)[f.index] = e ;
}

inline void Map1::phi1unsew(Dart d)
{
	Dart e = (*m_phi1)[d.index] ;
	Dart f = (*m_phi1)[e.index] ;
	(*m_phi1)[d.index] = f ;
	(*m_phi1)[e.index] = e ;
	(*m_phi_1)[f.index] = d ;
	(*m_phi_1)[e.index] = e ;
}

inline int Map1::getDartsPerTriangle()
{
	return 3 ;
}


/*! @name Topological Operators
 *  Topological operations on 1-maps
 *************************************************************************/

inline void Map1::cutEdge(Dart d)
{
	Dart e = newDart() ;	// Create a new dart
	phi1sew(d, e) ;			// Insert dart e between d and phi1(d)

	if (isBoundaryMarked(d))
		boundaryMark(e);
}

inline void Map1::collapseEdge(Dart d)
{
	phi1unsew(phi_1(d)) ;	// Dart before d is linked to its successor
	deleteDart(d) ;			// Dart d is erased
}

inline void Map1::splitCycle(Dart d, Dart e)
{
	assert(d != e && sameCycle(d, e)) ;
	phi1sew(phi_1(d), phi_1(e)) ;
}

inline void Map1::mergeCycles(Dart d, Dart e)
{
	assert(!sameCycle(d, e)) ;
	phi1sew(phi_1(d), phi_1(e)) ;
}

inline void Map1::linkCycles(Dart d, Dart e)
{
	assert(d != e && !sameCycle(d, e)) ;
	Map1::cutEdge(phi_1(d));		// cut the edge before d (insert a new dart before d)
	Map1::cutEdge(phi_1(e));		// cut the edge before e (insert a new dart before e)
	phi1sew(phi_1(d), phi_1(e)) ;	// phi1sew between the 2 new inserted darts
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

inline bool Map1::sameCycle(Dart d, Dart e)
{
	Dart it = d ;
	do
	{
		if(it == e)
			return true ;
		it = phi1(it) ;
	} while(it != d) ;
	return false ;
}

inline unsigned int Map1::cycleDegree(Dart d)
{
	unsigned int count = 0 ;
	Dart it = d ;
	do
	{
		++count ;
		it = phi1(it) ;
	} while (it != d) ;
	return count ;
}

inline bool Map1::isCycleTriangle(Dart d)
{
	return (phi1(d) != d) && (phi1(phi1(phi1(d))) == d) ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

inline bool Map1::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	return f(d) ;
}

inline bool Map1::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	return f(d) ;
}

inline bool Map1::foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread)
{
	Dart it = d ;
	do
	{
		if (f(it))
			return true ;
		it = phi1(it) ;
	} while (it != d) ;
	return false ;
}

inline bool Map1::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread)
{
	return foreach_dart_of_oriented_face(d, f, thread) ;
}

inline bool Map1::foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread)
{
	return foreach_dart_of_oriented_face(d, f, thread) ;
}

inline bool Map1::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	return foreach_dart_of_oriented_face(d, f, thread) ;
}

} // namespace CGoGN
