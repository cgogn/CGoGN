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

#ifndef __ATTRIBUTE_HANDLER_H__
#define __ATTRIBUTE_HANDLER_H__

#include <vector>
#include <map>

#include "Topology/generic/genericmap.h"
#include "Container/attributeContainer.h"
#include "Container/fakeAttribute.h"
#include "Topology/generic/cells.h"

namespace CGoGN
{

class AttributeHandlerGen
{
protected:
	friend class GenericMap ;
	template <typename MAP_IMPL> friend class MapCommon;

	// boolean that states the validity of the handler
	bool valid ;

public:
	AttributeHandlerGen(bool v) :
		valid(v)
	{}

	bool isValid() const
	{
		return valid ;
	}

	virtual int getSizeOfType() const = 0;

	virtual unsigned int getOrbit() const = 0;

	virtual const std::string& name() const = 0;
	virtual const std::string& typeName() const = 0;

	virtual AttributeMultiVectorGen* getDataVectorGen() const = 0;

protected:
	void setInvalid()
	{
		valid = false ;
	}
} ;

template <typename MAP_IMPL> class MapCommon;

/**
 * Class that create an access-table to an existing attribute
 * Main available operations are:
 * - [ index ]
 * - [ dart ]
 * - begin / end / next to manage indexing
 */
template <typename T, unsigned int ORBIT, typename MAP_IMPL>
class AttributeHandler : public AttributeHandlerGen
{
protected:
	// the map that contains the linked attribute
	MapCommon<MAP_IMPL>* m_map;
	// the multi-vector that contains attribute data
	AttributeMultiVector<T>* m_attrib;

	void registerInMap() ;
	void unregisterFromMap() ;

public:
	typedef T DATA_TYPE ;

	/**
	 * Default constructor
	 * Constructs a non-valid AttributeHandler (i.e. not linked to any attribute)
	 */
	AttributeHandler() ;

	/**
	 * Constructor
	 * @param m the map which belong attribute
	 * @param amv a pointer to the AttributeMultiVector
	 */
	AttributeHandler(MapCommon<MAP_IMPL>* m, AttributeMultiVector<T>* amv) ;

	/**
	 * Copy constructor
	 * @param ta the table attribute
	 */
	AttributeHandler(const AttributeHandler<T, ORBIT, MAP_IMPL>& ta) ;

	/**
	 * Transmute Constructor
	 * Construct an attribute of Orbit from Orbit2
	 * @param h the table attribute
	 */
	template <unsigned int ORBIT2>
	AttributeHandler(const AttributeHandler<T, ORBIT2, MAP_IMPL>& h) ;

	/**
	 * affectation operator
	 * @param ta the table attribute to affect to this
	 */
	AttributeHandler<T, ORBIT, MAP_IMPL>& operator=(const AttributeHandler<T, ORBIT, MAP_IMPL>& ta) ;

	/**
	 * transmuted affectation operator
	 * @param ta the table attribute to affect to this
	 */
	template <unsigned int ORBIT2>
	AttributeHandler<T, ORBIT, MAP_IMPL>& operator=(const AttributeHandler<T, ORBIT2, MAP_IMPL>& ta) ;

	/**
	 * Destructor (empty & virtual)
	 */
	virtual ~AttributeHandler() ;

	/**
	 * @brief map
	 * @return the map that contains the attribute
	 */
	MapCommon<MAP_IMPL>* map() const
	{
		return m_map ;
	}

	/**
	 * get attribute data vector
	 */
	AttributeMultiVector<T>* getDataVector() const ;

	/**
	 * get attribute data vector (generic MultiVector)
	 */
	virtual AttributeMultiVectorGen* getDataVectorGen() const ;

	/**
	 * get size of attribute type
	 */
	virtual int getSizeOfType() const ;

	/**
	 * get attribute orbit
	 */
	virtual unsigned int getOrbit() const ;

	/**
	 * get attribute index
	 */
	unsigned int getIndex() const ;

	/**
	 * get attribute name
	 */
	virtual const std::string& name() const ;

	/**
	 * get attribute type name
	 */
	virtual const std::string& typeName() const ;

	/**
	 * give the number of elements of the attribute container
	 */
	unsigned int nbElements() const;

	/**
	 * [] operator with cell parameter
	 */
	T& operator[](Cell<ORBIT> c) ;

	/**
	 * const [] operator with cell parameter
	 */
	const T& operator[](Cell<ORBIT> c) const ;

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
	 * initialize all the lines of the attribute with the given value
	 */
	void setAllValues(const T& v) ;

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

/**
 *  shortcut class for Dart Attribute (Handler)
 */
template <typename T, typename MAP_IMPL>
class DartAttribute : public AttributeHandler<T, DART, MAP_IMPL>
{
public:
	DartAttribute() : AttributeHandler<T, DART, MAP_IMPL>() {}
	DartAttribute(const AttributeHandler<T, DART, MAP_IMPL>& ah) : AttributeHandler<T, DART, MAP_IMPL>(ah) {}
	DartAttribute(MapCommon<MAP_IMPL>* m, AttributeMultiVector<T>* amv) : AttributeHandler<T, DART, MAP_IMPL>(m,amv) {}
	DartAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, DART, MAP_IMPL>& ah) { this->AttributeHandler<T, DART, MAP_IMPL>::operator=(ah); return *this; }
};

/**
 *  shortcut class for Vertex Attribute (Handler)
 */
template <typename T, typename MAP_IMPL>
class VertexAttribute : public AttributeHandler<T, VERTEX, MAP_IMPL>
{
public:
	VertexAttribute() : AttributeHandler<T, VERTEX, MAP_IMPL>() {}
	VertexAttribute(const AttributeHandler<T, VERTEX, MAP_IMPL>& ah) : AttributeHandler<T, VERTEX, MAP_IMPL>(ah) {}
	VertexAttribute(MapCommon<MAP_IMPL>* m, AttributeMultiVector<T>* amv) : AttributeHandler<T, VERTEX, MAP_IMPL>(m,amv) {}
	VertexAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, VERTEX, MAP_IMPL>& ah) { this->AttributeHandler<T, VERTEX, MAP_IMPL>::operator=(ah); return *this; }
	VertexAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, EDGE, MAP_IMPL>& ah) { this->AttributeHandler<T,VERTEX, MAP_IMPL>::operator=(ah); return *this; }
	VertexAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, FACE, MAP_IMPL>& ah) { this->AttributeHandler<T,VERTEX, MAP_IMPL>::operator=(ah); return *this; }
	VertexAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, VOLUME, MAP_IMPL>& ah) { this->AttributeHandler<T,VERTEX, MAP_IMPL>::operator=(ah); return *this; }
};

/**
 *  shortcut class for Edge Attribute (Handler)
 */
template <typename T, typename MAP_IMPL>
class EdgeAttribute : public AttributeHandler<T, EDGE, MAP_IMPL>
{
public:
	EdgeAttribute() : AttributeHandler<T, EDGE, MAP_IMPL>() {}
	EdgeAttribute(const AttributeHandler<T, EDGE, MAP_IMPL>& ah) : AttributeHandler<T, EDGE, MAP_IMPL>(ah) {}
	EdgeAttribute(MapCommon<MAP_IMPL>* m, AttributeMultiVector<T>* amv) : AttributeHandler<T, EDGE, MAP_IMPL>(m,amv) {}
	EdgeAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, EDGE, MAP_IMPL>& ah) { this->AttributeHandler<T, EDGE, MAP_IMPL>::operator=(ah); return *this; }
};

/**
 *  shortcut class for Face Attribute (Handler)
 */
template <typename T, typename MAP_IMPL>
class FaceAttribute : public AttributeHandler<T, FACE, MAP_IMPL>
{
public:
	FaceAttribute() : AttributeHandler<T, FACE, MAP_IMPL>() {}
	FaceAttribute(const AttributeHandler<T, FACE, MAP_IMPL>& ah) : AttributeHandler<T, FACE, MAP_IMPL>(ah) {}
	FaceAttribute(MapCommon<MAP_IMPL>* m, AttributeMultiVector<T>* amv) : AttributeHandler<T, FACE, MAP_IMPL>(m,amv) {}
	FaceAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, FACE, MAP_IMPL>& ah) { this->AttributeHandler<T, FACE, MAP_IMPL>::operator=(ah); return *this; }
	FaceAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, VERTEX, MAP_IMPL>& ah) { this->AttributeHandler<T,FACE, MAP_IMPL>::operator=(ah); return *this; }
};

/**
 *  shortcut class for Volume Attribute (Handler)
 */
template <typename T, typename MAP_IMPL>
class VolumeAttribute : public AttributeHandler<T, VOLUME, MAP_IMPL>
{
public:
	VolumeAttribute() : AttributeHandler<T, VOLUME, MAP_IMPL>() {}
	VolumeAttribute(const AttributeHandler<T, VOLUME, MAP_IMPL>& ah) : AttributeHandler<T, VOLUME, MAP_IMPL>(ah) {}
	VolumeAttribute(MapCommon<MAP_IMPL>* m, AttributeMultiVector<T>* amv) : AttributeHandler<T, VOLUME, MAP_IMPL>(m,amv) {}
	VolumeAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, VOLUME, MAP_IMPL>& ah) { this->AttributeHandler<T, VOLUME, MAP_IMPL>::operator=(ah); return *this; }
	VolumeAttribute<T, MAP_IMPL>& operator=(const AttributeHandler<T, VERTEX, MAP_IMPL>& ah) { this->AttributeHandler<T, VOLUME, MAP_IMPL>::operator=(ah); return *this; }
};


// turn_to<b>(A*</b> obj) changes class of the object
// that means it just replaces VTBL of the object by VTBL of another class.
// NOTE: these two classes has to be ABI compatible!
template <typename TO_T, typename FROM_T>
inline void turn_to(FROM_T* p)
{
assert(sizeof(FROM_T) == sizeof(TO_T));
::new(p) TO_T(); // use of placement new
}

/**
 * apply function on each element of attribute
 * Warning attr must also be captured by lambda funct
 */
template <typename ATTR, typename FUNC>
inline void foreach_attribute(ATTR& attr, FUNC func)
{
	for (unsigned int id=attr.begin(); id != attr.end(); attr.next(id))
		func(id);
}

namespace Parallel
{
template <typename ATTR, typename FUNC>
void foreach_attribute(ATTR& attribute, FUNC func, unsigned int nbth=0);
}

} // namespace CGoGN

#include "Topology/generic/attributeHandler.hpp"

#endif
