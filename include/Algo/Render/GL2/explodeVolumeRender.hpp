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
#include "Topology/generic/autoAttributeHandler.h"

#include "Algo/Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

inline ExplodeVolumeRender::ExplodeVolumeRender(bool withColorPerFace, bool withExplodeFace, bool withSmoothFaces):
		m_cpf(withColorPerFace),m_ef(withExplodeFace),m_smooth(withSmoothFaces),
		m_nbTris(0), m_nbLines(0),m_globalColor(0.7f,0.7f,0.7f)
{
	m_vboPos = new Utils::VBO();
	m_vboPos->setDataSize(3);

	m_vboColors = new Utils::VBO();
	m_vboColors->setDataSize(3);

	m_vboPosLine = new Utils::VBO();
	m_vboPosLine->setDataSize(3);

	if (m_smooth)
	{
		m_shaderS = new Utils::ShaderExplodeSmoothVolumes(withColorPerFace,withExplodeFace);
		m_shader = NULL;
		m_vboNormals = new Utils::VBO();
		m_vboNormals->setDataSize(3);
	}
	else
	{
		m_shader = new Utils::ShaderExplodeVolumes(withColorPerFace,withExplodeFace);
		m_shaderS = NULL;
		m_vboNormals = NULL;
	}

	m_shaderL = new Utils::ShaderExplodeVolumesLines();
	m_shaderL->setColor(Geom::Vec4f(1.0f,1.0f,1.0f,0.0f));

}

inline ExplodeVolumeRender::~ExplodeVolumeRender()
{
	delete m_vboPos;
	delete m_vboColors;
	delete m_vboPosLine;

	if (m_vboNormals != NULL)
		delete m_vboNormals;
	if (m_shader != NULL)
		delete m_shader;
	if (m_shaderS != NULL)
		delete m_shaderS;
	delete m_shaderL;
}



template<typename PFP>
void ExplodeVolumeRender::updateSmooth(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const VolumeAttribute<typename PFP::VEC3>& colorPerXXX, const FunctorSelect& good)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	VolumeAutoAttribute<VEC3> centerVolumes(map, "centerVolumes");
	Algo::Volume::Geometry::Parallel::computeCentroidVolumes<PFP>(map, positions, centerVolumes, good);

	std::vector<VEC3> buffer;
	buffer.reserve(16384);

	std::vector<VEC3> bufferColors;
	bufferColors.reserve(16384);
	std::vector<VEC3> bufferNormals;
	bufferNormals.reserve(16384);
	std::vector<VEC3> normals;
	bufferNormals.reserve(20);

	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> traFace(map, good);


	for (Dart d = traFace.begin(); d != traFace.end(); d = traFace.next())
	{

		// compute normals
		normals.clear();
		VEC3 centerFace(0);
		unsigned int nbs=0;
		Dart a = d;
		do
		{
			VEC3 v1 = Algo::Surface::Geometry::vectorOutOfDart<PFP>(map,a,positions);
			Dart e = map.phi1(a);
			VEC3 v2 = Algo::Surface::Geometry::vectorOutOfDart<PFP>(map,e,positions);
			VEC3 N = v1^v2;
			N.normalize();
			normals.push_back(N);
			a = e;
			centerFace += positions[a];
			nbs++;
		} while (a != d);

		centerFace /= float(nbs);

		typename std::vector<VEC3>::iterator in = normals.begin();
		a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		// loop to cut a polygon in triangle on the fly (works only with convex faces)
		do
		{
			buffer.push_back(centerVolumes[d]);
			bufferColors.push_back(centerFace);
			bufferNormals.push_back(centerFace); // unsused just for fill
			buffer.push_back(positions[d]);
			bufferColors.push_back(colorPerXXX[d]);
			bufferNormals.push_back(normals.back());
			buffer.push_back(positions[b]);
			bufferColors.push_back(colorPerXXX[b]);
			bufferNormals.push_back(*in++);
			buffer.push_back(positions[c]);
			bufferColors.push_back(colorPerXXX[c]);
			bufferNormals.push_back(*in);
			b = c;
			c = map.phi1(b);
		} while (c != d);

//		a = d;
//		Dart b = map.phi1(a);
//		// loop to cut a polygon in triangle on the fly (works only with convex faces)
//		do
//		{
//			buffer.push_back(centerVolumes[d]);
//			bufferColors.push_back(centerFace);
//			bufferNormals.push_back(centerFace); // unsused just for fill
//			buffer.push_back(positions[a]);
//			bufferColors.push_back(colorPerXXX[a]);
//
//			VEC3 N1 = positions[a]^positions[b];
//			N1.normalize();
//			bufferNormals.push_back(N1);
//
//			buffer.push_back(positions[b]);
//			bufferColors.push_back(colorPerXXX[b]);
//
//			VEC3 N2 = positions[b]^centerFace;
//			N2.normalize();
//			bufferNormals.push_back(N2);
//
//			buffer.push_back(centerFace);
//			bufferColors.push_back(colorPerXXX[a]);
//
//			VEC3 N3 = centerFace^positions[a];
//			N3.normalize();
//			bufferNormals.push_back(N3);
//
//			a = b;
//			b = map.phi1(a);
//		} while (a != d);


	}


	m_nbTris = buffer.size()/4;

	m_vboPos->allocate(buffer.size());
	VEC3* ptrPos = reinterpret_cast<VEC3*>(m_vboPos->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));
	m_vboPos->releasePtr();
	m_shaderS->setAttributePosition(m_vboPos);

	m_vboColors->allocate(bufferColors.size());
	VEC3* ptrCol = reinterpret_cast<VEC3*>(m_vboColors->lockPtr());
	memcpy(ptrCol,&bufferColors[0],bufferColors.size()*sizeof(VEC3));
	m_vboColors->releasePtr();
	m_shaderS->setAttributeColor(m_vboColors);

	m_vboNormals->allocate(bufferNormals.size());
	VEC3* ptrNorm = reinterpret_cast<VEC3*>(m_vboNormals->lockPtr());
	memcpy(ptrNorm,&bufferNormals[0],bufferNormals.size()*sizeof(VEC3));
	m_vboNormals->releasePtr();
	m_shaderS->setAttributeNormal(m_vboNormals);

	buffer.clear();

	TraversorCell<typename PFP::MAP, PFP::MAP::EDGE_OF_PARENT> traEdge(map, good);
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
void ExplodeVolumeRender::updateSmooth(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const FunctorSelect& good)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	VolumeAutoAttribute<VEC3> centerVolumes(map, "centerVolumes");
	Algo::Volume::Geometry::Parallel::computeCentroidVolumes<PFP>(map, positions, centerVolumes, good);

	std::vector<VEC3> buffer;
	buffer.reserve(16384);

	std::vector<VEC3> bufferColors;
	bufferColors.reserve(16384);
	std::vector<VEC3> bufferNormals;
	bufferNormals.reserve(16384);
	std::vector<VEC3> normals;
	bufferNormals.reserve(20);

	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> traFace(map, good);

	for (Dart d = traFace.begin(); d != traFace.end(); d = traFace.next())
	{

		// compute normals

		normals.clear();
		VEC3 centerFace(0);
		unsigned int nbs=0;
		Dart a = d;
		do
		{
			VEC3 v1 = Algo::Surface::Geometry::vectorOutOfDart<PFP>(map,a,positions);
			Dart e = map.phi1(a);
			VEC3 v2 = Algo::Surface::Geometry::vectorOutOfDart<PFP>(map,e,positions);
			VEC3 N = v1^v2;
			N.normalize();
			normals.push_back(N);
			a = e;
			centerFace += positions[a];
			nbs++;
		} while (a != d);

		centerFace /= float(nbs);

		typename std::vector<VEC3>::iterator in = normals.begin();
		a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		// loop to cut a polygon in triangle on the fly (works only with convex faces)
		do
		{
			buffer.push_back(centerVolumes[d]);
			bufferColors.push_back(centerFace);
			bufferNormals.push_back(centerFace); // unsused just for fill
			buffer.push_back(positions[d]);
			bufferColors.push_back(m_globalColor);
			bufferNormals.push_back(normals.back());
			buffer.push_back(positions[b]);
			bufferColors.push_back(m_globalColor);
			bufferNormals.push_back(*in++);
			buffer.push_back(positions[c]);
			bufferColors.push_back(m_globalColor);
			bufferNormals.push_back(*in);
			b = c;
			c = map.phi1(b);
		} while (c != d);
	}


	m_nbTris = buffer.size()/4;

	m_vboPos->allocate(buffer.size());
	VEC3* ptrPos = reinterpret_cast<VEC3*>(m_vboPos->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));
	m_vboPos->releasePtr();
	m_shaderS->setAttributePosition(m_vboPos);

	m_vboColors->allocate(bufferColors.size());
	VEC3* ptrCol = reinterpret_cast<VEC3*>(m_vboColors->lockPtr());
	memcpy(ptrCol,&bufferColors[0],bufferColors.size()*sizeof(VEC3));
	m_vboColors->releasePtr();
	m_shaderS->setAttributeColor(m_vboColors);

	m_vboNormals->allocate(bufferNormals.size());
	VEC3* ptrNorm = reinterpret_cast<VEC3*>(m_vboNormals->lockPtr());
	memcpy(ptrNorm,&bufferNormals[0],bufferNormals.size()*sizeof(VEC3));
	m_vboNormals->releasePtr();
	m_shaderS->setAttributeNormal(m_vboNormals);

	buffer.clear();

	TraversorCell<typename PFP::MAP, PFP::MAP::EDGE_OF_PARENT> traEdge(map, good);
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
		CGoGNerr<< "ExplodeVolumeRender: problem wrong update fonction use the other (without VolumeAttribute parameter)" << CGoGNendl;
		return;
	}

	if (m_smooth)
	{

		updateSmooth<PFP>(map,positions,colorPerXXX,good);
		return;
	}


	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	VolumeAutoAttribute<VEC3> centerVolumes(map, "centerVolumes");
	Algo::Volume::Geometry::Parallel::computeCentroidVolumes<PFP>(map, positions, centerVolumes, good);

	std::vector<VEC3> buffer;
	buffer.reserve(16384);

	std::vector<VEC3> bufferColors;

	bufferColors.reserve(16384);

	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> traFace(map, good);

	for (Dart d = traFace.begin(); d != traFace.end(); d = traFace.next())
	{
		VEC3 centerFace = Algo::Surface::Geometry::faceCentroid<PFP>(map, d, positions);

		Dart a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		// loop to cut a polygon in triangle on the fly (works only with convex faces)
		do
		{
			buffer.push_back(centerVolumes[d]);
			bufferColors.push_back(centerFace);
			buffer.push_back(positions[d]);
			bufferColors.push_back(colorPerXXX[d]);
			buffer.push_back(positions[b]);
			bufferColors.push_back(colorPerXXX[b]);
			buffer.push_back(positions[c]);
			bufferColors.push_back(colorPerXXX[c]);
			b = c;
			c = map.phi1(b);
		} while (c != d);

//		Dart a = d;
//		Dart b = map.phi1(a);
//		// loop to cut a polygon in triangle on the fly (works only with convex faces)
//		do
//		{
//			buffer.push_back(centerVolumes[a]);
//			bufferColors.push_back(centerFace);
//			buffer.push_back(positions[a]);
//			bufferColors.push_back(colorPerXXX[a]);
//
//			buffer.push_back(positions[b]);
//			bufferColors.push_back(colorPerXXX[b]);
//
//			buffer.push_back(centerFace);
//			bufferColors.push_back(colorPerXXX[a]);
//
//			a = b;
//			b = map.phi1(a);
//		} while (a != d);
	}

	m_nbTris = buffer.size()/4;

	m_vboPos->allocate(buffer.size());
	VEC3* ptrPos = reinterpret_cast<VEC3*>(m_vboPos->lockPtr());
	memcpy(ptrPos,&buffer[0],buffer.size()*sizeof(VEC3));
	m_vboPos->releasePtr();
	m_shader->setAttributePosition(m_vboPos);

	m_vboColors->allocate(bufferColors.size());
	VEC3* ptrCol = reinterpret_cast<VEC3*>(m_vboColors->lockPtr());
	memcpy(ptrCol,&bufferColors[0],bufferColors.size()*sizeof(VEC3));
	m_vboColors->releasePtr();
	m_shader->setAttributeColor(m_vboColors);

	buffer.clear();

	TraversorCell<typename PFP::MAP, PFP::MAP::EDGE_OF_PARENT> traEdge(map, good);
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
void ExplodeVolumeRender::updateData(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& positions, const FunctorSelect& good)
{
	if (m_cpf)
	{
		CGoGNerr<< "ExplodeVolumeRender: problem wrong update fonction use the other (without VolumeAttribute parameter)" << CGoGNendl;
		return;
	}

	if (m_smooth)
	{
		updateSmooth<PFP>(map,positions,good);
		return;
	}


	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	VolumeAutoAttribute<VEC3> centerVolumes(map, "centerVolumes");
	Algo::Volume::Geometry::Parallel::computeCentroidVolumes<PFP>(map, positions, centerVolumes, good);

	std::vector<VEC3> buffer;
	buffer.reserve(16384);

	std::vector<VEC3> bufferColors;

	bufferColors.reserve(16384);

	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> traFace(map, good);

	for (Dart d = traFace.begin(); d != traFace.end(); d = traFace.next())
	{
		VEC3 centerFace = Algo::Surface::Geometry::faceCentroid<PFP>(map, d, positions);

		Dart a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		// loop to cut a polygon in triangle on the fly (works only with convex faces)
		do
		{
			buffer.push_back(centerVolumes[d]);
			bufferColors.push_back(centerFace);
			buffer.push_back(positions[d]);
			bufferColors.push_back(m_globalColor);
			buffer.push_back(positions[b]);
			bufferColors.push_back(m_globalColor);
			buffer.push_back(positions[c]);
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

	m_vboColors->allocate(bufferColors.size());
	VEC3* ptrCol = reinterpret_cast<VEC3*>(m_vboColors->lockPtr());
	memcpy(ptrCol,&bufferColors[0],bufferColors.size()*sizeof(VEC3));
	m_vboColors->releasePtr();
	m_shader->setAttributeColor(m_vboColors);

	buffer.clear();

	TraversorCell<typename PFP::MAP, PFP::MAP::EDGE_OF_PARENT> traEdge(map, good);
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
	if (m_smooth)
	{
		m_shaderS->enableVertexAttribs();
		glDrawArrays(GL_LINES_ADJACENCY_EXT , 0 , m_nbTris*4 );
		m_shaderS->disableVertexAttribs();
	}
	else
	{
		m_shader->enableVertexAttribs();
		glDrawArrays(GL_LINES_ADJACENCY_EXT , 0 , m_nbTris*4 );
		m_shader->disableVertexAttribs();
	}
}

inline void ExplodeVolumeRender::drawEdges()
{

	m_shaderL->enableVertexAttribs();
	glDrawArrays(GL_TRIANGLES , 0 , m_nbLines*3 );
	m_shaderL->disableVertexAttribs();
}

inline void ExplodeVolumeRender::setExplodeVolumes(float explode)
{
	if (m_smooth)
		m_shaderS->setExplodeVolumes(explode);
	else
		m_shader->setExplodeVolumes(explode);
	m_shaderL->setExplodeVolumes(explode);
}

inline void ExplodeVolumeRender::setExplodeFaces(float explode)
{
	if (m_smooth)
		m_shaderS->setExplodeFaces(explode);
	else
		m_shader->setExplodeFaces(explode);
}

inline void ExplodeVolumeRender::setClippingPlane(const Geom::Vec4f& p)
{
	if (m_smooth)
		m_shaderS->setClippingPlane(p);
	else
		m_shader->setClippingPlane(p);
	m_shaderL->setClippingPlane(p);
}

inline void ExplodeVolumeRender::setNoClippingPlane()
{
	Geom::Vec4f p(1.0f,1.0f,1.0f,-99999999.9f);
	if (m_smooth)
		m_shaderS->setClippingPlane(p);
	else
		m_shader->setClippingPlane(p);
	m_shaderL->setClippingPlane(p);
}

inline void ExplodeVolumeRender::setAmbiant(const Geom::Vec4f& ambiant)
{
	if (m_smooth)
		m_shaderS->setAmbiant(ambiant);
	else
		m_shader->setAmbiant(ambiant);
}

inline void ExplodeVolumeRender::setBackColor(const Geom::Vec4f& color)
{
	if (m_smooth)
		m_shaderS->setBackColor(color);
	else
		m_shader->setBackColor(color);
}

inline void ExplodeVolumeRender::setLightPosition(const Geom::Vec3f& lp)
{
	if (m_smooth)
		m_shaderS->setLightPosition(lp);
	else
		m_shader->setLightPosition(lp);
}

inline void ExplodeVolumeRender::setColorLine(const Geom::Vec4f& col)
{
	m_shaderL->setColor(col);
}

inline Utils::GLSLShader* ExplodeVolumeRender::shaderFaces()
{
	if (m_smooth)
		return m_shaderS;
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
