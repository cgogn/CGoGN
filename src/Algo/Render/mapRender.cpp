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

#include "Algo/Render/GL2/mapRender.h"
#include "Utils/GLSLShader.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

MapRender::MapRender():
	m_nbIndicesTri(0),
	m_nbIndicesLines(0),
	m_nbIndicesPoints(0),
	m_nbFlatElts(0)
{
	glGenBuffersARB(4, m_VBOBuffers) ;
}


MapRender::~MapRender()
{
	glDeleteBuffersARB(4, m_VBOBuffers);
	delete[] m_VBOBuffers ;

}



void MapRender::initPrimitives(int prim, std::vector<GLuint>& tableIndices)
{
	// indice du VBO a utiliser
	int vbo_ind = 0;
	switch(prim)
	{
		case TRIANGLES:
			m_nbIndicesTri = tableIndices.size();
			vbo_ind = m_VBOBuffers[TRIANGLE_INDICES];
			break;
		case LINES:
			m_nbIndicesLines = tableIndices.size();
			vbo_ind = m_VBOBuffers[LINE_INDICES];
			break;
		case POINTS:
			m_nbIndicesPoints = tableIndices.size();
			vbo_ind = m_VBOBuffers[POINT_INDICES];
			break;
		default:
			CGoGNerr << "problem initializing VBO indices" << CGoGNendl;
			break;
	}
	int size = tableIndices.size();

	// setup du buffer d'indices
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, vbo_ind);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, size*sizeof(GLuint), &(tableIndices[0]), GL_STREAM_DRAW);
}


void MapRender::draw(Utils::GLSLShader* sh, int prim)
{
	sh->enableVertexAttribs();
//	const std::vector<Utils::GLSLShader::VAStr>& vastr = sh->getVA_VBO_Bindings();
//
//	for (std::vector<Utils::GLSLShader::VAStr>::const_iterator it= vastr.begin(); it != vastr.end(); ++it)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, it->vbo_ptr->id());
//		glEnableVertexAttribArray(it->va_id);
//		glVertexAttribPointer(it->va_id ,it->vbo_ptr->dataSize(), GL_FLOAT, false, 0, 0);
//	}

	switch(prim)
	{
		case TRIANGLES:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[TRIANGLE_INDICES]);
			glDrawElements(GL_TRIANGLES, m_nbIndicesTri, GL_UNSIGNED_INT, 0);
			break;
		case LINES:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[LINE_INDICES]);
			glDrawElements(GL_LINES, m_nbIndicesLines, GL_UNSIGNED_INT, 0);
			break;
		case POINTS:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[POINT_INDICES]);
			glDrawElements(GL_POINTS, m_nbIndicesPoints, GL_UNSIGNED_INT, 0) ;
			break;
		default:
			break;
	}

//	for (std::vector<Utils::GLSLShader::VAStr>::const_iterator it= vastr.begin(); it != vastr.end(); ++it)
//	{
//		glDisableVertexAttribArray(it->va_id);
//	}
	sh->disableVertexAttribs();
}






} // namespace GL2

} // namespace Render

} // namespace Algo

} // namespace CGoGN
