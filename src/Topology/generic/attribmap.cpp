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

#include "Topology/generic/attribmap.h"

namespace CGoGN
{

void AttribMap::init()
{
	AttributeContainer& dartCont = m_attribs[DART] ;
	for (unsigned int t = 0; t < m_nbThreads; ++t)
	{
		std::stringstream ss ;
		ss << "Mark_"<< t ;
		AttributeMultiVector<Mark>* amvMark = dartCont.addAttribute<Mark>(ss.str()) ;
		for(unsigned int i = dartCont.begin(); i < dartCont.end(); dartCont.next(i))
			amvMark->operator[](i).clear() ;
		m_markTables[DART][t] = amvMark ;
	}
}

AttribMap::AttribMap() : GenericMap()
{
	init() ;
}

void AttribMap::clear(bool removeAttrib)
{
	GenericMap::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

/****************************************
 *   EMBEDDING ATTRIBUTES MANAGEMENT    *
 ****************************************/

void AttribMap::addEmbedding(unsigned int orbit)
{
	assert(!isOrbitEmbedded(orbit) || !"Invalid parameter: orbit already embedded") ;

	std::ostringstream oss;
	oss << "EMB_" << orbit;

	AttributeContainer& dartCont = m_attribs[DART] ;
	AttributeMultiVector<unsigned int>* amv = dartCont.addAttribute<unsigned int>(oss.str()) ;
	m_embeddings[orbit] = amv ;

	// set new embedding to EMBNULL for all the darts of the map
	for(unsigned int i = dartCont.begin(); i < dartCont.end(); dartCont.next(i))
		amv->operator[](i) = EMBNULL ;

	AttributeContainer& cellCont = m_attribs[orbit];
	for (unsigned int t = 0; t < m_nbThreads; ++t)
	{
		std::stringstream ss ;
		ss << "Mark_"<< t ;
		AttributeMultiVector<Mark>* amvMark = cellCont.addAttribute<Mark>(ss.str()) ;
		for(unsigned int i = cellCont.begin(); i < cellCont.end(); cellCont.next(i))
			amvMark->operator[](i).clear() ;
		m_markTables[orbit][t] = amvMark ;
	}
}

/****************************************
 *               UTILITIES              *
 ****************************************/

unsigned int AttribMap::computeIndexCells(AttributeHandler<unsigned int>& idx)
{
	AttributeContainer& cont = m_attribs[idx.getOrbit()] ;
	unsigned int cpt = 0 ;
	for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
		idx[i] = cpt++ ;
	return cpt ;
}

} // namespace CGoGN
