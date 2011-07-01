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

#ifndef __ATTRIBUTE_HANDLER_H__
#define __ATTRIBUTE_HANDLER_H__

#include <vector>
#include <map>

#include "Topology/generic/genericmap.h"

namespace CGoGN
{

/**
 * Class that create an access-table to an existing attribute
 * Main available operations are:
 * - [ index ]
 * - [ dart ]
 * - begin / end / next to manage indexing
 */
template <typename T>
class AttributeHandler
{
protected:
	// we need the map to use dart as index
	GenericMap* m_map;

	// access to the data
	AttributeMultiVector<T>* m_attrib;

public:
	typedef T DATA_TYPE ;

	/**
	 * Constructor
	 * @param m the map which belong attribute
	 * @param amv a pointer to the AttributeMultiVector
	 */
	AttributeHandler(GenericMap* m, AttributeMultiVector<T>* amv) ;

	AttributeHandler() ;

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
	AttributeMultiVector<T>* getDataVector() const ;

	/**
	 * get attribute orbit
	 */
	unsigned int getOrbit() const ;

	/**
	 * get attribute index
	 */
	unsigned int getIndex() const ;

	/**
	 * get attribute name
	 */
	const std::string& name() const ;

	/**
	 * check if the attribute handler is linked to a valid attribute
	 * -> MUST BE USED AFTER A CALL TO :
	 * getAttribute(unsigned int orbit, const std::string& nameAttr)
	 * addAttribute(unsigned int orbit, const std::string& nameAttr)
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
	 * insert an element with default value (warning we add here a complete line in container)
	 */
	unsigned int newElt() ;

	/**
	 * begin of table
	 * @return the iterator of the begin of container
	 */
	unsigned int begin() const;

	/**
	 * end of table
	 * @return the iterator of the end of container
	 */
	unsigned int end() const;

	/**
	 * Next on iterator (equivalent to stl ++)
	 * @param iter iterator to
	 */
	void next(unsigned int& iter) const;
} ;

} // namespace CGoGN

#include "Topology/generic/attributeHandler.hpp"

#endif
