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
* Web site: http://cgogn.unistra.fr/                                  *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __COLLECTOR_H__
#define __COLLECTOR_H__

/*****************************************
 * Class hierarchy :
 * Collector (virtual)
 * - Collector_WithinSphere
 * - Collector_OneRing
 ****************************************/

namespace CGoGN
{

namespace Algo
{

namespace Selection
{

/*********************************************************
 * Generic Collector
 *********************************************************/

template <typename PFP>
class Collector
{
protected:
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	typename PFP::MAP& map;

	Dart centerDart;

	std::vector<Dart> insideVertices;
	std::vector<Dart> insideEdges;
	std::vector<Dart> insideFaces;
	std::vector<Dart> border;

public:
	Collector(typename PFP::MAP& m);

	inline void init(Dart d)
	{
		centerDart = d;
		insideVertices.clear();
		insideEdges.clear();
		insideFaces.clear();
		border.clear();
	}

	virtual void collectAll(Dart d) = 0;
	virtual void collectBorder(Dart d) = 0;

	bool applyOnInsideVertices(FunctorType& f);
	bool applyOnInsideEdges(FunctorType& f);
	bool applyOnInsideFaces(FunctorType& f);
	bool applyOnBorder(FunctorType& f);

	inline void sort()
	{
		std::sort(insideVertices.begin(), insideVertices.end());
		std::sort(insideEdges.begin(), insideEdges.end());
		std::sort(insideFaces.begin(), insideFaces.end());
		std::sort(border.begin(), border.end());
	}

	inline typename PFP::MAP& getMap() { return map; }

	inline Dart getCenterDart() const { return centerDart; }

	inline const std::vector<Dart>& getInsideVertices() const { return insideVertices; }
	inline const std::vector<Dart>& getInsideEdges() const { return insideEdges; }
	inline const std::vector<Dart>& getInsideFaces() const { return insideFaces; }
	inline const std::vector<Dart>& getBorder() const { return border; }

	inline unsigned int getNbInsideVertices() const { return insideVertices.size(); }
	inline unsigned int getNbInsideEdges() const { return insideEdges.size(); }
	inline unsigned int getNbInsideFaces() const { return insideFaces.size(); }
	inline unsigned int getNbBorder() const { return border.size(); }

	template <typename PPFP>
	friend std::ostream& operator<<(std::ostream &out, const Collector<PPFP>& c);
};

/*********************************************************
 * Collector One Ring
 *********************************************************/

/*
 * insideVertices = centerDart
 * insideEdges = star (edges incident to centerDart)
 * insideFaces = triangles incident to centerDart
 * border = vertices of 1-ring -> link (set of adjacent vertices)
 *        = edges of 1-ring
 */
template <typename PFP>
class Collector_OneRing : public Collector<PFP>
{
public:
	Collector_OneRing(typename PFP::MAP& m) :
		Collector<PFP>(m)
	{}
	void collectAll(Dart d);
	void collectBorder(Dart d);
};

/*********************************************************
 * Collector Within Sphere
 *********************************************************/

/*
 * collect all primitives of the connected component containing "centerDart"
 * within the sphere of radius "radius" and center "position[centerDart]"
 * (hopefully) it defines a 2-manifold (if inserting border-vertices along the border-edges)
 */
template <typename PFP>
class Collector_WithinSphere : public Collector<PFP>
{
protected:
	const AttributeHandler<typename PFP::VEC3, VERTEX>& position;
	typename PFP::REAL radius;
	typename PFP::REAL area;

public:
	Collector_WithinSphere(typename PFP::MAP& m, const AttributeHandler<typename PFP::VEC3, VERTEX>& p, typename PFP::REAL r = 0) :
		Collector<PFP>(m),
		position(p),
		radius(r),
		area(0)
	{}
	inline void setRadius(typename PFP::REAL r) { radius = r; }
	inline typename PFP::REAL getRadius() const { return radius; }
	inline const AttributeHandler<typename PFP::VEC3, VERTEX>& getPosition() const { return position; }

	void collectAll(Dart d);
	void collectBorder(Dart d);

	void computeArea();
	inline typename PFP::REAL getArea() const { return area; }
};

} // namespace Selection

} // namespace Algo

} // namespace CGoGN

#include "Algo/Selection/collector.hpp"

#endif
