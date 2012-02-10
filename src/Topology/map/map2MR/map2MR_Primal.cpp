/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Topology/map/map2MR/map2MR_Primal.h"

namespace CGoGN
{

Map2MR_Primal::Map2MR_Primal()
{
	initMR() ;
}

/***************************************************
 *               CELLS INFORMATION                 *
 ***************************************************/

unsigned int Map2MR_Primal::vertexInsertionLevel(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"vertexInsertionLevel : called with a dart introduced after current level") ;
	return getDartLevel(d) ;
}

unsigned int Map2MR_Primal::edgeLevel(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"edgeLevel : called with a dart introduced after current level") ;

	unsigned int ld = getDartLevel(d) ;
	unsigned int ldd = getDartLevel(phi2(d)) ;	// the level of an edge is the maximum of the
	return ld > ldd ? ld : ldd ;				// insertion levels of its two darts
}

unsigned int Map2MR_Primal::faceLevel(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceLevel : called with a dart introduced after current level") ;

	if(getCurrentLevel() == 0)
		return 0 ;

	Dart it = d ;
	unsigned int min1 = getDartLevel(it) ;		// the level of a face is the second minimum of the
	it = phi1(it) ;
	unsigned int min2 = getDartLevel(it) ;		// insertion levels of its darts

	if(min2 < min1)
	{
		unsigned int tmp = min1 ;
		min1 = min2 ;
		min2 = tmp ;
	}

	it = phi1(it) ;
	while(it != d)
	{
		unsigned int dl = getDartLevel(it) ;
		if(dl < min2)
		{
			if(dl < min1)
			{
				min2 = min1 ;
				min1 = dl ;
			}
			else
				min2 = dl ;
		}
		it = phi1(it) ;
	}

	return min2 ;
}

Dart Map2MR_Primal::faceOrigin(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceOrigin : called with a dart introduced after current level") ;

	pushLevel() ;
	Dart p = d ;
	unsigned int pLevel = getDartLevel(p) ;
	while(pLevel > 0)
	{
		p = faceOldestDart(p) ;
		pLevel = getDartLevel(p) ;
		setCurrentLevel(pLevel) ;
	}
	popLevel() ;
	return p ;
}

Dart Map2MR_Primal::faceOldestDart(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceOldestDart : called with a dart introduced after current level") ;

	Dart it = d ;
	Dart oldest = it ;
	unsigned int l_old = getDartLevel(oldest) ;
	do
	{
		unsigned int l = getDartLevel(it) ;
		if(l == 0)
			return it ;
		if(l < l_old)
		{
			oldest = it ;
			l_old = l ;
		}
		it = phi1(it) ;
	} while(it != d) ;
	return oldest ;
}

bool Map2MR_Primal::edgeIsSubdivided(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"edgeIsSubdivided : called with a dart introduced after current level") ;

	if(getCurrentLevel() == getMaxLevel())
		return false ;

	Dart d2 = phi2(d) ;
	setCurrentLevel(getCurrentLevel() + 1) ;
	Dart d2_l = phi2(d) ;
	setCurrentLevel(getCurrentLevel() - 1) ;
	if(d2 != d2_l)
		return true ;
	else
		return false ;
}

bool Map2MR_Primal::edgeCanBeCoarsened(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"edgeCanBeCoarsened : called with a dart introduced after current level") ;

	if(edgeIsSubdivided(d))
	{
		bool subdOnce = true ;
		bool degree2 = false ;

		Dart d2 = phi2(d) ;
		setCurrentLevel(getCurrentLevel() + 1) ;
		if(vertexDegree(phi1(d)) == 2)
		{
			degree2 = true ;
			if(edgeIsSubdivided(d) || edgeIsSubdivided(d2))
				subdOnce = false ;
		}
		setCurrentLevel(getCurrentLevel() - 1) ;

		return degree2 && subdOnce ;
	}

	return false ;
}

bool Map2MR_Primal::faceIsSubdivided(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceIsSubdivided : called with a dart introduced after current level") ;

	if(getCurrentLevel() == getMaxLevel())
		return false ;

	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < getCurrentLevel())	// a face whose level in the current level map is lower than
		return false ;				// the current level can not be subdivided to higher levels

	bool subd = false ;
	setCurrentLevel(getCurrentLevel() + 1) ;
	if(getDartLevel(phi1(phi1(d))) == getCurrentLevel())
		subd = true ;
	setCurrentLevel(getCurrentLevel() - 1) ;
	return subd ;
}

bool Map2MR_Primal::faceIsSubdividedOnce(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceIsSubdividedOnce : called with a dart introduced after current level") ;

	if(getCurrentLevel() == getMaxLevel())
		return false ;

	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < getCurrentLevel())	// a face whose level in the current level map is lower than
		return false ;				// the current level can not be subdivided to higher levels

	unsigned int degree = 0 ;
	bool subd = false ;
	bool subdOnce = true ;

	setCurrentLevel(getCurrentLevel() + 1) ;
	if(getDartLevel(phi1(phi1(d))) == getCurrentLevel())
		subd = true ;
	setCurrentLevel(getCurrentLevel() - 1) ;

	if(subd)
	{
		setCurrentLevel(getCurrentLevel() + 1) ;

		if(getCurrentLevel() == getMaxLevel())
		{
			setCurrentLevel(getCurrentLevel() - 1) ;
			return true ;
		}

		Dart fit = d ;
		do
		{
			setCurrentLevel(getCurrentLevel() + 1) ;
			if(getDartLevel(phi1(phi1(fit))) == getCurrentLevel())
				subdOnce = false ;
			setCurrentLevel(getCurrentLevel() - 1) ;
			++degree ;
			fit = phi1(fit) ;
		} while(subdOnce && fit != d) ;

		if(degree == 3 && subdOnce)
		{
			Dart cf = phi2(phi1(d)) ;
			setCurrentLevel(getCurrentLevel() + 1) ;
			if(getDartLevel(phi1(phi1(cf))) == getCurrentLevel())
				subdOnce = false ;
			setCurrentLevel(getCurrentLevel() - 1) ;
		}

		setCurrentLevel(getCurrentLevel() - 1) ;

		return subdOnce ;
	}

	return false ;
}

/***************************************************
 *               SUBDIVISION                       *
 ***************************************************/

void Map2MR_Primal::subdivideEdge(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"subdivideEdge : called with a dart introduced after current level") ;
	assert(!edgeIsSubdivided(d) || !"Trying to subdivide an already subdivided edge") ;

	unsigned int eLevel = edgeLevel(d) ;

	pushLevel() ;

	if(eLevel == getMaxLevel())
		addLevel() ;

	setCurrentLevel(eLevel + 1) ;
	cutEdge(d) ;

	popLevel() ;
}

void Map2MR_Primal::subdivideFace(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"subdivideFace : called with a dart introduced after current level") ;
	assert(!faceIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int fLevel = faceLevel(d) ;
	Dart old = faceOldestDart(d) ;

	pushLevel() ;
	setCurrentLevel(fLevel) ;		// go to the level of the face to subdivide its edges

	unsigned int degree = 0 ;
	Dart it = old ;
	do
	{
		++degree ;
		if(!edgeIsSubdivided(it))	// first cut the edges (if they are not already)
			subdivideEdge(it) ;		// and compute the degree of the face
		it = phi1(it) ;
	} while(it != old) ;

	setCurrentLevel(fLevel + 1) ;	// go to the next level to perform face subdivision

	if(degree == 3)					// if subdividing a triangle
	{
		Dart dd = phi1(old) ;
		Dart e = phi1(phi1(dd)) ;
		splitFace(dd, e) ;

		dd = e ;
		e = phi1(phi1(dd)) ;
		splitFace(dd, e) ;

		dd = e ;
		e = phi1(phi1(dd)) ;
		splitFace(dd, e) ;
	}
	else							// if subdividing a polygonal face
	{
		Dart dd = phi1(old) ;
		splitFace(dd, phi1(phi1(dd))) ;	// insert a first edge
		Dart ne = alpha1(dd) ;

		cutEdge(ne) ;					// cut the new edge to insert the central vertex

		dd = phi1(phi1(phi1(phi1(ne)))) ;
		while(dd != ne)								// turn around the face and insert new edges
		{											// linked to the central vertex
			Dart next = phi1(phi1(dd)) ;
			splitFace(phi1(ne), dd) ;
			dd = next ;
		}
	}

	popLevel() ;
}

void Map2MR_Primal::coarsenEdge(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"coarsenEdge : called with a dart introduced after current level") ;
	assert(edgeCanBeCoarsened(d) || !"Trying to coarsen an edge that can not be coarsened") ;

	setCurrentLevel(getCurrentLevel() + 1) ;
	uncutEdge(d) ;
	setCurrentLevel(getCurrentLevel() - 1) ;
}

void Map2MR_Primal::coarsenFace(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"coarsenFace : called with a dart introduced after current level") ;
	assert(faceIsSubdividedOnce(d) || !"Trying to coarsen a non-subdivided face or a more than once subdivided face") ;

	unsigned int degree = 0 ;
	Dart fit = d ;
	do
	{
		++degree ;
		fit = phi1(fit) ;
	} while(fit != d) ;

	if(degree == 3)
	{
		fit = d ;
		do
		{
			setCurrentLevel(getCurrentLevel() + 1) ;
			Dart innerEdge = phi1(fit) ;
			setCurrentLevel(getMaxLevel()) ;
			mergeFaces(innerEdge) ;
			setCurrentLevel(getCurrentLevel() - 1) ;
			fit = phi1(fit) ;
		} while(fit != d) ;
	}
	else
	{
		setCurrentLevel(getCurrentLevel() + 1) ;
		Dart centralV = phi1(phi1(d)) ;
		setCurrentLevel(getMaxLevel()) ;
		deleteVertex(centralV) ;
		setCurrentLevel(getCurrentLevel() - 1) ;
	}

	fit = d ;
	do
	{
		if(edgeCanBeCoarsened(fit))
			coarsenEdge(fit) ;
		fit = phi1(fit) ;
	} while(fit != d) ;
}

} // namespace CGoGN
