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

#include "Topology/gmap/gmap3.h"
#include "Topology/generic/dartmarker.h"

namespace CGoGN
{

void GMap3::deleteOrientedVolume(Dart d)
{
	DartMarkerStore mark(*this);				// Lock a marker
	bool found = false;					// Last functor return value

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
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


// TODO:optimize traversal of edges ?
bool GMap3::foreach_dart_of_edge(Dart d, FunctorType& f, unsigned int thread)
{
	DartMarkerStore mv(*this,thread);			// Lock a marker
	bool found = false;					// Last functor return value
	std::list<Dart> darts_list;		//Darts that are traversed
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

bool GMap3::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread)
{
	return GMap1::foreach_dart_of_face(d, f, thread) || GMap1::foreach_dart_of_face(beta3(d), f, thread) ;
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
