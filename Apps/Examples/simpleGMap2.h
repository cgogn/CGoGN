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

#include <iostream>

#include "Utils/Qt/qtSimple.h"

#include "Topology/generic/parameters.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Geometry/vector_gen.h"

#include "Algo/Render/GL1/topo_render.h"

using namespace CGoGN ;

struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedGMap2 MAP ;
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;

class SimpleGMap2 : public Utils::QT::SimpleQT
{
	Q_OBJECT

public:
	MAP myMap ;

	PFP::TVEC3 position ;

	SimpleGMap2() ;

	void initGUI() ;

	void cb_initGL() ;
	void cb_redraw() ;
};

