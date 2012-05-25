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

#include "Topology/map/map2MR/map2MR_PrimalAdapt.h"

namespace CGoGN
{

Map2MR_PrimalAdapt::Map2MR_PrimalAdapt() :
	shareVertexEmbeddings(true),
	vertexVertexFunctor(NULL),
	edgeVertexFunctor(NULL),
	faceVertexFunctor(NULL)
{
	initMR() ;
}

/***************************************************
 *               CELLS INFORMATION                 *
 ***************************************************/

unsigned int Map2MR_PrimalAdapt::edgeLevel(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"edgeLevel : called with a dart inserted after current level") ;

	unsigned int ld = getDartLevel(d) ;
	unsigned int ldd = getDartLevel(phi2(d)) ;	// the level of an edge is the maximum of the
	return ld > ldd ? ld : ldd ;				// insertion levels of its two darts
}

unsigned int Map2MR_PrimalAdapt::faceLevel(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceLevel : called with a dart inserted after current level") ;

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

Dart Map2MR_PrimalAdapt::faceOrigin(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceOrigin : called with a dart inserted after current level") ;

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

Dart Map2MR_PrimalAdapt::faceOldestDart(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceOldestDart : called with a dart inserted after current level") ;

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

bool Map2MR_PrimalAdapt::edgeIsSubdivided(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"edgeIsSubdivided : called with a dart inserted after current level") ;

	if(getCurrentLevel() == getMaxLevel())
		return false ;

	Dart d2 = phi2(d) ;
	incCurrentLevel() ;
	Dart d2_l = phi2(d) ;
	decCurrentLevel() ;
	if(d2 != d2_l)
		return true ;
	else
		return false ;
}

bool Map2MR_PrimalAdapt::edgeCanBeCoarsened(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"edgeCanBeCoarsened : called with a dart inserted after current level") ;

	if(edgeIsSubdivided(d))
	{
		bool subdOnce = true ;
		bool degree2 = false ;

		Dart d2 = phi2(d) ;
		incCurrentLevel() ;
		if(vertexDegree(phi1(d)) == 2)
		{
			degree2 = true ;
			if(edgeIsSubdivided(d) || edgeIsSubdivided(d2))
				subdOnce = false ;
		}
		decCurrentLevel() ;

		return degree2 && subdOnce ;
	}

	return false ;
}

bool Map2MR_PrimalAdapt::faceIsSubdivided(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceIsSubdivided : called with a dart inserted after current level") ;

	if(getCurrentLevel() == getMaxLevel())
		return false ;

	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < getCurrentLevel())	// a face whose level in the current level map is lower than
		return false ;				// the current level can not be subdivided to higher levels

	bool subd = false ;
	incCurrentLevel() ;
	if(getDartLevel(phi1(phi1(d))) == getCurrentLevel())
		subd = true ;
	decCurrentLevel() ;
	return subd ;
}

bool Map2MR_PrimalAdapt::faceIsSubdividedOnce(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"faceIsSubdividedOnce : called with a dart inserted after current level") ;

	if(getCurrentLevel() == getMaxLevel())
		return false ;

	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < getCurrentLevel())	// a face whose level in the current level map is lower than
		return false ;				// the current level can not be subdivided to higher levels

	unsigned int degree = 0 ;
	bool subd = false ;
	bool subdOnce = true ;

	incCurrentLevel() ;
	if(getDartLevel(phi1(phi1(d))) == getCurrentLevel())
		subd = true ;
	decCurrentLevel() ;

	if(subd)
	{
		incCurrentLevel() ;

		if(getCurrentLevel() == getMaxLevel())
		{
			decCurrentLevel() ;
			return true ;
		}

		Dart fit = d ;
		do
		{
			incCurrentLevel() ;
			if(getDartLevel(phi1(phi1(fit))) == getCurrentLevel())
				subdOnce = false ;
			decCurrentLevel() ;
			++degree ;
			fit = phi1(fit) ;
		} while(subdOnce && fit != d) ;

		if(degree == 3 && subdOnce)
		{
			Dart cf = phi2(phi1(d)) ;
			incCurrentLevel() ;
			if(getDartLevel(phi1(phi1(cf))) == getCurrentLevel())
				subdOnce = false ;
			decCurrentLevel() ;
		}

		decCurrentLevel() ;

		return subdOnce ;
	}

	return false ;
}

/***************************************************
 *               SUBDIVISION                       *
 ***************************************************/

void Map2MR_PrimalAdapt::addNewLevel(bool embedNewVertices)
{
	addLevel() ;
}

void Map2MR_PrimalAdapt::propagateDartRelation(Dart d, AttributeMultiVector<Dart>* rel)
{
	Dart dd = (*rel)[dartIndex(d)] ;
	pushLevel() ;
	for(unsigned int i = getCurrentLevel() + 1; i <= getMaxLevel(); ++i)
	{
		setCurrentLevel(i) ;
		(*rel)[dartIndex(d)] = dd ;
	}
	popLevel() ;
}

template <unsigned int ORBIT>
void Map2MR_PrimalAdapt::propagateDartEmbedding(Dart d)
{
	unsigned int emb = getEmbedding<ORBIT>(d) ;
	pushLevel() ;
	for(unsigned int i = getCurrentLevel() + 1; i <= getMaxLevel(); ++i)
	{
		setCurrentLevel(i) ;
		setDartEmbedding<ORBIT>( d, emb) ;
	}
	popLevel() ;
}

template <unsigned int ORBIT>
void Map2MR_PrimalAdapt::propagateOrbitEmbedding(Dart d)
{
	unsigned int emb = getEmbedding<ORBIT>(d) ;
	pushLevel() ;
	for(unsigned int i = getCurrentLevel() + 1; i <= getMaxLevel(); ++i)
	{
		setCurrentLevel(i) ;
		embedOrbit<ORBIT>(d, emb) ;
	}
	popLevel() ;
}

//Dart Map2MR_PrimalAdapt::cutEdge(Dart d)
//{
//	Dart dd = phi2(d) ;
//
//	Dart d1 = newDart() ;
//	Dart dd1 = newDart() ;
//
//	pushLevel() ;
//	for(unsigned int i = getCurrentLevel(); i <= getMaxLevel(); ++i)
//	{
//		setCurrentLevel(i) ;
//
//		phi2unsew(d) ;
//
//		phi1sew(d, d1) ;
//		if (isBoundaryMarked(d))
//			boundaryMark(d1) ;
//
//		phi1sew(dd, dd1) ;
//		if (isBoundaryMarked(dd))
//			boundaryMark(dd1) ;
//
//		phi2sew(d, dd1) ;
//		phi2sew(dd, d1) ;
//	}
//	popLevel() ;
//
//	return d1 ;
//}

Dart Map2MR_PrimalAdapt::cutEdge(Dart d)
{
	Dart dd = phi2(d) ;
	Dart d1 = EmbeddedMap2::cutEdge(d) ;
	Dart dd1 = phi1(dd) ;
	Dart d11 = phi1(d1) ;
	Dart dd11 = phi1(dd1) ;

	propagateDartRelation(d, m_phi1) ;
	propagateDartRelation(d, m_phi2) ;
	propagateDartRelation(dd, m_phi1) ;
	propagateDartRelation(dd, m_phi2) ;
	propagateDartRelation(d1, m_phi1) ;
	propagateDartRelation(d1, m_phi_1) ;
	propagateDartRelation(d1, m_phi2) ;
	propagateDartRelation(dd1, m_phi1) ;
	propagateDartRelation(dd1, m_phi_1) ;
	propagateDartRelation(dd1, m_phi2) ;
	propagateDartRelation(d11, m_phi_1) ;
	propagateDartRelation(dd11, m_phi_1) ;

	return d1 ;
}

//void Map2MR_PrimalAdapt::splitFace(Dart d, Dart e)
//{
//	Dart dprev = phi_1(d) ;
//	Dart eprev = phi_1(e) ;
//
//	Dart dd = newDart() ;
//	Dart ee = newDart() ;
//
//	pushLevel() ;
//	for(unsigned int i = getCurrentLevel(); i <= getMaxLevel(); ++i)
//	{
//		setCurrentLevel(i) ;
//
//		phi1sew(dprev, dd) ;
//		if (isBoundaryMarked(dprev))
//			boundaryMark(dd);
//
//		phi1sew(eprev, ee) ;
//		if (isBoundaryMarked(eprev))
//			boundaryMark(ee);
//
//		phi1sew(dprev, eprev) ;
//
//		phi2sew(dd, ee) ;
//
//		copyDartEmbedding<VERTEX>(ee, d) ;
//		copyDartEmbedding<VERTEX>(dd, e) ;
//	}
//	popLevel() ;
//}

void Map2MR_PrimalAdapt::splitFace(Dart d, Dart e)
{
	Dart dprev = phi_1(d) ;
	Dart eprev = phi_1(e) ;
	EmbeddedMap2::splitFace(d, e) ;
	Dart dd = phi1(dprev) ;
	Dart ee = phi1(eprev) ;

	propagateDartRelation(d, m_phi_1) ;
	propagateDartRelation(e, m_phi_1) ;
	propagateDartRelation(dd, m_phi1) ;
	propagateDartRelation(dd, m_phi_1) ;
	propagateDartRelation(dd, m_phi2) ;
	propagateDartRelation(ee, m_phi1) ;
	propagateDartRelation(ee, m_phi_1) ;
	propagateDartRelation(ee, m_phi2) ;
	propagateDartRelation(dprev, m_phi1) ;
	propagateDartRelation(eprev, m_phi1) ;

	propagateDartEmbedding<VERTEX>(dd) ;
	propagateDartEmbedding<VERTEX>(ee) ;
}

void Map2MR_PrimalAdapt::subdivideEdge(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"subdivideEdge : called with a dart inserted after current level") ;
	assert(!edgeIsSubdivided(d) || !"Trying to subdivide an already subdivided edge") ;

	assert(getCurrentLevel() == edgeLevel(d) || !"Trying to subdivide an edge on a bad current level") ;

	incCurrentLevel() ;

	Dart d1 = cutEdge(d) ;
	Dart dd1 = phi2(d) ;

	(*edgeVertexFunctor)(d1) ;
	propagateDartEmbedding<VERTEX>(d1) ;
	propagateDartEmbedding<VERTEX>(dd1) ;

	decCurrentLevel() ;
}

void Map2MR_PrimalAdapt::coarsenEdge(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"coarsenEdge : called with a dart inserted after current level") ;
	assert(edgeCanBeCoarsened(d) || !"Trying to coarsen an edge that can not be coarsened") ;

	incCurrentLevel() ;
	uncutEdge(d) ;
	decCurrentLevel() ;

	unsigned int maxL = getMaxLevel() ;
	if(getCurrentLevel() == maxL - 1 && getNbInsertedDarts(maxL) == 0)
		removeLevel() ;
}

unsigned int Map2MR_PrimalAdapt::subdivideFace(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"subdivideFace : called with a dart inserted after current level") ;
	assert(!faceIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int fLevel = faceLevel(d) ;
	Dart old = faceOldestDart(d) ;

	pushLevel() ;
	setCurrentLevel(fLevel) ;		// go to the level of the face to subdivide its edges

	if(getCurrentLevel() == getMaxLevel())
		addNewLevel() ;

	unsigned int degree = 0 ;
	Dart it = old ;
	do
	{
		++degree ;						// compute the degree of the face
//		Dart nf = phi2(it) ;
//		if(faceLevel(nf) == fLevel - 1)	// check if neighboring faces have to be subdivided first
//			subdivideFace(nf) ;
		if(!edgeIsSubdivided(it))
			subdivideEdge(it) ;			// and cut the edges (if they are not already)
		it = phi1(it) ;
	} while(it != old) ;

	setCurrentLevel(fLevel + 1) ;	// go to the next level to perform face subdivision

	if(degree == 3)					// if subdividing a triangle
	{
		Dart dd = phi1(old) ;
		Dart e = phi1(dd) ;
		(*vertexVertexFunctor)(e) ;
		propagateOrbitEmbedding<VERTEX>(e) ;
		e = phi1(e) ;
		splitFace(dd, e) ;

		dd = e ;
		e = phi1(dd) ;
		(*vertexVertexFunctor)(e) ;
		propagateOrbitEmbedding<VERTEX>(e) ;
		e = phi1(e) ;
		splitFace(dd, e) ;

		dd = e ;
		e = phi1(dd) ;
		(*vertexVertexFunctor)(e) ;
		propagateOrbitEmbedding<VERTEX>(e) ;
		e = phi1(e) ;
		splitFace(dd, e) ;
	}
	else							// if subdividing a polygonal face
	{
		Dart dd = phi1(old) ;
		Dart next = phi1(dd) ;
		(*vertexVertexFunctor)(next) ;
		propagateOrbitEmbedding<VERTEX>(next) ;
		next = phi1(next) ;
		splitFace(dd, next) ;			// insert a first edge
		Dart ne = alpha1(dd) ;

		cutEdge(ne) ;					// cut the new edge to insert the central vertex

		dd = phi1(next) ;
		(*vertexVertexFunctor)(dd) ;
		propagateOrbitEmbedding<VERTEX>(dd) ;
		dd = phi1(dd) ;
		while(dd != ne)					// turn around the face and insert new edges
		{								// linked to the central vertex
			splitFace(phi1(ne), dd) ;
			dd = phi1(dd) ;
			(*vertexVertexFunctor)(dd) ;
			propagateOrbitEmbedding<VERTEX>(dd) ;
			dd = phi1(dd) ;
		}

		(*faceVertexFunctor)(phi2(ne)) ;
		propagateOrbitEmbedding<VERTEX>(phi2(ne)) ;
	}

	popLevel() ;

	return fLevel ;
}

void Map2MR_PrimalAdapt::coarsenFace(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"coarsenFace : called with a dart inserted after current level") ;
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
			incCurrentLevel() ;
			Dart innerEdge = phi1(fit) ;
			setCurrentLevel(getMaxLevel()) ;
			mergeFaces(innerEdge) ;
			decCurrentLevel() ;
			fit = phi1(fit) ;
		} while(fit != d) ;
	}
	else
	{
		incCurrentLevel() ;
		Dart centralV = phi1(phi1(d)) ;
		setCurrentLevel(getMaxLevel()) ;
		deleteVertex(centralV) ;
		decCurrentLevel() ;
	}

	fit = d ;
	do
	{
		if(edgeCanBeCoarsened(fit))
			coarsenEdge(fit) ;
		fit = phi1(fit) ;
	} while(fit != d) ;

	unsigned int maxL = getMaxLevel() ;
	if(getCurrentLevel() == maxL - 1 && getNbInsertedDarts(maxL) == 0)
		removeLevel() ;
}

} // namespace CGoGN
