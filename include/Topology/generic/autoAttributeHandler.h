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

#ifndef __AUTO_ATTRIBUTE_HANDLER_H__
#define __AUTO_ATTRIBUTE_HANDLER_H__

#include "Topology/generic/attribmap.h"
#include "Topology/generic/attributeHandler.h"

namespace CGoGN
{

/**
 * Class that creates an access-table to an attribute
 * that is added especially. The attribute is removed when
 * the AutoAttributeHandler object is destroyed
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
		AttributeContainer& cellCont = this->m_map->m_attribs[orbit] ;
		AttributeMultiVector<T>* amv = cellCont.addAttribute<T>(nameAttr) ;
		this->m_attrib = amv ;
		this->valid = true ;
		this->registerInMap() ;
	}

	~AutoAttributeHandler()
	{
		reinterpret_cast<AttribMap*>(this->m_map)->removeAttribute<T>(*this) ;
	}
} ;

} // namespace CGoGN

#endif
