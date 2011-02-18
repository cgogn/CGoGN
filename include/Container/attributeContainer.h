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

#ifndef __ATTRIBUTE_CONTAINER__
#define __ATTRIBUTE_CONTAINER__

#include "Container/sizeblock.h"
#include "Container/holeblockref.h"
#include "Container/attributeMultiVector.h"

#include <vector>
#include <map>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>

namespace CGoGN
{

class RegisteredBaseAttribute;

/**
* Container de stockage d'attributs par block de vecteur
* Les trous sont gérés automatiquement par ligne
* ie: on a tableau (multi-blocs) pour chaque attributs, mais
* Tous les tableaux ont le même nombre d'attributs et les 
* trous (et les indices) sont les mêmes pour tous les attributs
* On peut ajouter et enlever des attributs a la volée
*/
class AttributeContainer
{
protected:

	typedef std::map<std::string, unsigned int> MapNameId;

	/**
	* vecteur des blocs qui gerent les cases libres
	*/
	std::vector<HoleBlockRef*> m_holesBlocks;

	/**
	* tables of indices of block with free elts
	*/
	std::vector<unsigned int> m_tableBlocksWithFree;

	std::vector<unsigned int> m_tableBlocksEmpty;

	/**
	* vecteur de pointeur sur les gestionnaires d'attributs
	*/
	std::vector<AttributeMultiVectorGen*> m_tableAttribs;

	std::vector<unsigned int> m_freeIndices;

	/**
	* map de correspondance string / indice pour les attributs
	*/
	MapNameId m_attribNameMap;

//	std::vector<unsigned int> m_processAttribs;

	/**
	* nombre d'attributs
	*/ 
	unsigned int m_nbAttributes;

	/**
	 * counter for attributes without name
	 */
	unsigned int m_nbUnknown;

	/**
	* size (number of elts) of the container
	*/
	unsigned int m_size;

	/**
	* size of the container with holes
	*/
	unsigned int m_maxSize;

	/**
	* Cout memoire d'une ligne
	*/
	unsigned int m_lineCost;

	/**
	 * load xmlpart of container
	 */
	bool loadXmlHB(xmlNodePtr node);

	/**
	 * load xmlpart of container
	 */
	bool loadXmlBWF(xmlNodePtr node);

	/**
	 * load xmlpart of container
	 */
	bool loadXmlAN(xmlNodePtr node, unsigned int nbb);

	/**
	 * load xmlpart of container
	 */
	bool loadXmlDL(xmlNodePtr node);

	/**
	 * map ptr (shared for all container of the same map) for attribute registry
	 */
	std::map< std::string, RegisteredBaseAttribute* >* m_attributes_registry_map;

public:
	/**
	* constante d'attribut inconnu
	*/
	static const unsigned int UNKNOWN = 0xffffffff;

	/**
	* Taille du bloc
	*/
	static const unsigned int BlockSize = _BLOCKSIZE_;

	/**
	* constructor
	*/
	AttributeContainer();

	/**
	* destructor
	*/
	~AttributeContainer();

	/**
	* is the line used in the container
	*/
	inline bool used(unsigned int eltIdx) const;

	/**
	 * set the registry map
	 */
	inline void setRegistry(std::map< std::string, RegisteredBaseAttribute* >* re);

	/**
	 * swapping
	 */
	void swap(AttributeContainer&);

	/**
	 * merge
	 */
	void merge(AttributeContainer& cont);

	/**
	 * extract orbit from code (orbit & id)
	 */
	inline static unsigned int orbitAttr(unsigned int idAttr)
	{
		return idAttr >> 24;
	}

	/**
	 * extract index of attribute in container from code (orbit & id)
	 */
	inline static unsigned int indexAttr(unsigned int idAttr)
	{
		return idAttr & 0x00ffffff;
	}

	/**
	 * create an attrib Id based on the orbit and the index
	 */
	inline static unsigned int attrId(unsigned int orbit, unsigned int index)
	{
		return (orbit << 24) | index ;
	}

	/**
	* ajout d'un attribut
	* @param T (template) type de l'attribut a ajouter
	* @return the id of attribute
	*/
	template <typename T>
	unsigned int addAttribute(const std::string& attribName);

	/**
	* ajout d'un attribut avec identificateur (pour le chargement)
	* @param T (template) type de l'attribut a ajouter
	* @param attribName nom de l'attribut
	* @param id index de l'attribut
	*/
	template <typename T>
	unsigned int addAttribute(const std::string& attribName, const std::string& nametype, unsigned int id);

	bool copyAttribute(unsigned int index_dst, unsigned int index_src);

	bool swapAttributes(unsigned int index1, unsigned int index2);

	/**
	* recuperation du code d'un attribut
	* @param attribName nom de l'attribut
	* @return l'indice de l'attribut 
	*/
	unsigned int getAttribute(const std::string& attribName);

	/**
	* Remove an attribute (destroys data)
	* @param attribName name of the attribute to remove
	*/	
	bool removeAttribute(const std::string& attribName);

	/**
	* Remove an attribute (destroys data)
	* @param index index of the attribute to remove
	* @return removed or not
	*/
	bool removeAttribute(unsigned int index);

	/**
	* teste si un attribut est valide (encore dans la map de noms)
	* @param attr attribut a tester
	* @return vrai attribut encore utilise dans le container
	*/	
	bool isValidAttribute(unsigned int attr);
	
	/**
	* remplit un vecteur avec les noms des attributs du container.
	* @param strings (OUT) tableau des noms d'attributs
	* @return le nombre d'attributs
	*/	
	unsigned int getAttributesNames(std::vector<std::string>& names);

	/**
	 * get the name of an attribute, given its index in the container
	 */
	const std::string& getAttributeName(unsigned int attrIndex);

	/**
	 * return the number of attributes of the container
	 */
	unsigned int getNbAttributes();

	/**
	* insert une ligne vide dans tous les tableaux (compteur de referencements = 0)
	* @return retourne un iterator sur la ligne (indice uint)
	*/
	unsigned int insertLine();

	/**
	 *
	 */
	void initLine(unsigned int idx);

	/**
	* Supprime une ligne d'attribut
	* @param eltIdx iterator sur cette ligne
	*/
	void removeLine(unsigned int eltIdx);

	/**
	 *
	 */
	void copyLine(unsigned int dstIdx, unsigned int srcIdx);

	/**
	* Incremente le compteur de reference de la ligne
	* @param eltIdx iterator sur cette ligne
	*/
	void refLine(unsigned int eltIdx);

	/**
	* Decremente le compteur de reference de la ligne
	* @param eltIdx iterator sur cette ligne
	*/
	bool unrefLine(unsigned int eltIdx);

	void setRefLine(unsigned int eltIdx, unsigned int nb);

	/**
	* retourne une reference sur un attribut
	* @param T type de l'attribut a recuperer
	* @param codeAttrib code de l'attribut
	* @param eltIdx indice de la ligne
	* @return le pointeur sur l'attribut recherche
	*/
	template <typename T>
	T& getData(unsigned int codeAttrib, unsigned int eltIdx);

	/**
	* retourne une reference constnate sur un attribut
	* @param T type de l'attribut a recuperer
	* @param codeAttrib code de l'attribut
	* @param eltIdx indice de la ligne
	* @return le pointeur sur l'attribut recherche
	*/
	template <typename T>
	const T& getData(unsigned int codeAttrib, unsigned int eltIdx) const;

	/**
	* affecte un attribut dans le container
	* @param T type de l'attribut a affecter, compile souvent sans le mettre, mais danger: (cast implicites sur plusieurs niveaux de template ...)
	* @param codeAttrib code de l'attribut
	* @param eltIdx indice de la ligne
	* @param data attribut a inserer
	*/
	template <typename T>
	void setData(unsigned int codeAttrib, unsigned int eltIdx, const T& data);

	/**
	* Renvoit un pseudo vecteur d'attribut (operateur []) 
	* Attention plantage assure si l'attribut n'existe pas
	* A utiliser apres un getAttribute et test du resultat
	* @param codeAttrib code de l'attribut
	*/
	template<typename T>
	AttributeMultiVector<T>& getDataVector(unsigned int codeAttrib);

	AttributeMultiVectorGen& getVirtualDataVector(unsigned int codeAttrib);

	/**
	* Renvoit un pseudo vecteur d'attribut (operateur []) 
	* Attention plantage assure si l'attribut n'existe pas
	* @param name nom de l'attribut
	*/
// 	template<typename T>
// 	AttributeMultiVector<T>& getAttributesVector(const std::string& name);

	/**
	* Renvoit un pseudo vecteur d'attribut (operateur []) 
	* Avantages: pas de plantage et pas besoin de mettre le parametre template si le type ptr est ok
	* @param name nom de l'attribut
	* @param ptr pointeur de pointeur du container
	* @return vrai si OK faux si pb
	*/
	template<typename T>
	bool getAttributesVector(const std::string& name, AttributeMultiVector<T>** ptr);

	/**
	* insert un attribut dans le container la ligne est crée
	* @param T type de l'attribut a affecter (si le type de data est deductible pas besoin de le mettre)
	* @param codeAttrib code de l'attribut
	* @param data attribut a inserer
	* @return l'indice de la ligne crée
	*/
//	template <typename T>
//	unsigned int insertLineWidthData(unsigned int codeAttrib,const T& data);

	/**
	* iterateur begin
	*/
	unsigned int begin() const;

	/**
	*  iterateur end (attention: fait un petit calcul)
	*/
	unsigned int end() const;

	/**
	* applique next sur un iterateur. ATTENTION DOIT ETRE UTILISE A LA PLACE DE ++
	* @param it iterateur a incrementer
	*/	
	void next(unsigned int &it) const;

	/**
	* Taille du container en nombre de lignes
	*/
	unsigned int size();

	/**
	* Taille du container en nombre de lignes en memoire (avec les trous)
	*/
	unsigned int capacity();

	/**
	* Cout memoire total approximatif
	*/
	unsigned int memoryTotalSize() ;

	/**
	* Nombre de reference d'une ligne
	*/
	unsigned int nbRefs(unsigned int it);

	/**
	* compactage du container:
	* @param mapOldNew une table qui represente une map anciens_indices -> nouveaux_indices. Les trous ont la valeur 0xffffffff.
	*/
	void compact(std::vector<unsigned int>& mapOldNew);

	/**
	* clear the container
	* @param clearAttrib detruit les attributs ?
	*/
	void clear(bool clearAttrib=false);

	/**
	* Recupere les pointeurs sur les premiers elements de chaque bloc d'un attribut
	* @param attr attribut 
	* @param vect_adr (OUT) vecteur de pointeurs
	* @param vect_adr (OUT) vecteur de tailles (en nombre d'elts)
	* @return nombres de pointeurs
	*/
	template<typename T>
	unsigned int getAddresses(unsigned int attr, std::vector<T*>& vect_adr);

	/**
	* save Xml file
	* @param writer a xmlTextWriterPtr obj
	* @param id the id to save
	*/
	void saveXml(xmlTextWriterPtr writer, unsigned int id);

	/**
	* get id from xml node
	* @param node the node of container node of xml tree
	* @return the value of id of the node
	*/
	static unsigned int getIdXmlNode(xmlNodePtr node);

	/**
	* load from xml node
	* @param node the node of container node of xml tree
	*/
	bool loadXml(xmlNodePtr node);

	/**
	* get id from file binary stream
	* @param fs file stream
	* @return the id of attribute container
	*/
	static unsigned int loadBinId(CGoGNistream& fs);

	/**
	* load from binary file
	* @param fs a file stream
	* @param id  ??
	*/
	bool loadBin(CGoGNistream& fs);

	/**
	* save binary file
	* @param fs a file stream
	* @param id the id to save
	*/
	void saveBin(CGoGNostream& fs, unsigned int id);

	/**
	 *  reserve and clear list of attribute to process
	 *  @param nb max number of attributes to add to the list (just for optimization)
	 */
//	void clearProcessList(unsigned int nb) { m_processAttribs.reserve(nb); m_processAttribs.clear();}

	/**
	 * Toggle an attribute to process
	 * @param id the id (or code) of attribute to add
	 */
	void toggleProcess(unsigned int id);

	/**
	 * Toggle an attribute to process
	 * @param id the id (or code) of attribute to add
	 */
	void toggleNoProcess(unsigned int id);

	/**
	 * copy process attributes of line j in line i
	 * @param i line to modify
	 * @param j line to copy to i
	 */
	void affect(unsigned int i, unsigned int j);

	/**
	 * add process attributes of line j to line i
	 * @param i line to modify
	 * @param j line to add to i
	 */
	void add(unsigned int i, unsigned int j);

	/**
	 * sub process attributes of line j from line i
	 * @param i line to modify
	 * @param j line to sub from i
	 */
	void sub(unsigned int i, unsigned int j);

	/**
	 * multiy process attributes of line j by a scalar
	 * @param i line to multiply
	 * @param alpha scalar
	 */
	void mult(unsigned int i, double alpha);

	/**
	 * multiy process attributes of line j by a scalar
	 * @param i line to multiply
	 * @param alpha scalar
	 */
	void div(unsigned int i, double alpha);

	/**
	 * interpole process attributes A_res = alpha*A_i + (1-alpha)*A_j
	 * @param res result line
	 * @param i first line
	 * @param j second line
	 * @param alpha coefficient of interpolation
	 */
	void lerp(unsigned res, unsigned int i, unsigned int j, double alpha);
};

} // namespace CGoGN

#include "attributeContainer.hpp"

#endif
