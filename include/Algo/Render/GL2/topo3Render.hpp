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

#include "Geometry/vector_gen.h"
#include "Topology/generic/autoAttributeHandler.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"

#include "Topology/map/map3.h"
#include "Topology/gmap/gmap3.h"

#include "Topology/generic/traversorCell.h"
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{
template<typename PFP>
void Topo3Render::updateData(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, float kv, const FunctorSelect& good)
{
	Map3* ptrMap3 = dynamic_cast<Map3*>(&map);
	if (ptrMap3 != NULL)
	{
		updateDataMap3<PFP>(map,positions,ke,kf,kv,good);
	}
	GMap3* ptrGMap3 = dynamic_cast<GMap3*>(&map);
	if (ptrGMap3 != NULL)
	{
		updateDataGMap3<PFP>(map,positions,ke,kf,kv,good);
	}
}


template<typename PFP>
void Topo3Render::updateDataMap3(typename PFP::MAP& mapx, const typename PFP::TVEC3& positions, float ke, float kf, float kv, const FunctorSelect& good)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

//	Map3& map = dynamic_cast<Map3&>(mapx);

	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;


	if (m_attIndex.map() != &mapx)
	{
		m_attIndex  = mapx.template getAttribute<unsigned int>(DART, "dart_index");
		if (!m_attIndex.isValid())
			m_attIndex  = mapx.template addAttribute<unsigned int>(DART, "dart_index");
	}

	m_nbDarts = 0;
	for (Dart d = mapx.begin(); d != mapx.end(); mapx.next(d))
	{
		if (good(d))
			m_nbDarts++;
	}

	// compute center of each volumes
	CellMarker cmv(mapx,VOLUME);
	AutoAttributeHandler<VEC3> centerVolumes(mapx,VOLUME,"centerVolumes");

	Algo::Geometry::computeCentroidVolumes<PFP>(mapx,positions,centerVolumes,good);


	// debut phi1
	AutoAttributeHandler<VEC3> fv1(mapx, DART);
	// fin phi1
	AutoAttributeHandler<VEC3> fv11(mapx, DART);

	// phi2
	AutoAttributeHandler<VEC3> fv2(mapx, DART);
	AutoAttributeHandler<VEC3> fv2x(mapx, DART);

	m_vbo4->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* ColorDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* colorDartBuf = reinterpret_cast<VEC3*>(ColorDartsBuffer);

	m_vbo0->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);


	std::vector<Dart> vecDartFaces;
	vecDartFaces.reserve(m_nbDarts/3);
	unsigned int posDBI=0;

	// traverse each face of each volume
	TraversorCell<typename PFP::MAP> traFace(mapx, PFP::MAP::ORBIT_IN_PARENT(FACE),good);
	for (Dart d=traFace.begin(); d!=traFace.end(); d=traFace.next())
	{
		vecDartFaces.push_back(d);

		std::vector<VEC3> vecPos;
		vecPos.reserve(16);

		// store the face & center
		float okv = 1.0f - kv;

		VEC3 vc = centerVolumes[d];
		VEC3 centerFace(0, 0, 0);
		Dart dd = d;
		do
		{
			VEC3 P = positions[dd];
			P  = vc*okv + P*kv;
			vecPos.push_back(P);
			centerFace += P;
			dd = mapx.phi1(dd);
		} while (dd != d);
		centerFace /= REAL(vecPos.size());

		//shrink the face
		unsigned int nb = vecPos.size();

		float okf = 1.0f - kf;

		for (unsigned int i = 0; i < nb; ++i)
		{
			vecPos[i] = centerFace*okf + vecPos[i]*kf;
		}
		vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

		// compute position of points to use for drawing topo
		float oke = 1.0f - ke;
		for (unsigned int i = 0; i < nb; ++i)
		{
			VEC3 P = vecPos[i]*ke + vecPos[i+1]*oke;
			VEC3 Q = vecPos[i+1]*ke + vecPos[i]*oke;

			m_attIndex[d] = posDBI;
			posDBI+=2;

			*positionDartBuf++ = P;
			*positionDartBuf++ = Q;
			*colorDartBuf++ = m_dartsColor;
			*colorDartBuf++ = m_dartsColor;

			fv1[d] = P*0.1f + Q*0.9f;
			fv11[d] = P*0.9f + Q*0.1f;

			fv2[d] = P*0.52f + Q*0.48f;
			fv2x[d] = P*0.48f + Q*0.52f;

			d = mapx.phi1(d);
		}
	}

	m_vbo0->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo4->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// phi1
	m_vbo1->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer1 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	//phi2
	m_vbo2->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer2 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	//phi3
	m_vbo3->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer3 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);
	VEC3* positionF3 = reinterpret_cast<VEC3*>(PositionBuffer3);

	m_nbRel2=0;
	m_nbRel3=0;

	for(std::vector<Dart>::iterator face = vecDartFaces.begin(); face != vecDartFaces.end(); ++face)
	{
		Dart d = *face;
		do
		{
			Dart e = mapx.phi2(d);
			if (d < e)
			{
				*positionF2++ = fv2[d];
				*positionF2++ = fv2x[e];
				*positionF2++ = fv2[e];
				*positionF2++ = fv2x[d];
				m_nbRel2++;
			}
			e = mapx.phi3(d);
			if (!mapx.isBoundaryMarked(e) && (d < e))
			{
				*positionF3++ = fv2[d];
				*positionF3++ = fv2x[e];
				*positionF3++ = fv2[e];
				*positionF3++ = fv2x[d];
				m_nbRel3++;
			}
			e = mapx.phi1(d);
			*positionF1++ = fv1[d];
			*positionF1++ = fv11[e];

			d = mapx.phi1(d);
		} while (d != *face );
	}

	m_vbo3->bind();
	glUnmapBufferARB(GL_ARRAY_BUFFER);

	m_vbo2->bind();
	glUnmapBufferARB(GL_ARRAY_BUFFER);

	m_vbo1->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo4->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
}



template<typename PFP>
void Topo3Render::setDartsIdColor(typename PFP::MAP& map, const FunctorSelect& good)
{
	m_vbo4->bind();
	float* colorBuffer =  reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
	unsigned int nb=0;

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (good(d))
		{
			if (nb < m_nbDarts)
			{
				float r,g,b;
				dartToCol(d, r,g,b);
				
				float* local = colorBuffer+3*m_attIndex[d]; // get the right position in VBO
				*local++ = r;
				*local++ = g;
				*local++ = b;
				*local++ = r;
				*local++ = g;
				*local++ = b;

				nb++;
			}
			else
			{
				CGoGNerr << "Error buffer too small for color picking (change the good parameter ?)" << CGoGNendl;
				break;
			}
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

template<typename PFP>
Dart Topo3Render::picking(typename PFP::MAP& map, int x, int y, const FunctorSelect& good)
{
	pushColors();
	setDartsIdColor<PFP>(map,good);
	Dart d = pickColor(x,y);
	popColors();
	return d;
}



template<typename PFP>
void Topo3Render::updateDataGMap3(typename PFP::MAP& mapx, const typename PFP::TVEC3& positions, float ke, float kf, float kv, const FunctorSelect& good)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	GMap3& map = dynamic_cast<GMap3&>(mapx);	// TODO reflechir comment virer ce warning quand on compile avec PFP::MAP=Map3

	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;


	if (m_attIndex.map() != &mapx)
	{
		m_attIndex  = mapx.template getAttribute<unsigned int>(DART, "dart_index");
		if (!m_attIndex.isValid())
			m_attIndex  = mapx.template addAttribute<unsigned int>(DART, "dart_index");
	}

	m_nbDarts = 0;
	for (Dart d = mapx.begin(); d != mapx.end(); mapx.next(d))
	{
		if (good(d))
			m_nbDarts++;
	}

	// compute center of each volumes
	AutoAttributeHandler<VEC3> centerVolumes(mapx,VOLUME,"centerVolumes");
	Algo::Geometry::computeCentroidVolumes<PFP>(mapx,positions,centerVolumes,good);


	// beta1
	AutoAttributeHandler<VEC3> fv1(mapx, DART);
	// beta2/3
	AutoAttributeHandler<VEC3> fv2(mapx, DART);
	AutoAttributeHandler<VEC3> fv2x(mapx, DART);

	m_vbo4->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* ColorDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* colorDartBuf = reinterpret_cast<VEC3*>(ColorDartsBuffer);

	m_vbo0->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);


	std::vector<Dart> vecDartFaces;
	vecDartFaces.reserve(m_nbDarts/6);
	unsigned int posDBI=0;

	//traverse each face of each volume
	TraversorCell<typename PFP::MAP> traFace(mapx, PFP::MAP::ORBIT_IN_PARENT(FACE),good);
	for (Dart d=traFace.begin(); d!=traFace.end(); d=traFace.next())
	{
		vecDartFaces.push_back(d);

		std::vector<VEC3> vecPos;
		vecPos.reserve(16);

		// store the face & center
		float okv = 1.0f - kv;

		VEC3 vc = centerVolumes[d];
		VEC3 centerFace(0, 0, 0);
		Dart dd = d;
		do
		{
			VEC3 P = positions[dd];
			P  = vc*okv + P*kv;
			vecPos.push_back(P);
			centerFace += P;
			dd = mapx.phi1(dd);
		} while (dd != d);
		centerFace /= REAL(vecPos.size());

		//shrink the face
		unsigned int nb = vecPos.size();

		float okf = 1.0f - kf;

		for (unsigned int i = 0; i < nb; ++i)
		{
			vecPos[i] = centerFace*okf + vecPos[i]*kf;
		}
		vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

		// compute position of points to use for drawing topo
		float oke = 1.0f - ke;
		for (unsigned int i = 0; i < nb; ++i)
		{
			VEC3 P = vecPos[i]*ke + vecPos[i+1]*oke;
			VEC3 Q = vecPos[i+1]*ke + vecPos[i]*oke;

			VEC3 PP = 0.52f*P + 0.48f*Q;
			VEC3 QQ = 0.52f*Q + 0.48f*P;

			*positionDartBuf++ = P;
			*positionDartBuf++ = PP;
			*positionDartBuf++ = Q;
			*positionDartBuf++ = QQ;
			*colorDartBuf++ = m_dartsColor;
			*colorDartBuf++ = m_dartsColor;
			*colorDartBuf++ = m_dartsColor;
			*colorDartBuf++ = m_dartsColor;

			m_attIndex[d] = posDBI;
			posDBI+=2;


			fv1[d] = P*0.9f + PP*0.1f;
			fv2x[d] = P*0.52f + PP*0.48f;
			fv2[d] = P*0.48f + PP*0.52f;
			Dart dx = map.beta0(d);
			fv1[dx] = Q*0.9f + QQ*0.1f;
			fv2[dx] = Q*0.52f + QQ*0.48f;
			fv2x[dx] = Q*0.48f + QQ*0.52f;

			m_attIndex[dx] = posDBI;
			posDBI+=2;

			d = mapx.phi1(d);		}
	}

	m_vbo0->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo4->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// beta3
	m_vbo1->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer1 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	// beta3
	m_vbo2->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer2 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	// beta3
	m_vbo3->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer3 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);
	VEC3* positionF3 = reinterpret_cast<VEC3*>(PositionBuffer3);

	m_nbRel2=0;
	m_nbRel3=0;

	for(std::vector<Dart>::iterator face = vecDartFaces.begin(); face != vecDartFaces.end(); ++face)
	{
		Dart d = *face;
		do
		{
			Dart e = map.beta2(d);
			if (d < e)
			{
				*positionF2++ = fv2[d];
				*positionF2++ = fv2x[e];
				*positionF2++ = fv2[e];
				*positionF2++ = fv2x[d];
				m_nbRel2++;
			}
			e = map.beta3(d);
			if (!map.isBoundaryMarked(e) && (d < e))
			{
				*positionF3++ = fv2[d];
				*positionF3++ = fv2x[e];
				*positionF3++ = fv2[e];
				*positionF3++ = fv2x[d];
				m_nbRel3++;
			}
			d = map.beta0(d);
			e = map.beta2(d);
			if (d < e)
			{
				*positionF2++ = fv2[d];
				*positionF2++ = fv2x[e];
				*positionF2++ = fv2[e];
				*positionF2++ = fv2x[d];
				m_nbRel2++;
			}
			e = map.beta3(d);
			if (!map.isBoundaryMarked(e) && (d < e))
			{
				*positionF3++ = fv2[d];
				*positionF3++ = fv2x[e];
				*positionF3++ = fv2[e];
				*positionF3++ = fv2x[d];
				m_nbRel3++;
			}
			*positionF1++ = fv1[d];
			d = map.beta1(d);
			*positionF1++ = fv1[d];
		} while (d != *face );
	}

	m_vbo3->bind();
	glUnmapBufferARB(GL_ARRAY_BUFFER);

	m_vbo2->bind();
	glUnmapBufferARB(GL_ARRAY_BUFFER);

	m_vbo1->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo4->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
}



template<typename PFP>
void Topo3Render::computeDartMiddlePositions(typename PFP::MAP& map, typename PFP::TVEC3& posExpl, const FunctorSelect& good)
{
	m_vbo0->bind();
	typename PFP::VEC3* positionsPtr = reinterpret_cast<typename PFP::VEC3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (good(d))
		{
			posExpl[d] = (positionsPtr[m_attIndex[d]] + positionsPtr[m_attIndex[d]+1])*0.5f;
		}
	}

	m_vbo0->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
}



}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
