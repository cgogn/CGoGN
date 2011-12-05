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

#include "Topology/gmap/gmap3.h"
#include "Topology/generic/dartmarker.h"

namespace CGoGN
{

/*! @name Generator and Deletor
 *  To generate or delete volumes in a 3-G-map
 *************************************************************************/

void GMap3::deleteVolume(Dart d)
{
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(512);
	visitedFaces.push_back(d);			// Start with the face of d

	mark.markOrbit(FACE, d) ;

	// For every face added to the list
	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		if(!isBoundaryFace(e))
			unsewVolumes(e) ;

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

	Dart dd = phi3(d) ;
	GMap2::deleteCC(d) ;
	GMap2::deleteCC(dd) ;
}

void GMap3::fillHole(Dart d)
{
	assert(isBoundaryFace(d)) ;
	Dart dd = d ;
	if(!isBoundaryMarked(dd))
		dd = phi3(dd) ;
	boundaryUnmarkOrbit(VOLUME, dd) ;
}

/*! @name Topological Operators
 *  Topological operations on 3-G-maps
 *************************************************************************/

Dart GMap3::deleteVertex(Dart d)
{
//	if(isBoundaryVertex(d))
//		return NIL ;
//
//	// Save the darts around the vertex
//	// (one dart per face should be enough)
//	std::vector<Dart> fstoretmp;
//	fstoretmp.reserve(128);
//	FunctorStore fs(fstoretmp);
//	foreach_dart_of_vertex(d, fs);
//
//	// just one dart per face
//	std::vector<Dart> fstore;
//	fstore.reserve(128);
//	DartMarker mf(*this);
//	for(std::vector<Dart>::iterator it = fstoretmp.begin() ; it != fstoretmp.end() ; ++it)
//	{
//		if(!mf.isMarked(*it))
//		{
//			mf.markOrbit(FACE, *it);
//			fstore.push_back(*it);
//		}
//	}
//
//	Dart res = NIL ;
//	for(std::vector<Dart>::iterator it = fstore.begin() ; it != fstore.end() ; ++it)
//	{
//		Dart fit = *it ;
//		Dart end = phi_1(fit) ;
//		fit = phi1(fit) ;
//		while(fit != end)
//		{
//			Dart d2 = phi2(fit) ;
//			Dart d3 = phi3(fit) ;
//			Dart d32 = phi2(d3) ;
//			if(res == NIL)
//				res = d2 ;
//			phi2unsew(d2) ;
//			phi2unsew(d32) ;
//			phi2sew(d2, d32) ;
//			phi2sew(fit, d3) ;
//		}
//	}
//	Map2::deleteCC(d) ;
//
//	return res ;
	return NIL ;
}

void GMap3::cutEdge(Dart d)
{
	Dart prev = d ;
	Dart dd = alpha2(d) ;
	GMap2::cutEdge(d) ;

	while(dd != d)
	{
		prev = dd ;
		dd = alpha2(dd) ;

		GMap2::cutEdge(prev) ;

		Dart d3 = beta3(prev);
		beta3sew(beta0(prev), beta0(d3));
		beta3sew(phi1(prev), phi1(d3));

	}

	Dart d3 = beta3(d);
	beta3sew(beta0(d), beta0(d3));
	beta3sew(phi1(prev), phi1(d3));
}

bool GMap3::uncutEdge(Dart d)
{
//	if(vertexDegree(phi1(d)) == 2)
//	{
//		Dart prev = d ;
//		phi3unsew(phi1(prev)) ;
//
//		Dart dd = d;
//		do
//		{
//			prev = dd;
//			dd = alpha2(dd);
//
//			phi3unsew(phi2(prev)) ;
//			phi3unsew(phi2(phi1(prev))) ;
//			Map2::uncutEdge(prev);
//			phi3sew(dd, phi2(prev));
//		} while (dd != d) ;
//
//		return true;
//	}
	return false;
}

void GMap3::splitFace(Dart d, Dart e)
{
	assert(d != e && sameOrientedFace(d, e)) ;

	Dart dd = phi1(phi3(d));
	Dart ee = phi1(phi3(e));

	GMap2::splitFace(d, e);
	GMap2::splitFace(dd, ee);

	phi3sew(phi_1(d), phi_1(ee));
	phi3sew(phi_1(e), phi_1(dd));
}

void GMap3::sewVolumes(Dart d, Dart e, bool withBoundary)
{
	assert(faceDegree(d) == faceDegree(e));

	// if sewing with fixed points
	if (!withBoundary)
	{
		assert(phi3(d) == d && phi3(e) == e) ;
		Dart fitD = d ;
		Dart fitE = e ;
		do
		{
			phi3sew(fitD, fitE) ;
			fitD = phi1(fitD) ;
			fitE = phi_1(fitE) ;
		} while(fitD != d) ;
		return ;
	}

	Dart dd = phi3(d) ;
	Dart ee = phi3(e) ;

	Dart fitD = dd ;
	Dart fitE = ee ;
	do
	{
		Dart fitD2 = phi2(fitD) ;
		Dart fitE2 = phi2(fitE) ;
		if(fitD2 != fitE)
		{
			phi2unsew(fitD) ;
			phi2unsew(fitE) ;
			phi2sew(fitD2, fitE2) ;
			phi2sew(fitD, fitE) ;
		}
		phi3unsew(fitD) ;
		phi3unsew(fitE) ;
		fitD = phi1(fitD) ;
		fitE = phi_1(fitE) ;
	} while(fitD != dd) ;
	Map2::deleteCC(dd) ;

	fitD = d ;
	fitE = e ;
	do
	{
		phi3sew(fitD, fitE) ;
		fitD = phi1(fitD) ;
		fitE = phi_1(fitE) ;
	} while(fitD != d) ;
}

void GMap3::unsewVolumes(Dart d)
{
	assert(!isBoundaryFace(d)) ;

	unsigned int nbE = faceDegree(d) ;
	Dart d3 = phi3(d);

	Dart b1 = newBoundaryFace(nbE) ;
	Dart b2 = newBoundaryFace(nbE) ;

	Dart fit1 = d ;
	Dart fit2 = d3 ;
	Dart fitB1 = b1 ;
	Dart fitB2 = b2 ;
	do
	{
		Dart f = findBoundaryFaceOfEdge(fit1) ;
		if(f != NIL)
		{
			Dart f2 = phi2(f) ;
			phi2unsew(f) ;
			phi2sew(fitB1, f) ;
			phi2sew(fitB2, f2) ;
		}
		else
			phi2sew(fitB1, fitB2) ;

		phi3unsew(fit1) ;
		phi3sew(fit1, fitB1) ;
		phi3sew(fit2, fitB2) ;

		fit1 = phi1(fit1) ;
		fit2 = phi_1(fit2) ;
		fitB1 = phi_1(fitB1) ;
		fitB2 = phi1(fitB2) ;
	} while(fitB1 != b1) ;
}

bool GMap3::mergeVolumes(Dart d)
{
	if(!GMap3::isBoundaryFace(d))
	{
		GMap2::mergeVolumes(d, phi3(d)); // merge the two volumes along common face
		return true ;
	}
	return false ;
}

void GMap3::splitVolume(std::vector<Dart>& vd)
{
	assert(checkSimpleOrientedPath(vd)) ;

	Dart e = vd.front();
	Dart e2 = phi2(e);

	//unsew the edge path
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
		GMap2::unsewFaces(*it);

	GMap2::fillHole(e) ;
	GMap2::fillHole(e2) ;

	//sew the two connected components
	GMap3::sewVolumes(phi2(e), phi2(e2), false);
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool GMap3::sameOrientedVertex(Dart d, Dart e)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator it = darts.begin(); it != darts.end() ; ++it)
	{
		if(*it == e)
			return true;

		// add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(*it);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts.push_back(d21);
			mv.mark(d21);
		}
		if(!mv.isMarked(d23))
		{
			darts.push_back(d23);
			mv.mark(d23);
		}
	}
	return false;
}

bool GMap3::sameVertex(Dart d, Dart e)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator it = darts.begin(); it != darts.end() ; ++it)
	{
		if(*it == e)
			return true;

		Dart dx = beta1(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
		dx = beta2(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
		dx = beta3(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
	}
	return false;
}

unsigned int GMap3::vertexDegree(Dart d)
{
	unsigned int count = 0;
	DartMarkerStore mv(*this);	// Lock a marker

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator it = darts.begin(); it != darts.end() ; ++it)
	{
		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(*it);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts.push_back(d21);
			mv.mark(d21);
		}
		if(!mv.isMarked(d23))
		{
			darts.push_back(d23);
			mv.mark(d23);
		}
	}

	DartMarkerStore me(*this);
	for(std::vector<Dart>::iterator it = darts.begin(); it != darts.end() ; ++it)
	{
		if(!me.isMarked(*it))
		{
			++count;
			me.markOrbit(EDGE, *it);
		}
	}

	return count;
}

bool GMap3::isBoundaryVertex(Dart d)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator it = darts.begin(); it != darts.end() ; ++it)
	{
		if(isBoundaryMarked(*it))
			return true ;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(*it);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts.push_back(d21);
			mv.mark(d21);
		}
		if(!mv.isMarked(d23))
		{
			darts.push_back(d23);
			mv.mark(d23);
		}
	}
	return false ;
}

bool GMap3::sameOrientedEdge(Dart d, Dart e)
{
	Dart it = d;
	do
	{
		if(it == e || phi2(it) == e)
			return true;
		it = alpha2(it);
	} while (it != d);
	return false;
}

bool GMap3::sameEdge(Dart d, Dart e)
{
	Dart it = d;
	do
	{
		if(it == e || beta0(it) == e || beta2(it) == e || phi2(it) == e)
			return true;

		it = alpha2(it);
	} while (it != d);
	return false;
}

unsigned int GMap3::edgeDegree(Dart d)
{
	unsigned int deg = 0;
	Dart it = d;
	do
	{
		++deg;
		it = alpha2(it);
	} while(it != d);
	return deg;
}

bool GMap3::isBoundaryEdge(Dart d)
{
	Dart it = d;
	do
	{
		if(isBoundaryMarked(it))
			return true ;
		it = alpha2(it);
	} while(it != d);
	return false;
}

Dart GMap3::findBoundaryFaceOfEdge(Dart d)
{
	Dart it = d;
	do
	{
		if (isBoundaryMarked(it))
			return it ;
		it = alpha2(it);
	} while(it != d);
	return NIL ;
}

bool GMap3::sameOrientedFace(Dart d, Dart e)
{
	Dart it = d;
	do
	{
		if(it == e || phi3(it) == e)
			return true;
		it = phi1(it);
	} while (it != d);
	return false;
}

bool GMap3::isBoundaryVolume(Dart d)
{
	DartMarkerStore mark(*this);	// Lock a marker

	std::vector<Dart> visitedFaces ;
	visitedFaces.reserve(128) ;
	visitedFaces.push_back(d) ;
	mark.markOrbit(FACE, d) ;

	for(std::vector<Dart>::iterator it = visitedFaces.begin(); it != visitedFaces.end(); ++it)
	{
		if (isBoundaryMarked(beta3(*it)))
			return true ;

		Dart e = *it ;
		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != *it) ;
	}
	return false;
}

bool GMap3::check()
{
    CGoGNout << "Check: topology begin" << CGoGNendl;
    DartMarker m(*this);
    m.unmarkAll();
    for(Dart d = this->begin(); d != this->end(); this->next(d))
    {
        Dart d0 = beta0(d);
        if (beta0(d0) != d) // beta0 involution ?
		{
             CGoGNout << "Check: beta0 is not an involution" << CGoGNendl;
            return false;
        }

        Dart d3 = beta3(d);
        if (beta3(d3) != d) // beta3 involution ?
		{
             CGoGNout << "Check: beta3 is not an involution" << CGoGNendl;
            return false;
        }

        if(d3 != d)
        {
        	if(beta1(d3) != beta3(beta1(d)))
        	{
        		CGoGNout << "Check: beta3 , faces are not entirely sewn" << CGoGNendl;
        		return false;
        	}
        }

        Dart d2 = beta2(d);
        if (beta2(d2) != d) // beta2 involution ?
		{
            CGoGNout << "Check: beta2 is not an involution" << CGoGNendl;
            return false;
        }

        Dart d1 = phi1(d);
        if (phi_1(d1) != d) // phi1 a une image correcte ?
		{
            CGoGNout << "Check: unconsistent phi_1 link" << CGoGNendl;
            return false;
        }

        if (m.isMarked(d1)) // phi1 a un seul antécédent ?
		{
            CGoGNout << "Check: dart with two phi1 predecessors" << CGoGNendl;
            return false;
        }
        m.mark(d1);

        if (d1 == d)
            CGoGNout << "Check: (warning) face loop (one edge)" << CGoGNendl;

        if (phi1(d1) == d)
            CGoGNout << "Check: (warning) face with only two edges" << CGoGNendl;

        if (phi2(d1) == d)
            CGoGNout << "Check: (warning) dandling edge (phi2)" << CGoGNendl;

        if (phi3(d1) == d)
            CGoGNout << "Check: (warning) dandling edge (phi3)" << CGoGNendl;
    }

    for(Dart d = this->begin(); d != this->end(); this->next(d))
    {
        if (!m.isMarked(d)) // phi1 a au moins un antécédent ?
		{
        	std::cout << "dart = " << d << std::endl;
            CGoGNout << "Check: dart with no phi1 predecessor" << CGoGNendl;
            return false;
        }
    }

    CGoGNout << "Check: topology ok" << CGoGNendl;

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

bool GMap3::foreach_dart_of_oriented_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this, thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator it = darts.begin(); !found && it != darts.end() ; ++it)
	{
		// add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(*it);
		Dart d21 = phi1(d2); // turn in volume
		Dart d23 = phi3(d2); // change volume

		if(!mv.isMarked(d21))
		{
			darts.push_back(d21);
			mv.mark(d21);
		}
		if(!mv.isMarked(d23))
		{
			darts.push_back(d23);
			mv.mark(d23);
		}

		found = f(*it);
	}
	return found;
}

bool GMap3::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this, thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator it = darts.begin(); !found && it != darts.end() ; ++it)
	{
		Dart dx = beta1(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
		dx = beta2(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
		dx = beta3(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}

		found = f(*it);
	}
	return found;
}

bool GMap3::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	Dart it = d;
	do
	{
		if (GMap2::foreach_dart_of_edge(it, f, thread))
			return true;
		it = alpha2(it);
	} while (it != d);
	return false;
}

bool GMap3::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(1024);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(std::vector<Dart>::iterator it = darts.begin(); !found && it != darts.end() ; ++it)
	{
		Dart dx = beta0(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
		dx = beta1(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
		dx = beta2(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}
		dx = beta3(*it);
		if (!mv.isMarked(dx))
		{
			darts.push_back(dx);
			mv.mark(dx);
		}

		found =  f(*it);
	}
	return found;
}

/*! @name Close map after import or creation
 *  These functions must be used with care, generally only by import/creation algorithms
 *************************************************************************/

unsigned int GMap3::closeHole(Dart d, bool forboundary)
{
	assert(phi3(d) == d);		// Nothing to close
	DartMarkerStore m(*this) ;

	std::vector<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.reserve(1024) ;
	visitedFaces.push_back(d);		// Start with the face of d
	m.markOrbit(ORIENTED_FACE, d) ;

	unsigned int count = 0 ;

	// For every face added to the list
	for (std::vector<Dart>::iterator it = visitedFaces.begin(); it != visitedFaces.end(); ++it)
	{
		Dart f = *it ;
		unsigned int degree = faceDegree(f) ;
		Dart b = newBoundaryFace(degree) ;
		++count ;

		Dart bit = b ;
		do
		{
			Dart e = alpha2(f) ;
			bool found = false ;
			do
			{
				if(phi3(e) == e)
				{
					found = true ;
					if(!m.isMarked(e))
					{
						visitedFaces.push_back(e) ;
						m.markOrbit(ORIENTED_FACE, e) ;
					}
				}
				else if(isBoundaryMarked(e))
				{
					found = true ;
					phi2sew(e, bit) ;
				}
				else
					e = alpha2(e) ;
			} while(!found) ;

			phi3sew(f, bit) ;
			bit = phi_1(bit) ;
			f = phi1(f);
		} while(f != *it);
	}

	return count ;
}

void GMap3::closeMap()
{
	// Search the map for topological holes (fix points of beta3)
	for (Dart d = begin(); d != end(); next(d))
	{
		if (beta3(d) == d)
			closeHole(d);
	}
}

} // namespace CGoGN
