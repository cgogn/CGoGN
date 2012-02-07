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
#include "Topology/generic/traversor3.h"

namespace CGoGN
{

void Map3::compactTopoRelations(const std::vector<unsigned int>& oldnew)
{
	for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
	{
		{
			Dart& d = m_phi1->operator [](i);
			Dart e = Dart(oldnew[d.index]);
			if (d != e)
				d = e;
		}
		{
			Dart& d = m_phi_1->operator [](i);
			Dart e = Dart(oldnew[d.index]);
			if (d != e)
				d = e;
		}
		{
			Dart& d = m_phi2->operator [](i);
			Dart e = Dart(oldnew[d.index]);
			if (d != e)
				d = e;
		}
		{
			Dart& d = m_phi3->operator [](i);
			Dart e = Dart(oldnew[d.index]);
			if (d != e)
				d = e;
		}
	}
}

/*! @name Generator and Deletor
 *  To generate or delete volumes in a 3-map
 *************************************************************************/

void Map3::deleteVolume(Dart d)
{
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(512);
	visitedFaces.push_back(d);			// Start with the face of d

//	mark.markOrbit(ORIENTED_FACE, d) ;
	mark.markOrbit(FACE2, d) ;


	for(unsigned int i = 0; i < visitedFaces.size(); ++i)
	{
		Dart e = visitedFaces[i] ;

		if(!isBoundaryFace(e))
			unsewVolumes(e) ;

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
//				mark.markOrbit(ORIENTED_FACE, ee) ;
				mark.markOrbit(FACE2, ee) ;
			}
			e = phi1(e) ;
		} while(e != visitedFaces[i]) ;
	}

	Dart dd = phi3(d) ;
	Map2::deleteCC(d) ;
	Map2::deleteCC(dd) ;
}

void Map3::fillHole(Dart d)
{
	assert(isBoundaryFace(d)) ;
	Dart dd = d ;
	if(!isBoundaryMarked(dd))
		dd = phi3(dd) ;
	boundaryUnmarkOrbit(VOLUME, dd) ;
}

/*! @name Topological Operators
 *  Topological operations on 3-maps
 *************************************************************************/

Dart Map3::deleteVertex(Dart d)
{
	if(isBoundaryVertex(d))
		return NIL ;

	// Save the darts around the vertex
	// (one dart per face should be enough)
	std::vector<Dart> fstoretmp;
	fstoretmp.reserve(128);
	FunctorStore fs(fstoretmp);
	foreach_dart_of_vertex(d, fs);

	 // just one dart per face
	std::vector<Dart> fstore;
	fstore.reserve(128);
	DartMarker mf(*this);
	for(unsigned int i = 0; i < fstoretmp.size(); ++i)
	{
		if(!mf.isMarked(fstoretmp[i]))
		{
			mf.markOrbit(FACE, fstoretmp[i]);
			fstore.push_back(fstoretmp[i]);
		}
	}

	Dart res = NIL ;
	for(std::vector<Dart>::iterator it = fstore.begin() ; it != fstore.end() ; ++it)
	{
		Dart fit = *it ;
		Dart end = phi_1(fit) ;
		fit = phi1(fit) ;
		while(fit != end)
		{
			Dart d2 = phi2(fit) ;
			Dart d3 = phi3(fit) ;
			Dart d32 = phi2(d3) ;

			if(res == NIL)
				res = d2 ;

			phi2unsew(d2) ;
			phi2unsew(d32) ;
			phi2sew(d2, d32) ;
			phi2sew(fit, d3) ;

			fit = phi1(fit) ;
		}
	}

	Map2::deleteCC(d) ;

	return res ;
}

Dart Map3::cutEdge(Dart d)
{
	Dart prev = d;
	Dart dd = alpha2(d);
	Dart nd = Map2::cutEdge(d);

	while (dd != d)
	{
		prev = dd;
		dd = alpha2(dd);

		Map2::cutEdge(prev);

		Dart d3 = phi3(prev);
		phi3unsew(prev);
		phi3sew(prev, phi1(d3));
		phi3sew(d3, phi1(prev));
	}

	Dart d3 = phi3(d);
	phi3unsew(d);
	phi3sew(d, phi1(d3));
	phi3sew(d3, phi1(d));

	return nd;
}

bool Map3::uncutEdge(Dart d)
{
	if(vertexDegree(phi1(d)) == 2)
	{
		Dart prev = d ;
		phi3unsew(phi1(prev)) ;

		Dart dd = d;
		do
		{
			prev = dd;
			dd = alpha2(dd);

			phi3unsew(phi2(prev)) ;
			phi3unsew(phi2(phi1(prev))) ;
			Map2::uncutEdge(prev);
			phi3sew(dd, phi2(prev));
		} while (dd != d) ;

		return true;
	}
	return false;
}

Dart Map3::deleteEdge(Dart d)
{
	if(isBoundaryEdge(d))
		return NIL ;

	Dart res = NIL ;
	Dart dit = d ;
	do
	{
		Dart fit = dit ;
		Dart end = fit ;
		fit = phi1(fit) ;
		while(fit != end)
		{
			Dart d2 = phi2(fit) ;
			Dart d3 = phi3(fit) ;
			Dart d32 = phi2(d3) ;

			if(res == NIL)
				res = d2 ;

			phi2unsew(d2) ;
			phi2unsew(d32) ;
			phi2sew(d2, d32) ;
			phi2sew(fit, d3) ;

			fit = phi1(fit) ;
		}
		dit = alpha2(dit) ;
	} while(dit != d) ;

	Map2::deleteCC(d) ;

	return res ;
}

Dart Map3::collapseEdge(Dart d, bool delDegenerateVolumes)
{
	Dart resV = NIL;

	Dart dit = d;

	do
	{
		Dart e = dit;
		dit = alpha2(dit);

		//test si un seul polyedre autour de l'arete
		if(e == dit)
			resV == phi3(phi2(phi1(e)));

		if(delDegenerateVolumes)
		{
			Map2::collapseEdge(e, true);
			collapseDegeneretedVolume(e);
		}
		else
			Map2::collapseEdge(e, false);

		if(resV == NIL)
		{

		}

	}while(d != dit);

	return resV;
}



//	Dart e = d;
//
//	// stocke un brin par volume autour de l'arete
//	std::vector<Dart> tmp;
//	tmp.reserve(32);
//	do
//	{
//		tmp.push_back(e);
//		e = alpha2(e);
//	} while (e != d);
//
//	// contraction de la 2 carte de chaque 2-arete
//	for (std::vector<Dart>::iterator it = tmp.begin(); it != tmp.end(); ++it)
//	{
//		// un brin d'une face adjacente a l'arrete contracte
//		Dart d = phi2(phi_1(*it));
//		Map2::collapseEdge(*it, true);
//
//		// test de la degeneresence
//		// impossible d'avoir un volume de moins de 4 faces sans avoir de phi2 en points fixe donc on les vire
//		if(delDegenerateVolumes && Map2::volumeDegree(d) < 4)
//		{
//			Dart e = d;
//			// pour tous les brins de la face adjacente
//
//			do
//			{
//				Dart ee = phi3(e);
//				Dart ff = phi3(phi2(e));
//
//				// si les brins ont un voisin par phi3
//				if(ee != e)
//
//					phi3unsew(ee);
//				if(ff != phi2(e))
//					phi3unsew(ff);
//
//				// si les deux en ont un, il faut les coudres ensemble
//				if(ee != e && ff != phi2(e))
//					phi3sew(ee, ff);
//
//				// on peut supprimer les brins de cette arete
//				deleteDart(e);
//				deleteDart(phi2(e));
//				e = phi1(e);
//
//			} while (e != d);
//		}
//	}



void Map3::splitFace(Dart d, Dart e)
{
	assert(d != e && Map2::sameOrientedFace(d, e)) ;

	Dart dd = phi1(phi3(d));
	Dart ee = phi1(phi3(e));

	Map2::splitFace(d, e);
	Map2::splitFace(dd, ee);

	phi3sew(phi_1(d), phi_1(ee));
	phi3sew(phi_1(e), phi_1(dd));
}

bool Map3::collapseDegeneretedVolume(Dart d)
{
	Dart e1 = phi2(d);
	Dart e2 = phi2(phi1(d));

	//Si les deux faces ne sont pas du bord
	if(!isBoundaryFace(e1) && !isBoundaryFace(e2))
	{
		sewVolumes(phi3(e1),phi3(e2));
		deleteVolume(d);
		return true;
	}
	else
	{
		//alors simple suppression du volume degenere
		deleteVolume(d);
		return true;
	}

	return false;
}

void Map3::sewVolumes(Dart d, Dart e, bool withBoundary)
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

void Map3::unsewVolumes(Dart d)
{
	assert(!isBoundaryFace(d)) ;

	unsigned int nbE = faceDegree(d) ;
	Dart d3 = phi3(d);

	Dart b1 = newBoundaryCycle(nbE) ;
	Dart b2 = newBoundaryCycle(nbE) ;

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

bool Map3::mergeVolumes(Dart d)
{
	if(!Map3::isBoundaryFace(d))
	{
		Map2::mergeVolumes(d, phi3(d)); // merge the two volumes along common face
		return true ;
	}
	return false ;
}

void Map3::splitVolume(std::vector<Dart>& vd)
{
	//assert(checkSimpleOrientedPath(vd)) ;

	Dart e = vd.front();
	Dart e2 = phi2(e);

	//unsew the edge path
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
		Map2::unsewFaces(*it);

	Map2::fillHole(e) ;

	Dart b = Map1::newBoundaryCycle(vd.size());

	Dart fit1 = phi2(e) ;
	Dart fit2 = b ;
	do
	{
		phi3sew(fit1, fit2) ;

		fit1 = phi1(fit1) ;
		fit2 = phi_1(fit2) ;
	} while(fit2 != b) ;



//	if(isBoundaryMarked(e2))
//		Map2::fillHole(e2) ;

	//sew the two connected components
//	Map3::sewVolumes(phi2(e), phi2(e2), false);
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool Map3::sameVertex(Dart d, Dart e)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(unsigned int i = 0; i < darts.size(); ++i)
	{
		if(darts[i] == e)
			return true;

		// add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(darts[i]);
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

unsigned int Map3::vertexDegree(Dart d)
{
	unsigned int count = 0;
	DartMarkerStore mv(*this);	// Lock a marker

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(unsigned int i = 0; i < darts.size(); ++i)
	{
		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(darts[i]);
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

bool Map3::isBoundaryVertex(Dart d)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(unsigned int i = 0; i < darts.size(); ++i)
	{
		if(isBoundaryMarked(darts[i]))
			return true ;

		//add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(darts[i]);
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

bool Map3::sameOrientedEdge(Dart d, Dart e)
{
	Dart it = d;
	do
	{
		if(it == e)
			return true;
		it = alpha2(it);
	} while (it != d);
	return false;
}

unsigned int Map3::edgeDegree(Dart d)
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

bool Map3::isBoundaryEdge(Dart d)
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

Dart Map3::findBoundaryFaceOfEdge(Dart d)
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

bool Map3::isBoundaryVolume(Dart d)
{
	Traversor3WF<Map3> tra(*this, d);
	for(Dart dit = tra.begin() ; dit != tra.end() ; dit = tra.next())
	{
		if(isBoundaryMarked(phi3(dit)))
			return true ;
	}
	return false;
}

bool Map3::hasBoundaryEdge(Dart d)
{
	Traversor3WE<Map3> tra(*this, d);
	for(Dart dit = tra.begin() ; dit != tra.end() ; dit = tra.next())
	{
		if(isBoundaryEdge(dit))
			return true;
	}

	return false;
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

		if(phi1(d3) != phi3(phi_1(d)))
		{
			std::cout << "Check: phi3 , faces are not entirely sewn" << std::endl;
			return false;
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
            std::cout << "Check: dart with no phi1 predecessor" << std::endl;
            return false;
        }
    }

    std::cout << "Check: topology ok" << std::endl;

    return true;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool Map3::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this, thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(256);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(unsigned int i = 0; !found && i < darts.size(); ++i)
	{
		// add phi21 and phi23 successor if they are not marked yet
		Dart d2 = phi2(darts[i]);
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

		found = f(darts[i]);
	}
	return found;
}

bool Map3::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	Dart it = d;
	do
	{
		if (Map2::foreach_dart_of_edge(it, f, thread))
			return true;
		it = alpha2(it);
	} while (it != d);
	return false;
}

bool Map3::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> darts;	// Darts that are traversed
	darts.reserve(1024);
	darts.push_back(d);			// Start with the dart d
	mv.mark(d);

	for(unsigned int i = 0; !found && i < darts.size(); ++i)
	{
		// add all successors if they are not marked yet
		Dart d2 = phi1(darts[i]); // turn in face
		Dart d3 = phi2(darts[i]); // change face
		Dart d4 = phi3(darts[i]); // change volume

		if (!mv.isMarked(d2))
		{
			darts.push_back(d2);
			mv.mark(d2);
		}
		if (!mv.isMarked(d3))
		{
			darts.push_back(d2);
			mv.mark(d2);
		}
		if (!mv.isMarked(d4))
		{
			darts.push_back(d4);
			mv.mark(d4);
		}

		found = f(darts[i]);
	}
	return found;
}

/*! @name Close map after import or creation
 *  These functions must be used with care, generally only by import/creation algorithms
 *************************************************************************/

unsigned int Map3::closeHole(Dart d, bool forboundary)
{
	assert(phi3(d) == d);		// Nothing to close
	DartMarkerStore m(*this) ;

	std::vector<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.reserve(1024) ;
	visitedFaces.push_back(d);		// Start with the face of d
//	m.markOrbit(ORIENTED_FACE, d) ;
	m.markOrbit(FACE2, d) ;

	unsigned int count = 0 ;

	// For every face added to the list
	for(unsigned int i = 0; i < visitedFaces.size(); ++i)
	{
		Dart it = visitedFaces[i] ;
		Dart f = it ;

		unsigned int degree = faceDegree(f) ;
		Dart b = newBoundaryCycle(degree) ;
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
						m.markOrbit(FACE2, e) ;
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
		} while(f != it) ;
	}

	return count ;
}

void Map3::closeMap()
{
	// Search the map for topological holes (fix points of phi3)
	for (Dart d = begin(); d != end(); next(d))
	{
		if (phi3(d) == d)
			closeHole(d);
	}
}

} // namespace CGoGN
