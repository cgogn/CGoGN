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

#ifndef __ATTRIBUTE_MULTI_VECTOR__
#define __ATTRIBUTE_MULTI_VECTOR__

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#include <typeinfo>

#include "Container/sizeblock.h"

namespace CGoGN
{

class AttributeMultiVectorGen
{
protected:
	std::string m_attName;

	std::string m_typeName;

	bool m_toProcess;

public:

	AttributeMultiVectorGen(const std::string& strName, const std::string& strType);

	AttributeMultiVectorGen();

 	virtual ~AttributeMultiVectorGen();

 	void toggleProcess();

 	void toggleNoProcess();

 	bool toProcess();

 	unsigned int BlockSize();

 	/**
 	 * construct a new object using this type
 	 */
 	virtual AttributeMultiVectorGen* new_obj()=0;

	virtual bool copy(const AttributeMultiVectorGen* atmvg)=0;

	virtual bool swap(AttributeMultiVectorGen* atmvg)=0;

	/**
	* add a vector in table
	*/
	virtual void addBlock()=0;

	/**
	* set the number of block
	*/
	virtual void setNbBlocks(unsigned int nbt)=0;

	/**
	* swap two element in the attribute container
	*/
	virtual void overwrite(unsigned int src_b, unsigned int src_id, unsigned int dst_b, unsigned int dst_id)=0;

	/**
	* Free the used memory
	*/
	virtual void free()=0;

	virtual void initElt(unsigned int id)=0;

	virtual void copyElt(unsigned int dst, unsigned int src)=0;

	/**
	* get name of the type of attribute
	*/
	const std::string& getTypeName() { return m_typeName;}

	const std::string& getName() { return m_attName;}

	virtual void saveBin(CGoGNostream& fs, unsigned int id)=0;

	static unsigned int loadBinInfos(CGoGNistream& fs, std::string& name, std::string& type);

	virtual bool loadBin(CGoGNistream& fs)=0;

	static bool skipLoadBin(CGoGNistream& fs);

	virtual std::string output(unsigned int i)=0;

	virtual void input(unsigned int i,const std::string& st)=0;

	virtual bool swap(AttributeMultiVectorGen& att)=0;

	virtual bool merge(const AttributeMultiVectorGen& att)=0;

	virtual void addBlocksBefore(unsigned int nbb)=0;

	virtual unsigned int getStartAddresses(std::vector<void*>& addr, unsigned int& byteTableSize)=0;

	/**
	 * copy attribute j on i
	 */
	virtual void affect(unsigned int i, unsigned int j)=0;

	/**
	 * add attribute j to i
	 */
	virtual void add(unsigned int i, unsigned int j)=0;

	/**
	 * sub attribute j from i
	 */
	virtual void sub(unsigned int i, unsigned int j)=0;

	/**
	 * multiply attribute i by alpha
	 */
	virtual void mult(unsigned int i, double alpha)=0;

	/**
	 * divide attribute i by alpha
	 */
	virtual void div(unsigned int i, double alpha)=0;

	/**
	 * interpole attribute i and j in res (with alpha coef)
	 */
	virtual void lerp(unsigned res, unsigned int i, unsigned int j, double alpha)=0;

	/**
	 * load info of binary stream
	 * @param fs filestream
	 * @param name [out] name of attribute
	 * @param type [out] type of attribute
	 */
};

template <typename T>
class AttributeMultiVector:  public AttributeMultiVectorGen
{
	/**
	* table of blocks of data ptr: vectors!
	*/
	std::vector< T* > m_tableData;


public:

	AttributeMultiVector(const std::string& strName, const std::string& strType);

	AttributeMultiVector();

	~AttributeMultiVector();

	bool copy(const AttributeMultiVectorGen* atmvg);

	bool swap(AttributeMultiVectorGen* atmvg);

	AttributeMultiVectorGen* new_obj();

	bool swap(AttributeMultiVectorGen& att);

	bool merge(const AttributeMultiVectorGen& att);

	/**
	* get a reference on a elt
	* @param i index of element
	*/
	T& operator[](unsigned int i);
	/**
	* get a const reference on a elt
	* @param i index of element
	*/
	const T& operator[](unsigned int i) const ;

	/**
	 *  export en string d'un element
	 *  @param i index de l'element a sortie
	 */
	std::string output(unsigned int i);

	/**
	 *  import d'un element depuis une string
	 *  @param i index de l'element a importer
	 *  @param st string contenant l'element a importer
	 */
	void input(unsigned int i,const std::string& st);

	/**
	* add a block of data in the attribute container
	*/
	void addBlock();

	void initElt(unsigned int id);

	void copyElt(unsigned int dst, unsigned int src);

	/**
	* set the number of block of the container
	* Usefull when we add a new attribute in not empty container
	* @param nbb the number of block to set
	*/
	void setNbBlocks(unsigned int nbb);

	void addBlocksBefore(unsigned int nbb);

	/**
	* swap to elements in container (usefull for compact function)
	* @param src_b  block index of source element
	* @param src_id index in block of source element
	* @param dst_b  block index of destination element
	* @param dst_id index in block of destination element
	*/
	void overwrite(unsigned int src_b, unsigned int src_id, unsigned int dst_b, unsigned int dst_id);

	/**
	* Free the used memory
	*/
	void free();

	/**
	 * Get the addresses of each block of data
	 */
	unsigned int getStartAddresses(std::vector<void*>& addr, unsigned int& byteTableSize);

	/**
	 * Sauvegarde binaire
	 * @param fs filestream
	 * @param id id of mv
	 */
	void saveBin(CGoGNostream& fs, unsigned int id);


	/**
	 * lecture binaire
	 * @param fs filestream
	 */
	bool loadBin(CGoGNistream& fs);


	// Processing attributes

	void affect(unsigned int i, unsigned int j);

	void add(unsigned int i, unsigned int j);

	void sub(unsigned int i, unsigned int j);

	void mult(unsigned int i, double alpha);

	void div(unsigned int i, double alpha);

	void lerp(unsigned res, unsigned int i, unsigned int j, double alpha);
};

}

#include "attributeMultiVector.hpp"

#endif
