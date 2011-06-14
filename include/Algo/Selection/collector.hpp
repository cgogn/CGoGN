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
Collector<PFP>::Collector(typename PFP::MAP& mymap, const typename PFP::TVEC3& myposition):
	map(mymap),
	position(myposition)
{}

template <typename PFP>
bool Collector<PFP>::apply(FunctorType& f)
{
	for(std::vector<Dart>::iterator iv = insideVertices.begin(); iv != insideVertices.end(); ++iv)
		if(f(*iv))
			return true ;
	return false ;
}

template <typename PPFP>
std::ostream& operator<<(std::ostream &out, const Collector<PPFP>& c)
{
    out << "Collector around " << c.centerDart << std::endl;
    out << "insideVertices (" << c.insideVertices.size() << ") = {";
    for (unsigned int i=0; i< c.insideVertices.size(); ++i) out << c.insideVertices[i] << " ";
    out << "}" << std::endl ;
    out << "insideEdges (" << c.insideEdges.size() << ") = {";
    for (unsigned int i=0; i< c.insideEdges.size(); ++i) out << c.insideEdges[i] << " ";
    out << "}" << std::endl ;
    out << "insideFaces (" << c.insideFaces.size() << ") = {";
    for (unsigned int i=0; i< c.insideFaces.size(); ++i) out << c.insideFaces[i] << " ";
    out << "}" << std::endl ;
    out << "border (" << c.border.size() << ") = {";
    for (unsigned int i=0; i< c.border.size(); ++i) out << c.border[i] << " ";
    out << "}" << std::endl;
    return out;
}

/*********************************************************
 * Collector One Ring
 *********************************************************/

template <typename PFP>
void Collector_OneRing<PFP>::init(Dart d, typename PFP::REAL r)
{
	this->centerDart = d;
	this->radius = r;
	this->insideVertices.clear();
	this->insideEdges.clear();
	this->insideFaces.clear();
	this->border.clear();
}

template <typename PFP>
void Collector_OneRing<PFP>::collect()
{
	const Dart end = this->centerDart;
	this->insideVertices.push_back(end);
	Dart dd = end;
	do
	{
		this->insideEdges.push_back(dd);
		this->insideFaces.push_back(dd);
		this->border.push_back(this->map.phi1(dd));
		dd = this->map.alpha1(dd);
	} while(dd != end);
}

/*********************************************************
 * Collector Within Sphere
 *********************************************************/

template <typename PFP>
void Collector_WithinSphere<PFP>::init(Dart d, typename PFP::REAL r)
{
	this->centerDart = d;
	this->radius = r;
	this->insideVertices.clear();
	this->insideEdges.clear();
	this->insideFaces.clear();
	this->border.clear();
	centerPosition = this->position[d];
	area = 0;
}

template <typename PFP>
void Collector_WithinSphere<PFP>::collect()
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	CellMarkerStore vm(this->map,VERTEX); // mark the collected inside-vertices
	CellMarkerStore em(this->map,EDGE); // mark the collected inside-edges + border-edges
	CellMarkerStore fm(this->map,FACE); // mark the collected inside-faces + border-faces

	this->insideVertices.push_back(this->centerDart);
	vm.mark(this->centerDart);

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
			e = this->map.alpha1(e);
		} while (e != end);
		++i;
	}
}

template <typename PFP>
void Collector_WithinSphere<PFP>::computeArea()
{
	for (std::vector<Dart>::const_iterator it = this->insideFaces.begin(); it != this->insideFaces.end(); ++it)
	{
		area += Algo::Geometry::triangleArea<PFP>(this->map, *it, this->position);
	}

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
