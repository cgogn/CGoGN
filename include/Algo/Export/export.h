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

#ifndef __EXPORT_H__
#define __EXPORT_H__

#include "Topology/generic/attributeHandler.h"

namespace CGoGN
{

namespace Algo
{

namespace Export
{

/**
* export the map into a PLY file
* @param the_map map to be exported
* @param filename filename of ply file
* @return true
*/
template <typename PFP>
bool exportPLY(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good = SelectorTrue()) ;

/**
* export the map into a OFF file
* @param the_map map to be exported
* @param filename filename of off file
* @return true
*/
template <typename PFP>
bool exportOFF(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good = SelectorTrue()) ;

/**
* export the map into a Trian file
* @param the_map map to be exported
* @param filename filename of trian file
* @return true
*/
template <typename PFP>
bool exportTrian(typename PFP::MAP& the_map, const typename PFP::TVEC3& position, char* filename, const FunctorSelect& good = SelectorTrue()) ;

/**
* export the map into a Trian file
* @param the_map map to be exported
* @param filename filename of ply file
* @return true
*/
template <typename PFP>
bool exportCTM(typename PFP::MAP& the_map, const typename PFP::TVEC3& position, const std::string& filename, const FunctorSelect& good = SelectorTrue()) ;

/**
* export the map into a PLYPTM file
* @param the_map map to be exported
* @param filename filename of ply file
* @param position the position container
* @param frame[3] table of 3 vectors representing the local frame
* @param colorPTM[6] function coefficients (6) for each color channel (3)
* @return true
*/
template <typename PFP>
bool exportPLYPTM(typename PFP::MAP& map, const char* filename, const typename PFP::TVEC3& position, const typename PFP::TVEC3 frame[3], const typename PFP::TVEC3 colorPTM[6], const typename PFP::TREAL errL2 = AttributeHandler<typename PFP::REAL>(), const typename PFP::TREAL errLmax = AttributeHandler<typename PFP::REAL>(), const typename PFP::TREAL stdDev = AttributeHandler<typename PFP::REAL>(), const FunctorSelect& good = SelectorTrue()) ;


/**
 *  export pout l'InESS
 */
template <typename PFP>
bool exportInESS(typename PFP::MAP& map, const char *filename, const typename PFP::TVEC3& position);

} // namespace Export

} // namespace Algo

} // namespace CGoGN

#include "Algo/Export/export.hpp"

#endif
