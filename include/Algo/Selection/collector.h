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
	const typename PFP::TVEC3& position;

	Dart centerDart;
	REAL radius;

	std::vector<Dart> insideVertices;
	std::vector<Dart> insideEdges;
	std::vector<Dart> insideFaces;
	std::vector<Dart> border;

public:
	Collector(typename PFP::MAP& mymap, const typename PFP::TVEC3& myposition);

	virtual void init(Dart d, REAL r = 0) = 0;

	virtual void collect() = 0;

	bool apply(FunctorType& f);

	void sort()
	{
		std::sort(insideVertices.begin(), insideVertices.end());
		std::sort(insideEdges.begin(), insideEdges.end());
		std::sort(insideFaces.begin(), insideFaces.end());
		std::sort(border.begin(), border.end());
	}

	typename PFP::MAP& getMap() { return map; }
	const AttributeHandler<typename PFP::VEC3>& getPosition() const { return position; }

	Dart getCenter() const { return centerDart; }
	REAL getRadius() const { return radius; }

	const std::vector<Dart>& getInsideVertices() const { return insideVertices; }
	const std::vector<Dart>& getInsideEdges() const { return insideEdges; }
	const std::vector<Dart>& getInsideFaces() const { return insideFaces; }
	const std::vector<Dart>& getBorder() const { return border; }

	unsigned int getNbInsideVertices() const { return insideVertices.size(); }
	unsigned int getNbInsideEdges() const { return insideEdges.size(); }
	unsigned int getNbInsideFaces() const { return insideFaces.size(); }
	unsigned int getNbBorder() const { return border.size(); }

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
 * border = vertices of 1-ring
 *        = edges of 1-ring
 */
template <typename PFP>
class Collector_OneRing : public Collector<PFP>
{
public:
	Collector_OneRing(typename PFP::MAP& mymap, const typename PFP::TVEC3& myposition) :
		Collector<PFP>(mymap, myposition)
	{}
	void init(Dart d, typename PFP::REAL r = 0);
	void collect();
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
	typename PFP::REAL radius_2;
	typename PFP::VEC3 centerPosition;
	typename PFP::REAL area;

public:
	Collector_WithinSphere(typename PFP::MAP& mymap, const typename PFP::TVEC3& myposition) :
		Collector<PFP>(mymap,myposition)
	{}
	void init(Dart d, typename PFP::REAL r = 0);
	void collect();

	bool isInside(Dart d)
	{
		return (this->position[d] - centerPosition).norm2() < radius_2;
	}
	// alpha = coef d'interpolation dans [0 ,1] tel que v= (1-alpha)*pin + alpha*pout
	// est le point d'intersection entre la sphère et le segment [pin, pout]
	// avec pin = position[din] à l'intérieur de la sphère
	// avec pout = position[dout] à l'extérieur de la sphère
	typename PFP::REAL intersect_SphereEdge(const Dart din, const Dart dout);
	void computeArea();
	typename PFP::REAL getArea() const { return area; }
};

} // namespace Selection

} // namespace Algo

} // namespace CGoGN

#include "Algo/Selection/collector.hpp"

#endif
