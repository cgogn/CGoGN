/*
 * registred.h
 *
 *  Created on: Jun 10, 2010
 *      Author: thery
 */

#ifndef _ATTRIBUTE_REGISTERED_H_
#define _ATTRIBUTE_REGISTERED_H_

namespace CGoGN
{

/**
 * Classe d'enregistrement d'attribut
 */
class RegisteredBaseAttribute
{
protected:
	std::string m_name; // TODO inutile ??

public:
	/**
	 * affecte un nom de type a l'attribut
	 */
	void setTypeName(const std::string& nameType) { m_name  = nameType;}

	/**
	 * recupere le nom de type affecté à l'attribut
	 */
	const std::string& getTypeName() { return m_name;}

	/**
	 * Ajout de l'attribut au container (A INMPLEMENTER)
	 */
//	virtual unsigned int addAttribute(AttribContainer& container, const std::string& attribName, unsigned int id)=0;

	/**
	 * Ajout de l'attribut au container (A INMPLEMENTER)
	 */
	virtual unsigned int addAttribute(AttribContainer& container, const std::string& attribName)=0;
};

/**
 * Version template de la classe d'enregistrement d'attribut
 * Fonction avec tout type qui répond aux contraintes suivantes:
 *  - I/O avec les flux (operator >> et <<)
 *  - copie mémoire directe utilisable (pas de pointeurs dans la structure)
 */
template<typename T>
class RegisteredAttribute : public RegisteredBaseAttribute
{
public:

	unsigned int addAttribute(AttribContainer& container, const std::string& attribName)
	{
		return container.addAttribute<T>(attribName);
	}
};

}

#endif /* REGISTRED_H_ */

