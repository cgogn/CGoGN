/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/
#include <cstring>

namespace CGoGN
{

class MarkerBool
{
public:
	std::string CGoGNnameOfType() const
	{
		return "MarkerBool";
	}
};



//template <>
//class AttributeMultiVector<MarkerBool> : public AttributeMultiVectorGen
//{
//	/**
//	* table of blocks of data pointers: vectors!
//	*/
//	std::vector< unsigned char* > m_tableData;

//public:
//	AttributeMultiVector(const std::string& strName, const std::string& strType):
//		AttributeMultiVectorGen(strName, strType)
//	{
//		m_tableData.reserve(1024);
//	}

//	AttributeMultiVector()
//	{
//		m_tableData.reserve(1024);
//	}

//	~AttributeMultiVector() {}

//	AttributeMultiVectorGen* new_obj()
//	{
//		AttributeMultiVectorGen* ptr = new AttributeMultiVector<MarkerBool>;
//		ptr->setTypeName(m_typeName);
//		return ptr;
//	}

//	/**************************************
//	 *       MULTI VECTOR MANAGEMENT      *
//	 **************************************/

//	void addBlock()
//	{
//		unsigned char* ptr = new unsigned char[_BLOCKSIZE_];
//		memset(ptr,0,_BLOCKSIZE_);
//		m_tableData.push_back(ptr);
//	}

//	void setNbBlocks(unsigned int nbb)
//	{
//		if (nbb >= m_tableData.size())
//		{
//			for (unsigned int i= m_tableData.size(); i <nbb; ++i)
//				addBlock();
//		}
//		else
//		{
//			for (unsigned int i = m_tableData.size()-1; i>=nbb; --i)
//				delete[] m_tableData[i];
//			m_tableData.resize(nbb);
//		}
//	}


//	unsigned int getNbBlocks() const
//	{
//		return m_tableData.size();
//	}

////	void addBlocksBefore(unsigned int nbb);

//	bool copy(const AttributeMultiVectorGen* atmvg)
//	{
//		const AttributeMultiVector<MarkerBool>* atmv = dynamic_cast<const AttributeMultiVector<MarkerBool>*>(atmvg);

//		if (atmv == NULL)
//		{
//			CGoGNerr << "trying to copy attributes of different type" << CGoGNendl;
//			return false;
//		}
////		if (atmv->m_typeName != m_typeName)
////		{
////			CGoGNerr << "trying to copy attributes with different type names" << CGoGNendl;
////			return false;
////		}

//		m_tableData.resize( atmv->m_tableData.size() );
//		for (unsigned int i = 0; i < atmv->m_tableData.size(); ++i)
//			m_tableData[i] = atmv->m_tableData[i];

//		return true;
//	}

//	bool swap(AttributeMultiVectorGen* atmvg)
//	{
//		AttributeMultiVector<MarkerBool>* atmv = dynamic_cast<AttributeMultiVector<MarkerBool>*>(atmvg);

//		if (atmv == NULL)
//		{
//			CGoGNerr << "trying to swap attributes of different type" << CGoGNendl;
//			return false;
//		}
//		if (atmv->m_typeName != m_typeName)
//		{
//			CGoGNerr << "trying to swap attributes with different type names" << CGoGNendl;
//			return false;
//		}

//		m_tableData.swap(atmv->m_tableData) ;
//		return true;
//	}

//	bool merge(const AttributeMultiVectorGen& att)
//	{
//		const AttributeMultiVector<MarkerBool>* attrib = dynamic_cast< const AttributeMultiVector<MarkerBool>* >(&att);
//		if (attrib==NULL)
//		{
//			CGoGNerr << "trying to merge attributes of different type" << CGoGNendl;
//			return false;
//		}

//		if (attrib->m_typeName != m_typeName)
//		{
//			CGoGNerr << "trying to merge attributes with different type names" << CGoGNendl;
//			return false;
//		}

//		for (auto it = attrib->m_tableData.begin(); it != attrib->m_tableData.end(); ++it)
//			m_tableData.push_back(*it);

//		return true;
//	}

//	void clear()
//	{
//		m_tableData.clear();
//	}

//	int getSizeOfType() const
//	{
//		return sizeof(bool); // ?
//	}

//	void allFalse()
//	{
////		std::cout << "Marker "<<this->getName()<<" - allFalse"<< std::endl;
//		for (unsigned int i = 0; i < m_tableData.size(); ++i)
//		{
//			memset(m_tableData[i],0,_BLOCKSIZE_);
//		}
//	}

//	/**************************************
//	 *             DATA ACCESS            *
//	 **************************************/


//	void setFalse(unsigned int i)
//	{
//		m_tableData[i / _BLOCKSIZE_][i % _BLOCKSIZE_] = 0;

////		std::cout << "Marker "<<this->getName()<<" - ";
////		std::cout << "SET FALSE: "<< this->operator[](i)<< std::endl;
//	}

//	void setTrue(unsigned int i)
//	{
//		m_tableData[i / _BLOCKSIZE_][i % _BLOCKSIZE_] = 1;

////		std::cout << "Marker "<<this->getName()<<" - ";
////		std::cout << "SET TRUE: "<< this->operator[](i)<< std::endl;
//	}

//	void setVal(unsigned int i, bool b)
//	{
//		m_tableData[i / _BLOCKSIZE_][i % _BLOCKSIZE_] = b;

////		std::cout << "Marker "<<this->getName()<<" - ";
////		std::cout << "SET VAL: "<< b << " -> "<< this->operator[](i)<< std::endl;
//	}

//	/**
//	 * get a const reference on a elt
//	 * @param i index of element
//	 */
//	bool operator[](unsigned int i) const
//	{
////		std::cout << "BITS="<<std::hex<< m_tableData[jj][x]<< " MASK="<<mask<<" ~MASK="<<~mask<<std::endl;
//		return m_tableData[i / _BLOCKSIZE_][i % _BLOCKSIZE_] !=0;
//	}

//	/**
//	 * Get the addresses of each block of data
//	 */
//	unsigned int getBlocksPointers(std::vector<void*>& addr, unsigned int& /*byteBlockSize*/) const
//	{
//		CGoGNerr << "DO NOT USE getBlocksPointers with bool attribute"<< CGoGNendl;
//		return 0;
////		addr.reserve(m_tableData.size());
////		addr.clear();

////		for (auto it = m_tableData.begin(); it != m_tableData.end(); ++it)
////			addr.push_back(NULL );

////		return addr.size();
//	}


//	/**************************************
//	 *          LINES MANAGEMENT          *
//	 **************************************/

//	void initElt(unsigned int id)
//	{
////		std::cout << "Marker "<<this->getName()<<" - InitElt"<< std::endl;
//		setFalse(id);
//	}

//	void copyElt(unsigned int dst, unsigned int src)
//	{
////		std::cout << "Marker "<<this->getName()<<" - CopyElt"<< std::endl;
//		setVal(dst,this->operator [](src));
//	}


//	void swapElt(unsigned int id1, unsigned int id2)
//	{
////		std::cout << "Marker "<<this->getName()<<" - SwapElt"<< std::endl;
//		bool data = this->operator [](id1);
//		setVal(id1,this->operator [](id2));
//		setVal(id2,data);
//	}

//	/**
//	* swap two elements in container (useful for compact function)
//	* @param src_b  block index of source element
//	* @param src_id index in block of source element
//	* @param dst_b  block index of destination element
//	* @param dst_id index in block of destination element
//	*/
//	void overwrite(unsigned int src_b, unsigned int src_id, unsigned int dst_b, unsigned int dst_id)
//	{
////		std::cout << "Marker "<<this->getName()<<" - overwrite"<< std::endl;
//		m_tableData[dst_b][dst_id]  = m_tableData[src_b][src_id];
//	}

//	/**************************************
//	 *            SAVE & LOAD             *
//	 **************************************/

//	/**
//	 * Sauvegarde binaire
//	 * @param fs filestream
//	 * @param id id of mv
//	 */
//	void saveBin(CGoGNostream& fs, unsigned int id)
//	{
////		unsigned int nbs[3];
////		nbs[0] = id;
////		int len1 = m_attrName.size()+1;
////		int len2 = m_typeName.size()+1;
////		nbs[1] = len1;
////		nbs[2] = len2;
////		fs.write(reinterpret_cast<const char*>(nbs),3*sizeof(unsigned int));
////		// store names
////		char buffer[256];
////		const char* s1 = m_attrName.c_str();
////		memcpy(buffer,s1,len1);
////		const char* s2 = m_typeName.c_str();
////		memcpy(buffer+len1,s2,len2);
////		fs.write(reinterpret_cast<const char*>(buffer),(len1+len2)*sizeof(char));

////		nbs[0] = m_tableData.size();
////		nbs[1] = nbs[0] * _BLOCKSIZE_/32;
////		fs.write(reinterpret_cast<const char*>(nbs),2*sizeof(unsigned int));

////		for (auto ptrIt = m_tableData.begin(); ptrIt!=m_tableData.end(); ++ptrIt)
////			fs.write(reinterpret_cast<const char*>(*ptrIt),_BLOCKSIZE_/8);
//	}


//	/**
//	 * lecture binaire
//	 * @param fs filestream
//	 */
//	bool loadBin(CGoGNistream& fs)
//	{
////		unsigned int nbs[2];
////		fs.read(reinterpret_cast<char*>(nbs), 2*sizeof(unsigned int));

////		unsigned int nb = nbs[0];

////		bool *bbuff = new bool[nb*_BLOCKSIZE_];
////		fs.read(reinterpret_cast<char*>(bbuff),nb*_BLOCKSIZE_*sizeof(bool));

////		// load data blocks
////		m_tableData.resize(nb);
////		for(unsigned int i = 0; i < nb; ++i)
////		{
////			m_tableData[i] = new unsigned int[_BLOCKSIZE_/32];
////			for (unsigned int j=0; j<_BLOCKSIZE_;++j)
////				fs.read(reinterpret_cast<char*>(m_tableData[i]),_BLOCKSIZE_/8);
////		}

//		return true;
//	}

//	/**
//	 * lecture binaire
//	 * @param fs filestream
//	 */
//	virtual void dump(unsigned int i) const
//	{
//		CGoGNout << this->operator[](i);
//	}
//};





template <>
class AttributeMultiVector<MarkerBool> : public AttributeMultiVectorGen
{
	/**
	* table of blocks of data pointers: vectors!
	*/
	std::vector< unsigned int* > m_tableData;

public:
	AttributeMultiVector(const std::string& strName, const std::string& strType):
		AttributeMultiVectorGen(strName, strType)
	{
		m_tableData.reserve(1024);
	}

	AttributeMultiVector()
	{
		m_tableData.reserve(1024);
	}

	~AttributeMultiVector() {}

	inline AttributeMultiVectorGen* new_obj()
	{
		AttributeMultiVectorGen* ptr = new AttributeMultiVector<MarkerBool>;
		ptr->setTypeName(m_typeName);
		return ptr;
	}

	/**************************************
	 *       MULTI VECTOR MANAGEMENT      *
	 **************************************/

	void addBlock()
	{
		unsigned int* ptr = new unsigned int[_BLOCKSIZE_/32];
		memset(ptr,0,_BLOCKSIZE_/8);
		m_tableData.push_back(ptr);
//		std::cout << "Marker "<<this->getName()<<" - addBlock"<< std::endl;
	}

	void setNbBlocks(unsigned int nbb)
	{
		if (nbb >= m_tableData.size())
		{
			for (unsigned int i= m_tableData.size(); i <nbb; ++i)
				addBlock();
		}
		else
		{
			for (unsigned int i = m_tableData.size()-1; i>=nbb; --i)
				delete[] m_tableData[i];

			m_tableData.resize(nbb);
		}
	}


	unsigned int getNbBlocks() const
	{
		return m_tableData.size();
	}

//	void addBlocksBefore(unsigned int nbb);

	bool copy(const AttributeMultiVectorGen* atmvg)
	{
		const AttributeMultiVector<MarkerBool>* atmv = dynamic_cast<const AttributeMultiVector<MarkerBool>*>(atmvg);

		if (atmv == NULL)
		{
			CGoGNerr << "trying to copy attributes of different type" << CGoGNendl;
			return false;
		}
//		if (atmv->m_typeName != m_typeName)
//		{
//			CGoGNerr << "trying to copy attributes with different type names" << CGoGNendl;
//			return false;
//		}

		for (unsigned int i = 0; i < atmv->m_tableData.size(); ++i)
			memcpy(m_tableData[i],atmv->m_tableData[i],_BLOCKSIZE_/32);

		return true;
	}

	bool swap(AttributeMultiVectorGen* atmvg)
	{
		AttributeMultiVector<MarkerBool>* atmv = dynamic_cast<AttributeMultiVector<MarkerBool>*>(atmvg);

		if (atmv == NULL)
		{
			CGoGNerr << "trying to swap attributes of different type" << CGoGNendl;
			return false;
		}
		if (atmv->m_typeName != m_typeName)
		{
			CGoGNerr << "trying to swap attributes with different type names" << CGoGNendl;
			return false;
		}

		m_tableData.swap(atmv->m_tableData) ;
		return true;
	}

	bool merge(const AttributeMultiVectorGen& att)
	{
		const AttributeMultiVector<MarkerBool>* attrib = dynamic_cast< const AttributeMultiVector<MarkerBool>* >(&att);
		if (attrib==NULL)
		{
			CGoGNerr << "trying to merge attributes of different type" << CGoGNendl;
			return false;
		}

		if (attrib->m_typeName != m_typeName)
		{
			CGoGNerr << "trying to merge attributes with different type names" << CGoGNendl;
			return false;
		}

		for (auto it = attrib->m_tableData.begin(); it != attrib->m_tableData.end(); ++it)
			m_tableData.push_back(*it);

		return true;
	}

	void clear()
	{
		for (auto it=m_tableData.begin(); it !=m_tableData.end(); ++it)
			delete[] *it;
		m_tableData.clear();
	}

	int getSizeOfType() const
	{
		return sizeof(bool); // ?
	}

	inline void allFalse()
	{
//		std::cout << "Marker "<<this->getName()<<" - allFalse"<< std::endl;
		for (unsigned int i = 0; i < m_tableData.size(); ++i)
		{
			unsigned int *ptr =m_tableData[i];
			for (unsigned int j=0; j<_BLOCKSIZE_/32;++j)
				*ptr++ = 0;
		}
		//memset(m_tableData[i],0,_BLOCKSIZE_/8);
	}

	/**************************************
	 *             DATA ACCESS            *
	 **************************************/


	inline void setFalse(unsigned int i)
	{
		unsigned int jj = i / _BLOCKSIZE_;
		unsigned int j = i % _BLOCKSIZE_;
		unsigned int x = j/32;
		unsigned int y = j%32;
		unsigned int mask = 1 << y;
		m_tableData[jj][x] &= ~mask;

//		std::cout << "Marker "<<this->getName()<<" - ";
//		std::cout << "SET FALSE: "<< this->operator[](i)<< std::endl;
	}

	inline void setTrue(unsigned int i)
	{
		unsigned int jj = i / _BLOCKSIZE_;
		unsigned int j = i % _BLOCKSIZE_;
		unsigned int x = j/32;
		unsigned int y = j%32;
		unsigned int mask = 1 << y;
		m_tableData[jj][x] |= mask;

//		std::cout << "Marker "<<this->getName()<<" - ";
//		std::cout << "SET TRUE: "<< this->operator[](i)<< std::endl;
	}

	inline void setVal(unsigned int i, bool b)
	{
		unsigned int jj = i / _BLOCKSIZE_;
		unsigned int j = i % _BLOCKSIZE_;
		unsigned int x = j/32;
		unsigned int y = j%32;
		unsigned int mask = 1 << y;

		if (b)
			m_tableData[jj][x] |= mask;
		else
			m_tableData[jj][x] &= ~mask;

//		std::cout << "Marker "<<this->getName()<<" - ";
//		std::cout << "SET VAL: "<< b << " -> "<< this->operator[](i)<< std::endl;
	}

	/**
	 * get a const reference on a elt
	 * @param i index of element
	 */
	inline bool operator[](unsigned int i) const
	{
		unsigned int jj = i / _BLOCKSIZE_;
		unsigned int j = i % _BLOCKSIZE_;
		unsigned int x = j/32;
		unsigned int y = j%32;

		unsigned int mask = 1 << y;

//		std::cout << "BITS="<<std::hex<< m_tableData[jj][x]<< " MASK="<<mask<<" ~MASK="<<~mask<<std::endl;
		return (m_tableData[jj][x] & mask) != 0;
	}

	/**
	 * Get the addresses of each block of data
	 */
	unsigned int getBlocksPointers(std::vector<void*>& addr, unsigned int& /*byteBlockSize*/) const
	{
		CGoGNerr << "DO NOT USE getBlocksPointers with bool attribute"<< CGoGNendl;
		addr.reserve(m_tableData.size());
		addr.clear();

		for (auto it = m_tableData.begin(); it != m_tableData.end(); ++it)
			addr.push_back(NULL );

		return addr.size();
	}


	/**************************************
	 *          LINES MANAGEMENT          *
	 **************************************/

	inline void initElt(unsigned int id)
	{
//		std::cout << "Marker "<<this->getName()<<" - InitElt"<< std::endl;
		setFalse(id);
	}

	inline void copyElt(unsigned int dst, unsigned int src)
	{
//		std::cout << "Marker "<<this->getName()<<" - CopyElt"<< std::endl;
		setVal(dst,this->operator [](src));
	}


	inline void swapElt(unsigned int id1, unsigned int id2)
	{
//		std::cout << "Marker "<<this->getName()<<" - SwapElt"<< std::endl;
		bool data = this->operator [](id1);
		setVal(id1,this->operator [](id2));
		setVal(id2,data);
	}

	/**
	* swap two elements in container (useful for compact function)
	* @param src_b  block index of source element
	* @param src_id index in block of source element
	* @param dst_b  block index of destination element
	* @param dst_id index in block of destination element
	*/
	void overwrite(unsigned int src_b, unsigned int src_id, unsigned int dst_b, unsigned int dst_id)
	{
//		std::cout << "Marker "<<this->getName()<<" - overwrite"<< std::endl;
		bool b = m_tableData[src_b][src_id/32] & (1 << (src_id%32));

		unsigned int mask = 1 << (dst_id%32);

		if (b)
			m_tableData[dst_b][dst_id/32] |= mask;
		else
			m_tableData[dst_b][dst_id/32] &= ~mask;

	}

	/**************************************
	 *            SAVE & LOAD             *
	 **************************************/

	/**
	 * Sauvegarde binaire
	 * @param fs filestream
	 * @param id id of mv
	 */
	void saveBin(CGoGNostream& fs, unsigned int id)
	{
		unsigned int nbs[3];
		nbs[0] = id;
		int len1 = m_attrName.size()+1;
		int len2 = m_typeName.size()+1;
		nbs[1] = len1;
		nbs[2] = len2;
		fs.write(reinterpret_cast<const char*>(nbs),3*sizeof(unsigned int));
		// store names
		char buffer[256];
		const char* s1 = m_attrName.c_str();
		memcpy(buffer,s1,len1);
		const char* s2 = m_typeName.c_str();
		memcpy(buffer+len1,s2,len2);
		fs.write(reinterpret_cast<const char*>(buffer),(len1+len2)*sizeof(char));

		nbs[0] = m_tableData.size();
		nbs[1] = nbs[0] * _BLOCKSIZE_/32;
		fs.write(reinterpret_cast<const char*>(nbs),2*sizeof(unsigned int));

		for (auto ptrIt = m_tableData.begin(); ptrIt!=m_tableData.end(); ++ptrIt)
			fs.write(reinterpret_cast<const char*>(*ptrIt),_BLOCKSIZE_/8);
	}


	/**
	 * lecture binaire
	 * @param fs filestream
	 */
	bool loadBin(CGoGNistream& fs)
	{
		unsigned int nbs[2];
		fs.read(reinterpret_cast<char*>(nbs), 2*sizeof(unsigned int));

		unsigned int nb = nbs[0];

		bool *bbuff = new bool[nb*_BLOCKSIZE_];
		fs.read(reinterpret_cast<char*>(bbuff),nb*_BLOCKSIZE_*sizeof(bool));

		// load data blocks
		m_tableData.resize(nb);
		for(unsigned int i = 0; i < nb; ++i)
		{
			m_tableData[i] = new unsigned int[_BLOCKSIZE_/32];
			for (unsigned int j=0; j<_BLOCKSIZE_;++j)
				fs.read(reinterpret_cast<char*>(m_tableData[i]),_BLOCKSIZE_/8);
		}

		return true;
	}

	/**
	 * lecture binaire
	 * @param fs filestream
	 */
	virtual void dump(unsigned int i) const
	{
		CGoGNout << this->operator[](i);
	}
};




//template <>
//class AttributeMultiVector<MarkerBool> : public AttributeMultiVectorGen
//{
//	/**
//	* table of blocks of data pointers: vectors!
//	*/
//	std::vector< std::vector<bool>* > m_tableData;

//public:
//	AttributeMultiVector(const std::string& strName, const std::string& strType):
//		AttributeMultiVectorGen(strName, strType)
//	{
//		m_tableData.reserve(1024);
//	}

//	AttributeMultiVector()
//	{
//		m_tableData.reserve(1024);
//	}

//	~AttributeMultiVector() {}

//	AttributeMultiVectorGen* new_obj()
//	{
//		AttributeMultiVectorGen* ptr = new AttributeMultiVector<MarkerBool>;
//		ptr->setTypeName(m_typeName);
//		return ptr;
//	}

//	/**************************************
//	 *       MULTI VECTOR MANAGEMENT      *
//	 **************************************/

//	void addBlock()
//	{
//		std::vector<bool>* vb = new std::vector<bool>(_BLOCKSIZE_,false);
//		m_tableData.push_back(vb);
//	}

//	void setNbBlocks(unsigned int nbb)
//	{
//		if (nbb >= m_tableData.size())
//		{
//			for (unsigned int i= m_tableData.size(); i <nbb; ++i)
//				addBlock();
//		}
//		else
//		{
//			// not really released ?
//			m_tableData.resize(nbb);
//		}
//	}


//	unsigned int getNbBlocks() const
//	{
//		return m_tableData.size();
//	}

////	void addBlocksBefore(unsigned int nbb);

//	bool copy(const AttributeMultiVectorGen* atmvg)
//	{
//		const AttributeMultiVector<MarkerBool>* atmv = dynamic_cast<const AttributeMultiVector<MarkerBool>*>(atmvg);

//		if (atmv == NULL)
//		{
//			CGoGNerr << "trying to copy attributes of different type" << CGoGNendl;
//			return false;
//		}
////		if (atmv->m_typeName != m_typeName)
////		{
////			CGoGNerr << "trying to copy attributes with different type names" << CGoGNendl;
////			return false;
////		}

//		for (unsigned int i = 0; i < atmv->m_tableData.size(); ++i)
//			*(m_tableData[i]) = *(atmv->m_tableData[i]);

//		return true;
//	}

//	bool swap(AttributeMultiVectorGen* atmvg)
//	{
//		AttributeMultiVector<MarkerBool>* atmv = dynamic_cast<AttributeMultiVector<MarkerBool>*>(atmvg);

//		if (atmv == NULL)
//		{
//			CGoGNerr << "trying to swap attributes of different type" << CGoGNendl;
//			return false;
//		}
//		if (atmv->m_typeName != m_typeName)
//		{
//			CGoGNerr << "trying to swap attributes with different type names" << CGoGNendl;
//			return false;
//		}

//		m_tableData.swap(atmv->m_tableData) ;
//		return true;
//	}

//	bool merge(const AttributeMultiVectorGen& att)
//	{
//		const AttributeMultiVector<MarkerBool>* attrib = dynamic_cast< const AttributeMultiVector<MarkerBool>* >(&att);
//		if (attrib==NULL)
//		{
//			CGoGNerr << "trying to merge attributes of different type" << CGoGNendl;
//			return false;
//		}

//		if (attrib->m_typeName != m_typeName)
//		{
//			CGoGNerr << "trying to merge attributes with different type names" << CGoGNendl;
//			return false;
//		}

//		for (auto it = attrib->m_tableData.begin(); it != attrib->m_tableData.end(); ++it)
//			m_tableData.push_back(*it);

//		return true;
//	}

//	void clear()
//	{
//		for (auto it=m_tableData.begin(); it !=m_tableData.end(); ++it)
//			delete *it;
//		m_tableData.clear();
//	}

//	int getSizeOfType() const
//	{
//		return sizeof(bool); // ?
//	}

//	void allFalse()
//	{
//		for (unsigned int i = 0; i < m_tableData.size(); ++i)
//			m_tableData[i]->assign(_BLOCKSIZE_,false);
//	}

//	/**************************************
//	 *             DATA ACCESS            *
//	 **************************************/

//	/**
//	 * get a reference on a elt
//	 * @param i index of element
//	 */
//	 std::vector<bool>::reference operator[](unsigned int i)
//	 {
//		 return m_tableData[i / _BLOCKSIZE_]->operator[](i % _BLOCKSIZE_);
//	 }

//	/**
//	 * get a const reference on a elt
//	 * @param i index of element
//	 */
//	bool operator[](unsigned int i) const
//	{
//		return m_tableData[i / _BLOCKSIZE_]->operator[](i % _BLOCKSIZE_);
//	}

//	/**
//	 * Get the addresses of each block of data
//	 */
//	unsigned int getBlocksPointers(std::vector<void*>& addr, unsigned int& /*byteBlockSize*/) const
//	{
//		CGoGNerr << "DO NOT USE getBlocksPointers with bool attribute"<< CGoGNendl;
//		addr.reserve(m_tableData.size());
//		addr.clear();

//		for (auto it = m_tableData.begin(); it != m_tableData.end(); ++it)
//			addr.push_back(NULL );

//		return addr.size();
//	}


//	/**************************************
//	 *          LINES MANAGEMENT          *
//	 **************************************/

//	void initElt(unsigned int id)
//	{
//		m_tableData[id / _BLOCKSIZE_]->operator[](id % _BLOCKSIZE_) = false; // T(0);
//	}

//	void copyElt(unsigned int dst, unsigned int src)
//	{
//		m_tableData[dst / _BLOCKSIZE_]->operator[](dst % _BLOCKSIZE_) = m_tableData[src / _BLOCKSIZE_]->operator[](src % _BLOCKSIZE_);
//	}


//	void swapElt(unsigned int id1, unsigned int id2)
//	{
//		bool data = m_tableData[id1 / _BLOCKSIZE_]->operator[](id1 % _BLOCKSIZE_) ;
//		m_tableData[id1 / _BLOCKSIZE_]->operator[](id1 % _BLOCKSIZE_) = m_tableData[id2 / _BLOCKSIZE_]->operator[](id2 % _BLOCKSIZE_) ;
//		m_tableData[id2 / _BLOCKSIZE_]->operator[](id2 % _BLOCKSIZE_) = data ;
//	}

//	/**
//	* swap two elements in container (useful for compact function)
//	* @param src_b  block index of source element
//	* @param src_id index in block of source element
//	* @param dst_b  block index of destination element
//	* @param dst_id index in block of destination element
//	*/
//	void overwrite(unsigned int src_b, unsigned int src_id, unsigned int dst_b, unsigned int dst_id)
//	{
//		m_tableData[dst_b]->operator[](dst_id) = m_tableData[src_b]->operator[](src_id);
//	}

//	/**************************************
//	 *            SAVE & LOAD             *
//	 **************************************/

//	/**
//	 * Sauvegarde binaire
//	 * @param fs filestream
//	 * @param id id of mv
//	 */
//	void saveBin(CGoGNostream& fs, unsigned int id)
//	{
//		unsigned int nbs[3];
//		nbs[0] = id;
//		int len1 = m_attrName.size()+1;
//		int len2 = m_typeName.size()+1;
//		nbs[1] = len1;
//		nbs[2] = len2;
//		fs.write(reinterpret_cast<const char*>(nbs),3*sizeof(unsigned int));
//		// store names
//		char buffer[256];
//		const char* s1 = m_attrName.c_str();
//		memcpy(buffer,s1,len1);
//		const char* s2 = m_typeName.c_str();
//		memcpy(buffer+len1,s2,len2);
//		fs.write(reinterpret_cast<const char*>(buffer),(len1+len2)*sizeof(char));

//		nbs[0] = m_tableData.size();
//		nbs[1] = nbs[0] * _BLOCKSIZE_/8;
//		fs.write(reinterpret_cast<const char*>(nbs),2*sizeof(unsigned int));

//		unsigned int nb = _BLOCKSIZE_*m_tableData.size();
//		bool *bbuff = new bool[nb];
//		bool *ptr = bbuff;

//		for (unsigned int i =0; i<m_tableData.size(); ++i)
//			for (unsigned int j=0; j<_BLOCKSIZE_;++j)
//				*ptr++ = m_tableData[i]->operator[](j);

//		fs.write(reinterpret_cast<const char*>(bbuff),nb*sizeof(bool));
//		delete[] bbuff;
//	}


//	/**
//	 * lecture binaire
//	 * @param fs filestream
//	 */
//	bool loadBin(CGoGNistream& fs)
//	{
//		unsigned int nbs[2];
//		fs.read(reinterpret_cast<char*>(nbs), 2*sizeof(unsigned int));

//		unsigned int nb = nbs[0];

//		bool *bbuff = new bool[nb*_BLOCKSIZE_];
//		fs.read(reinterpret_cast<char*>(bbuff),nb*_BLOCKSIZE_*sizeof(bool));

//		// load data blocks
//		m_tableData.resize(nb);
//		bool *ptr = bbuff;
//		for(unsigned int i = 0; i < nb; ++i)
//		{
//			m_tableData[i] = new std::vector<bool>(_BLOCKSIZE_);
//			for (unsigned int j=0; j<_BLOCKSIZE_;++j)
//				m_tableData[i]->operator[](j) = *ptr++;
//		}
//		delete[] bbuff;

//		return true;
//	}

//	/**
//	 * lecture binaire
//	 * @param fs filestream
//	 */
//	virtual void dump(unsigned int i) const
//	{
//		CGoGNout << this->operator[](i);
//	}
//};


} // namespace CGoGN

