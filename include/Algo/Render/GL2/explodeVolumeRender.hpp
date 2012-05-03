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

inline ExplodeVolumeRender::ExplodeVolumeRender(bool withColorPerFace, bool withExplodeFace):
		m_cpf(withColorPerFace),m_ef(withExplodeFace),m_globalColor(0.7f,0.7f,0.7f)
{
	m_vboPos = new Utils::VBO();
	m_vboPos->setDataSize(3);

	m_vboColors = new Utils::VBO();
	m_vboColors->setDataSize(3);


	m_vboPosLine = new Utils::VBO();
	m_vboPosLine->setDataSize(3);

	m_shader = new Utils::ShaderExplodeVolumes(withColorPerFace,withExplodeFace);
	m_shaderL = new Utils::ShaderExplodeVolumesLines();

//	m_shader->setAmbiant(Geom::Vec4f(0.1f,0.1f,0.1f,0.0f));
//	m_shader->setDiffuse(Geom::Vec4f(1.0f,1.0f,0.1f,0.0f));
	//m_shaderL->setColor(Geom::Vec4f(1.0f,1.0f,1.0f,0.0f));

	//m_shaderL->setColor(Geom::Vec4f(0.113f,0.337f,0.0f,0.113f));

	m_shaderL->setColor(Geom::Vec4f(1.0f,1.0f,1.0f,0.0f));

}

inline ExplodeVolumeRender::~ExplodeVolumeRender()
{
	delete m_vboPos;
	delete m_vboColors;
	delete m_vboPosLine;
	delete m_shader;
	delete m_shaderL;
}

template<typename PFP>
void ExplodeVolumeRender::updateData(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const FunctorSelect& good)
{
	if (m_cpf)
	{
		CGoGNerr<< "ExplodeVolumeRender: problem wrong update fonction use the other" << CGoGNendl;
		return;
	}

	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	VolumeAutoAttribute<VEC3> centerVolumes(map, "centerVolumes");
	Algo::Geometry::computeCentroidVolumes<PFP>(map, positions, centerVolumes, good);

	std::vector<VEC3> buffer;
	buffer.reserve(16384);

	TraversorCell<typename PFP::MAP, FACE + PFP::MAP::IN_PARENT> traFace(map, good);

	for (Dart d = traFace.begin(); d != traFace.end(); d = traFace.next())
	{
		if (m_ef)
		{
			VEC3 centerFace = Algo::Geometry::faceCentroid<PFP>(map, d, positions);
			Dart a = d;
			Dart b = map.phi1(a);
			Dart c = map.phi1(b);

			// loop to cut a polygon in triangle on the fly (works only with convex faces)
			do
			{
				buffer.push_back(centerVolumes[d]);
				buffer.push_back(positions[d]);
				buffer.push_back(positions[b]);
				buffer.push_back(positions[c]);
				buffer.push_back(centerFace); // not used
				buffer.push_back(centerFace);
				b = c;
				c = map.phi1(b);

			} while (c != d);
		}
		else
		{
			Dart a = d;
			Dart b = map.phi1(a);
			Dart c = map.phi1(b);

			// loop to cut a polygon in triangle on the fly (works only with convex faces)
			do
			{
				buffer.push_back(centerVolumes[d]);
				buffer.push_back(positions[d]);
				buffer.push_back(positions[b]);
				buffer.push_back(positions[c]);
				b = c;
				c = map.phi1(b);

			} while (c != d);
		}
	}

	m_nbTris = buffer.size()/4;

	m_vboPos->allocate(buffer.size());
	VEC3* ptrPos = reinterpret_cast<VEC3*>(m_vboPos->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));
	m_vboPos->releasePtr();
	m_shader->setAttributePosition(m_vboPos);

	buffer.clear();

	TraversorCell<typename PFP::MAP, EDGE + PFP::MAP::IN_PARENT> traEdge(map, good);
	for (Dart d = traEdge.begin(); d != traEdge.end(); d = traEdge.next())
	{
		buffer.push_back(centerVolumes[d]);
		buffer.push_back(positions[d]);
		buffer.push_back(positions[map.phi1(d)]);
	}

	m_nbLines = buffer.size()/3;

	m_vboPosLine->allocate(buffer.size());

	ptrPos = reinterpret_cast<VEC3*>(m_vboPosLine->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));

	m_vboPosLine->releasePtr();
	m_shaderL->setAttributePosition(m_vboPosLine);
}

template<typename PFP>
void ExplodeVolumeRender::updateData(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const VolumeAttribute<typename PFP::VEC3>& colorPerXXX, const FunctorSelect& good)
{
	if (!m_cpf)
	{
		CGoGNerr<< "ExplodeVolumeRender: problem wrong update fonction use the other" << CGoGNendl;
		return;
	}

	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	VolumeAutoAttribute<VEC3> centerVolumes(map, "centerVolumes");
	Algo::Geometry::computeCentroidVolumes<PFP>(map, positions, centerVolumes, good);

	std::vector<VEC3> buffer;
	buffer.reserve(16384);

	std::vector<VEC3> bufferColors;

//	bool withColors = (positions == colorPerXXX);
	bool withColors = true;
	if (withColors)
		bufferColors.reserve(16384);

	TraversorCell<typename PFP::MAP, FACE + PFP::MAP::IN_PARENT> traFace(map, good);

	for (Dart d = traFace.begin(); d != traFace.end(); d = traFace.next())
	{
		VEC3 centerFace = Algo::Geometry::faceCentroid<PFP>(map, d, positions);

		Dart a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		// loop to cut a polygon in triangle on the fly (works only with convex faces)
		do
		{
			buffer.push_back(centerVolumes[d]);
			bufferColors.push_back(centerFace);
			buffer.push_back(positions[d]);
			if (m_cpf)
				bufferColors.push_back(colorPerXXX[d]);
			else
				bufferColors.push_back(m_globalColor);
			buffer.push_back(positions[b]);
			if (m_cpf)
				bufferColors.push_back(colorPerXXX[b]);
			else
				bufferColors.push_back(m_globalColor);
			buffer.push_back(positions[c]);
			if (m_cpf)
				bufferColors.push_back(colorPerXXX[c]);
			else
				bufferColors.push_back(m_globalColor);
			b = c;
			c = map.phi1(b);

		} while (c != d);
	}

	m_nbTris = buffer.size()/4;

	m_vboPos->allocate(buffer.size());
	VEC3* ptrPos = reinterpret_cast<VEC3*>(m_vboPos->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));
	m_vboPos->releasePtr();
	m_shader->setAttributePosition(m_vboPos);

	if (withColors)
	{
		m_vboColors->allocate(bufferColors.size());
		VEC3* ptrCol = reinterpret_cast<VEC3*>(m_vboColors->lockPtr());
		memcpy(ptrCol,&bufferColors[0],bufferColors.size()*sizeof(VEC3));
		m_vboColors->releasePtr();
		m_shader->setAttributeColor(m_vboColors);
	}


	buffer.clear();

	TraversorCell<typename PFP::MAP, EDGE + PFP::MAP::IN_PARENT> traEdge(map, good);
	for (Dart d = traEdge.begin(); d != traEdge.end(); d = traEdge.next())
	{
			buffer.push_back(centerVolumes[d]);
			buffer.push_back(positions[d]);
			buffer.push_back(positions[map.phi1(d)]);
	}

	m_nbLines = buffer.size()/3;

	m_vboPosLine->allocate(buffer.size());

	ptrPos = reinterpret_cast<VEC3*>(m_vboPosLine->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));

	m_vboPosLine->releasePtr();
	m_shaderL->setAttributePosition(m_vboPosLine);
}

inline void ExplodeVolumeRender::drawFaces()
{
	m_shader->enableVertexAttribs();
//	if (m_cpf)
//		glDrawArrays(GL_TRIANGLES_ADJACENCY_EXT , 0 , m_nbTris*6 );
//	else
		glDrawArrays(GL_LINES_ADJACENCY_EXT , 0 , m_nbTris*4 );
	m_shader->disableVertexAttribs();
}

inline void ExplodeVolumeRender::drawEdges()
{

	m_shaderL->enableVertexAttribs();
	glDrawArrays(GL_TRIANGLES , 0 , m_nbLines*3 );
	m_shaderL->disableVertexAttribs();
}

inline void ExplodeVolumeRender::setExplodeVolumes(float explode)
{
	m_shader->setExplodeVolumes(explode);
	m_shaderL->setExplodeVolumes(explode);
}

inline void ExplodeVolumeRender::setExplodeFaces(float explode)
{
	m_shader->setExplodeFaces(explode);
}

inline void ExplodeVolumeRender::setClippingPlane(const Geom::Vec4f& p)
{
	m_shader->setClippingPlane(p);
	m_shaderL->setClippingPlane(p);
}

inline void ExplodeVolumeRender::setNoClippingPlane()
{
	Geom::Vec4f p(1.0f,1.0f,1.0f,-99999999.9f);
	m_shader->setClippingPlane(p);
	m_shaderL->setClippingPlane(p);
}

inline void ExplodeVolumeRender::setAmbiant(const Geom::Vec4f& ambiant)
{
	m_shader->setAmbiant(ambiant);
}

inline void ExplodeVolumeRender::setBackColor(const Geom::Vec4f& color)
{
	m_shader->setBackColor(color);
}

inline void ExplodeVolumeRender::setLightPosition(const Geom::Vec3f& lp)
{
	m_shader->setLightPosition(lp);
}

inline void ExplodeVolumeRender::setColorLine(const Geom::Vec4f& col)
{
	m_shaderL->setColor(col);
}

inline Utils::GLSLShader* ExplodeVolumeRender::shaderFaces()
{
	return m_shader;
}

inline Utils::GLSLShader* ExplodeVolumeRender::shaderLines()
{
	return m_shaderL;
}

}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
