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

#ifndef _TOPO_GL_RENDER
#define _TOPO_GL_RENDER

#include <list>
//#include <gmtl/VecOps.h>
//#include <gmtl/Output.h>
#include <GL/gl.h>

#include "Topology/generic/functor.h"

// OpenGL direct mode rendering of darts of maps

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL1
{

/**
* Render darts of generalized map
*/
template <typename PFP>
void renderTopoGM2(typename PFP::MAP& the_map, Marker m);

/**
* Render darts of dual map
*
* @param the_map map to render
* @param drawPhi1 draw the phi1 relation ?
* @param drawPhi2 draw the phi2 relation ?
* @param ke exploding coefficient for edge (1.0 normal draw)
* @param kf exploding coefficient for edge (1.0 normal draw)
*/

template <typename PFP>
void renderTopoMD2(typename PFP::MAP& the_map, const typename PFP::TVEC3& positions, bool drawPhi1, bool drawPhi2, float ke, float kf);


/**
 * Render darts of dual map
 *
 * @param the_map map to render
 * @param drawPhi1 draw the phi1 relation ?
 * @param drawPhi2 draw the phi2 relation ?
 * @param drawPhi3 draw the phi3 relation ?
 * @param ke exploding coefficient for edge (1.0 normal draw)
 * @param kf exploding coefficient for edge (1.0 normal draw)
 * @param kv exploding coefficient for volumes (0.0 normal draw)
 */
template <typename PFP>
void renderTopoMD3(typename PFP::MAP& the_map, const typename PFP::TVEC3& positions, bool drawPhi1, bool drawPhi2, bool drawPhi3, float ke, float kf, float kv, FunctorType& good);

template <typename PFP>
void renderTopoMD3(typename PFP::MAP& map, typename PFP::TVEC3& positions, bool drawPhi1, bool drawPhi2, bool drawPhi3, float ke, float kf, float kv);


}// end namespace
}// end namespace
}// end namespace
}// end namespace

#include "Algo/Render/GL1/topo_render.hpp"

#endif
