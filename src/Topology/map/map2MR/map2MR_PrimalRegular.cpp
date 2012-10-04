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

#include "Topology/map/map2MR/map2MR_PrimalRegular.h"

namespace CGoGN
{

Map2MR_PrimalRegular::Map2MR_PrimalRegular() :
	shareVertexEmbeddings(true)
{
	initMR() ;
}

void Map2MR_PrimalRegular::addNewLevel(bool embedNewVertices)
{
	pushLevel() ;

	addLevel() ;
	setCurrentLevel(getMaxLevel()) ;

//	for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
//	{
//		unsigned int newindex = copyDartLine((*m_mrDarts[m_mrCurrentLevel])[i]) ;	// duplicate all darts
//		(*m_mrDarts[m_mrCurrentLevel])[i] = newindex ;								// on the new max level
//		if(!shareVertexEmbeddings)
//			(*m_embeddings[VERTEX])[newindex] = EMBNULL ;	// set vertex embedding to EMBNULL if no sharing
//	}

	// cut edges
	TraversorE<Map2MR_PrimalRegular> travE(*this) ;
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
		if(!shareVertexEmbeddings && embedNewVertices)
		{
			if(getEmbedding<VERTEX>(d) == EMBNULL)
				embedNewCell<VERTEX>(d) ;
			if(getEmbedding<VERTEX>(phi1(d)) == EMBNULL)
				embedNewCell<VERTEX>(d) ;
		}

		cutEdge(d) ;
		travE.skip(d) ;
		travE.skip(phi1(d)) ;

		if(embedNewVertices)
			embedNewCell<VERTEX>(phi1(d)) ;
	}

	// split faces
	TraversorF<Map2MR_PrimalRegular> travF(*this) ;
	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
	{
		Dart old = d ;
		if(getDartLevel(old) == getMaxLevel())
			old = phi1(old) ;

		decCurrentLevel() ;
		unsigned int degree = faceDegree(old) ;
		incCurrentLevel() ;

		if(degree == 3)					// if subdividing a triangle
		{
			Dart dd = phi1(old) ;
			Dart e = phi1(phi1(dd)) ;
			splitFace(dd, e) ;
			travF.skip(dd) ;

			dd = e ;
			e = phi1(phi1(dd)) ;
			splitFace(dd, e) ;
			travF.skip(dd) ;

			dd = e ;
			e = phi1(phi1(dd)) ;
			splitFace(dd, e) ;
			travF.skip(dd) ;

			travF.skip(e) ;
		}
		else							// if subdividing a polygonal face
		{
			Dart dd = phi1(old) ;
			Dart next = phi1(phi1(dd)) ;
			splitFace(dd, next) ;		// insert a first edge

			Dart ne = alpha1(dd) ;
			cutEdge(ne) ;				// cut the new edge to insert the central vertex
			travF.skip(dd) ;

			if(embedNewVertices)
				embedNewCell<VERTEX>(phi1(ne)) ;

			dd = phi1(phi1(next)) ;
			while(dd != ne)				// turn around the face and insert new edges
			{							// linked to the central vertex
				Dart tmp = phi1(ne) ;
				splitFace(tmp, dd) ;
				travF.skip(tmp) ;
				dd = phi1(phi1(dd)) ;
			}
			travF.skip(ne) ;
		}
	}

	popLevel() ;
}

void Map2MR_PrimalRegular::analysis()
{
	assert(getCurrentLevel() > 0 || !"analysis : called on level 0") ;

	decCurrentLevel() ;

	for(unsigned int i = 0; i < analysisFilters.size(); ++i)
		(*analysisFilters[i])() ;
}

void Map2MR_PrimalRegular::synthesis()
{
	assert(getCurrentLevel() < getMaxLevel() || !"synthesis : called on max level") ;

	for(unsigned int i = 0; i < synthesisFilters.size(); ++i)
		(*synthesisFilters[i])() ;

	incCurrentLevel() ;
}

} // namespace CGoGN
