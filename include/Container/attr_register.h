/*
 * attr_register.h
 *
 *  Created on: May 28, 2010
 *      Author: thery
 */



#include <iostream>
#include <string>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "attrib_container.h"


namespace CGoGN
{

/**
 * Classe d'enregistrement d'attribut a
 */
class RegisteredAttribute
{
protected:
	std::string m_name;

public:
	/**
	 * affecte un nom de type a l'attribut
	 */
	void setTypeName(const std::string& nameType) { n_name  = nameType;}

	/**
	 * recupere le nom de type affecté à l'attribut
	 */
	const std::string& getTypeName() { return m_name;}
	
	/**
	 * Ajout de l'attribut au container (A INMPLEMENTER)
	 */
	virtual AttribContainer::Attr_Id addAttribute(AttribContainer& container, const std::string& attribName, unsigned int id)=0;

	/**
	 * Lecture en XML d'une donnée et la met dans le container (A INMPLEMENTER)
	 */
	virtual bool readXml(xmlNode* node, AttribContainer& container, unsigned int att_id, unsigned int index);

	/**
	 * Ecriture des elements dans un arbre XML (A INMPLEMENTER)
	 */
	virtual bool writeXml(xmlTextWriterPtr writer, const AttribContainer& container, unsigned int att_id);

	/**
	 * Lecture binaire (A INMPLEMENTER)
	 */
	virtual bool readBin(const std::fstream& fs, AttribContainer& container, unsigned int att_id);

	/**
	 * Ecriture binaire (A INMPLEMENTER)
	 */
	virtual bool writeBin(std::fstream& fs, const AttribContainer& container, unsigned int att_id);

};

// version template du type qui marche 
//
/**
 * Version template de la classe d'enregistrement d'attribut
 * Fonction avec tout type qui répond aux contraintes suivantes:
 *  - I/O avec les flux (operator >> et <<)
 *  - copie mémoire directe utilisable (pas de pointeurs dans la structure)
 */
template<typename T>
class RegisteredAttributeSimple : RegisteredAttribute
{

public:

	AttribContainer::Attr_Id addAttribute(AttribContainer& container, const std::string& attribName, unsigned int id)
	{
		container.addAttribute<T>(attribName, id);
	}
	
	void readXml(xmlNodePtr node, AttribContainer& container, unsigned int codeAttrib)
	{
		// recuperer le pseudo-tableau d'attribut
		AttribManager<T>& atm = container.getDataVector(codeAttrib);
		AttribContainer::iterator i = container.begin();
		
		std::stringstream ss;
		// lit tous les attributs dans le noeud xml et les inserts dans le container
		for (xmlNode* cur = node->children; cur!=NULL; cur = cur->next)
		{
			//std::string st((char*)(xmlNodeGetContent(cur)));
			ss.str( std::string((char*)(xmlNodeGetContent(cur))) );
			ss >> atm[i];
			container.next(i);
		}
	}


	void writeXml(xmlTextWriterPtr writer, const AttribContainer& container, unsigned int codeAttrib)
	{
		// recuperer le pseudo-tableau d'attribut
		const AttribManager<T>& atm = container.getDataVector(codeAttrib);

		std::stringstream ss;
		// parcours le tableau avec un iterateur du container et ecrit les elements
		for (AttribContainer::const_iterator i = container.begin(); i != container.end(); container.next(i))
		{
			ss << atm[i];
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "val ", (xmlChar*)(ss.str().c_str()));
			ss.str("");
		}
	}


	bool readBin(const std::fstream& fs, AttribContainer& container, unsigned int att_id)
	{
		// recupere les pointeurs et longueurs des zones contigues dans le container
		std::vector<T*> vect_adr;
		std::vector<unsigned int> vect_len;
		unsigned int nb = container.getAddresses<T>(att_id, vect_adr, vect_len);

		// lit les zones directement depuis le fichier
		for (int i=0;i<nb;++i)
			fs.read(reinterpret_cast<const char*>(vect_adr[i]),vect_len[i]*sizeof(T));
	}


	bool writeBin(std::fstream& fs, const AttribContainer& container, unsigned int att_id)
	{
		// recupere les pointeurs et longueurs des zones contigues dans le container
		std::vector<T*> vect_adr;
		std::vector<unsigned int> vect_len;
		unsigned int nb = container.getAddresses<T>(att_id, vect_adr, vect_len);

		// ecrit les zones directement dans le fichier
		for (int i=0;i<nb;++i)
			fs.write(reinterpret_cast<const char*>(vect_adr[i]),vect_len[i]*sizeof(T));
	}
};





template <typename R>
bool registerAttribute(const std::string &nameType)
{
	R* rc = new RegisteredAttributeSimple<R>;
	RegisteredAttribute* ra = dynamic_cast<RegisteredAttribute*>(rc);

	if (ra == NULL)
	{
		std::err << "Erreur enregistrement attribut"<<std::endl;
		return false;
	}

	ra->setTypeName(nametype);

	m_attributes_registry_map.insert(std::pair<std::string, RegisteredAttribute*>(nametype),ra);

}

}
// registerAttribute< Vec3f >(std::string("Vec3f"));
