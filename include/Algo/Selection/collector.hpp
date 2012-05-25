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

#include "Topology/generic/traversor2.h"
#include "Algo/Geometry/intersection.h"

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
Collector<PFP>::Collector(typename PFP::MAP& m) : map(m)
{}

template <typename PFP>
inline bool Collector<PFP>::applyOnInsideVertices(FunctorType& f)
{
	for(std::vector<Dart>::iterator iv = insideVertices.begin(); iv != insideVertices.end(); ++iv)
		if(f(*iv))
			return true ;
	return false ;
}

template <typename PFP>
inline bool Collector<PFP>::applyOnInsideEdges(FunctorType& f)
{
	for(std::vector<Dart>::iterator iv = insideEdges.begin(); iv != insideEdges.end(); ++iv)
		if(f(*iv))
			return true ;
	return false ;
}

template <typename PFP>
inline bool Collector<PFP>::applyOnInsideFaces(FunctorType& f)
{
	for(std::vector<Dart>::iterator iv = insideFaces.begin(); iv != insideFaces.end(); ++iv)
		if(f(*iv))
			return true ;
	return false ;
}

template <typename PFP>
inline bool Collector<PFP>::applyOnBorder(FunctorType& f)
{
	for(std::vector<Dart>::iterator iv = border.begin(); iv != border.end(); ++iv)
		if(f(*iv))
			return true ;
	return false ;
}

template <typename PPFP>
std::ostream& operator<<(std::ostream &out, const Collector<PPFP>& c)
{
    out << "Collector around " << c.centerDart << std::endl;
    out << "insideVertices (" << c.insideVertices.size() << ") = {";
    for (unsigned int i = 0; i< c.insideVertices.size(); ++i) out << c.insideVertices[i] << " ";
    out << "}" << std::endl ;
    out << "insideEdges (" << c.insideEdges.size() << ") = {";
    for (unsigned int i = 0; i< c.insideEdges.size(); ++i) out << c.insideEdges[i] << " ";
    out << "}" << std::endl ;
    out << "insideFaces (" << c.insideFaces.size() << ") = {";
    for (unsigned int i = 0; i< c.insideFaces.size(); ++i) out << c.insideFaces[i] << " ";
    out << "}" << std::endl ;
    out << "border (" << c.border.size() << ") = {";
    for (unsigned int i = 0; i< c.border.size(); ++i) out << c.border[i] << " ";
    out << "}" << std::endl;
    return out;
}

/*********************************************************
 * Collector One Ring
 *********************************************************/

template <typename PFP>
void Collector_OneRing<PFP>::collectAll(Dart d)
{
	this->init(d);
	this->insideEdges.reserve(16);
	this->insideFaces.reserve(16);
	this->border.reserve(16);

	this->insideVertices.push_back(this->centerDart);

	Traversor2VE<typename PFP::MAP> te(this->map, this->centerDart) ;
	for(Dart it = te.begin(); it != te.end(); it = te.next())
		this->insideEdges.push_back(it);

	Traversor2VF<typename PFP::MAP> tf(this->map, this->centerDart) ;
	for(Dart it = tf.begin(); it != tf.end(); it = tf.next())
	{
		this->insideFaces.push_back(it);
		this->border.push_back(this->map.phi1(it));
	}
}

template <typename PFP>
void Collector_OneRing<PFP>::collectBorder(Dart d)
{
	this->init(d);
	this->border.reserve(12);

	Traversor2VF<typename PFP::MAP> t(this->map, this->centerDart) ;
	for(Dart it = t.begin(); it != t.end(); it = t.next())
		this->border.push_back(this->map.phi1(it));
}

/*********************************************************
 * Collector Within Sphere
 *********************************************************/

template <typename PFP>
void Collector_WithinSphere<PFP>::collectAll(Dart d)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	this->init(d);
	this->insideEdges.reserve(32);
	this->insideFaces.reserve(32);
	this->border.reserve(32);

	CellMarkerStore<VERTEX> vm(this->map);	// mark the collected inside-vertices
	CellMarkerStore<EDGE> em(this->map);	// mark the collected inside-edges + border-edges
	CellMarkerStore<FACE> fm(this->map);	// mark the collected inside-faces + border-faces

	this->insideVertices.push_back(this->centerDart);
	vm.mark(this->centerDart);

	VEC3 centerPosition = this->position[d];
	unsigned int i = 0;
//	for(std::vector<Dart>::iterator iv = this->insideVertices.begin(); iv != this->insideVertices.end(); ++iv)
	while (i < this->insideVertices.size())
	{
		Dart end = this->insideVertices[i];
		Dart e = end;
		do
		{
			if (! em.isMarked(e) || ! fm.isMarked(e)) // are both tests useful ?
			{
				const Dart f = this->map.phi1(e);
				const Dart g = this->map.phi1(f);

				if (! Geom::isPointInSphere(this->position[f], centerPosition, this->radius))
				{
					this->border.push_back(e); // add to border
					em.mark(e);
					fm.mark(e); // is it useful ?
				}
				else
				{
					if (! vm.isMarked(f))
					{
						this->insideVertices.push_back(f);
						vm.mark(f);
					}
					if (! em.isMarked(e))
					{
						this->insideEdges.push_back(e);
						em.mark(e);
					}
					if (! fm.isMarked(e) && Geom::isPointInSphere(this->position[g], centerPosition, this->radius))
					{
						this->insideFaces.push_back(e);
						fm.mark(e);
					}
				}
			}
			e = this->map.phi2_1(e);
		} while (e != end);
		++i;
	}
}

template <typename PFP>
void Collector_WithinSphere<PFP>::collectBorder(Dart d)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	this->init(d);
	this->border.reserve(128);
	this->insideVertices.reserve(128);

	CellMarkerStore<VERTEX> vm(this->map);	// mark the collected inside-vertices
	CellMarkerStore<EDGE> em(this->map);	// mark the collected inside-edges + border-edges

	this->insideVertices.push_back(this->centerDart);
	vm.mark(this->centerDart);

	VEC3 centerPosition = this->position[d];
	unsigned int i = 0;
	while (i < this->insideVertices.size())
	{
		Dart end = this->insideVertices[i];
		Dart e = end;
		do
		{
			if ( ! em.isMarked(e) )
			{
				const Dart f = this->map.phi1(e);

				if (! Geom::isPointInSphere(this->position[f], centerPosition, this->radius))
				{
					this->border.push_back(e); // add to border
				}
				else
				{
					if (! vm.isMarked(f))
					{
						this->insideVertices.push_back(f);
						vm.mark(f);
					}
				}
				em.mark(e);
			}
			e = this->map.phi2_1(e);
		} while (e != end);
		++i;
	}
	this->insideVertices.clear();
}

template <typename PFP>
void Collector_WithinSphere<PFP>::computeArea()
{
	area = 0;
	typename PFP::VEC3 centerPosition = this->position[this->centerDart];

	for (std::vector<Dart>::const_iterator it = this->insideFaces.begin(); it != this->insideFaces.end(); ++it)
		area += Algo::Geometry::triangleArea<PFP>(this->map, *it, this->position);

	for (std::vector<Dart>::const_iterator it = this->border.begin(); it != this->border.end(); ++it)
	{
		const Dart f = this->map.phi1(*it); // we know that f is outside
		const Dart g = this->map.phi1(f);
		if (Geom::isPointInSphere(this->position[g], centerPosition, this->radius))
		{ // only f is outside
			typename PFP::REAL alpha, beta;
			Algo::Geometry::intersectionSphereEdge<PFP>(this->map, centerPosition, this->radius, *it, this->position, alpha);
			Algo::Geometry::intersectionSphereEdge<PFP>(this->map, centerPosition, this->radius, this->map.phi2(f), this->position, beta);
			area += (alpha+beta - alpha*beta) * Algo::Geometry::triangleArea<PFP>(this->map, *it, this->position);
		}
		else
		{ // f and g are outside
			typename PFP::REAL alpha, beta;
			Algo::Geometry::intersectionSphereEdge<PFP>(this->map, centerPosition, this->radius, *it, this->position, alpha);
			Algo::Geometry::intersectionSphereEdge<PFP>(this->map, centerPosition, this->radius, this->map.phi2(g), this->position, beta);
			area += alpha * beta * Algo::Geometry::triangleArea<PFP>(this->map, *it, this->position);
		}
	}
}

} // namespace Selection

} // namespace Algo

} // namespace CGoGN
