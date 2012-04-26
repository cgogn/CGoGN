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
#ifndef _TUTO4_H_
#define _TUTO4_H_

#include "Utils/Qt/qtSimple.h"
#include "Utils/cgognStream.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Algo/Render/GL2/topoRender.h"


using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the type of the map
	typedef EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP ;
typedef PFP::VEC3 VEC3 ;


class MyQT: public Utils::QT::SimpleQT
{
	Q_OBJECT
public:
	MyQT():m_render_topo(NULL){}

	void cb_redraw();
	void cb_initGL();
	void cb_mouseClick(int button, int x, int y);

protected:
	// declaration of the map
	MAP myMap;

	// attribute handler on position;
	AttributeHandler<VEC3, VERTEX> position;

	// render (for the topo)
	Algo::Render::GL2::TopoRender* m_render_topo;

	// selected dart (mouse click)
	std::vector<Dart> dart_selected;
	//
	Geom::Vec3f color;

	// just for more compact writing
	inline Dart PHI1(Dart d)	{return myMap.phi1(d);}
	inline Dart PHI_1(Dart d)	{return myMap.phi_1(d);}
	inline Dart PHI2(Dart d)	{return myMap.phi2(d);}
	template<int X>
	Dart PHI(Dart d)	{return myMap.phi<X>(d);}

public:

	// create the map (here tetrahedron
	void createMap();

	// traverse the map using markers
	void traverseMap();
};


#endif
