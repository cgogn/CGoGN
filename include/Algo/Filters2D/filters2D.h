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

#ifndef __FILTERS_2D__
#define __FILTERS_2D__

namespace CGoGN
{

namespace Algo
{

namespace Filters2D
{

enum Filters { AVERAGE, TAUBIN, BILATERAL, AVERAGE_NORMAL, MMSE, SUSAN, TNBA, VNBA } ;

/**
 * Average filter need:
 * vertex embedding with position & position2
 */
template <typename PFP>
void filterAverage(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select = SelectorTrue()) ;

/**
 * Taubin filter need:
 * vertex embedding with position & position2
 */
template <typename PFP>
void filterTaubin(typename PFP::MAP& map, typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select = SelectorTrue()) ;

/**
 * bilateral filter need 
 * vertex embedding with: position & position2
 */
template <typename PFP>
void filterBilateral(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select = SelectorTrue()) ;

template <typename PFP>
void filterSUSAN(typename PFP::MAP& map, float SUSANthreshold, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select = SelectorTrue()) ;

/**
 * Average normal filter need:
 * vertex embedding with position & position2
 * face embedding with normal, centroid & area
 */
template <typename PFP>
void filterAverageNormals(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select = SelectorTrue()) ;

/**
 * MMSE filter need:
 * vertex embedding with position & position2
 * face embedding with normal, centroid & area
 */
template <typename PFP>
void filterMMSE(typename PFP::MAP& map, float sigmaN2, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select = SelectorTrue()) ;

template <typename PFP>
void filterTNBA(typename PFP::MAP& map, float sigmaN2, float SUSANthreshold, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select = SelectorTrue()) ;

template <typename PFP>
void filterVNBA(typename PFP::MAP& map, float sigmaN2, float SUSANthreshold, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select = SelectorTrue()) ;

/**
 * some useful tools
 */

template <typename PFP>
float computeHaussdorf(typename PFP::MAP& map, const typename PFP::TVEC3& originalPosition, const typename PFP::TVEC3& position2, const FunctorSelect& select = SelectorTrue()) ;

template <typename PFP>
void computeNoise(typename PFP::MAP& map, long amount, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select = SelectorTrue()) ;

} //namespace Filters2D

} //namespace Algo

} //namespace CGoGN

#include "Algo/Filters2D/average.hpp"
#include "Algo/Filters2D/taubin.hpp"
#include "Algo/Filters2D/bilateral.hpp"
#include "Algo/Filters2D/average_normals.hpp"
#include "Algo/Filters2D/tools.hpp"

#endif
