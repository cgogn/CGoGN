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

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Export/export.h"


#include "Algo/Tiling/square.h"
//#include "Algo/Tiling/triangular.h"
//#include "Algo/Tiling/hexagonal.h"

using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
    // definition of the map
    typedef EmbeddedMap2 MAP;
};

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        CGoGNout << "Usage : " << argv[0] << " tiling-type" << CGoGNendl;
        return 0;
    }

    unsigned int tiling;
    std::istringstream iss(argv[1]);
    iss >> tiling;

    // declaration of the map
    PFP::MAP myMap;

    VertexAttribute<PFP::VEC3> position = myMap.addAttribute<PFP::VEC3, VERTEX>("position");


    switch(tiling)
    {
        case 1:
        {
            std::cout << "square grid tiling" << std::endl;
            Algo::Surface::Tilings::Square::Grid<PFP> g(myMap,10,10,true);
            g.embedIntoGrid(position,50,50);

            Algo::Surface::Export::exportOFF<PFP>(myMap,position,"square_grid.off");

            break;
        }
        case 2:
        {
            std::cout << "square grid twisted strip tiling" << std::endl;
            Algo::Surface::Tilings::Square::Grid<PFP> g(myMap,10,10,true);
            g.embedIntoTwistedStrip(position, 0.3, 0.8, 5);

            Algo::Surface::Export::exportOFF<PFP>(myMap,position,"square_grid_strip.off");

            break;
        }
        case 3:
        {
            std::cout << "square grid helocoid tiling" << std::endl;
            Algo::Surface::Tilings::Square::Grid<PFP> g(myMap,20,20,true);
            g.embedIntoHelicoid(position, 0.3,  0.8, 5.0, 2.0);

            Algo::Surface::Export::exportOFF<PFP>(myMap,position,"square_grid_helicoid.off");

            break;
        }
        case 4:
        {
            std::cout << "square cylinder tiling" << std::endl;
            Algo::Surface::Tilings::Square::Cylinder<PFP> c(myMap,20,20);
            c.closeTop();
            c.closeBottom();
            //c.triangleTop();
            //c.triangleBottom();
            c.embedIntoCylinder(position,0.5,0.7,5.0);

            Algo::Surface::Export::exportOFF<PFP>(myMap,position,"square_cylinder.off");

            break;
        }
        case 5:
        {
            std::cout << "square cylinder sphere tiling" << std::endl;
            Algo::Surface::Tilings::Square::Cylinder<PFP> c(myMap,20,20);
            c.closeTop();
            c.closeBottom();
            c.triangleTop();
            c.triangleBottom();
            c.embedIntoSphere(position,0.5);

            Algo::Surface::Export::exportOFF<PFP>(myMap,position,"square_cylinder_sphere.off");

            break;
        }
        default:
        {
            break;
        }
    }


    return 0;
}

