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

namespace CGoGN
{

namespace Utils
{

template <unsigned int VEC_DIM, typename T>
void VBO::updateData(std::vector<T>& data)
{
	if (m_lock)
	{
		CGoGNerr << "VBO::updateData : Error locked VBO" << CGoGNendl;
		return;
	}

	if (data.empty())
		return;

	m_data_size = VEC_DIM;
	m_nbElts = uint32(data.size());
	glBindBuffer(GL_ARRAY_BUFFER, *m_id);

	if (sizeof(T) / sizeof(double) == VEC_DIM)
	{
		unsigned int nbf = m_nbElts * m_data_size;
		float* buffer = new float[nbf];
		float* ptr_out = buffer;
		double* ptr_in = reinterpret_cast<double*>(&(data[0]));
		for (unsigned int i=0; i<nbf; ++i)
			*ptr_out++ = float(*ptr_in++);
		glBufferData(GL_ARRAY_BUFFER, nbf * sizeof(float), buffer, GL_STREAM_DRAW);
		delete[] buffer;
		return;
	}
	if (sizeof(T) / sizeof(float) == VEC_DIM)
	{
		glBufferData(GL_ARRAY_BUFFER, m_nbElts * VEC_DIM * sizeof(float), &(data[0]), GL_STREAM_DRAW);
		return;
	}

	// we should normally never reach this code
	CGoGNerr << "VBO::updateData : Error only float, double, VecXf, VecXd" << CGoGNendl;
}

} // namespace Utils

} // namespace CGoGN
