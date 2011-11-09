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

void GMap3::deleteOrientedVolume(Dart d)
{
	DartMarkerStore mark(*this);				// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(512);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); !found && face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))		// Face has not been visited yet
		{
			unsewVolumes(*face);

			Dart dNext = *face ;

			do
			{
				mark.mark(dNext);					// Mark
				Dart adj = beta2(dNext);				// Get adjacent face
				if (adj != dNext && !mark.isMarked(adj))
					visitedFaces.push_back(adj);	// Add it

				dNext = beta0(dNext);				// same with beta0, in most case nothing happend
				mark.mark(dNext);					// except marking the dart
				adj = beta2(dNext);
				if (adj != dNext && !mark.isMarked(adj))
					visitedFaces.push_back(adj);

				dNext = beta1(dNext);
			} while(dNext != *face);		// assume here that faces are closed

		}
	}

	// delete every visited face
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
		GMap1::deleteFace(*face);
}





void GMap3::sewVolumes(Dart d, Dart e)
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

void GMap3::unsewVolumes(Dart d)
{
	Dart fitD = d;
	do
	{
		phi3unsew(fitD);
		fitD = phi1(fitD);
	} while(fitD != d);
}

bool GMap3::mergeVolumes(Dart d)
{
	Dart e = phi3(d) ;
	if(e != d)
	{
		unsewVolumes(d);
		GMap2::mergeVolumes(d, e); // merge the two volumes along common face
		return true ;
	}
	return false ;
}

void GMap3::splitFace(Dart d, Dart e)
{
	GMap2::splitFace(d,e);

	if (phi3(d) != d)
	{
		Dart dd = phi1(phi3(d));
		Dart ee = phi1(phi3(e));

		GMap2::splitFace(dd,ee);

		phi3sew(phi_1(d), phi_1(ee));
		phi3sew(phi_1(e), phi_1(dd));
	}
}

void GMap3::cutEdge(Dart d)
{
	if(beta3(d)==d)
		d = beta2(d);

	GMap2::cutEdge(d);		// Cut the edge of d

	Dart ndPrev = phi1(d);
	Dart nE = phi2(d);

	Dart dd = alpha2(d);

	//cut all edges on the orbit : sew them to their predecessors
	while(dd!=d)
	{
		Dart e = dd;

		GMap2::cutEdge(dd);	// Cut the opposite edge
		Dart ndd = phi1(dd);
		ndPrev = beta2(ndPrev);
		if (beta3(dd) != dd)
		{
			beta3sew(ndd,ndPrev);	// Correct the phi3 links
			beta3sew(beta1(ndPrev), beta1(ndd));
		}

		ndPrev = ndd;
		dd = alpha2(dd);
	}

	//sew the first edge with the last one if necessary
	if (beta3(d) != d)
	{
		Dart ne = phi1(d);
		ndPrev = beta2(ndPrev);
		beta3sew(ne,ndPrev);	// Correct the phi3 links
		beta3sew(beta1(ndPrev), beta1(ne));
	}
}

int GMap3::collapseEdge(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{
	Dart e = d;
	int i = 0;

	//stocke un brin par volume autour de l'arete
	std::list<Dart> tmp;
	do
	{
		tmp.push_back(e);
		e = alpha2(e);
		i++;
	} while (e != d);

	for (std::list<Dart>::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		Dart e = phi2(*it);

		cutSpike(e);

		Dart t1=e,t2=e;
		//si les faces opposées ont un tetraedre cousu
		if(phi3(phi2(phi1(e))) != phi2(phi1(e))) {
			t1 = phi3(phi2(phi1(e)));
			unsewVolumes(t1);
		}

		if(phi3(phi2(phi_1(e))) != phi2(phi_1(e))) {
			t2 = phi3(phi2(phi_1(e)));
			unsewVolumes(t2);
		}

		if(t1 != e && t2 != e) {
			sewVolumes(t1,t2);
		}

		//unsewVolumes(e);
		//unsewVolumes(*it);

		deleteOrientedVolume(*it);
	}

	return i;
}

//TODO
void GMap3::collapseFace(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{
	Dart e = d;
	std::list<Dart> tmp;

	//save a dart from the edge for all neighbors
	do
	{
		//if(phi3(phi2(e)) != phi2(e))
		//		tmp.push_back(phi3(phi2(e)));
		tmp.push_back(phi3(phi2(e)));
		e = phi1(e);
	}while(e != d);

	//del the last one (n-1 edge collapse)
	tmp.pop_back();

	//CGoGNout << "#voisin=" << tmp.size() << CGoGNendl;

	//collapse all the edges in the list
	for(std::list<Dart>::iterator it = tmp.begin() ; it != tmp.end() ; ++it)
	{
		Dart d = *it;
		//CGoGNout << "collapseEdge" << CGoGNendl;
		//collapseEdge(*it, delDegenerateFaces, delDegenerateVolumes);
		//stocke un brin par volume autour de l'arete

		Dart e = d;

		//stocke un brin par volume autour de l'arete
		std::list<Dart> tmpedge;
		do
		{
			tmpedge.push_back(e);
			e = alpha2(e);
		} while (e != d);

		for (std::list<Dart>::iterator it = tmpedge.begin(); it != tmpedge.end(); ++it)
		{
			Dart e = phi2(*it);

			cutSpike(e);

			Dart t1=e,t2=e;
			//si les faces opposées ont un tetraedre cousu
			if(phi3(phi2(phi1(e))) != phi2(phi1(e))) {
				t1 = phi3(phi2(phi1(e)));
				unsewVolumes(t1);
			}

			if(phi3(phi2(phi_1(e))) != phi2(phi_1(e))) {
				t2 = phi3(phi2(phi_1(e)));
				unsewVolumes(t2);
			}

			if(t1 != e && t2 != e) {
				sewVolumes(t1,t2);
			}

			//deleteOrientedVolume(*it);
		}
	}
//	for(std::list<Dart>::iterator it = tmp.begin() ; it != tmp.end() ; ++it)
//	{
//		deleteOrientedVolume(*it);
//	}
}

Dart GMap3::cutSpike(Dart d)
{
  Dart e=d;
  int nb=0;
  Dart dNew;
  int tet=0;

  //count the valence of the vertex
  do {
    nb++;
    e=phi1(phi2(e));
  } while (nb<3 && e!=d);

  if(nb<3)
  {
	CGoGNout << "Warning : cannot cut 2 volumes without creating a degenerated face " << CGoGNendl;
	return d;
  }
  else
  {
	 //triangulate around the vertex
	do {
		if(phi1(phi1(phi1(e)))!=e)
		{
			splitFace(phi_1(e),phi1(e));
			//CGoGNout << "split" << CGoGNendl;
		}
		else
			tet++;

		e=phi1(phi2(e));
	} while (e!=d);

//	CGoGNout << "#tet= " << tet << CGoGNendl;
//	CGoGNout << "#nb= " << nb << CGoGNendl;

	//si toute ces faces ne sont pas triangulaires (on insere une face)
	if(tet != nb) {
		//CGoGNout << "new face" << CGoGNendl;
		dNew=newFace(nb);
		Dart d3 = newFace(nb);
		sewVolumes(dNew,d3);

		//sew a face following the triangles
		Dart dTurn=dNew;
		do {
			Dart d1 = phi1(e);
			Dart dSym = phi2(d1);
			phi2unsew(d1);
			phi2sew(dTurn,d1);
			phi2sew(phi3(dTurn),dSym);
			dTurn = phi1(dTurn);
			e=phi1(phi2(e));
		}while(e!=d);
	}
	else
		dNew = d;
  }

  return dNew;
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool GMap3::sameOrientedVertex(Dart d, Dart e)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	mv.mark(d);

	for(std::list<Dart>::iterator darts = darts_list.begin(); darts != darts_list.end() ; ++darts)
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

bool GMap3::sameVertex(Dart d, Dart e)
{
	DartMarkerStore mv(*this);	// Lock a marker

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		if(dc==e)
			return true;

		Dart dx = beta1(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		dx = beta2(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		dx = beta3(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}
	}

	return false;
}

bool GMap3::sameOrientedEdge(Dart d, Dart e)
{
	Dart dNext = d;
	do
	{
		if(dNext==e)
			return true;
		if(phi2(dNext)==e)
			return true;
		dNext = alpha2(dNext);
	} while (dNext != d);

	return false;
}

bool GMap3::sameEdge(Dart d, Dart e)
{
	Dart dNext = d;
	do
	{
		if(dNext==e || beta0(dNext)==e)
			return true;
		if(beta2(dNext)==e || beta0(beta2(dNext))==e)
			return true;

		dNext = alpha2(dNext);
	} while (dNext != d);

	return false;
}

bool GMap3::sameOrientedFace(Dart d, Dart e)
{
	Dart dd = d;
	do
	{
		if(dd==e || phi3(dd)==e)
			return true;

		dd = phi1(dd);
	} while (dd!=d);

	return false;
}

bool GMap3::sameFace(Dart d, Dart e)
{
	Dart dd = d;
	do
	{
		if(dd==e || beta3(dd)==e)
			return true;

		dd = beta0(dd);
		if(dd==e || beta3(dd)==e)
			return true;

		dd = beta1(dd);
	} while (dd!=d);

	return false;
}

unsigned int GMap3::edgeDegree(Dart d)
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
    return true;
}

bool GMap3::isBoundaryVertex(Dart d)
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

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool GMap3::foreach_dart_of_vertex(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		Dart dx = beta1(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		dx = beta2(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		dx = beta3(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		found = f(dc);
	}

	return found;
}

bool GMap3::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	Dart dNext = d;
	do {
		if (GMap2::foreach_dart_of_edge(dNext,f,thread))
			return true;
		dNext = alpha2(dNext);
	} while (dNext != d);
	return false;
}

bool GMap3::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread)
{
	return GMap1::foreach_dart_of_face(d, f, thread) || (beta3(d)==d) || GMap1::foreach_dart_of_face(beta3(d), f, thread) ;
}

bool GMap3::foreach_dart_of_volume(Dart d, FunctorType& f, unsigned int thread)
{
	return GMap2::foreach_dart_of_cc(d, f, thread);
}


bool GMap3::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);	// Lock a marker
	bool found = false;					// Last functor return value

	std::list<Dart> darts_list;			//Darts that are traversed
	darts_list.push_back(d);			//Start with the dart d
	std::list<Dart>::iterator darts;

	mv.mark(d);

	for(darts = darts_list.begin(); !found && darts != darts_list.end() ; ++darts)
	{
		Dart dc = *darts;

		Dart dx = beta0(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		dx = beta1(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		dx = beta2(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		dx = beta3(dc);
		if (!mv.isMarked(dx))
		{
			mv.mark(dx);
			darts_list.push_back(dx);
		}

		found =  f(dc);
	}

	return found;
}









} // end namespace
