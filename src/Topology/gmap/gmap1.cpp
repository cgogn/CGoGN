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

Dart GMap1::newFace(unsigned int nbEdges)
{
	Dart d0 =  GMap0::newEdge();	// create the first edge
	Dart dp = beta0(d0);			// store an extremity
	for (unsigned int i = 1; i < nbEdges; ++i)
	{
		Dart di = GMap0::newEdge();	// create the next edge
		beta1sew(dp,di);
		dp = beta0(di);	// change the preceding
	}
	beta1sew(dp,d0);	// sew the last with the first
	return d0;
}

Dart GMap1::newBoundaryFace(unsigned int nbEdges)
{
	Dart d0 =  GMap0::newEdge();	// create the first edge
	boundaryMark(d0);
	boundaryMark(beta0(d0));
	Dart dp = beta0(d0);			// store an extremity
	for (unsigned int i = 1; i < nbEdges; ++i)
	{
		Dart di = GMap0::newEdge();	// create the next edge
		boundaryMark(di);
		boundaryMark(beta0(di));
		beta1sew(dp,di);
		dp = beta0(di);	// change the preceding
	}
	beta1sew(dp,d0);	// sew the last with the first
	return d0;
}

void GMap1::deleteFace(Dart d)
{
	Dart e = phi1(d);
	while (e != d)
	{
		Dart f = phi1(e);
		deleteEdge(e);
		e = f;
	}
	deleteEdge(d);
}

} // namespace CGoGN

