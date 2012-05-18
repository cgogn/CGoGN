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

#include "Topology/map/map2MR/map2MR_PM.h"

namespace CGoGN
{

Map2MR_PM::Map2MR_PM() :
	shareVertexEmbeddings(true)
{
	initMR() ;
}

void Map2MR_PM::addNewLevel(bool embedNewVertices)
{
	pushLevel() ;

	//Add the current level higher

	unsigned int newLevel = m_mrDarts.size() ;
	std::stringstream ss ;
	ss << "MRdart_"<< newLevel ;
	AttributeMultiVector<unsigned int>* newAttrib = m_mrattribs.addAttribute<unsigned int>(ss.str()) ;
	m_mrDarts.push_back(newAttrib) ;
	m_mrNbDarts.push_back(0) ;

	if(m_mrDarts.size() > 1)
	{
		for(unsigned int i = newLevel; i > 0 ; --i)
		{
			AttributeMultiVector<unsigned int>* currentAttrib = m_mrDarts[i] ;
			AttributeMultiVector<unsigned int>* prevAttrib = m_mrDarts[i - 1] ;	// copy the indices of
			m_mrattribs.copyAttribute(currentAttrib->getIndex(), prevAttrib->getIndex()) ;	// previous level into new level
		}
	}

	popLevel() ;

	setCurrentLevel(0);

	selectedEdges = new DartMarkerStore(*this);

	TraversorE<Map2MR_PM> travE(*this);
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
		if(!selectedEdges->isMarked(d))
		{
			if(edgeCanCollapse(d))
			{
				//interdire la contraction d'arete partageant un meme sommet
				//interdire la contraction d'arete
				//interdire la contraction d'une arete d'une face incidente au sommet de d
				//=> mark incident edges of vertices adjacent to the vertex of d through a common face
				Traversor2VVaF<Map2MR_PM> travVVaF(*this, d);
				for (Dart dit = travVVaF.begin(); dit != travVVaF.end(); dit = travVVaF.next())
				{
					Dart ditV = dit;
					do
					{
						if(!selectedEdges->isMarked(ditV))
							selectedEdges->markOrbit(EDGE, ditV);

						ditV = phi2(phi_1(ditV));
					} while(ditV != dit);
				}

				//=> the same for the vertex phi2(d)
				Traversor2VVaF<Map2MR_PM> travVVaF2(*this, phi2(d));
				for (Dart dit = travVVaF2.begin(); dit != travVVaF2.end(); dit = travVVaF2.next())
				{
					Dart ditV = dit;
					do
					{
						if(!selectedEdges->isMarked(ditV))
							selectedEdges->markOrbit(EDGE, ditV);

						ditV = phi2(phi_1(ditV));
					} while(ditV != dit);
				}
			}
		}
	}

	//(*filter).decimate() ;
}

void Map2MR_PM::analysis()
{
	assert(getCurrentLevel() > 0 || !"analysis : called on level 0") ;

	decCurrentLevel() ;

	//(*filter).coarsen() ;
}

void Map2MR_PM::synthesis()
{
	assert(getCurrentLevel() < getMaxLevel() || !"synthesis : called on max level") ;

	//(*filter).refine() ;

	incCurrentLevel() ;
}

} // namespace CGoGN
