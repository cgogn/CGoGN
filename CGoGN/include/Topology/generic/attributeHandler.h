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

/// Macro that checks if ATTRIBUTEHANDLER type is an AttributeHandler
#define CHECK_ATTRIBUTEHANDLER(ATTRIBUTEHANDLER)\
	static_assert(std::is_base_of<AttributeHandlerGen, ATTRIBUTEHANDLER>::value, "Error not AttributeHandler");

/// Macro that checks if ATTRIBUTEHANDLER type is an AttributeHandler of orbit ORBITVALUE
#define CHECK_ATTRIBUTEHANDLER_ORBIT(ATTRIBUTEHANDLER, ORBITVALUE)\
	static_assert(std::is_base_of<AttributeHandlerGen, ATTRIBUTEHANDLER>::value, "Error not AttributeHandler");\
	static_assert(ATTRIBUTEHANDLER::ORBIT == ORBITVALUE, "Error wrong orbit of AttributeHandler");

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

	bool isValid() const { return valid ; }

	virtual int getSizeOfType() const = 0;

	virtual unsigned int getOrbit() const = 0;

	virtual const std::string& name() const = 0;
	virtual const std::string& typeName() const = 0;

	virtual AttributeMultiVectorGen* getDataVectorGen() const = 0;

protected:
	void setInvalid() { valid = false ; }
	void setValid() { valid = true ; }
} ;

template <unsigned int ORB>
class AttributeHandlerOrbit: public AttributeHandlerGen
{
public:
	AttributeHandlerOrbit(bool v) :
		AttributeHandlerGen(v)
	{}

	static const unsigned int ORBIT = ORB;
};


// forward declaration
template <typename T, unsigned int ORB, typename MAP>
class AttributeHandlerIter;



/**
 * Class that create an access-table to an existing attribute
 * Main available operations are:
 * - [ index ]
 * - [ dart ]
 * - begin / end / next to manage indexing
 */
template <typename T, unsigned int ORB, typename MAP>
class AttributeHandler : public AttributeHandlerOrbit<ORB>
{
protected:
	// the map that contains the linked attribute
	MAP* m_map;
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
	AttributeHandler(MAP* m, AttributeMultiVector<T>* amv) ;

	/**
	 * Copy constructor
	 * @param ta the table attribute
	 */
	AttributeHandler(const AttributeHandler<T, ORB, MAP>& ta) ;

	/**
	 * Transmute Constructor
	 * Construct an attribute of Orbit from Orbit2
	 * @param h the table attribute
	 */
	//template <unsigned int ORBIT2>
	//AttributeHandler(const AttributeHandler<T, ORBIT2, MAP>& h) ;

	/**
	 * affectation operator
	 * @param ta the table attribute to affect to this
	 */
	AttributeHandler<T, ORB, MAP>& operator=(const AttributeHandler<T, ORB, MAP>& ta) ;

	/**
	 * transmuted affectation operator
	 * @param ta the table attribute to affect to this
	 */
	template <unsigned int ORBIT2>
	AttributeHandler<T, ORB, MAP>& operator=(const AttributeHandler<T, ORBIT2, MAP>& ta) ;

	/**
	 * Destructor (empty & virtual)
	 */
	virtual ~AttributeHandler() ;

	/**
	 * @brief map
	 * @return the map that contains the attribute
	 */
	MAP* map() const
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
	T& operator[](Cell<ORB> c) ;

	/**
	 * const [] operator with cell parameter
	 */
	const T& operator[](Cell<ORB> c) const ;

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

	AttributeHandlerIter<T,ORB,MAP> iterable() const;
} ;



/**
 *  c++11 shortcut for Dart Attribute (Handler)
 */
template <typename T, typename MAP>
using DartAttribute = AttributeHandler<T, DART, MAP>;

typedef AttributeHandlerOrbit<DART> DartAttributeGen;


/**
 *  c++11 shortcut for Vertex Attribute (Handler)
 */
template <typename T, typename MAP>
using VertexAttribute = AttributeHandler<T, VERTEX, MAP>;

typedef AttributeHandlerOrbit<VERTEX> VertexAttributeGen;


/**
 *  c++11 shortcut for Edge Attribute (Handler)
 */
template <typename T, typename MAP>
using EdgeAttribute = AttributeHandler<T, EDGE, MAP>;

typedef AttributeHandlerOrbit<EDGE> EdgeAttributeGen;

/**
 *  c++11 shortcut for Face Attribute (Handler)
 */
template <typename T, typename MAP>
using FaceAttribute = AttributeHandler<T, FACE, MAP>;

typedef AttributeHandlerOrbit<FACE> FaceAttributeGen;

/**
 *  c++11 shortcut for Volume Attribute (Handler)
 */
template <typename T, typename MAP>
using VolumeAttribute = AttributeHandler<T, VOLUME, MAP>;

typedef AttributeHandlerOrbit<VOLUME> VolumeAttributeGen;




template <typename T, unsigned int ORB, typename MAP>
class AttributeHandlerIter : public AttributeHandler<T,ORB,MAP>
{
public:

	AttributeHandlerIter(const AttributeHandler<T, ORB, MAP>& ta):
		AttributeHandler<T,ORB,MAP>(ta)
	{
	}


	class iterator
	{
		AttributeHandlerIter<T,ORB,MAP>* m_ptr;
		unsigned int m_index;

	public:

		inline iterator(AttributeHandlerIter<T, ORB, MAP>* p, unsigned int i): m_ptr(p),m_index(i){}

		inline iterator& operator++()
		{
			m_ptr->next(m_index);
			return *this;
		}

		inline T& operator*()
		{
			T& v = m_ptr->operator[](m_index);
			return v;
		}

		inline bool operator!=(iterator it)
		{
			return m_index != it.m_index;
		}

	};

	inline iterator begin()
	{
		return iterator(this,AttributeHandler<T,ORB,MAP>::begin());
	}

	inline iterator end()
	{
		return iterator(this,AttributeHandler<T,ORB,MAP>::end());
	}

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
void foreach_attribute(ATTR& attribute, FUNC func, unsigned int nbth = NumberOfThreads);

}

template<unsigned int ORBIT>
inline bool checkAttributeHandlerOrbit(const AttributeHandlerGen& att)
{
	return dynamic_cast<const AttributeHandlerOrbit<ORBIT>*>(&att) != NULL;
}

} // namespace CGoGN

#include "Topology/generic/attributeHandler.hpp"

#endif
