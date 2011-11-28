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

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <vector>
#include "Algo/Modelisation/subdivision.h"
#include "Geometry/transfo.h"
#include "Topology/generic/cellmarker.h"

#include "Utils/os_spec.h"


namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

enum { NONE,GRID, CUBE, CYLINDER, CONE, SPHERE, TORE, COMPOSED };

/**
* sudivide the all quads of primtive into 2 triangles
* the vertex darts table remain valid (adding only darts, no remove)
*/
// template <typename PFP>
// void onlyTriangles(typename PFP::MAP& the_map, Dart primd);

/**
* Create a triangle fans (to close cylinders)
* simple topo creation  no modification of Polyhedron
* @param n nb of triangles in the fan in n
* @return the dart
*/
// template <typename PFP>
// Dart triangleFan_topo(typename PFP::MAP& the_map, int n);



/**
 * Unsex the Umbrella aroud a vertex, close the hole and then
 * create a symetric to construct a polyedron
 * @param d a dart from the vertex
 */
template <typename PFP>
void explodPolyhedron(typename PFP::MAP& map, Dart d, typename PFP::TVEC3 position);

/**
* class of geometric Polyhedron
* It alloaw the creation of:
* - grid 2D
* - subdivided cube
* - subdivided cone
* - subdivided cylinder
* - subdivided tore
* - subdivided sphere (with pole)
*
* Topological creation methods are separated from embedding to
* easily allow specific embedding.

*/
template <typename PFP>
class Polyhedron
{
	typedef typename PFP::MAP MAP;

public:
	enum {NONE,GRID, CUBE, CYLINDER, CONE, SPHERE, TORE, COMPOSED};

protected:
	/**
	* Map in which we are working
	*/
	MAP& m_map;

	/**
	* Reference dart of Polyhedron
	*/
	Dart m_dart;

	/**
	* Kind of Polyhedron (grid, cylinder,
	*/
	int m_kind;

	/**
	* Table of vertex darts (one dart per vertex)
	* Order depend on Polyhedron kind
	*/
	std::vector<Dart> m_tableVertDarts;

	/**
	* numbers that defined the subdivision of  Polyhedron
	*/
	int m_nx;

	int m_ny;

	int m_nz;

	bool m_top_closed;

	bool m_bottom_closed;

	typename PFP::TVEC3& m_positions;

	typename PFP::VEC3 m_center;

	/**
	* return the dart of next vertex when traversing the boundary of a quad or trifan grid
	*/
	Dart nextDV(Dart d) { return m_map.phi1(m_map.phi2(m_map.phi1(d))); }

	/**
	* return the dart of preceding vertex when traversing the boundary of a quad or trifan grid
	*/
	Dart precDV(Dart d) { return m_map.phi_1(m_map.phi2(m_map.phi_1(d))); }

	void computeCenter();


public:
	/**
	* Constructor
	* @param map the map in which we want to work
	* @param idPositions id of attribute position
	*/
	Polyhedron(MAP& map, typename PFP::TVEC3& position):
	 m_map(map),
	 m_kind(NONE),
	 m_nx(-1), m_ny(-1), m_nz(-1),
	 m_top_closed(false), m_bottom_closed(false),
	 m_positions(position)
	 {
		computeCenter();
	 }

	/**
	* Polyhedron fusion: give a COMPOSED type
	* @param p1 first Polyhedron
	* @param p1 second Polyhedron
	*/
	Polyhedron(const Polyhedron<PFP>& p1, const Polyhedron<PFP>& p2);

	/**
	 * create simple simple polyhedron (not handled by Polyhedron object)
	 */
	static Dart createPolyhedron(typename PFP::MAP& the_map, int nbFaces);

	/**
	 * create simple simple tetrahedron (not handled by Polyhedron object)
	 */
	static Dart createTetra(typename PFP::MAP& the_map);

	/**
	 * create simple simple pyramid (not handled by Polyhedron object)
	 */
	static Dart createPyra(typename PFP::MAP& the_map);

	/**
	 * create simple simple hexaedron (not handled by Polyhedron object)
	 */
	static Dart createHexa(typename PFP::MAP& the_map);

	/**
	 * create simple simple prism (not handled by Polyhedron object)
	 */
	static Dart createPrism(typename PFP::MAP& the_map);

	/*
	* get the reference dart
	*/
	Dart getDart() { return m_dart;}

	/*
	* get the center of Polyhedron
	*/
	const typename PFP::VEC3&  getCenter() { return m_center;}

	/**
	* get the table of darts (one per vertex)
	*/
	std::vector<Dart>& getVertexDarts() { return m_tableVertDarts;}

	/**
	* Create a 2D grid
	* quads are oriented counter-clockwise and the returned dart
	* is the lower left dart (upper right it is symetric)
	* @param x nb of quads in x
	* @param y nb of quads in y
	* @return the dart
	*/
	Dart grid_topo(unsigned int x, unsigned int y);

	/**
	* Create a subdivided (surface) cylinder
	* @param n nb of quads around circunference
	* @param z nb of quads in height
	* @param top_closed close the top with triangles fan
	* @param bottom_closed close the bottom with triangles fan
	* @return the dart
	*/
	Dart cylinder_topo(int n, int z, bool top_closed, bool bottom_closed);

	/**
	* Create a subdivided (surface) cone (with param 1,3,true create tetrahedron)
	* @param n nb of quads around circunference (must be >=3)
	* @param z nb of quads in height (must be >=1)
	* @param bottom_closed close the bottom with triangles fan
	* @return the dart
	*/
	Dart cone_topo(int n, int z, bool bottom_closed);

	/**
	* Create a subdived (surface) cube
	* quads are oriented counter-clockwise
	* @param x nb of quads in x
	* @param y nb of quads in y
	* @param z nb of quads in z
	* @return the dart
	*/
	Dart cube_topo(int x, int y, int z);

	/**
	* Create a subdivided (surface) cylinder
	* @param m nb of quads around big circunference
	* @param n nb of quads around small circunference
	* @param top_closed close the top with triangles fan
	* @param bottom_closed close the bottom with triangles fan
	* @return the dart
	*/
	Dart tore_topo(int m, int n);

	/**
	* embed the topo grid Polyhedron
	* Grid has size x,y centered on 0
	* @param x
	* @param y
	*/
	void embedGrid(float x, float y, float z=0.0f);

	/**
	* embed the topo cylinder Polyhedron
	* @param bottom_radius
	* @parma top_radius
	* @param height
	*/
	void embedCylinder(float bottom_radius, float top_radius, float height);

	/**
	* embed the topo cylinder Polyhedron
	* @param radius
	* @param height
	*/
	void embedCone(float radius, float height);

	/**
	* embed the topo sphere Polyhedron
	* @param radius
	* @param height
	*/
	void embedSphere(float radius);

	/**
	* embed the topo tore Polyhedron
	* @param big_radius
	* @param small_radius
	*/
	void embedTore(float big_radius, float small_radius);

	/**
	* embed the topo cube Polyhedron
	* @param sx size of cube in X
	* @param sy size of cube in Y
	* @param sz size of cube in Z
	*/
	void embedCube(float sx, float sy, float sz);

	/**
	* embed the a grid into a twister open ribbon
	* with turns=PI it is a Moebius strip, needs only to be closed (if model allow it)
	* @param radius_min
	* @param radius_max
	* @param turns number of turn multiplied by 2*PI
	*/
	void embedTwistedStrip(float radius_min, float radius_max, float turns);

	/**
	* embed a grid into a helicoid
	* @param radius_min
	* @param radius_max
	* @param maxHeight height to reach
	* @param turns number of turn
	*/
	void embedHelicoid( float radius_min,  float radius_max, float maxHeight, float nbTurn, int orient=1);

	/**
	* transform the Polyhedron with transformation matrice
	* @param matrice
	*/
	void transform(const Geom::Matrix44f& matrice);

	/**
	* mark all darts of the Polyhedron
	* @param m the CellMarker(VERTEX) to use
	*/
	void mark(CellMarker& m);

	/**
	* mark all embedded vertices of the Polyhedron
	* @param m the marker to use
	*/
//	void markEmbVertices(Mark m);

	/**
	* test if a vertex is in the Polyhedron
	* @param d a dart of the vertex to test
	*/
	bool containVertex(Dart d);
};

} // namespace Modelisation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Modelisation/polyhedron.hpp"

#endif
