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
inline void Map1<MAP>::init()
{
	MAP::addPermutation() ;
}

template <class MAP>
inline Map1<MAP>::Map1() : MAP()
{
	init() ;
}

template <class MAP>
inline std::string Map1<MAP>::mapTypeName() const
{
	return "Map1" ;
}

template <class MAP>
inline unsigned int Map1<MAP>::dimension() const
{
	return 1 ;
}

template <class MAP>
inline void Map1<MAP>::clear(bool removeAttrib)
{
	MAP::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

template <class MAP>
inline void Map1<MAP>::update_topo_shortcuts()
{
	MAP::update_topo_shortcuts();
	m_phi1 = MAP::getRelation("phi1");
	m_phi_1 = MAP::getRelation("phi_1");
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

template <class MAP>
inline Dart Map1<MAP>::newDart()
{
	Dart d = GenericMap::newDart() ;
	unsigned int d_index = MAP::dartIndex(d) ;
	(*m_phi1)[d_index] = d ;
	(*m_phi_1)[d_index] = d ;
//	if(m_isMultiRes)
//	{
//		pushLevel() ;
//		for(unsigned int i = m_mrCurrentLevel + 1;  i < m_mrDarts.size(); ++i)
//		{
//			setCurrentLevel(i) ;
//			unsigned int d_index = dartIndex(d) ;
//			(*m_phi1)[d_index] = d ;
//			(*m_phi_1)[d_index] = d ;
//		}
//		popLevel() ;
//	}
	return d ;
}

template <class MAP>
inline Dart Map1<MAP>::phi1(Dart d) const
{
	return MAP::getPermutation<0>(d);
}

template <class MAP>
inline Dart Map1<MAP>::phi_1(Dart d) const
{
	return MAP::getPermutationInv<0>(d);
}

template <class MAP>
template <int N>
inline Dart Map1<MAP>::phi(Dart d) const
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

template <class MAP>
inline Dart Map1<MAP>::alpha1(Dart d) const
{
	return phi1(d) ;
}

template <class MAP>
inline Dart Map1<MAP>::alpha_1(Dart d) const
{
	return phi_1(d) ;
}

template <class MAP>
inline void Map1<MAP>::phi1sew(Dart d, Dart e)
{
	MAP::permutationSew<0>(d,e);
}

template <class MAP>
inline void Map1<MAP>::phi1unsew(Dart d)
{
	MAP::permutationUnsew<0>(d);
}

/*! @name Topological Operators
 *  Topological operations on 1-maps
 *************************************************************************/

template <class MAP>
inline Dart Map1<MAP>::cutEdge(Dart d)
{
	Dart e = newDart() ;	// Create a new dart
	phi1sew(d, e) ;			// Insert dart e between d and phi1(d)

	if (this->isBoundaryMarked2(d))
		MAP::boundaryMark2(e);

	if (MAP::isBoundaryMarked3(d))
		MAP::boundaryMark3(e);

	return e ;
}

template <class MAP>
inline void Map1<MAP>::uncutEdge(Dart d)
{
	Dart d1 = phi1(d) ;
	phi1unsew(d) ;			// Dart d is linked to the successor of its successor
	MAP::deleteDart(d1) ;	// Dart d1 is erased
}

template <class MAP>
inline void Map1<MAP>::collapseEdge(Dart d)
{
	phi1unsew(phi_1(d)) ;	// Dart before d is linked to its successor
	MAP::deleteDart(d) ;	// Dart d is erased
}

template <class MAP>
inline void Map1<MAP>::splitCycle(Dart d, Dart e)
{
	assert(d != e && sameCycle(d, e)) ;
	phi1sew(phi_1(d), phi_1(e)) ;
}

template <class MAP>
inline void Map1<MAP>::mergeCycles(Dart d, Dart e)
{
	assert(!sameCycle(d, e)) ;
	phi1sew(phi_1(d), phi_1(e)) ;
}

template <class MAP>
inline void Map1<MAP>::linkCycles(Dart d, Dart e)
{
	assert(d != e && !sameCycle(d, e)) ;
	Map1<MAP>::cutEdge(phi_1(d));		// cut the edge before d (insert a new dart before d)
	Map1<MAP>::cutEdge(phi_1(e));		// cut the edge before e (insert a new dart before e)
	phi1sew(phi_1(d), phi_1(e)) ;	// phi1sew between the 2 new inserted darts
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

template <class MAP>
inline bool Map1<MAP>::sameCycle(Dart d, Dart e) const
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

template <class MAP>
inline unsigned int Map1<MAP>::cycleDegree(Dart d) const
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

template <class MAP>
inline int Map1<MAP>::checkCycleDegree(Dart d, unsigned int degree) const
{
	unsigned int count = 0 ;
	Dart it = d ;
	do
	{
		++count ;
		it = phi1(it) ;
	} while ((count<=degree) && (it != d)) ;

	return count-degree;
}

template <class MAP>
inline bool Map1<MAP>::isCycleTriangle(Dart d) const
{
	return (phi1(d) != d) && (phi1(phi1(phi1(d))) == d) ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

template <class MAP>
inline bool Map1<MAP>::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	return f(d) ;
}

template <class MAP>
inline bool Map1<MAP>::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	return f(d) ;
}

template <class MAP>
inline bool Map1<MAP>::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int /*thread*/) const
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


} // namespace CGoGN
