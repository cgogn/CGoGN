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

#include "Geometry/vector_gen.h"
#include "Topology/generic/autoAttributeHandler.h"
#include "Topology/generic/dartmarker.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

template<typename PFP>
void TopoRenderMapD::updateData(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, const FunctorSelect& good)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	std::vector<Dart> vecDarts;
	vecDarts.reserve(map.getNbDarts()/4); // not too big but not too many reallocation

	if (m_attIndex.map() != &map)
	{
		m_attIndex  = map.template addAttribute<unsigned int>(DART_ORBIT, "dart_index");
	}

	for(Dart d = map.begin(); d!= map.end(); map.next(d))
	{
		if (good(d))
		{
			vecDarts.push_back(d);
		}
	}
	m_nbDarts = vecDarts.size();

	// debut phi1
	AutoAttributeHandler<VEC3> fv1(map, DART_ORBIT);
	// fin phi1
	AutoAttributeHandler<VEC3> fv11(map, DART_ORBIT);
	// phi2
	AutoAttributeHandler<VEC3> fv2(map, DART_ORBIT);

	m_vbo3->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* ColorDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* colorDartBuf = reinterpret_cast<VEC3*>(ColorDartsBuffer);

	m_vbo0->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);
	unsigned int posDBI = 0;

	DartMarker mf(map);
	for(std::vector<Dart>::iterator id = vecDarts.begin(); id!= vecDarts.end(); id++)
	{
		Dart d = *id;
		if (!mf.isMarked(d))
		{
			std::vector<VEC3> vecPos;
			vecPos.reserve(16);

			// store the face & center
			VEC3 center(0.0f,0.0f,0.0f);
			Dart dd = d;
			do
			{
				const VEC3& P = positions[d];
				vecPos.push_back(P);
				m_attIndex[d] = posDBI;
				posDBI+=2;
				center += P;
				d = map.phi1(d);
			} while (d != dd);
			center /= REAL(vecPos.size());

			//shrink the face
			unsigned int nb = vecPos.size();
			float k = 1.0f - kf;
			for (unsigned int i = 0; i < nb; ++i)
			{
				vecPos[i] = center*k + vecPos[i]*kf;
			}
			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

			k = 1.0f - ke;
			for (unsigned int i = 0; i < nb; ++i)
			{
				VEC3 P = vecPos[i]*ke + vecPos[i+1]*k;
				VEC3 Q = vecPos[i+1]*ke + vecPos[i]*k;

				*positionDartBuf++ = P;
				*positionDartBuf++ = Q;
				*colorDartBuf++ = VEC3(1.,1.,1.);
				*colorDartBuf++ = VEC3(1.,1.,1.);

				VEC3 f = P*0.5f + Q*0.5f;
				fv2[d] = f;
				f = P*0.1f + Q*0.9f;
				fv1[d] = f;
				f = P*0.9f + Q*0.1f;
				fv11[d] = f;
				d = map.phi1(d);
			}
			mf.markOrbit(FACE_ORBIT, d);
		}
	}

	m_vbo0->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo3->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo1->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer1 = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	m_vbo2->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer2 = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);

	m_nbRel2 =0;

	for(std::vector<Dart>::iterator id = vecDarts.begin(); id!= vecDarts.end(); id++)
	{
		Dart d = *id;

		Dart e = map.phi2(d);
		if (e.index > d.index)
		{
			*positionF2++ = fv2[d];
			*positionF2++ = fv2[e];
			m_nbRel2++;
		}

		e = map.phi1(d);
		*positionF1++ = fv1[d];
		*positionF1++ = fv11[e];
	}

	m_vbo1->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo2->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

}


template<typename PFP>
void TopoRenderGMap::updateData(typename PFP::MAP& map, const typename PFP::TVEC3& positions, float ke, float kf, const FunctorSelect& good)
{
	/*
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	std::vector<Dart> vecDarts;
	vecDarts.reserve(map.getNbDarts()/4); // not too big but not too many reallocation

	if (m_attIndex.map() != &map)
	{
		m_attIndex  = map.template addAttribute<unsigned int>(DART_ORBIT, "dart_index");
	}

	for(Dart d = map.begin(); d!= map.end(); map.next(d))
	{
		if (good(d))
		{
			vecDarts.push_back(d);
		}
	}
	m_nbDarts = vecDarts.size();

	// beta1
	AutoAttributeHandler<VEC3> fv1(map, DART_ORBIT);
	// beta2
	AutoAttributeHandler<VEC3> fv2(map, DART_ORBIT);
	// sommets du brin
//	AutoAttributeHandler<VEC3> vert(map, DART_ORBIT);
	// 2ieme sommet du brin
//	AutoAttributeHandler<VEC3> vert2(map, DART_ORBIT);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* ColorDartsBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* colorDartBuf = reinterpret_cast<VEC3*>(ColorDartsBuffer);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionDartsBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);
	unsigned int posDBI=0;

	DartMarker mf(map);
	for(std::vector<Dart>::iterator id = vecDarts.begin(); id!= vecDarts.end(); id++)
	{
		Dart d = *id;
		if (!mf.isMarked(d))
		{
			std::vector<VEC3> vecPos;
			vecPos.reserve(16);

			// store the face & center
			VEC3 center(0.0f,0.0f,0.0f);
			Dart dd = d;
			do
			{
				const VEC3& P = positions[d];
				vecPos.push_back(P);
				center += P;
				m_attIndex[d] = posDBI;
				posDBI+=2;
				m_attIndex[map.beta0(d)] = posDBI;
				posDBI+=2;
				d = map.phi1(d);
			} while (d != dd);
			center /= REAL(vecPos.size());

			//shrink the face
			unsigned int nb = vecPos.size();
			float k = 1.0f - kf;
			for (unsigned int i = 0; i < nb; ++i)
			{
				vecPos[i] = center*k + vecPos[i]*kf;
			}
			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

			k = 1.0f - ke;
			for (unsigned int i = 0; i < nb; ++i)
			{
				VEC3 P = vecPos[i]*ke + vecPos[i+1]*k;
				VEC3 Q = vecPos[i+1]*ke + vecPos[i]*k;
				VEC3 PP = REAL(0.52)*P + REAL(0.48)*Q;
				VEC3 QQ = REAL(0.52)*Q + REAL(0.48)*P;

				*positionDartBuf++ = P;
				*positionDartBuf++ = PP;
				*colorDartBuf++ = VEC3(1.,1.,1.);
				*colorDartBuf++ = VEC3(1.,1.,1.);

				*positionDartBuf++ = Q;
				*positionDartBuf++ = QQ;
				*colorDartBuf++ = VEC3(1.,1.,1.);
				*colorDartBuf++ = VEC3(1.,1.,1.);

				Dart d0 = map.beta0(d);

				VEC3 f = Q*REAL(0.5) + QQ*REAL(0.5);
				fv2[d0] = f;
				f = QQ*REAL(0.2) + Q*REAL(0.8);
				fv1[d0] = f;

				f = P*REAL(0.5) + PP*REAL(0.5);
				fv2[d] = f;
				f = PP*REAL(0.2) + P*REAL(0.8);
				fv1[d] = f;

				d = map.phi1(d);
			}
			mf.markOrbit(FACE_ORBIT, d);
		}
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[0]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[3]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer1 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[2]);
	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer2 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);

	m_nbRel2=0;

	for(std::vector<Dart>::iterator id = vecDarts.begin(); id!= vecDarts.end(); id++)
	{
		Dart d = *id;

		Dart e = map.beta2(d);
		if (e.index > d.index)
		{
			*positionF2++ = fv2[d];
			*positionF2++ = fv2[e];
			m_nbRel2++;
		}


		e = map.beta1(d);
		*positionF1++ = fv1[d];
		*positionF1++ = fv1[e];
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[1]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[2]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
*/
}


template<typename PFP>
void TopoRender::setDartsIdColor(typename PFP::MAP& map, const FunctorSelect& good)
{

	m_vbo3->bind();
	float* colorBuffer =  reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
	unsigned int nb=0;

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (nb < m_nbDarts)
		{
			if (good(d))
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
		}
		else
		{
			CGoGNerr << "Error buffer too small for color picking (change the good parameter ?)" << CGoGNendl;
			d = map.end();
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

template<typename PFP>
Dart TopoRender::picking(typename PFP::MAP& map, const FunctorSelect& good, int x, int y)
{
	pushColors();
	setDartsIdColor<PFP>(map,good);
	Dart d = pickColor(x,y);
	popColors();
	return d;

}


}//end namespace GL2

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
