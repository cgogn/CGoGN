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

#ifndef _REGISTERED_ATTRIBUTE_H_
#define _REGISTERED_ATTRIBUTE_H_

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
	void setTypeName(const std::string& nameType) { m_name  = nameType; }

	/**
	 * recupere le nom de type affecté à l'attribut
	 */
	const std::string& getTypeName() { return m_name; }

	/**
	 * Ajout de l'attribut au container (A IMPLEMENTER)
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

} // namespace CGoGN

#endif // _REGISTERED_ATTRIBUTE_H_
