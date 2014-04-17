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

#include <iostream>
#include "dart.h"
#include "traversor2.h"
#include "traversor3.h"
#include "traversorCell.h"



namespace CGoGN
{

class ForeachHide
{

};

/// MACRO FOR SIMPLE TRAVERSALS WITH CELLS

#define YforeachIncident2(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)\
  IncidentTrav2<MAP_TYPE,FROM,TO> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM,Cell_PARAM); \
  for ( Cell<TO> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.t.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.t.end());\
	Cell_ITER != Cell_ITER ## _endTMLoop; Cell_ITER.dart() = Cell_ITER ## _TraversalMacroLocalLoop##LNN.t.next())
#define XforeachIncident2(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN) YforeachIncident2(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)
#define foreachIncident2(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM) XforeachIncident2(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,__LINE__)



#define YforeachAdjacent2(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)\
  AdjacentTrav2<MAP_TYPE,ORBIT,THRU> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM,Cell_PARAM); \
  for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.t.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.t.end()); \
	Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop##LNN.t.next())
#define XforeachAdjacent2(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN) YforeachAdjacent2(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)
#define foreachAdjacent2(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM) XforeachAdjacent2(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,__LINE__)


#define YforeachIncident3(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)\
  Traversor3XY<MAP_TYPE,FROM,TO> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM,Cell_PARAM); \
  for ( Cell<TO> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.end());\
	Cell_ITER != Cell_ITER ## _endTMLoop; Cell_ITER.dart() = Cell_ITER ## _TraversalMacroLocalLoop##LNN.next())
#define XforeachIncident3(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN) YforeachIncident3(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)
#define foreachIncident3(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM) XforeachIncident3(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM,__LINE__)

#define YforeachAdjacent3(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)\
Traversor3XXaY<MAP_TYPE,ORBIT,THRU> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM,Cell_PARAM); \
  for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.end()); \
	Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop##LNN.next())
#define XforeachAdjacent3(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN) YforeachAdjacent3(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)
#define foreachAdjacent3(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM) XforeachAdjacent3(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM,__LINE__)


#define YforeachCell(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)\
	TraversorCell<MAP_TYPE,ORBIT> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM);\
	for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.end()); \
	  Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop##LNN.next())
#define XforeachCell(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)  YforeachCell(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM,LNN)
#define foreachCell(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM)  XforeachCell(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM,__LINE__)


///// VERSION WITHOUT TYPE OF MAP, use PFP::MAP

/*

#define foreachIncident2PFP(FROM, Cell_PARAM, TO, Cell_ITER, MAP_PARAM)\
  IncidentTrav2<typename PFP::MAP,FROM,TO> Cell_ITER##_TraversalMacroLocalLoop__LINE__(MAP_PARAM,Cell_PARAM); \
  for ( Cell<TO> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop__LINE__.t.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop__LINE__.t.end());\
	Cell_ITER != Cell_ITER ## _endTMLoop; Cell_ITER.dart() = Cell_ITER ## _TraversalMacroLocalLoop__LINE__.t.next())

#define foreachAdjacent2PFP(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_PARAM)\
  AdjacentTrav2<typename PFP::MAP,ORBIT,THRU> Cell_ITER##_TraversalMacroLocalLoop__LINE__(MAP_PARAM,Cell_PARAM); \
  for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop__LINE__.t.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop__LINE__.t.end()); \
	Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop__LINE__.t.next())


#define foreachIncident3PFP(FROM, Cell_PARAM, TO, Cell_ITER, MAP_PARAM)\
  Traversor3XY<typename PFP::MAP,FROM,TO> Cell_ITER##_TraversalMacroLocalLoop__LINE__(MAP_PARAM,Cell_PARAM); \
  for ( Cell<TO> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop__LINE__.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop__LINE__.end());\
	Cell_ITER != Cell_ITER ## _endTMLoop; Cell_ITER.dart() = Cell_ITER ## _TraversalMacroLocalLoop__LINE__.next())

#define foreachAdjacent3PFP(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_PARAM)\
  Traversor3XXaY<typename PFP::MAP,ORBIT,THRU> Cell_ITER##_TraversalMacroLocalLoop__LINE__(MAP_PARAM,Cell_PARAM); \
  for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop__LINE__.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop__LINE__.end()); \
	Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop__LINE__.next())


#define foreachCellPFP(ORBIT, Cell_ITER, MAP_PARAM)\
	TraversorCell<typename PFP::MAP,ORBIT> Cell_ITER##_TraversalMacroLocalLoop__LINE__(MAP_PARAM);\
	for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop__LINE__.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop__LINE__.end()); \
	  Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop__LINE__.next())

*/


/// MultiThread versions of foreach___3
/// for dimension no need (to not use marker)

#define YforeachIncident3MT(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN)\
Traversor3XY<MAP_TYPE,FROM,TO> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM,Cell_PARAM,false,THREAD); \
for ( Cell<TO> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.end());\
  Cell_ITER != Cell_ITER ## _endTMLoop; Cell_ITER.dart() = Cell_ITER ## _TraversalMacroLocalLoop##LNN.next())
#define XforeachIncident3MT(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN) YforeachIncident3MT(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN)
#define foreachIncident3MT(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD) XforeachIncident3MT(FROM, Cell_PARAM, TO, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,__LINE__)

#define YforeachAdjacent3MT(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN)\
Traversor3XXaY<MAP_TYPE,ORBIT,THRU> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM,Cell_PARAM,false,THREAD); \
for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.end()); \
  Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop##LNN.next())
#define XforeachAdjacent3MT(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN) YforeachAdjacent3MT(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN)
#define foreachAdjacent3MT(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD) XforeachAdjacent3MT(ORBIT,THRU, Cell_PARAM, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,__LINE__)



#define YforeachCellMT(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN)\
TraversorCell<MAP_TYPE,ORBIT> Cell_ITER##_TraversalMacroLocalLoop##LNN(MAP_PARAM,false,THREAD);\
for (Cell<ORBIT> Cell_ITER ( Cell_ITER##_TraversalMacroLocalLoop##LNN.begin()), Cell_ITER##_endTMLoop( Cell_ITER##_TraversalMacroLocalLoop##LNN.end()); \
  Cell_ITER != Cell_ITER##_endTMLoop; Cell_ITER.dart() = Cell_ITER##_TraversalMacroLocalLoop##LNN.next())
#define XforeachCellMT(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN) YforeachCellMT(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,LNN)
#define foreachCellMT(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD) XforeachCellMT(ORBIT, Cell_ITER, MAP_TYPE, MAP_PARAM, THREAD,__LINE__)



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
	Dart m_d;
public:
	/// emoty construtor
	Cell(): m_d() {}
	/// construtor from Dart
	inline Cell(Dart d): m_d(d) {}
	/// copy constructor
	inline Cell(const Cell<ORBIT>& c): m_d(c.m_d) {}
	/// return the dart
	inline Dart dart() const { return m_d; }
	/// return a ref to dart
	inline Dart& dart() { return m_d; }
	/// equal operation
	inline bool operator==(Cell<ORBIT> d) const { return d.m_d.index == m_d.index; }
	inline bool operator!=(Cell<ORBIT> d) const { return d.m_d.index != m_d.index; }
	/// Dart cast operator
	inline operator Dart() {return m_d;}
};

typedef Cell<VERTEX> Vertex;
typedef Cell<EDGE>   Edge;
typedef Cell<FACE>   Face;
typedef Cell<VOLUME> Vol;




}

#endif /* CELLS_H_ */
