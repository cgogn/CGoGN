
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
unsigned int AttribContainer::addAttribute(const std::string& attribName)
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
	AttribMultiVect<T>* ptr = new AttribMultiVect<T>(attribName, nametype);
	unsigned int idxAttrib = m_tableAttribs.size();
	// add it to table of attribut_manager
	m_tableAttribs.push_back(ptr);

	// add it in the map 
	if (attribName == "") // if no name, generate a fake name
	{
		std::stringstream ss;
		ss << "unknown" << m_nbunknown++;
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
unsigned int AttribContainer::addAttribute(const std::string& attribName, const std::string& nametype, unsigned int idxAttrib)
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
	AttribMultiVect<T>* ptr = new AttribMultiVect<T>(attribName, nametype);
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

///////////////////////////
// ACCES AUX DONNEES
///////////////////////////

template <typename T>
T&  AttribContainer::getData(unsigned int codeAttrib, unsigned int eltIdx)
{
	assert(eltIdx < m_maxSize && "Attribut non existant (indice trop grand)");
	assert(m_holesBlocks[eltIdx/_BLOCKSIZE_]->used(eltIdx%_BLOCKSIZE_)&&"Attribut non existant");
	assert((m_tableAttribs[codeAttrib]!=NULL)&&"Attribut detruit");

	AttribMultiVect<T>* atm = dynamic_cast< AttribMultiVect<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");

	return atm->operator[](eltIdx);
}

template <typename T>
const T&  AttribContainer::getData(unsigned int codeAttrib, unsigned int eltIdx) const
{
 	assert(eltIdx < m_maxSize && "Attribut non existant (indice trop grand)");
	assert(m_holesBlocks[eltIdx/_BLOCKSIZE_]->used(eltIdx%_BLOCKSIZE_)&&"Attribut non existant");
	assert((m_tableAttribs[codeAttrib]!=NULL)&&"Attribut detruit");

	AttribMultiVect<T>* atm = dynamic_cast< AttribMultiVect<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");

	return atm->operator[](eltIdx);
}

template <typename T>
void  AttribContainer::setData(unsigned int codeAttrib, unsigned int eltIdx,  const T& data)
{
	
 	assert(eltIdx < m_maxSize && "Attribut non existant (indice trop grand)");
	assert(m_holesBlocks[eltIdx/_BLOCKSIZE_]->used(eltIdx%_BLOCKSIZE_)&&"Attribut non existant");
	assert((m_tableAttribs[codeAttrib]!=NULL)&&"Attribut detruit");

	AttribMultiVect<T>* atm = dynamic_cast< AttribMultiVect<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");

	atm->operator[](eltIdx) = data;
}

//template <typename T>
//unsigned int AttribContainer::insertLineWidthData(unsigned int codeAttrib,const T& data)
//{
//	unsigned int it =  insertLine();
//	setData<T>(codeAttrib, it, data);
//	return it;
//}

template<typename T>
AttribMultiVect<T>& AttribContainer::getDataVector(unsigned int codeAttrib)
{
	assert((codeAttrib < m_tableAttribs.size()) && "Attribut inexistant");
	assert((m_tableAttribs[codeAttrib] != NULL) && "Attribut detruit");

	AttribMultiVect<T>* atm = dynamic_cast< AttribMultiVect<T>* >(m_tableAttribs[codeAttrib]);
	assert((atm!=NULL)&& "type attribut non concordant");
	return *atm;
}

 template<typename T>
bool AttribContainer::getAttributesVector(const std::string& attribName, AttribMultiVect<T>** ptr)
{
	MapNameId::iterator it = m_attribNameMap.find(attribName);
	if (it == m_attribNameMap.end())
		return false;
	
	int codeAttrib = it->second;

	if (m_tableAttribs[codeAttrib]==NULL) 
		return false;

	AttribMultiVect<T>* atm = dynamic_cast< AttribMultiVect<T>* >(m_tableAttribs[codeAttrib]);
	if (atm==NULL) 
		return false;

	*ptr = atm;
	return true;
}

///////////////////////////
//  ADRESSES MEMOIRES
///////////////////////////

template <typename T>
unsigned int AttribContainer::getAddresses(unsigned int attr, std::vector<T*>& vect_adr)
{
	AttribMultiVect<T>* atm = dynamic_cast< AttribMultiVect<T>* >(m_tableAttribs[attr]);
	assert((atm!=NULL)&& "type attribut non concordant");
	return atm->getStartAddresses(vect_adr);
}

//////////////////////////////
//  Enregistrement attributs
//////////////////////////////

//template <typename R>
//bool AttribContainer::registerAttribute(const std::string &nameType)
//{
//	RegisteredBasedAttribute* ra = new RegisteredAttribute<R>;
//	if (ra == NULL)
//	{
//		std::cerr << "Erreur enregistrement attribut"<<std::endl;
//		return false;
//	}
//
//	ra->setTypeName(nameType);
//
//	m_attributes_registry_map->insert(std::pair<std::string, RegisteredBasedAttribute*>(nameType,ra));
//	return true;
//}


//// INLINED FUNCTIONS
inline bool AttribContainer::used(unsigned int eltIdx) const
{
	return m_holesBlocks[ eltIdx / _BLOCKSIZE_ ]->used( eltIdx % _BLOCKSIZE_ );
}

inline void AttribContainer::setRegistry(std::map< std::string, RegisteredBaseAttribute* >* re)
{
	m_attributes_registry_map = re;
}

inline bool AttribContainer::copyAttribute(unsigned int index_dst, unsigned int index_src)
{
	return m_tableAttribs[index_dst]->copy(m_tableAttribs[index_src]);
}

inline bool AttribContainer::swapAttributes(unsigned int index1, unsigned int index2)
{
	return m_tableAttribs[index1]->swap(m_tableAttribs[index2]);
}

inline void AttribContainer::refLine(unsigned int eltIdx)
{
	m_holesBlocks[eltIdx / _BLOCKSIZE_]->ref(eltIdx % _BLOCKSIZE_);
}

inline void AttribContainer::unrefLine(unsigned int eltIdx)
{
	if (m_holesBlocks[eltIdx / _BLOCKSIZE_]->unref(eltIdx % _BLOCKSIZE_)) m_size--;
}

inline void AttribContainer::setRefLine(unsigned int eltIdx, unsigned int nb)
{
	m_holesBlocks[ eltIdx / _BLOCKSIZE_]->setNbRefs(eltIdx % _BLOCKSIZE_,nb);
}

inline AttribMultiVectGen& AttribContainer::getVirtualDataVector(unsigned int codeAttrib)
{
	return *(m_tableAttribs[indexAttr(codeAttrib)]);
}

inline unsigned int AttribContainer::end() const
{
	return m_maxSize;
}

inline unsigned int AttribContainer::begin() const
{
	unsigned int it = 0;
	while ((it < m_maxSize) && (!used(it)))
		++it;
	return it;
}

inline void AttribContainer::next(unsigned int &it) const
{
	do
	{
		++it;
	} while ((it < m_maxSize) && (!used(it)));
}

inline void AttribContainer::toggleProcess(unsigned int id)
{
	m_tableAttribs[indexAttr(id)]->toggleProcess();
}

inline void AttribContainer::toggleNoProcess(unsigned int id)
{
	m_tableAttribs[indexAttr(id)]->toggleNoProcess();
}

} // namespace CGoGN

