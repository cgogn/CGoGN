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
#include "Topology/generic/autoAttributeHandler.h"
#include "Topology/generic/dartmarker.h"

namespace CGoGN
{

void AttribMap::init()
{
	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		AttributeContainer& cont = m_attribs[orbit];
		for (unsigned int t = 0; t < m_nbThreads; ++t)
		{
			std::stringstream ss ;
			ss << "Mark_"<< t ;
			AttributeMultiVector<Mark>* amvMark = cont.addAttribute<Mark>(ss.str()) ;
			for(unsigned int i = cont.begin(); i < cont.end(); cont.next(i))
				amvMark->operator[](i).clear() ;
			m_markTables[orbit][t] = amvMark ;
		}
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

void AttribMap::bijectiveOrbitEmbedding(unsigned int orbit)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded") ;

	AutoAttributeHandler<int> counter(*this, orbit) ;
	counter.setAllValues(int(0)) ;

	DartMarker mark(*this) ;
	for(Dart d = begin(); d != end(); next(d))
	{
		if(!mark.isMarked(d))
		{
			mark.markOrbit(orbit, d) ;
			unsigned int emb = getEmbedding(orbit, d) ;
			if (emb != EMBNULL)
			{
				if (counter[d] > 0)
				{
					unsigned int newEmb = embedNewCell(orbit, d) ;
					copyCell(orbit, newEmb, emb) ;
				}
				counter[d]++ ;
			}
		}
	}
}

} // namespace CGoGN

