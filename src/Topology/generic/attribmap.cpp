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

#include "Topology/generic/attribmap.h"

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

	for(unsigned int i = 0; i < cellMarkers.size(); ++i)
	{
		CellMarkerGen* cm = cellMarkers[i] ;
		cm->updateMarkVector(m_markTables[cm->getCell()][cm->getThread()]) ;
	}

	for(unsigned int i = 0; i < dartMarkers.size(); ++i)
	{
		DartMarkerGen* cm = dartMarkers[i] ;
		cm->updateMarkVector(m_markTables[DART][cm->getThread()]) ;
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

} // namespace CGoGN
