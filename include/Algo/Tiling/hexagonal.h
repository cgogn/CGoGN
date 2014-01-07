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

#ifndef _TILINGS_H_
#define _TILINGS_H_

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Tilings
{

//square -> grid -> open or closed (closeHole)
//square -> cylinder -> grid + finish sewing -> open or closed (closeHole) -> triangule face
//square -> tore -> cylinder + finish sewing

template <typename PFP>
class Square
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
    * Order depend on Polyhedron kind
    */
    std::vector<Dart> m_tableVertDarts;

public:
    Square(MAP& map):
        m_map(map)
    { }

    void grid(unsigned int x, unsigned int y, bool closed);

    void cylinder(unsigned int n, unsigned int z, bool top_closed, bool bottom_closed);

    void tore(unsigned int n, unsigned int m);


    void embedGrid(VertexAttribute<VEC3>& position, float x, float y, float z = 0.0f);
};

//template <typename PFP>
//class Triangular
//{

//};

//template <typename PFP>
//class Hexagonal
//{

//};

//template <typename PFP>
//void triangular(typename PFP::MAP& map, unsigned int rows, unsigned int columns);

} // namespace Tilings

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Modelisation/tilings.hpp"

#endif
