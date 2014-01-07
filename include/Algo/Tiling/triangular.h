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

#ifndef _TILING_TRIANGULAR_H_
#define _TILING_TRIANGULAR_H_

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Tilings
{

/*! \brief The class of regular triangular tiling
 * It allows the creation of
 * - grid 2D
 * - subdivided cube
 * - subdivided cone
 * - subdivided cylinder
 * - subdivided tore
 * - subdivided sphere (with pole)
 */
template <typename PFP>
class Triangular
{
    typedef typename PFP::MAP MAP;
    typedef typename PFP::VEC3 VEC3;

protected:
    /**
    * Map in which we are working
    */
    MAP& m_map;

    /**
    * Table of vertex darts (one dart per vertex)
    * Order depend on tiling kind
    */
    std::vector<Dart> m_tableVertDarts;

public:
    Triangular(MAP& map):
        m_map(map)
    { }

    /*! @name Topological Operators
     * Tiling creation
     *************************************************************************/

    //@{
    //! Create a 2D grid
    /*! @param x nb of squares in x
     *  @param y nb of squares in y
     *  @param closed close the boundary face of the 2D grid
     */
    void grid(unsigned int x, unsigned int y, bool closed);
    //@}

    /*! @name Embedding Operators
     * Tiling creation
     *************************************************************************/

    //@{
    //! Embed a topological grid
    /*! @param position Attribute used to store vertices positions
     *  @param x size in X
     *  @param x size in Y
     *  @param y position in Z (centered on 0 by default)
     */
    void embedGrid(VertexAttribute<VEC3>& position, float x, float y, float z = 0.0f);
    //@}
};

} // namespace Tilings

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Tiling/triangular.hpp"

#endif //_TILING_TRIANGULAR_H_
