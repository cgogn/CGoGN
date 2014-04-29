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

#ifndef CELLS_H_
#define CELLS_H_

#include "Topology/generic/dart.h"

namespace CGoGN
{

/**
 * class for cellular typing
 *
 * warning to automatic conversion
 * cell -> Dart (or const Dart&) ok
 * Dart -> Cell (or const Cell&) ok
 */
template <unsigned int ORBIT>
class Cell
{
public:
	Dart dart;

	/// empty construtor
	Cell(): dart() {}

	/// constructor from Dart
	inline Cell(Dart d): dart(d) {}

	/// copy constructor
	inline Cell(const Cell<ORBIT>& c): dart(c.dart) {}

	/// Dart cast operator
	inline operator Dart() const { return dart; }

	friend std::ostream& operator<<( std::ostream &out, const Cell<ORBIT>& fa ) { return out << fa.dart; }

	inline bool valid() const { return !dart.isNil(); }
};

typedef Cell<VERTEX> Vertex;
typedef Cell<EDGE>   Edge;
typedef Cell<FACE>   Face;
typedef Cell<VOLUME> Vol;  // not Volume because of the namespace Volume

}

#endif /* CELLS_H_ */
