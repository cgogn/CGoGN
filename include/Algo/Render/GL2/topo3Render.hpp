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

#include "Geometry/vector_gen.h"
#include "Topology/generic/autoAttributeHandler.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"

#include "Topology/map/map3.h"
#include "Topology/gmap/gmap3.h"


namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

//template<>
//template<typename PFP>
//void topo3_VBORenderMap<Map3>::updateData(Map3& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv)
//{
//	typedef typename PFP::VEC3 VEC3;
//	typedef typename PFP::REAL REAL;
//
//	m_nbDarts = 0;
//
//	// table of center of volume
//	std::vector<VEC3> vecCenters;
//	vecCenters.reserve(1000);
//	// table of nbfaces per volume
//	std::vector<unsigned int> vecNbFaces;
//	vecNbFaces.reserve(1000);
//	// table of face (one dart of each)
//	std::vector<Dart> vecDartFaces;
//	vecDartFaces.reserve(map.getNbDarts()/4);
//
//	DartMarker mark(map);					// marker for darts
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if (good(d))
//		{
//			CellMarkerStore markVert(map, VERTEX_CELL);		//marker for vertices
//			VEC3 center(0, 0, 0);
//			unsigned int nbv = 0;
//			unsigned int nbf = 0;
//			std::list<Dart> visitedFaces;	// Faces that are traversed
//			visitedFaces.push_back(d);		// Start with the face of d
//
//			// For every face added to the list
//			for (std::list<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//			{
//				if (!mark.isMarked(*face))		// Face has not been visited yet
//				{
//					// store a dart of face
//					vecDartFaces.push_back(*face);
//					nbf++;
//					Dart dNext = *face ;
//					do
//					{
//						if (!markVert.isMarked(dNext))
//						{
//							markVert.mark(dNext);
//							center += positions[dNext];
//							nbv++;
//						}
//						mark.mark(dNext);					// Mark
//						m_nbDarts++;
//						Dart adj = map.phi2(dNext);				// Get adjacent face
//						if (adj != dNext && !mark.isMarked(adj))
//							visitedFaces.push_back(adj);	// Add it
//						dNext = map.phi1(dNext);
//					} while(dNext != *face);
//				}
//			}
//			center /= typename PFP::REAL(nbv);
//			vecCenters.push_back(center);
//			vecNbFaces.push_back(nbf);
//		}
//	}
//
//	// debut phi1
//	AutoAttributeHandler<VEC3> fv1(map, DART_ORBIT);
//	// fin phi1
//	AutoAttributeHandler<VEC3> fv11(map, DART_ORBIT);
//
//	// phi2
//	AutoAttributeHandler<VEC3> fv2(map, DART_ORBIT);
//	AutoAttributeHandler<VEC3> fv2x(map, DART_ORBIT);
//
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
//	GLvoid* PositionDartsBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);
//
//	std::vector<Dart>::iterator face = vecDartFaces.begin();
//	for (unsigned int iVol=0; iVol<vecNbFaces.size(); ++iVol)
//	{
//		for (unsigned int iFace = 0; iFace < vecNbFaces[iVol]; ++iFace)
//		{
//			Dart d = *face++;
//
//			std::vector<VEC3> vecPos;
//			vecPos.reserve(16);
//
//			// store the face & center
//			VEC3 center(0, 0, 0);
//			Dart dd = d;
//			do
//			{
//				const VEC3& P = positions[d];
//				vecPos.push_back(P);
//				center += P;
//				d = map.phi1(d);
//			} while (d != dd);
//			center /= REAL(vecPos.size());
//
//			//shrink the face
//			unsigned int nb = vecPos.size();
//			float okf = 1.0f - kf;
//			float okv = 1.0f - kv;
//			for (unsigned int i = 0; i < nb; ++i)
//			{
//				vecPos[i] = vecCenters[iVol]*okv + vecPos[i]*kv;
//				vecPos[i] = center*okf + vecPos[i]*kf;
//			}
//			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop
//
//			// compute position of points to use for drawing topo
//			float oke = 1.0f - ke;
//			for (unsigned int i = 0; i < nb; ++i)
//			{
//				VEC3 P = vecPos[i]*ke + vecPos[i+1]*oke;
//				VEC3 Q = vecPos[i+1]*ke + vecPos[i]*oke;
//
//				*positionDartBuf++ = P;
//				*positionDartBuf++ = Q;
//
//				fv1[d] = P*0.1f + Q*0.9f;
//				fv11[d] = P*0.9f + Q*0.1f;
//
//				fv2[d] = P*0.52f + Q*0.48f;
//				fv2x[d] = P*0.48f + Q*0.52f;
//
//				d = map.phi1(d);
//			}
//
//		}
//	}
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[0]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	// phi1
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer1 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//	//phi2
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[2]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer2 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//	//phi3
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(typename PFP::VEC3), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer3 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
//	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);
//	VEC3* positionF3 = reinterpret_cast<VEC3*>(PositionBuffer3);
//
//	m_nbRel2=0;
//	m_nbRel3=0;
//
//	for(std::vector<Dart>::iterator face = vecDartFaces.begin(); face != vecDartFaces.end(); ++face)
//	{
//		Dart d = *face;
//		do
//		{
//			Dart e = map.phi2(d);
//			if (d < e)
//			{
//				*positionF2++ = fv2[d];
//				*positionF2++ = fv2x[e];
//				*positionF2++ = fv2[e];
//				*positionF2++ = fv2x[d];
//				m_nbRel2++;
//			}
//			e = map.phi3(d);
//			if (d < e)
//			{
//				*positionF3++ = fv2[d];
//				*positionF3++ = fv2x[e];
//				*positionF3++ = fv2[e];
//				*positionF3++ = fv2x[d];
//				m_nbRel3++;
//			}
//			e = map.phi1(d);
//			*positionF1++ = fv1[d];
//			*positionF1++ = fv11[e];
//
//			d = map.phi1(d);
//		} while (d != *face );
//	}
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[1]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[2]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[3]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//}


//template<>
//template<typename PFP>
//void topo3_VBORenderMap<GMap3>::updateData(GMap3& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv)
//{
//	typedef typename PFP::VEC3 VEC3;
//	typedef typename PFP::REAL REAL;
//
//	m_nbDarts = 0;
//
//	// table of center of volume
//	std::vector<VEC3> vecCenters;
//	vecCenters.reserve(1000);
//	// table of nbfaces per volume
//	std::vector<unsigned int> vecNbFaces;
//	vecNbFaces.reserve(1000);
//	// table of face (one dart of each)
//	std::vector<Dart> vecDartFaces;
//	vecDartFaces.reserve(map.getNbDarts()/4);
//
//	DartMarker mark(map);					// marker for darts
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if (good(d))
//		{
//			CellMarkerStore markVert(map, VERTEX_CELL);		//marker for vertices
//			VEC3 center(0, 0, 0);
//			unsigned int nbv = 0;
//			unsigned int nbf = 0;
//			std::list<Dart> visitedFaces;	// Faces that are traversed
//			visitedFaces.push_back(d);		// Start with the face of d
//
//			// For every face added to the list
//			for (std::list<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//			{
//				if (!mark.isMarked(*face))		// Face has not been visited yet
//				{
//					// store a dart of face
//					vecDartFaces.push_back(*face);
//					nbf++;
//					Dart dNext = *face ;
//					do
//					{
//						if (!markVert.isMarked(dNext))
//						{
//							markVert.mark(dNext);
//							center += positions[dNext];
//							nbv++;
//						}
//						mark.mark(dNext);					// Mark
//						m_nbDarts++;
//						m_nbDarts++;
//						Dart adj = map.phi2(dNext);				// Get adjacent face
//						if (adj != dNext && !mark.isMarked(adj))
//							visitedFaces.push_back(adj);	// Add it
//						dNext = map.phi1(dNext);
//					} while(dNext != *face);
//				}
//			}
//			center /= typename PFP::REAL(nbv);
//			vecCenters.push_back(center);
//			vecNbFaces.push_back(nbf);
//		}
//	}
//
//	// sommets du brin
////	AutoAttributeHandler<VEC3> vert(map, DART_ORBIT);
////	AutoAttributeHandler<VEC3> vert2(map, DART_ORBIT);
//
//	// debut phi1
//	AutoAttributeHandler<VEC3> fv1(map, DART_ORBIT);
//
//	// phi2/3
//	AutoAttributeHandler<VEC3> fv2(map, DART_ORBIT);
//	AutoAttributeHandler<VEC3> fv2x(map, DART_ORBIT);
//
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionDartsBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);
//
//	std::vector<Dart>::iterator face = vecDartFaces.begin();
//
//	std::vector<VEC3> vecPos;
//	vecPos.reserve(16);
//
//	for (unsigned int iVol=0; iVol<vecNbFaces.size(); ++iVol)
//	{
//		for (unsigned int iFace = 0; iFace < vecNbFaces[iVol]; ++iFace)
//		{
//			Dart d = *face++;
//
//			vecPos.clear();
//
//			// store the face & center
//			VEC3 center(0, 0, 0);
//			Dart dd = d;
//			do
//			{
//				const VEC3& P = positions[d];
//				vecPos.push_back(P);
//				center += P;
//				d = map.phi1(d);
//			} while (d != dd);
//			center /= REAL(vecPos.size());
//
//			//shrink the face
//			unsigned int nb = vecPos.size();
//			float okf = 1.0f - kf;
//			float okv = 1.0f - kv;
//			for (unsigned int i = 0; i < nb; ++i)
//			{
//				vecPos[i] = vecCenters[iVol]*okv + vecPos[i]*kv;
//				vecPos[i] = center*okf + vecPos[i]*kf;
//			}
//			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop
//
//			// compute position of points to use for drawing topo
//			float oke = 1.0f - ke;
//			for (unsigned int i = 0; i < nb; ++i)
//			{
//				VEC3 P = vecPos[i]*REAL(ke) + vecPos[i+1]*REAL(oke);
//				VEC3 Q = vecPos[i+1]*REAL(ke) + vecPos[i]*REAL(oke);
//
//				VEC3 PP = REAL(0.52)*P + REAL(0.48)*Q;
//				VEC3 QQ = REAL(0.52)*Q + REAL(0.48)*P;
//
//				*positionDartBuf++ = P;
//				*positionDartBuf++ = PP;
//
//				*positionDartBuf++ = Q;
//				*positionDartBuf++ = QQ;
//
//				Dart d0 = map.beta0(d);
//
//				fv1[d] = PP*REAL(0.1) + P*REAL(0.9);
//				fv2[d] = P*REAL(0.54) + PP*REAL(0.46);
//				fv2x[d] = P*REAL(0.46) + PP*REAL(0.54);
//
//				fv1[d0] = QQ*REAL(0.1) + Q*REAL(0.9);
//				fv2x[d0] = Q*REAL(0.54) + QQ*REAL(0.46);
//				fv2[d0] = Q*REAL(0.46) + QQ*REAL(0.54);
//
//				d = map.beta1(d0);
//			}
//		}
//	}
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[0]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	// phi1
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer1 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//
//	//phi2
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[2]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer2 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//	//phi3
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer3 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//
//	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
//	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);
//	VEC3* positionF3 = reinterpret_cast<VEC3*>(PositionBuffer3);
//
//	for(std::vector<Dart>::iterator face = vecDartFaces.begin(); face != vecDartFaces.end(); ++face)
//	{
//		Dart d = *face;
//		do
//		{
//
//			Dart e = map.beta2(d);
//			if (d < e)
//			{
//				*positionF2++ = fv2[d];
//				*positionF2++ = fv2x[e];
//				*positionF2++ = fv2[e];
//				*positionF2++ = fv2x[d];
//				m_nbRel2++;
//			}
//			e = map.beta3(d);
//			if (d < e)
//			{
//				*positionF3++ = fv2[d];
//				*positionF3++ = fv2x[e];
//				*positionF3++ = fv2[e];
//				*positionF3++ = fv2x[d];
//				m_nbRel3++;
//			}
//			e = map.beta1(d);
//			*positionF1++ = fv1[d];
//			*positionF1++ = fv1[e];
//
//			d = map.phi1(d);
//		} while (d != *face );
//	}
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[1]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[2]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[3]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//}



template<typename PFP>
void Topo3RenderMapD::updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;


	if (m_attIndex.map() != &map)
	{
		m_attIndex  = map.template addAttribute<unsigned int>(DART_ORBIT, "dart_index");
	}

	m_nbDarts = 0;

	// table of center of volume
	std::vector<VEC3> vecCenters;
	vecCenters.reserve(1000);
	// table of nbfaces per volume
	std::vector<unsigned int> vecNbFaces;
	vecNbFaces.reserve(1000);
	// table of face (one dart of each)
	std::vector<Dart> vecDartFaces;
	vecDartFaces.reserve(map.getNbDarts()/4);

	DartMarker mark(map);					// marker for darts
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (good(d))
		{
			CellMarkerStore markVert(map, VERTEX_CELL);		//marker for vertices
			VEC3 center(0, 0, 0);
			unsigned int nbv = 0;
			unsigned int nbf = 0;
			std::list<Dart> visitedFaces;	// Faces that are traversed
			visitedFaces.push_back(d);		// Start with the face of d

			// For every face added to the list
			for (std::list<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
			{
				if (!mark.isMarked(*face))		// Face has not been visited yet
				{
					// store a dart of face
					vecDartFaces.push_back(*face);
					nbf++;
					Dart dNext = *face ;
					do
					{
						if (!markVert.isMarked(dNext))
						{
							markVert.mark(dNext);
							center += positions[dNext];
							nbv++;
						}
						mark.mark(dNext);					// Mark
						m_nbDarts++;
						Dart adj = map.phi2(dNext);				// Get adjacent face
						if (adj != dNext && !mark.isMarked(adj))
							visitedFaces.push_back(adj);	// Add it
						dNext = map.phi1(dNext);
					} while(dNext != *face);
				}
			}
			center /= typename PFP::REAL(nbv);
			vecCenters.push_back(center);
			vecNbFaces.push_back(nbf);
		}
	}

	// debut phi1
	AutoAttributeHandler<VEC3> fv1(map, DART_ORBIT);
	// fin phi1
	AutoAttributeHandler<VEC3> fv11(map, DART_ORBIT);

	// phi2
	AutoAttributeHandler<VEC3> fv2(map, DART_ORBIT);
	AutoAttributeHandler<VEC3> fv2x(map, DART_ORBIT);

	m_vbo4->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* ColorDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* colorDartBuf = reinterpret_cast<VEC3*>(ColorDartsBuffer);

	m_vbo0->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);
	unsigned int posDBI=0;



	std::vector<Dart>::iterator face = vecDartFaces.begin();
	for (unsigned int iVol=0; iVol<vecNbFaces.size(); ++iVol)
	{
		for (unsigned int iFace = 0; iFace < vecNbFaces[iVol]; ++iFace)
		{
			Dart d = *face++;

			std::vector<VEC3> vecPos;
			vecPos.reserve(16);

			// store the face & center
			VEC3 center(0, 0, 0);
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
			float okf = 1.0f - kf;
			float okv = 1.0f - kv;
			for (unsigned int i = 0; i < nb; ++i)
			{
				vecPos[i] = vecCenters[iVol]*okv + vecPos[i]*kv;
				vecPos[i] = center*okf + vecPos[i]*kf;
			}
			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

			// compute position of points to use for drawing topo
			float oke = 1.0f - ke;
			for (unsigned int i = 0; i < nb; ++i)
			{
				VEC3 P = vecPos[i]*ke + vecPos[i+1]*oke;
				VEC3 Q = vecPos[i+1]*ke + vecPos[i]*oke;

				*positionDartBuf++ = P;
				*positionDartBuf++ = Q;
				*colorDartBuf++ = VEC3(1.,1.,1.);
				*colorDartBuf++ = VEC3(1.,1.,1.);

				fv1[d] = P*0.1f + Q*0.9f;
				fv11[d] = P*0.9f + Q*0.1f;

				fv2[d] = P*0.52f + Q*0.48f;
				fv2x[d] = P*0.48f + Q*0.52f;

				d = map.phi1(d);
			}

		}
	}

	m_vbo0->bind();
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
			Dart e = map.phi2(d);
			if (d < e)
			{
				*positionF2++ = fv2[d];
				*positionF2++ = fv2x[e];
				*positionF2++ = fv2[e];
				*positionF2++ = fv2x[d];
				m_nbRel2++;
			}
			e = map.phi3(d);
			if (d < e)
			{
				*positionF3++ = fv2[d];
				*positionF3++ = fv2x[e];
				*positionF3++ = fv2[e];
				*positionF3++ = fv2x[d];
				m_nbRel3++;
			}
			e = map.phi1(d);
			*positionF1++ = fv1[d];
			*positionF1++ = fv11[e];

			d = map.phi1(d);
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




//
//
//
//template<typename PFP>
//void topo3RenderGMap::updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv)
//{
//	typedef typename PFP::VEC3 VEC3;
//	typedef typename PFP::REAL REAL;
//
//	m_nbDarts = 0;
//
//	// table of center of volume
//	std::vector<VEC3> vecCenters;
//	vecCenters.reserve(1000);
//	// table of nbfaces per volume
//	std::vector<unsigned int> vecNbFaces;
//	vecNbFaces.reserve(1000);
//	// table of face (one dart of each)
//	std::vector<Dart> vecDartFaces;
//	vecDartFaces.reserve(map.getNbDarts()/4);
//
//	DartMarker mark(map);					// marker for darts
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if (good(d))
//		{
//			CellMarkerStore markVert(map, VERTEX_CELL);		//marker for vertices
//			VEC3 center(0, 0, 0);
//			unsigned int nbv = 0;
//			unsigned int nbf = 0;
//			std::list<Dart> visitedFaces;	// Faces that are traversed
//			visitedFaces.push_back(d);		// Start with the face of d
//
//			// For every face added to the list
//			for (std::list<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//			{
//				if (!mark.isMarked(*face))		// Face has not been visited yet
//				{
//					// store a dart of face
//					vecDartFaces.push_back(*face);
//					nbf++;
//					Dart dNext = *face ;
//					do
//					{
//						if (!markVert.isMarked(dNext))
//						{
//							markVert.mark(dNext);
//							center += positions[dNext];
//							nbv++;
//						}
//						mark.mark(dNext);					// Mark
//						m_nbDarts++;
//						m_nbDarts++;
//						Dart adj = map.phi2(dNext);				// Get adjacent face
//						if (adj != dNext && !mark.isMarked(adj))
//							visitedFaces.push_back(adj);	// Add it
//						dNext = map.phi1(dNext);
//					} while(dNext != *face);
//				}
//			}
//			center /= typename PFP::REAL(nbv);
//			vecCenters.push_back(center);
//			vecNbFaces.push_back(nbf);
//		}
//	}
//
//	// sommets du brin
////	AutoAttributeHandler<VEC3> vert(map, DART_ORBIT);
////	AutoAttributeHandler<VEC3> vert2(map, DART_ORBIT);
//
//	// debut phi1
//	AutoAttributeHandler<VEC3> fv1(map, DART_ORBIT);
//
//	// phi2/3
//	AutoAttributeHandler<VEC3> fv2(map, DART_ORBIT);
//	AutoAttributeHandler<VEC3> fv2x(map, DART_ORBIT);
//
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionDartsBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//	VEC3* positionDartBuf = reinterpret_cast<VEC3*>(PositionDartsBuffer);
//
//	std::vector<Dart>::iterator face = vecDartFaces.begin();
//
//	std::vector<VEC3> vecPos;
//	vecPos.reserve(16);
//
//	for (unsigned int iVol=0; iVol<vecNbFaces.size(); ++iVol)
//	{
//		for (unsigned int iFace = 0; iFace < vecNbFaces[iVol]; ++iFace)
//		{
//			Dart d = *face++;
//
//			vecPos.clear();
//
//			// store the face & center
//			VEC3 center(0, 0, 0);
//			Dart dd = d;
//			do
//			{
//				const VEC3& P = positions[d];
//				vecPos.push_back(P);
//				center += P;
//				d = map.phi1(d);
//			} while (d != dd);
//			center /= REAL(vecPos.size());
//
//			//shrink the face
//			unsigned int nb = vecPos.size();
//			float okf = 1.0f - kf;
//			float okv = 1.0f - kv;
//			for (unsigned int i = 0; i < nb; ++i)
//			{
//				vecPos[i] = vecCenters[iVol]*okv + vecPos[i]*kv;
//				vecPos[i] = center*okf + vecPos[i]*kf;
//			}
//			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop
//
//			// compute position of points to use for drawing topo
//			float oke = 1.0f - ke;
//			for (unsigned int i = 0; i < nb; ++i)
//			{
//				VEC3 P = vecPos[i]*REAL(ke) + vecPos[i+1]*REAL(oke);
//				VEC3 Q = vecPos[i+1]*REAL(ke) + vecPos[i]*REAL(oke);
//
//				VEC3 PP = REAL(0.52)*P + REAL(0.48)*Q;
//				VEC3 QQ = REAL(0.52)*Q + REAL(0.48)*P;
//
//				*positionDartBuf++ = P;
//				*positionDartBuf++ = PP;
//
//				*positionDartBuf++ = Q;
//				*positionDartBuf++ = QQ;
//
//				Dart d0 = map.beta0(d);
//
//				fv1[d] = PP*REAL(0.1) + P*REAL(0.9);
//				fv2[d] = P*REAL(0.54) + PP*REAL(0.46);
//				fv2x[d] = P*REAL(0.46) + PP*REAL(0.54);
//
//				fv1[d0] = QQ*REAL(0.1) + Q*REAL(0.9);
//				fv2x[d0] = Q*REAL(0.54) + QQ*REAL(0.46);
//				fv2[d0] = Q*REAL(0.46) + QQ*REAL(0.54);
//
//				d = map.beta1(d0);
//			}
//		}
//	}
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[0]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	// phi1
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer1 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//
//	//phi2
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[2]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer2 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//	//phi3
//	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[3]);
//	glBufferDataARB(GL_ARRAY_BUFFER, 6*m_nbDarts*sizeof(GLfloat), 0, GL_STREAM_DRAW);
//	GLvoid* PositionBuffer3 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
//
//
//	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
//	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);
//	VEC3* positionF3 = reinterpret_cast<VEC3*>(PositionBuffer3);
//
//	for(std::vector<Dart>::iterator face = vecDartFaces.begin(); face != vecDartFaces.end(); ++face)
//	{
//		Dart d = *face;
//		do
//		{
//
//			Dart e = map.beta2(d);
//			if (d < e)
//			{
//				*positionF2++ = fv2[d];
//				*positionF2++ = fv2x[e];
//				*positionF2++ = fv2[e];
//				*positionF2++ = fv2x[d];
//				m_nbRel2++;
//			}
//			e = map.beta3(d);
//			if (d < e)
//			{
//				*positionF3++ = fv2[d];
//				*positionF3++ = fv2x[e];
//				*positionF3++ = fv2[e];
//				*positionF3++ = fv2x[d];
//				m_nbRel3++;
//			}
//			e = map.beta1(d);
//			*positionF1++ = fv1[d];
//			*positionF1++ = fv1[e];
//
//			d = map.phi1(d);
//		} while (d != *face );
//	}
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[1]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[2]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//
//	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[3]);
//	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);
//}



template<typename PFP>
void Topo3Render::setDartsIdColor(typename PFP::MAP& map, const FunctorSelect& good)
{

	m_vbo4->bind();
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
Dart Topo3Render::picking(typename PFP::MAP& map, const FunctorSelect& good, int x, int y)
{
	pushColors();
	setDartsIdColor<PFP>(map,good);
	Dart d = pickColor(x,y);
	popColors();
	return d;

}



}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
