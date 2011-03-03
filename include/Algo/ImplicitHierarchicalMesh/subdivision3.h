/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __IMPLICIT_HIERARCHICAL_MESH_SUBDIVISION3__
#define __IMPLICIT_HIERARCHICAL_MESH_SUBDIVISION3__


namespace CGoGN
{

namespace Algo
{

namespace IHM
{

enum SubdivideType
{
	S_TRI,
	S_QUAD
} ;

template <typename PFP>
void subdivideEdge(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position) ;

template <typename PFP>
void subdivideFace(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position, SubdivideType sType = S_QUAD);

template <typename PFP>
void subdivideVolume(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position, SubdivideType sType = S_QUAD);



template <typename PFP>
Dart subdivideFaceTri(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position);

template <typename PFP>
void subdivideVolumeTri(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position);

template <typename PFP>
void catmullClarck(typename PFP::MAP& map, Dart d, typename PFP::TVEC3& position);

} //namespace IHM

} //namespace Algo

} //namespace CGoGN

#include "Algo/ImplicitHierarchicalMesh/subdivision3.hpp"

#endif
