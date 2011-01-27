/*
 * holeblockref.cpp
 *
 *  Created on: Jun 10, 2010
 *      Author: thery
 */
#include "Container/holeblockref.h"

#include <map>
#include <string>
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>

#include <iostream>
#include <sstream>
#include <fstream>

namespace CGoGN
{

HoleBlockRef::HoleBlockRef():
m_nbfree(0),m_nbref(0),m_nb(0)
{
	m_tableFree = new uint[_BLOCKSIZE_];
	m_refCount = new uint[_BLOCKSIZE_];
}

HoleBlockRef::~HoleBlockRef()
{
		delete[] m_tableFree;
		delete[] m_refCount;
}

void HoleBlockRef::swap(HoleBlockRef& hb)
{
	uint temp = m_nbfree;
	m_nbfree = hb.m_nbfree;
	hb.m_nbfree = temp;

	temp = m_nbref;
	m_nbref = hb.m_nbref;
	hb.m_nbref = temp;

	temp = m_nb;
	m_nb = hb.m_nb;
	hb.m_nb = temp;

	uint* ptr = m_tableFree;
	m_tableFree = hb.m_tableFree;
	hb.m_tableFree = ptr;

	uint* ptr2 = m_refCount;
	m_refCount = hb.m_refCount;
	hb.m_refCount = ptr2;
}

uint HoleBlockRef::newRefElt(uint& nbEltsMax)
{
	// no hole then add a line at the end of block
	if (m_nbfree == 0)
	{
 		uint nbElts = m_nbref;

 		m_refCount[m_nbref++] = 1;

		m_nb++;
		nbEltsMax++;
		return nbElts;
	}

	uint index = m_tableFree[--m_nbfree];

	m_refCount[index] = 1;

	m_nb++;
	return index;
}

bool  HoleBlockRef::compressFree()
{
	if (m_nb)
	{

		m_nbfree = 0;
		m_nbref = m_nb;
		return false;
	}
	return true;
}

void HoleBlockRef::overwrite(int i, HoleBlockRef *bf, int j)
{
	m_refCount[i] = bf->m_refCount[j];
	bf->m_refCount[j] = 0;

	incNb();
	bf->decNb();
}

void HoleBlockRef::clear()
{
	m_nb = 0;
	m_nbfree = 0;
	m_nbref = 0;
}

bool HoleBlockRef::updateHoles(unsigned int nb)
{
	m_nbfree = 0;
	m_nbref = nb;
	bool notfull = false;
	for (unsigned int i = 0; i < nb; ++i)
	{
		if (m_refCount[i] == 0)
		{
			m_tableFree[m_nbfree++] = i;
			notfull = true;
		}
	}
	return notfull;
}

void HoleBlockRef::saveBin(CGoGNostream& fs)
{
//	std::cout << "save bf "<< m_nb<< " / "<< m_nbref<< " / "<< m_nbfree << std::endl;

	// on sauve les trois nombres;
	uint numbers[3];
	numbers[0] = m_nb;
	numbers[1] = m_nbref;
	numbers[2] = m_nbfree;
	fs.write(reinterpret_cast<const char*>(numbers), 3*sizeof(uint) );

	// sauve les ref count
	fs.write(reinterpret_cast<const char*>(m_refCount), _BLOCKSIZE_*sizeof(uint));

	// sauve les free lines
	fs.write(reinterpret_cast<const char*>(m_tableFree), m_nbfree*sizeof(uint));
}

bool HoleBlockRef::loadBin(CGoGNistream& fs)
{
	uint numbers[3];

	fs.read(reinterpret_cast<char*>(numbers), 3*sizeof(uint));
	m_nb = numbers[0];
	m_nbref = numbers[1];
	m_nbfree = numbers[2];

	fs.read(reinterpret_cast<char*>(m_refCount), _BLOCKSIZE_*sizeof(uint));
	fs.read(reinterpret_cast<char*>(m_tableFree), m_nbfree*sizeof(uint));

	return true;
}

}

