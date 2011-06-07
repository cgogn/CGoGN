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

#include "Topology/gmap/gmap1.h"

namespace CGoGN
{

/*! @name Constructors and Destructors
 *  To generate or delete faces in a 1-G-map
 *************************************************************************/

Dart GMap1::newFace(unsigned int n)
{
	// create the first edge
	Dart d0 =  GMap0::newEdge();
	// store an extremity
	Dart dp = beta0(d0);
	for (unsigned int i = 1; i < n; ++i)
	{
		Dart di = GMap0::newEdge();// create the next edge
		beta1sew(dp,di);
		dp = beta0(di);// change the preceeding
	}
	beta1sew(dp,d0);// sew the last with the first
	return d0;
}

void GMap1::deleteFace(Dart d)
{
	Dart e = phi1(d) ;
	while (e != d)
	{
		Dart f = phi1(e) ;
		deleteDart(beta0(e)) ;
		deleteDart(e) ;
		e = f ;
	}
	deleteDart(beta0(d)) ;
	deleteDart(d) ;
}

void GMap1::deleteEdge(Dart d)
{
	Dart e = beta0(d) ;
	beta1unsew(d) ;
	beta1unsew(e) ;
	GMap0::deleteEdge(d) ;
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool GMap1::sameOrientedFace(Dart d, Dart e)
{
	Dart dNext = d ;
	do
	{
		if (dNext == e)
			return true ;
		dNext = phi1(dNext) ;
	} while (dNext != d) ;
	return false ;
}

bool GMap1::sameFace(Dart d, Dart e)
{
	return sameOrientedFace(d, e) || sameOrientedFace(beta0(d), e) ;
}

unsigned int GMap1::faceDegree(Dart d)
{
	unsigned int count = 0 ;
	Dart dNext = d ;
	do
	{
		++count ;
		dNext = phi1(dNext) ;
	} while (dNext != d) ;
	return count ;
}

bool GMap1::isFaceTriangle(Dart d)
{
	return (phi1(d) != d) && (phi1(phi1(phi1(d))) == d) ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool GMap1::foreach_dart_of_oriented_face(Dart d, FunctorType& f, unsigned int thread)
{
	Dart dNext = d ;
	do
	{
		if (f(dNext))
			return true ;
		dNext = phi1(dNext) ;
	} while (dNext != d) ;
	return false ;
}

bool GMap1::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread)
{
	return foreach_dart_of_oriented_face(d, f, thread) || foreach_dart_of_oriented_face(beta0(d), f, thread) ;
}

bool GMap1::foreach_dart_of_cc(Dart d, FunctorType& f, unsigned int thread)
{
	// first go first through beta0
	Dart d1 = d;
	Dart d2;
	do
	{
		if (f(d1)) return true;
		d2 = beta0(d1);
		if (f(d2)) return true;
		d1 = beta1(d2);
	} while ((d1 != d2) && (d1 != d));

	if (d1 == d) return false;

	// if not closed go now through beta1
	d1 = d;
	d2 = beta1(d1);
	while (d2 != d1)  // only need to test fixed point
	{
		if (f(d2)) return true;
		d1 = beta0(d2);
		if (f(d1)) return true;
		d1 = beta1(d2);
	}
	return false;
}

} // namespace CGoGN

