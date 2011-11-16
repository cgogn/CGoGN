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

#include "Topology/map/map3.h"

namespace CGoGN
{

/*! @name Generator and Deletor
 *  To generate or delete volumes in a 3-map
 *************************************************************************/

void Map3::deleteVolume(Dart d)
{
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(512);
	visitedFaces.push_back(d);			// Start with the face of d

	mark.markOrbit(FACE, d) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		unsewVolumes(e);

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != *face) ;
	}


	// delete every visited face
	for (std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
		Map2::deleteOrientedFace(*face);
}

/*! @name Topological Operators
 *  Topological operations on 3-maps
 *************************************************************************/

bool Map3::deleteVertex(Dart d)
{
	//Save the darts around the vertex
	//(one dart per face should be enough)
	std::vector<Dart> fstoretmp;
	fstoretmp.reserve(100);
	FunctorStore fs(fstoretmp);
	foreach_dart_of_vertex(d, fs);

	//just one dart per face
	std::vector<Dart> fstore;
	fstore.reserve(100);
	DartMarker mf(*this);
	for(std::vector<Dart>::iterator it = fstoretmp.begin() ; it != fstoretmp.end() ; ++it)
	{
		if(!mf.isMarked(*it))
		{
			mf.markOrbit(FACE, *it);
			fstore.push_back(*it);
		}
	}

	for(std::vector<Dart>::iterator it = fstore.begin() ; it != fstore.end() ; ++it)
	{
		if(!mergeVolumes(*it))
			return false;
	}

	return true;
}

void Map3::cutEdge(Dart d)
{
	if(isBoundaryFace(d))
		d = phi2(d);

	Dart prev = d;
	Dart dd = alpha2(d);
	Map2::cutEdge(d);

	while (dd!=d)
	{
		prev = dd;
		dd = alpha2(dd);

		Map2::cutEdge(prev);

		if (phi3(prev) != prev)
		{
			Dart d3 = phi3(prev);
			phi3unsew(prev);
			phi3sew(prev, phi1(d3));
			phi3sew(d3, phi1(prev));
		}
	}

	if(!isBoundaryFace(d))
	{
		Dart d3 = phi3(d);
		phi3unsew(d);
		phi3sew(d, phi1(d3));
		phi3sew(d3, phi1(d));
	}
}

void Map3::uncutEdge(Dart d)
{
	if(isBoundaryFace(d))
		d = phi_1(phi2(d));

	Dart prev = d;
	Dart dd = alpha2(d);

	Map2::uncutEdge(prev);

	if(phi3(dd) != dd)
		phi3sew(dd,phi2(prev));

	while (dd!=d)
	{
		prev = dd;
		dd = alpha2(dd);

		Map2::uncutEdge(prev);

		phi3sew(dd, phi2(prev));
	}

}

void Map3::splitFace(Dart d, Dart e)
{
	Map2::splitFace(d,e);

	if(!isBoundaryFace(d))
	{
		Dart dd = phi1(phi3(d));
		Dart ee = phi1(phi3(e));

		Map2::splitFace(dd,ee);

		phi3sew(phi_1(d), phi_1(ee));
		phi3sew(phi_1(e), phi_1(dd));
	}
}

void Map3::sewVolumes(Dart d, Dart e)
{
	assert(faceDegree(d) == faceDegree(e));

	Dart fitD = d ;
	Dart fitE = e ;
	do
	{
		phi3sew(fitD,fitE);
		fitD = phi1(fitD) ;
		fitE = phi_1(fitE) ;
	} while(fitD != d) ;
}

void Map3::unsewVolumes(Dart d)
{
	Dart fitD = d;
	do
	{
		phi3unsew(fitD);
		fitD = phi1(fitD);
	} while(fitD != d);
}

bool Map3::mergeVolumes(Dart d)
{
	if(!isBoundaryFace(d))
	{
		Dart e = phi3(d);
		Map3::unsewVolumes(d);
		Map2::mergeVolumes(d, e); // merge the two volumes along common face
		return true ;
	}
	return false ;
}

void Map3::splitVolume(std::vector<Dart>& vd)
{
	Dart e = vd.front();
	Dart e2 = phi2(e);

	//unsew the edge path
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
		Map2::unsewFaces(*it);

	//close the two holes
	Map2::closeHole(e);
	Map2::closeHole(e2);

	//sew the two connexe componnents
	Map3::sewVolumes(phi2(e), phi2(e2));
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool Map3::sameOrientedVertex(Dart d, Dart e)
{
	return sameVertex(d,e);
}

bool Map3::sameVertex(Dart d, Dart e)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	mv.mark(d);

	for(std::list<Dart>::iterator darts = darts_list.begin();darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		if(dc==e)
			return true;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		if(d2 != dc)
		{
			Dart d21 = phi1(d2); // turn in volume
			Dart d23 = phi3(d2); // change volume

			if(!mv.isMarked(d21))
			{
				darts_list.push_back(d21);
				mv.mark(d21);
			}

			if((d23!=d2) && !mv.isMarked(d23))
			{
				darts_list.push_back(d23);
				mv.mark(d23);
			}
		}
	}

	return false;
}

unsigned int Map3::vertexDegree(Dart d)
{
	unsigned int count = 0;
	DartMarkerStore mv(*this);	// Lock a marker

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts_list.push_back(d21);
			mv.mark(d21);
		}

		if((d23!=d2) && !mv.isMarked(d23))
		{
			darts_list.push_back(d23);
			mv.mark(d23);
		}
	}

	DartMarkerStore me(*this);

	for(darts = darts_list.begin(); darts != darts_list.end() ; ++darts)
	{
		if(!me.isMarked(*darts))
		{
			++count;
			me.markOrbit(EDGE, *darts);
		}
	}

	return count;
}

bool Map3::isBoundaryVertex(Dart d)
{
	DartMarkerStore mv(*this);			// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts_list.push_back(d21);
			mv.mark(d21);
		}

		if((d23!=d2) && !mv.isMarked(d23))
		{
			darts_list.push_back(d23);
			mv.mark(d23);
		}

		found = phi3(dc) == dc;
	}

	return found;
}

bool Map3::sameOrientedEdge(Dart d, Dart e)
{
	return sameEdge(d,e);
}

bool Map3::sameEdge(Dart d, Dart e)
{
	Dart dd = d;
	do
	{
		if(dd==e || phi2(dd)==e)
			return true;

		dd = alpha2(dd);
	} while (dd!=d);

	return false;
}

unsigned int Map3::edgeDegree(Dart d)
{
	unsigned int deg = 0;
	Dart e = d;

	do
	{
		deg++;
		e = alpha2(e);
	} while(e != d);

	return deg;
}

bool Map3::sameFace(Dart d, Dart e)
{
	if(phi3(d) != d)
		if(Map2::sameOrientedFace(phi3(d), e))
			return true;

	return Map2::sameOrientedFace(d,e);
}

bool Map3::isBoundaryFace(Dart d)
{
	return (phi3(d) == d);
}

bool Map3::isBoundaryVolume(Dart d)
{
	bool isBoundary = false;
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces ;
	visitedFaces.reserve(100) ;
	visitedFaces.push_back(d) ;
	mark.markOrbit(FACE, d) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); !isBoundary && face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		//if this dart is not phi3-linked
		if(phi3(e) == e)
			isBoundary = true;

		if(!isBoundary)
		{

			do	// add all face neighbours to the table
			{
				Dart ee = phi2(e) ;
				if(!mark.isMarked(ee)) // not already marked
				{
					visitedFaces.push_back(ee) ;
					mark.markOrbit(FACE, ee) ;
				}
				e = phi1(e) ;
			} while(e != *face) ;
		}
	}

	return isBoundary;
}

bool Map3::check()
{
    std::cout << "Check: topology begin" << std::endl;
    DartMarker m(*this);
    for(Dart d = Map3::begin(); d != Map3::end(); Map3::next(d))
    {
        Dart d3 = phi3(d);
        if (phi3(d3) != d) // phi3 involution ?
		{
            std::cout << "Check: phi3 is not an involution" << std::endl;
            return false;
        }

        if(d3 != d)
        {
        	if(phi1(d3) != phi3(phi_1(d)))
        	{
        		std::cout << "Check: phi3 , faces are not entirely sewn" << std::endl;
        		return false;
        	}
        }

        Dart d2 = phi2(d);
        if (phi2(d2) != d) // phi2 involution ?
		{
            std::cout << "Check: phi2 is not an involution" << std::endl;
            return false;
        }

        Dart d1 = phi1(d);
        if (phi_1(d1) != d) // phi1 a une image correcte ?
		{
            std::cout << "Check: unconsistent phi_1 link" << std::endl;
            return false;
        }

        if (m.isMarked(d1)) // phi1 a un seul antécédent ?
		{
            std::cout << "Check: dart with two phi1 predecessors" << std::endl;
            return false;
        }
        m.mark(d1);

        if (d1 == d)
            std::cout << "Check: (warning) face loop (one edge)" << std::endl;

        if (phi1(d1) == d)
            std::cout << "Check: (warning) face with only two edges" << std::endl;

        if (phi2(d1) == d)
            std::cout << "Check: (warning) dandling edge (phi2)" << std::endl;

        if (phi3(d1) == d)
            std::cout << "Check: (warning) dandling edge (phi3)" << std::endl;
    }

    for(Dart d = this->begin(); d != this->end(); this->next(d))
    {
        if (!m.isMarked(d)) // phi1 a au moins un antécédent ?
		{
        	//std::cout << "dart = " << d << std::endl;
            std::cout << "Check: dart with no phi1 predecessor" << std::endl;
            return false;
        }
    }

    std::cout << "Check: topology ok" << std::endl;


    std::cout << "nb vertex orbits" << getNbOrbits(VERTEX) << std::endl ;
    std::cout << "nb vertex cells" << m_attribs[VERTEX].size() << std::endl ;

    std::cout << "nb edge orbits" << getNbOrbits(EDGE) << std::endl ;
    std::cout << "nb edge cells" << m_attribs[EDGE].size() << std::endl ;

    std::cout << "nb face orbits" << getNbOrbits(FACE) << std::endl ;
    std::cout << "nb face cells" << m_attribs[FACE].size() << std::endl ;

    std::cout << "nb volume orbits" << getNbOrbits(VOLUME) << std::endl ;
    std::cout << "nb volume cells" << m_attribs[VOLUME].size() << std::endl ;



    return true;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool Map3::foreach_dart_of_oriented_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> darts_list;			//Darts that are traversed
	darts_list.reserve(512);
	darts_list.push_back(d);			//Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(dc);
		if(d2 != dc)
		{
			Dart d21 = phi1(d2); // turn in volume
			Dart d23 = phi3(d2); // change volume

			if(!mv.isMarked(d21))
			{
				darts_list.push_back(d21);
				mv.mark(d21);
			}

			if((d23!=d2) && !mv.isMarked(d23))
			{
				darts_list.push_back(d23);
				mv.mark(d23);
			}
		}

		found = f(dc);
	}

	return found;
}

bool Map3::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	Dart dNext = d;
	do {
		if (Map2::foreach_dart_of_edge(dNext,f,thread)) return true;
		dNext = alpha2(dNext);
	} while (dNext != d);
	return false;
}

bool Map3::foreach_dart_of_open_edge(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart d1 = *darts;

		Dart d3 = phi2(d1); // change face
		Dart d4 = phi3(d1); // change volume

		if (!mv.isMarked(d3))
		{
			darts_list.push_back(d3);
			mv.mark(d3);
		}
		if (!mv.isMarked(d4))
		{
			darts_list.push_back(d4);
			mv.mark(d4);
		}

		found = f(d1);
	}


	return found;
}

bool Map3::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread)
{
	if (foreach_dart_of_oriented_face(d,f,thread)) return true;

	Dart d3 = phi3(d);
	if (d3 != d) return foreach_dart_of_oriented_face(d3,f,thread);
	return false;
}

bool Map3::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);			// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);


	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart d1 = *darts;

		// add all successors if they are not marked yet
		Dart d2 = phi1(d1); // turn in face
		Dart d3 = phi2(d1); // change volume
		Dart d4 = phi3(d1); // change volume

		if (!mv.isMarked(d2))
		{
			darts_list.push_back(d2);
			mv.mark(d2);
		}
		if (!mv.isMarked(d3))
		{
			darts_list.push_back(d2);
			mv.mark(d2);
		}
		if (!mv.isMarked(d4))
		{
			darts_list.push_back(d4);
			mv.mark(d4);
		}

		found = f(d1);	// functor say finish
	}


	return found;
}


} // namespace CGoGN
