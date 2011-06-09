/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN
{

namespace Utils
{

template <typename ATTR_HANDLER>
void VBO::updateData(const ATTR_HANDLER& attrib)
{
	if (m_lock)
	{
		CGoGNerr <<" Error locked VBO"<< CGoGNendl;
		return;
	}
	m_data_size = sizeof(typename ATTR_HANDLER::DATA_TYPE) / sizeof(float);
	AttributeMultiVector<typename ATTR_HANDLER::DATA_TYPE>* mv = attrib.getDataVector() ;

	std::vector<void*> addr;
	unsigned int byteTableSize;
	unsigned int nbb = mv->getBlocksPointers(addr, byteTableSize);

	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, nbb * byteTableSize, 0, GL_STREAM_DRAW);

	m_nbElts = nbb * byteTableSize / sizeof(typename ATTR_HANDLER::DATA_TYPE);

	unsigned int offset = 0;

	for (unsigned int i = 0; i < nbb; ++i)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER, offset, byteTableSize, addr[i]);
		offset += byteTableSize;
	}
}

template <typename ATTR_HANDLER>
void VBO::updateData(const ATTR_HANDLER& attrib, ConvertAttrib* conv)
{
	if (m_lock)
	{
		CGoGNerr <<" Error locked VBO"<< CGoGNendl;
		return;
	}
	m_data_size = conv->sizeElt();
	AttributeMultiVector<typename ATTR_HANDLER::DATA_TYPE>* mv = attrib.getDataVector() ;

	std::vector<void*> addr;
	unsigned int byteTableSize;
	unsigned int nbb = mv->getBlocksPointers(addr, byteTableSize);

	// alloue la memoire pour le buffer et initialise le conv
	conv->reserve(mv->getBlockSize());

	// bind buffer to update
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, nbb * conv->sizeBuffer(), 0, GL_STREAM_DRAW);

	m_nbElts = nbb * conv->nbElt();

	unsigned int offset = 0;

	for (unsigned int i = 0; i < nbb; ++i)
	{
		// convertit les donnees dans le buffer de conv
		conv->convert(addr[i]);
		// update sub-vbo
		glBufferSubDataARB(GL_ARRAY_BUFFER, offset, conv->sizeBuffer(), conv->buffer());
		// block suivant
		offset += conv->sizeBuffer();
	}

	// libere la memoire de la conversion
	conv->release();
}

} // namespace Utils

} // namespace CGoGN
