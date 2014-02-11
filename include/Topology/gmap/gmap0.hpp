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
inline void GMap0<MAP>::init()
{
	MAP::addInvolution() ;
}

template <class MAP>
inline GMap0<MAP>::GMap0() : MAP()
{
	init() ;
}

template <class MAP>
inline std::string GMap0<MAP>::mapTypeName() const
{
	return "GMap0";
}

template <class MAP>
inline unsigned int GMap0<MAP>::dimension() const
{
	return 0;
}

template <class MAP>
inline void GMap0<MAP>::clear(bool removeAttrib)
{
	MAP::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

template <class MAP>
inline void GMap0<MAP>::update_topo_shortcuts()
{
	MAP::update_topo_shortcuts();
//	m_beta0 = getRelation("beta0");
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

template <class MAP>
inline Dart GMap0<MAP>::beta0(Dart d) const
{
	return MAP::getInvolution<0>(d);
}

template <class MAP>
inline void GMap0<MAP>::beta0sew(Dart d, Dart e)
{
	MAP::involutionSew<0>(d,e);
}

template <class MAP>
inline void GMap0<MAP>::beta0unsew(Dart d)
{
	MAP::involutionUnsew<0>(d);
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

template <class MAP>
inline bool GMap0<MAP>::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	return f(d) ;
}

template <class MAP>
inline bool GMap0<MAP>::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int /*thread*/) const
{
	if (f(d)) return true;
	Dart d1 = beta0(d);
	if (d1 != d) return f(d1);
	return false;
}

} // namespace CGoGN
