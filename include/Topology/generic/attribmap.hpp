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
	AttributeContainer& cellCont = m_attribs[orbit] ;
	unsigned int index = cellCont.addAttribute<T>(nameAttr) ;
	unsigned int idAttr = AttributeContainer::attrId(orbit, index) ; // assume here that we have less than 16M attributes and 255 orbits!!
	return AttributeHandler<T>(this, idAttr) ;
}

template <typename T>
inline bool AttribMap::removeAttribute(AttributeHandler<T>& attr)
{
	assert(attr.isValid() || !"Invalid attribute handler") ;
	unsigned int idAttr = attr.id() ;
	return m_attribs[AttributeContainer::orbitAttr(idAttr)].removeAttribute(AttributeContainer::indexAttr(idAttr)) ;
}

template <typename T>
inline AttributeHandler<T> AttribMap::getAttribute(unsigned int orbit, const std::string& nameAttr)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	unsigned int index = m_attribs[orbit].getAttribute(nameAttr) ;
	return AttributeHandler<T>(this, AttributeContainer::attrId(orbit, index)) ;
}

template <typename T>
inline bool AttribMap::swapAttributes(AttributeHandler<T>& attr1, AttributeHandler<T>& attr2)
{
	assert((attr1.isValid() && attr2.isValid()) || !"Invalid attribute handler") ;
	assert(AttributeContainer::orbitAttr(attr1.id()) == AttributeContainer::orbitAttr(attr2.id()) || !"Cannot swap attributes of different orbits") ;
	unsigned int orbit = AttributeContainer::orbitAttr(attr1.id()) ;
	unsigned int index1 = AttributeContainer::indexAttr(attr1.id()) ;
	unsigned int index2 = AttributeContainer::indexAttr(attr2.id()) ;
	if(index1 != index2)
		return m_attribs[orbit].swapAttributes(index1, index2) ;
	return false ;
}

template <typename T>
inline bool AttribMap::copyAttribute(AttributeHandler<T>& dst, AttributeHandler<T>& src)
{
	assert((dst.isValid() && src.isValid()) || !"Invalid attribute handler") ;
	unsigned int orbit = AttributeContainer::orbitAttr(dst.id()) ;
	assert(orbit == AttributeContainer::orbitAttr(src.id()) || !"Cannot copy attributes of different orbits") ;
	unsigned int index_dst = AttributeContainer::indexAttr(dst.id()) ;
	unsigned int index_src = AttributeContainer::indexAttr(src.id()) ;
	if(index_dst != index_src)
		return m_attribs[orbit].copyAttribute(index_dst, index_src) ;
	return false ;
}

inline unsigned int AttribMap::getNbCells(unsigned int orbit)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	return this->m_attribs[orbit].size() ;
}

/****************************************
 *  TOPOLOGICAL ATTRIBUTES MANAGEMENT   *
 ****************************************/

inline AttributeMultiVector<Dart>* AttribMap::addRelation(const std::string& name)
{
	AttributeContainer& cont = m_attribs[DART_ORBIT] ;

	unsigned int index = cont.addAttribute<Dart>(name) ;
	AttributeMultiVector<Dart>& amv = cont.getDataVector<Dart>(index) ;

	// set new relation to fix point for all the darts of the map
	for(unsigned int i = cont.begin(); i < cont.end(); cont.next(i))
		amv[i] = i ;

	return &amv ;
}

} // namespace CGoGN
