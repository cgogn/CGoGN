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
#include <cmath>

#include "Geometry/vector_gen.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{


inline ColorPerFaceRender::ColorPerFaceRender(Utils::VBO& vboPosition, Utils::VBO& vboColor):
		m_vboPosition(vboPosition), m_vboColor(vboColor), m_nbTris(0)
{
}


template<typename PFP>
void ColorPerFaceRender::updateData(typename PFP::MAP& map, typename PFP::TVEC3& positions, typename PFP::TVEC3& colorPerXXX, const FunctorSelect& good)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	CellMarker cmv(map,VOLUME);
	AutoAttributeHandler<VEC3> centerVolumes(map,VOLUME,"centerVolumes");
	TraversorW<typename PFP::MAP> traVol(map,good);
	for (Dart d=traVol.begin(); d!=traVol.end(); d=traVol.next())
	{
		centerVolumes[d] = Algo::Geometry::volumeCentroid<PFP>(map, d, positions);
	}

	std::vector<VEC3> buffer;
	buffer.reserve(16384);

	std::vector<VEC3> bufferColors;
	bufferColors.reserve(16384);

	TraversorCell<typename PFP::MAP> traFace(map, PFP::MAP::ORBIT_IN_PARENT(FACE),good);

	for (Dart d=traFace.begin(); d!=traFace.end(); d=traFace.next())
	{
		Dart a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		// loop to cut a polygon in triangle on the fly (works only with convex faces)
		do
		{
			buffer.push_back(positions[d]);
			bufferColors.push_back(colorPerXXX[d]);
			buffer.push_back(positions[b]);
			bufferColors.push_back(colorPerXXX[b]);
			buffer.push_back(positions[c]);
			bufferColors.push_back(colorPerXXX[c]);
			b = c;
			c = map.phi1(b);
		} while (c != d);
	}

	m_nbTris = buffer.size()/4;

	m_vboPos->allocate(buffer.size());
	VEC3* ptrPos = reinterpret_cast<VEC3*>(m_vboPos->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));
	m_vboPos->releasePtr();

	m_vboColors->allocate(bufferColors.size());
	VEC3* ptrCol = reinterpret_cast<VEC3*>(m_vboColors->lockPtr());
	memcpy(ptrCol,&bufferColors[0],bufferColors.size()*sizeof(VEC3));
	m_vboColors->releasePtr();

}


void ColorPerFaceRender::draw(Utils::GLSLShader* sh)
{
	sh->enableVertexAttribs();
		glDrawArrays(GL_TRIANGLES , 0 , m_nbTris*3 );
	sh->disableVertexAttribs();
}




}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
