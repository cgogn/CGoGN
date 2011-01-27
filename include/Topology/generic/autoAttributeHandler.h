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

#ifndef __AUTO_ATTRIBUTE_HANDLER_H__
#define __AUTO_ATTRIBUTE_HANDLER_H__

#include "Topology/generic/attribmap.h"
#include "Topology/generic/attributeHandler.h"

namespace CGoGN
{

/**
 * Class that create an acces-table to an attribute
 * that is added especially. The attribute is removed when
 * the object is destroyed
 */
template <typename T>
class AutoAttributeHandler : public AttributeHandler<T>
{
public:
	/**
	 * constructor
	 * @param m the map
	 * @param orbit the orbit of map
	 * @param nameAttr the name of attribute
	 * @param nameType the name of type of attribute
	 */
	AutoAttributeHandler(AttribMap& m, unsigned int orbit, const std::string& nameAttr = "")
	{
		this->m_map = &m ;
		if(!m.isOrbitEmbedded(orbit))
			m.addEmbedding(orbit) ;
		AttribContainer& cellCont = this->m_map->m_attribs[orbit] ;
		unsigned int index = cellCont.addAttribute<T>(nameAttr) ;
		AttribMultiVect<T>& atm = cellCont.getDataVector<T>(index);
		this->m_attrib = &atm;
		this->m_id = AttribContainer::attrId(orbit, index) ;
	}

	~AutoAttributeHandler()
	{
		reinterpret_cast<AttribMap*>(this->m_map)->removeAttribute<T>(*this) ;
	}
};

} // namespace CGoGN

#endif
