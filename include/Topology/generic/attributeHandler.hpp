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

namespace CGoGN
{

template <typename T>
AttributeHandler<T>::AttributeHandler(GenericMap* m, AttributeMultiVector<T>* amv):
	m_map(m), m_attrib(amv)
{}

template <typename T>
AttributeHandler<T>::AttributeHandler():
	m_map(NULL), m_attrib(NULL)
{}

template <typename T>
AttributeHandler<T>::AttributeHandler(const AttributeHandler<T>& ta):
	m_map(ta.m_map), m_attrib(ta.m_attrib)
{}

template <typename T>
inline void AttributeHandler<T>::operator=(const AttributeHandler<T>& ta)
{
	m_map = ta.m_map ;
	m_attrib = ta.m_attrib ;
}

template <typename T>
AttributeHandler<T>::~AttributeHandler()
{}

template <typename T>
GenericMap* AttributeHandler<T>::map() const
{
	return m_map ;
}

template <typename T>
AttributeMultiVector<T>* AttributeHandler<T>::getDataVector() const
{
	return m_attrib ;
}

template <typename T>
unsigned int AttributeHandler<T>::getOrbit() const
{
	return m_attrib->getOrbit() ;
}

template <typename T>
unsigned int AttributeHandler<T>::getIndex() const
{
	return m_attrib->getIndex() ;
}

template <typename T>
const std::string& AttributeHandler<T>::name() const
{
	return m_attrib->getName() ;
}

template <typename T>
bool AttributeHandler<T>::isValid() const
{
	return !(m_map == NULL ||
			m_attrib == NULL ||
			m_attrib->getIndex() == AttributeContainer::UNKNOWN) ;
}

template <typename T>
inline T& AttributeHandler<T>::operator[](Dart d)
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	unsigned int orbit = m_attrib->getOrbit() ;
	unsigned int a = m_map->getEmbedding(orbit, d) ;

	if (a == EMBNULL)
		a = m_map->embedNewCell(orbit, d);

	return m_attrib->operator[](a);
}

template <typename T>
inline const T& AttributeHandler<T>::operator[](Dart d) const
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	unsigned int a = m_map->getEmbedding(m_attrib->getOrbit(), d) ;
	return m_attrib->operator[](a) ;
}

template <typename T>
inline T& AttributeHandler<T>::operator[](unsigned int a)
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	return m_attrib->operator[](a);
}

template <typename T>
inline const T& AttributeHandler<T>::operator[](unsigned int a) const
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	return m_attrib->operator[](a);
}

template <typename T>
inline unsigned int AttributeHandler<T>::insert(const T& elt)
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	unsigned int idx = m_map->getAttributeContainer(m_attrib->getOrbit()).insertLine();
	m_attrib->operator[](idx) = elt;
	return idx;
}

template <typename T>
inline unsigned int AttributeHandler<T>::newElt()
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	unsigned int idx = m_map->getAttributeContainer(m_attrib->getOrbit()).insertLine();
	return idx;
}

template <typename T>
inline void AttributeHandler<T>::setAllValues(T& v)
{
	for(unsigned int i = begin(); i != end(); next(i))
		m_attrib->operator[](i) = v ;
}

template <typename T>
inline unsigned int AttributeHandler<T>::begin() const
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	return m_map->getAttributeContainer(m_attrib->getOrbit()).begin();
}

template <typename T>
inline unsigned int AttributeHandler<T>::end() const
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	return m_map->getAttributeContainer(m_attrib->getOrbit()).end();
}

template <typename T>
inline void AttributeHandler<T>::next(unsigned int& iter) const
{
	assert(isValid() || !"Invalid AttributeHandler") ;
	m_map->getAttributeContainer(m_attrib->getOrbit()).next(iter);
}

} //namespace CGoGN
