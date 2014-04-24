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

namespace CGoGN
{

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline void AttributeHandler<T, ORBIT, MAP_IMPL>::registerInMap()
{
	boost::mutex::scoped_lock lockAH(m_map->attributeHandlersMutex);
	m_map->attributeHandlers.insert(std::pair<AttributeMultiVectorGen*, AttributeHandlerGen*>(m_attrib, this)) ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline void AttributeHandler<T, ORBIT, MAP_IMPL>::unregisterFromMap()
{
	typedef std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator IT ;

	boost::mutex::scoped_lock lockAH(m_map->attributeHandlersMutex);
	std::pair<IT, IT> bounds = m_map->attributeHandlers.equal_range(m_attrib) ;
	for(IT i = bounds.first; i != bounds.second; ++i)
	{
		if((*i).second == this)
		{
			m_map->attributeHandlers.erase(i) ;
			return ;
		}
	}
	assert(false || !"Should not get here") ;
}

// =================================================================

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
AttributeHandler<T, ORBIT, MAP_IMPL>::AttributeHandler() :
	AttributeHandlerGen(false),
	m_map(NULL),
	m_attrib(NULL)
{}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
AttributeHandler<T, ORBIT, MAP_IMPL>::AttributeHandler(MapCommon<MAP_IMPL>* m, AttributeMultiVector<T>* amv) :
	AttributeHandlerGen(false),
	m_map(m),
	m_attrib(amv)
{
	if(m != NULL && amv != NULL && amv->getIndex() != AttributeContainer::UNKNOWN)
	{
		assert(ORBIT == amv->getOrbit() || !"AttributeHandler: orbit incompatibility") ;
		valid = true ;
		registerInMap() ;
	}
	else
		valid = false ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
AttributeHandler<T, ORBIT, MAP_IMPL>::AttributeHandler(const AttributeHandler<T, ORBIT, MAP_IMPL>& ta) :
	AttributeHandlerGen(ta.valid),
	m_map(ta.m_map),
	m_attrib(ta.m_attrib)
{
	if(valid)
		registerInMap() ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
template <unsigned int ORBIT2>
AttributeHandler<T, ORBIT, MAP_IMPL>::AttributeHandler(const AttributeHandler<T, ORBIT2, MAP_IMPL>& h) :
	AttributeHandlerGen(h.valid),
	m_map(h.m_map),
	m_attrib(h.m_attrib)
{
	if(m_attrib->getOrbit() == ORBIT2)
	{
		if(valid)
			registerInMap() ;
	}
	else
		valid = false;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline AttributeHandler<T, ORBIT, MAP_IMPL>& AttributeHandler<T, ORBIT, MAP_IMPL>::operator=(const AttributeHandler<T, ORBIT, MAP_IMPL>& ta)
{
	if(valid)
		unregisterFromMap() ;
	m_map = ta.m_map ;
	m_attrib = ta.m_attrib ;
	valid = ta.valid ;
	if(valid)
		registerInMap() ;
	return *this ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
template <unsigned int ORBIT2>
inline AttributeHandler<T, ORBIT, MAP_IMPL>& AttributeHandler<T, ORBIT, MAP_IMPL>::operator=(const AttributeHandler<T, ORBIT2, MAP_IMPL>& ta)
{
	if(valid)
		unregisterFromMap() ;
	m_map = ta.map() ;
	m_attrib = ta.getDataVector() ;
	valid = ta.isValid() ;
	if(valid)
		registerInMap() ;
	return *this ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
AttributeHandler<T, ORBIT, MAP_IMPL>::~AttributeHandler()
{
	if(valid)
		unregisterFromMap() ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline AttributeMultiVector<T>* AttributeHandler<T, ORBIT, MAP_IMPL>::getDataVector() const
{
	return m_attrib ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline AttributeMultiVectorGen* AttributeHandler<T, ORBIT, MAP_IMPL>::getDataVectorGen() const
{
	return m_attrib ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline int AttributeHandler<T, ORBIT, MAP_IMPL>::getSizeOfType() const
{
	return sizeof(T) ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline unsigned int AttributeHandler<T, ORBIT, MAP_IMPL>::getOrbit() const
{
	return ORBIT ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline unsigned int AttributeHandler<T, ORBIT, MAP_IMPL>::getIndex() const
{
	return m_attrib->getIndex() ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline const std::string& AttributeHandler<T, ORBIT, MAP_IMPL>::name() const
{
	return m_attrib->getName() ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline const std::string& AttributeHandler<T, ORBIT, MAP_IMPL>::typeName() const
{
	return m_attrib->getTypeName();
}


template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline unsigned int AttributeHandler<T, ORBIT, MAP_IMPL>::nbElements() const
{
	return m_map->template getAttributeContainer<ORBIT>().size() ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline T& AttributeHandler<T, ORBIT, MAP_IMPL>::operator[](Cell<ORBIT> d)
{
	assert(valid || !"Invalid AttributeHandler") ;
	unsigned int a = m_map->template getEmbedding<ORBIT>(d) ;

	if (a == EMBNULL)
		a = m_map->template setOrbitEmbeddingOnNewCell<ORBIT>(d) ;

	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline const T& AttributeHandler<T, ORBIT, MAP_IMPL>::operator[](Cell<ORBIT> d) const
{
	assert(valid || !"Invalid AttributeHandler") ;
	unsigned int a = m_map->template getEmbedding<ORBIT>(d) ;
	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline T& AttributeHandler<T, ORBIT, MAP_IMPL>::operator[](unsigned int a)
{
	assert(valid || !"Invalid AttributeHandler") ;
	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline const T& AttributeHandler<T, ORBIT, MAP_IMPL>::operator[](unsigned int a) const
{
	assert(valid || !"Invalid AttributeHandler") ;
	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline unsigned int AttributeHandler<T, ORBIT, MAP_IMPL>::insert(const T& elt)
{
	assert(valid || !"Invalid AttributeHandler") ;
	unsigned int idx = m_map->template getAttributeContainer<ORBIT>().insertLine() ;
	m_attrib->operator[](idx) = elt ;
	return idx ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline unsigned int AttributeHandler<T, ORBIT, MAP_IMPL>::newElt()
{
	assert(valid || !"Invalid AttributeHandler") ;
	unsigned int idx = m_map->template getAttributeContainer<ORBIT>().insertLine() ;
	return idx ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline void AttributeHandler<T, ORBIT, MAP_IMPL>::setAllValues(const T& v)
{
	for(unsigned int i = begin(); i != end(); next(i))
		m_attrib->operator[](i) = v ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline unsigned int AttributeHandler<T, ORBIT, MAP_IMPL>::begin() const
{
	assert(valid || !"Invalid AttributeHandler") ;
	return m_map->template getAttributeContainer<ORBIT>().begin() ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline unsigned int AttributeHandler<T, ORBIT, MAP_IMPL>::end() const
{
	assert(valid || !"Invalid AttributeHandler") ;
	return m_map->template getAttributeContainer<ORBIT>().end() ;
}

template <typename T, unsigned int ORBIT, typename MAP_IMPL>
inline void AttributeHandler<T, ORBIT, MAP_IMPL>::next(unsigned int& iter) const
{
	assert(valid || !"Invalid AttributeHandler") ;
	m_map->template getAttributeContainer<ORBIT>().next(iter) ;
}

} //namespace CGoGN
