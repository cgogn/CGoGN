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

#ifndef __ATTRIBUTE_HANDLER_H__
#define __ATTRIBUTE_HANDLER_H__

#include <vector>
#include <map>

#include "Topology/generic/genericmap.h"

namespace CGoGN
{

/**
 * Class that create an access-table to an existing attribute
 * Permetting operation are:
 * - [ index ]
 * - [ dart ]
 * - begin / end / next to manage indexing
 */
template <typename T>
class AttributeHandler
{
	friend class AttribMap ;

protected:
	// we need the map to use dart as index
	GenericMap* m_map;

	// id of the attribute
	unsigned int m_id;

	// access to the data
	AttribMultiVect<T>* m_attrib;

protected:
	/**
	 * Constructor
	 * @param idAttr identifiant of attribute
	 * @param m the map which belong attribute
	 */
	AttributeHandler(GenericMap* m, unsigned int idAttr) ;

public:
	typedef T DATA_TYPE ;

	AttributeHandler() : m_map(NULL), m_id(AttribContainer::UNKNOWN), m_attrib(NULL) {}

	/**
	 * Copy constructor
	 * @param ta the table attribute
	 */
	AttributeHandler(const AttributeHandler<T>& ta) ;

	/**
	 * affectation operator
	 * @param ta the table attribute to affect to this
	 */
	void operator=(const AttributeHandler<T>& ta) ;

	/**
	 * Destructor (empty & virtual)
	 */
	virtual ~AttributeHandler() ;

	/**
	 * get associated map
	 */
	GenericMap* map() const ;

	/**
	 * get attribute data vector
	 */
	AttribMultiVect<T>* getDataVector() const ;

	/**
	 * get attribute id
	 */
	unsigned int id() const ;

	/**
	 * get attribute name
	 */
	const std::string& name() const ;

	/**
	 * check if the attribute handler is linked to a valid attribute
	 * -> MUST BE USED AFTER A CALL TO :
	 * getAttribute(unsigned int orbit, const std::string& nameAttr)
	 * addAttribute( unsigned int orbit, const std::string& nameAttr)
	 */
	bool isValid() const ;

	/**
	 * [] operator with dart parameter
	 */
	T& operator[](Dart d) ;

	/**
	 * const [] operator with dart parameter
	 */
	const T& operator[](Dart d) const ;

	/**
	 * at operator (same as [] but with index parameter)
	 */
	T& operator[](unsigned int a) ;

	/**
	 * const at operator (same as [] but with index parameter)
	 */
	const T& operator[](unsigned int a) const ;

	/**
	 * insert an element (warning we add here a complete line in container)
	 */
	unsigned int insert(const T& elt) ;

	/**
	 * insert un element with default value (warning we add here a complete line in container)
	 */
	unsigned int newElt() ;

	/**
	 * begin of table
	 * @return the iterator of the begin of container
	 */
	unsigned int begin() const;

	/**
	 * begin of table
	 * @return the iterator of the end of container
	 */
	unsigned int end() const;

	/**
	 * Next on iterator (equivalent too stl ++)
	 * @param iter iterator to
	 */
	void next(unsigned int& iter) const;
} ;

} // namespace CGoGN

#include "Topology/generic/attributeHandler.hpp"

#endif
