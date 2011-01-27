#ifndef __HOLE_BLOCK_REF__
#define __HOLE_BLOCK_REF__

#include <fstream>
#include <iostream>

#include "sizeblock.h"

namespace CGoGN
{
//WARNING WE USED SHORT INT BLOCK_SIZE MUST BE < 65536
class HoleBlockRef
{
protected:
	/**
	* Table of free index
	*/
	uint* m_tableFree;
	uint m_nbfree;

	/**
	* Reference counter (if 0 it is a hole)
	*/
	uint* m_refCount;
	uint m_nbref;

	/**
	* nb element in block
	*/
	uint m_nb;

public:
	/**
	* constructor
	*/
	HoleBlockRef();

	/**
	* destructor
	*/
	~HoleBlockRef();

	/**
	 * swapping
	 */
	void swap(HoleBlockRef& hb);

	/**
	* add a element and return its index (refCount = 1)
	* @param nbEltsMax (IN/OUT) max number of element stored
	* @return index on new element
	*/
	uint newRefElt(uint& nbEltsMax);

	/**
	* remove an element
	*/
	inline void removeElt(uint idx)
	{
		m_nb--;
		m_tableFree[m_nbfree++]=idx;
		m_refCount[idx]=0;
	}

	/**
	* is the block full
	*/
	inline bool full() const { return m_nb == _BLOCKSIZE_;  }

	/**
	*  is the block empty
	*/
	inline bool empty() const { return m_nb == 0; }

	/**
	* is this index used or not
	*/
	inline bool used(unsigned int i) const { return m_refCount[i] != 0; }

	/**
	* use with caution: from compress only !
	*/
	inline void incNb() { m_nb++; }

	/**
	* use with caution: from compress only !
	*/
	inline void decNb() { m_nb--; }

	/**
	* return the size of table
	*/
	inline unsigned int sizeTable() { return m_nbref; }

	/**
	* compress the free value (use only in compress )
	* @return true if it is empty
	*/
	bool compressFree();

	/**
	* clear the container of free block
	*/
	void clear();

	/**
	* overwrite a line with another (called from compact)
	* @param i index of line in the block
	* @param bf ptr on the block of other line
	* @param j index of the other line in bf
	*/
	void overwrite(int i, HoleBlockRef *bf, int j);

	/**
	* increment ref counter of element i
	*/
	inline void ref(uint i)
	{
		m_refCount[i]++;
	}

	/**
	* decrement ref counter of element i
	* @return true if ref=0 and element has been destroyed
	*/
	inline bool unref(uint i)
	{
		m_refCount[i]--;
		if (m_refCount[i] == 1)
		{
			removeElt(i);
			return true;
		}
		return false;
	}

	/**
	* set ref counter of element i with j
	*/
	inline void setNbRefs(uint i, uint nb) { m_refCount[i] = nb; }

	/**
	* number of references of element i
	* @return the number of references (0 = no elements)
	*/
	inline uint nbRefs(uint i) { return m_refCount[i]; }

	bool updateHoles(uint nb);

	void saveBin(CGoGNostream& fs);

	bool loadBin(CGoGNistream& fs);
};

}

#endif

