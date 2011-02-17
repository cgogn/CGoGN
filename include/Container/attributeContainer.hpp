
//#include <typeinfo>
//#include <stdio.h>
//#include <string.h>
//#include <libxml/encoding.h>
//#include <libxml/xmlwriter.h>

#include<iostream>
#include <cassert>
#include "Container/registered.h"
#include "Utils/nameTypes.h"

namespace CGoGN
{

///////////////////////////
// GESTION DES ATTRIBUTS
///////////////////////////

template <typename T>
unsigned int AttributeContainer::addAttribute(const std::string& attribName)
{
	std::string nametype = nameOfType(T());

	// first check if attribute already exist
	if (attribName != "")
	{
		MapNameId::iterator it = m_attribNameMap.find(attribName);
		if (it != m_attribNameMap.end())
		{
			return UNKNOWN;
		}
	}

	// new attribut
	AttributeMultiVector<T>* ptr = new AttributeMultiVector<T>(attribName, nametype);
	unsigned int idxAttrib = m_tableAttribs.size();
	// add it to table of attribut_manager
	m_tableAttribs.push_back(ptr);

	// add it in the map 
	if (attribName == "") // if no name, generate a fake name
	{
		std::stringstream ss;
		ss << "unknown" << m_nbUnknown++;
		m_attribNameMap.insert(std::pair<std::string, unsigned int>(ss.str(), idxAttrib));
	}
	else
		m_attribNameMap.insert(std::pair<std::string, unsigned int>(attribName, idxAttrib));

	// maj taille d'une ligne
	m_lineCost += sizeof(T);
	
	// resize the new attribute container to have same size than others
	int nbb = m_holesBlocks.size();
	ptr->setNbBlocks(nbb);

	m_nbAttributes++;

	return idxAttrib;
}

template <typename T>
unsigned int AttributeContainer::addAttribute(const std::string& attribName, const std::string& nametype, unsigned int idxAttrib)
{
	// first check if attribute already exist
	if (attribName != "")
	{
		MapNameId::iterator it = m_attribNameMap.find(attribName);
		if (it != m_attribNameMap.end())
		{
			return UNKNOWN;
		}
	}

	// new attribut
	AttributeMultiVector<T>* ptr = new AttributeMultiVector<T>(attribName, nametype);
	// add it to table of attribut_manager
	m_tableAttribs[idxAttrib] = ptr;
	// add it in the map
	m_attribNameMap.insert(std::pair<std::string, unsigned int>(attribName, idxAttrib));

	// maj taille d'une ligne
	m_lineCost += sizeof(T);

	// resize the new attribute container to have same size than others
	int nbb = m_holesBlocks.size();
	ptr->setNbBlocks(nbb);

	m_nbAttributes++;

	return idxAttrib;
}

inline unsigned int AttributeContainer::getNbAttributes()
{
	return m_tableAttribs.size();
}

///////////////////////////
// ACCES AUX DONNEES
///////////////////////////

template <typename T>
T& AttributeContainer::getData(unsigned int codeAttrib, unsigned int eltIdx)
{
	assert(eltIdx < m_maxSize && "Attribut non existant (indice trop grand)");
	assert(m_holesBlocks[eltIdx/_BLOCKSIZE_]->used(eltIdx%_BLOCKSIZE_)&&"Attribut non existant");
	assert((m_tableAttribs[codeAttrib]!=NULL)&&"Attribut detruit");

	AttributeMultiVector<T>* atm = dynamic_cast< AttributeMultiVector<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");

	return atm->operator[](eltIdx);
}

template <typename T>
const T& AttributeContainer::getData(unsigned int codeAttrib, unsigned int eltIdx) const
{
 	assert(eltIdx < m_maxSize && "Attribut non existant (indice trop grand)");
	assert(m_holesBlocks[eltIdx/_BLOCKSIZE_]->used(eltIdx%_BLOCKSIZE_)&&"Attribut non existant");
	assert((m_tableAttribs[codeAttrib]!=NULL)&&"Attribut detruit");

	AttributeMultiVector<T>* atm = dynamic_cast< AttributeMultiVector<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");

	return atm->operator[](eltIdx);
}

template <typename T>
void AttributeContainer::setData(unsigned int codeAttrib, unsigned int eltIdx,  const T& data)
{
	
 	assert(eltIdx < m_maxSize && "Attribut non existant (indice trop grand)");
	assert(m_holesBlocks[eltIdx/_BLOCKSIZE_]->used(eltIdx%_BLOCKSIZE_)&&"Attribut non existant");
	assert((m_tableAttribs[codeAttrib]!=NULL)&&"Attribut detruit");

	AttributeMultiVector<T>* atm = dynamic_cast< AttributeMultiVector<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");

	atm->operator[](eltIdx) = data;
}

//template <typename T>
//unsigned int AttributeContainer::insertLineWidthData(unsigned int codeAttrib,const T& data)
//{
//	unsigned int it =  insertLine();
//	setData<T>(codeAttrib, it, data);
//	return it;
//}

template<typename T>
AttributeMultiVector<T>& AttributeContainer::getDataVector(unsigned int codeAttrib)
{
	assert((codeAttrib < m_tableAttribs.size()) && "Attribut inexistant");
	assert((m_tableAttribs[codeAttrib] != NULL) && "Attribut detruit");

	AttributeMultiVector<T>* atm = dynamic_cast< AttributeMultiVector<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");
	return *atm;
}

template<typename T>
bool AttributeContainer::getAttributesVector(const std::string& attribName, AttributeMultiVector<T>** ptr)
{
	MapNameId::iterator it = m_attribNameMap.find(attribName);
	if (it == m_attribNameMap.end())
		return false;
	
	int codeAttrib = it->second;

	if (m_tableAttribs[codeAttrib]==NULL) 
		return false;

	AttributeMultiVector<T>* atm = dynamic_cast< AttributeMultiVector<T>* >(m_tableAttribs[codeAttrib]);
	if (atm==NULL) 
		return false;

	*ptr = atm;
	return true;
}

///////////////////////////
//  ADRESSES MEMOIRES
///////////////////////////

template <typename T>
unsigned int AttributeContainer::getAddresses(unsigned int attr, std::vector<T*>& vect_adr)
{
	AttributeMultiVector<T>* atm = dynamic_cast< AttributeMultiVector<T>* >(m_tableAttribs[attr]);
	assert((atm!=NULL)&& "type attribut non concordant");
	return atm->getStartAddresses(vect_adr);
}

//////////////////////////////
//  Enregistrement attributs
//////////////////////////////

//// INLINED FUNCTIONS
inline bool AttributeContainer::used(unsigned int eltIdx) const
{
	return m_holesBlocks[ eltIdx / _BLOCKSIZE_ ]->used( eltIdx % _BLOCKSIZE_ );
}

inline void AttributeContainer::setRegistry(std::map< std::string, RegisteredBaseAttribute* >* re)
{
	m_attributes_registry_map = re;
}

inline bool AttributeContainer::copyAttribute(unsigned int index_dst, unsigned int index_src)
{
	return m_tableAttribs[index_dst]->copy(m_tableAttribs[index_src]);
}

inline bool AttributeContainer::swapAttributes(unsigned int index1, unsigned int index2)
{
	return m_tableAttribs[index1]->swap(m_tableAttribs[index2]);
}

inline void AttributeContainer::refLine(unsigned int eltIdx)
{
	m_holesBlocks[eltIdx / _BLOCKSIZE_]->ref(eltIdx % _BLOCKSIZE_);
}

inline bool AttributeContainer::unrefLine(unsigned int eltIdx)
{
	if (m_holesBlocks[eltIdx / _BLOCKSIZE_]->unref(eltIdx % _BLOCKSIZE_))
	{
		m_size--;
		return true;
	}
	return false;
}

inline void AttributeContainer::setRefLine(unsigned int eltIdx, unsigned int nb)
{
	m_holesBlocks[ eltIdx / _BLOCKSIZE_]->setNbRefs(eltIdx % _BLOCKSIZE_,nb);
}

inline AttributeMultiVectorGen& AttributeContainer::getVirtualDataVector(unsigned int codeAttrib)
{
	return *(m_tableAttribs[indexAttr(codeAttrib)]);
}

inline unsigned int AttributeContainer::end() const
{
	return m_maxSize;
}

inline unsigned int AttributeContainer::begin() const
{
	unsigned int it = 0;
	while ((it < m_maxSize) && (!used(it)))
		++it;
	return it;
}

inline void AttributeContainer::next(unsigned int &it) const
{
	do
	{
		++it;
	} while ((it < m_maxSize) && (!used(it)));
}

inline void AttributeContainer::toggleProcess(unsigned int id)
{
	m_tableAttribs[indexAttr(id)]->toggleProcess();
}

inline void AttributeContainer::toggleNoProcess(unsigned int id)
{
	m_tableAttribs[indexAttr(id)]->toggleNoProcess();
}

} // namespace CGoGN
