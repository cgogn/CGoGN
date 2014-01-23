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

#include <cassert>
#include "Utils/compress.h"
#include "zlib.h"

namespace CGoGN
{

namespace Utils
{

void zlibWriteCompressed( unsigned char* input, unsigned int nbBytes, std::ofstream& fout)
{
	const int CHUNK=16384;
	int level = 6; // compression level 
	unsigned char* out = new unsigned char[CHUNK];

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	int ret = deflateInit(&strm, level);
	assert(ret == Z_OK); 

	unsigned char* ptrData = input;
	int remain = nbBytes;

	while (remain >0)
	{
		strm.avail_in = std::min(remain,CHUNK);	// taille buffer
		strm.next_in = ptrData;					// ptr buffer

		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			if (remain>= CHUNK)
				ret = deflate(&strm, 0);
			else
				ret = deflate(&strm, 1);
			assert(ret != Z_STREAM_ERROR); 
			unsigned int have = CHUNK - strm.avail_out;
			fout.write((char*)out, have);
		} while (strm.avail_out == 0);

		remain -= CHUNK;
		ptrData += CHUNK;	
	}

//	assert(ret == Z_STREAM_END);
	deflateEnd(&strm);
	delete[] out;
}

}
}
