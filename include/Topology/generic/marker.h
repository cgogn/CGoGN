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

#ifndef _MARKER_H_
#define _MARKER_H_

#include "Utils/mark.h"
#include "Topology/generic/dart.h"

namespace CGoGN
{

class Marker: public Mark
{
protected:
	unsigned int m_cell;

public:
	Marker(Mark m, unsigned int cell): Mark(m), m_cell(cell) {}
//#ifndef DART
//#define DART	(unsigned int)(5)
//#endif
	Marker(): Mark(0), m_cell(DART) {} // TODO trouver un moyen propre de faire ça

	Marker operator+(const Marker m) const
	{
		Marker n(*this);
		n.setMark(m);
		return n;
	}

	unsigned int getCell() const { return m_cell; }
};


//! Class that manages the locking (lock and release) of markers
class MarkerSet : public Mark
{
public:
	//! Constructor
	MarkerSet() : Mark() {};
	
	//! Get a free marker and lock it
	Marker getNewMarker(unsigned int cell);
	
	//! Release a locked marker
	void releaseMarker(Marker m)
	{
		unsetMark(m);
	}
};

} //namespace CGoGN

#endif
