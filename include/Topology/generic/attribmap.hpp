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

namespace CGoGN
{

template <typename T>
inline AttributeHandler<T> AttribMap::addAttribute(unsigned int orbit, const std::string& nameAttr)
{
	if(!isOrbitEmbedded(orbit))
		addEmbedding(orbit) ;
	AttributeMultiVector<T>* amv = m_attribs[orbit].addAttribute<T>(nameAttr) ;
	return AttributeHandler<T>(this, amv) ;
}

template <typename T>
inline bool AttribMap::removeAttribute(AttributeHandler<T>& attr)
{
	assert(attr.isValid() || !"Invalid attribute handler") ;
	return m_attribs[attr.getOrbit()].removeAttribute<T>(attr.getIndex()) ;
}

template <typename T>
inline AttributeHandler<T> AttribMap::getAttribute(unsigned int orbit, const std::string& nameAttr)
{
	AttributeMultiVector<T>* amv = m_attribs[orbit].getDataVector<T>(nameAttr) ;
	return AttributeHandler<T>(this, amv) ;
}

template <typename T>
inline bool AttribMap::swapAttributes(AttributeHandler<T>& attr1, AttributeHandler<T>& attr2)
{
	assert((attr1.isValid() && attr2.isValid()) || !"Invalid attribute handler") ;
	assert(attr1.getOrbit() == attr2.getOrbit() || !"Cannot swap attributes of different orbits") ;
	unsigned int orbit = attr1.getOrbit() ;
	unsigned int index1 = attr1.getIndex() ;
	unsigned int index2 = attr2.getIndex() ;
	if(index1 != index2)
		return m_attribs[orbit].swapAttributes(index1, index2) ;
	return false ;
}

template <typename T>
inline bool AttribMap::copyAttribute(AttributeHandler<T>& dst, AttributeHandler<T>& src)
{
	assert((dst.isValid() && src.isValid()) || !"Invalid attribute handler") ;
	unsigned int orbit = dst.getOrbit() ;
	assert(orbit == src.getOrbit() || !"Cannot copy attributes of different orbits") ;
	unsigned int index_dst = dst.getIndex() ;
	unsigned int index_src = src.getIndex() ;
	if(index_dst != index_src)
		return m_attribs[orbit].copyAttribute(index_dst, index_src) ;
	return false ;
}

inline unsigned int AttribMap::getNbCells(unsigned int orbit)
{
	return this->m_attribs[orbit].size() ;
}

/****************************************
 *  TOPOLOGICAL ATTRIBUTES MANAGEMENT   *
 ****************************************/

inline AttributeMultiVector<Dart>* AttribMap::addRelation(const std::string& name)
{
	AttributeContainer& cont = m_attribs[DART] ;
	AttributeMultiVector<Dart>* amv = cont.addAttribute<Dart>(name) ;

	// set new relation to fix point for all the darts of the map
	for(unsigned int i = cont.begin(); i < cont.end(); cont.next(i))
		amv->operator[](i) = i ;

	return amv ;
}

} // namespace CGoGN
