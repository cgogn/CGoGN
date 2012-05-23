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

#ifndef __TETRAHEDRALIZATION_H__
#define __TETRAHEDRALIZATION_H__

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

namespace Tetrahedralization
{

/**
* subdivide a hexahedron into 5 tetrahedron
* @param d dart of the hexahedron
*/
template <typename PFP>
void hexahedronToTetrahedron(typename PFP::MAP& map, Dart d);

/************************************************************************************************
 * 									Collapse / Split Operators
 ************************************************************************************************/
template <typename PFP>
Dart splitVertex(typename PFP::MAP& map, std::vector<Dart>& vd);


/************************************************************************************************
 * 																Tetrahedron functions																	   *
 ************************************************************************************************/

/**
 * test if the volume is a tetrahedron
 * @param map
 * @param a dart from the volume
 */
template <typename PFP>
bool isTetrahedron(typename PFP::MAP& the_map, Dart d);

/**
 * test if a mesh (or submesh) is a tetrahedral mesh
 * @param map
 * @param selected
 */
template <typename PFP>
bool isTetrahedralization(typename PFP::MAP& map, const FunctorSelect& selected = allDarts);

/************************************************************************************************
 *																		Swap Functions 																	   *
 ************************************************************************************************/

/**
 * Swap a configuration of 2 Tetrahedron to another one
 * @param map
 * @param d a dart from the face between the two tetahedron to swap
 *
 * TODO ajouter image
 */
template <typename PFP>
void swap2To2(typename PFP::MAP& map, Dart d);

/**
 *
 */
template <typename PFP>
void swap4To4(typename PFP::MAP& map, Dart d);

/**
 * Swap a configuration of 3 tetrahedron to another one with 2 tetrahedron
 * @param map
 * @param d
 *
 * TODO ajouter image
 *
 */
template <typename PFP>
void swap3To2(typename PFP::MAP& map, Dart d);

/**
 *
 */
template <typename PFP>
Dart swap2To3(typename PFP::MAP& map, Dart d);

/**
 *
 */
template <typename PFP>
void swap5To4(typename PFP::MAP& the_map, Dart d, typename PFP::TVEC3& positions);

/************************************************************************************************
 *																		Flip Functions 																	   *
 ************************************************************************************************/

/*
 *
 */
template <typename PFP>
void flip1To4(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position);

/**
 *
 */
template <typename PFP>
void edgeBisection(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position);

} // namespace Tetrahedralization

} // namespace Modelisation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Modelisation/tetrahedralization.hpp"

#endif
