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

template<typename PFP>
void Topo3RenderMapD::updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float ke, float kf, float kv)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;


	if (m_attIndex.map() != &map)
	{
		m_attIndex  = map.template addAttribute<unsigned int>(DART, "dart_index");
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
			CellMarkerStore markVert(map, VERTEX);		//marker for vertices
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
	AutoAttributeHandler<VEC3> fv1(map, DART);
	// fin phi1
	AutoAttributeHandler<VEC3> fv11(map, DART);

	// phi2
	AutoAttributeHandler<VEC3> fv2(map, DART);
	AutoAttributeHandler<VEC3> fv2x(map, DART);

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

template<typename PFP>
void Topo3RenderGMap::updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions, float kd, float ke, float kf, float kv)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	AutoAttributeHandler<Geom::Vec3f> posBeta1(map, DART);
	AutoAttributeHandler<Geom::Vec3f> posBeta2(map, DART); //beta 3 link is represented at the same location as beta2
	AutoAttributeHandler<Geom::Vec3f> posBeta2x(map, DART); //beta 3 link is represented at the same location as beta2
	AutoAttributeHandler<Geom::Vec3f> vert(map, DART);

	if (m_attIndex.map() != &map)
	{
		m_attIndex  = map.template addAttribute<unsigned int>(DART, "dart_index");
	}

	m_nbDarts = 0;

	// table of face (one dart of each)
	std::vector<Dart> vecDartFaces;
	vecDartFaces.reserve(map.getNbDarts()/6); //6 = nb of darts for tri mesh

	// table of nbfaces per volume
	std::vector<unsigned int> vecNbFaces;
	vecNbFaces.reserve(vecDartFaces.size());

	// table of center of volume
	std::vector<VEC3> vecVolCenters;
	vecVolCenters.reserve(vecDartFaces.size()/4); // = nb of volumes for a tetra mesh

	DartMarker mark(map);					// marker for darts
	CellMarker mVol(map, VOLUME);
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!mVol.isMarked(d))
		{
			mVol.mark(d);
			CellMarkerStore markVert(map, VERTEX);		//marker for vertices
			VEC3 center(0);
			unsigned int nbVertices = 0;
			unsigned int nbFace = 0;
			std::list<Dart> visitedFaces;	// Faces that are traversed
			visitedFaces.push_back(d);		// Start with the face of d

			// For every face added to the list
			for (std::list<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
			{
				if (!mark.isMarked(*face))		// Face has not been visited yet
				{
					// store a dart of face
					vecDartFaces.push_back(*face);
					nbFace++;
					Dart dNext = *face ;
					do
					{
						mark.mark(dNext); // Mark
						m_nbDarts++;

						if (!markVert.isMarked(dNext))
						{
							markVert.mark(dNext);
							center += positions[dNext];
							nbVertices++;
						}

						Dart adj = map.phi2(dNext); // add adjacent face if not done already
						if (adj != dNext && !mark.isMarked(adj))
							visitedFaces.push_back(adj);

						dNext = map.phi1(dNext);
					} while(dNext != *face);
				}
			}
			center /= typename PFP::REAL(nbVertices);
			vecVolCenters.push_back(center);
			vecNbFaces.push_back(nbFace);
		}
	}

	m_nbDarts *= 2; // x2 : only one orientation is used for the previous computation, multiply by 2 to get the number of darts

	//colors
	m_vbo4->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* ColorDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* colorDartBuf = reinterpret_cast<VEC3*>(ColorDartsBuffer);

	//darts
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
			VEC3 center(0);
			Dart dd = d;
			do
			{
				const VEC3& P = positions[dd];
				vecPos.push_back(P);
				m_attIndex[dd] = posDBI;
				posDBI+=2;
				m_attIndex[map.beta0(dd)] = posDBI; //for gmap : also affect a number to the other orientation for picking
				posDBI+=2;

				center += P;
				dd = map.phi1(dd);
			} while (dd != d);
			center /= REAL(vecPos.size());


			//shrink the face
			unsigned int nb = vecPos.size();
			float okf = 1.0f - kf;
			float okv = 1.0f - kv;
			for (unsigned int i = 0; i < nb; ++i)
			{
				vecPos[i] = vecVolCenters[iVol]*okv + vecPos[i]*kv;
				vecPos[i] = center*okf + vecPos[i]*kf;
			}
			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

			// compute position of points to use for drawing topo
			float oke = 1.0f - ke;
			for (unsigned int i = 0; i < nb; ++i)
			{
				VEC3 P = vecPos[i]*ke + vecPos[i+1]*oke;
				VEC3 Q = vecPos[i+1]*ke + vecPos[i]*oke;

				VEC3 P_mid = P+(Q-P)*kd*0.5f;
				VEC3 Q_mid = Q+(P-Q)*kd*0.5f;

				vert[d] = P;

				*positionDartBuf++ = P;
				*positionDartBuf++ = P_mid;
				*colorDartBuf++ = VEC3(1.,1.,1.);
				*colorDartBuf++ = VEC3(1.,1.,1.);

//					glColor3f(0.0f,0.0f,1.0f);
//					glVertex3fv(P_mid.data());
//					glVertex3fv(Q_mid.data());
//					glColor3f(1.0f,1.0f,1.0f);

				*positionDartBuf++ = Q;
				*positionDartBuf++ = Q_mid;
				*colorDartBuf++ = VEC3(1.,1.,1.);
				*colorDartBuf++ = VEC3(1.,1.,1.);

				posBeta1[d] = P*0.9f + P_mid*0.1f;
				posBeta1[map.beta0(d)] = Q*0.9f + Q_mid*0.1f;

				posBeta2[d] = P*0.52f + P_mid*0.48f;
				posBeta2x[d] = P*0.48f + P_mid*0.52f;
				posBeta2[map.beta0(d)] = Q*0.52f + Q_mid*0.48f;
				posBeta2x[map.beta0(d)] = Q*0.48f + Q_mid*0.52f;

				d = map.phi1(d);
			}
		}
	}

	//beta1
	m_vbo1->bind();
	glBufferData(GL_ARRAY_BUFFER, m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer1 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	//beta2
	m_vbo2->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer2 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	//beta3
	m_vbo3->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*m_nbDarts*sizeof(VEC3), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer3 = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	VEC3* positionF1 = reinterpret_cast<VEC3*>(PositionBuffer1);
	VEC3* positionF2 = reinterpret_cast<VEC3*>(PositionBuffer2);
	VEC3* positionF3 = reinterpret_cast<VEC3*>(PositionBuffer3);

	m_nbRel2=0;
	m_nbRel3=0;

	//draw beta1, beta2, beta3 if required
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		Dart e = map.beta2(d);
		if ((d<e))
		{
			glColor3f(1.0,0.0,0.0);
			*positionF2++ = posBeta2[d];
			*positionF2++ = posBeta2x[d];
			*positionF2++ = posBeta2x[e];
			*positionF2++ = posBeta2[e];
			m_nbRel2++;
		}

		e = map.beta3(d);
		if ((d<e))
		{
			glColor3f(1.0,1.0,0.0);
			*positionF3++ = posBeta2[d];
			*positionF3++ = posBeta2x[d];
			*positionF3++ = posBeta2x[e];
			*positionF3++ = posBeta2[e];
			m_nbRel3++;
		}

		e = map.beta1(d);
		if ((d<e))
		{
			e = map.beta1(d);
			*positionF1++ = posBeta1[d];
			*positionF1++ = posBeta1[e];
		}
	}

	m_vbo3->bind();
	glUnmapBufferARB(GL_ARRAY_BUFFER);

	m_vbo2->bind();
	glUnmapBufferARB(GL_ARRAY_BUFFER);

	m_vbo1->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo0->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_vbo4->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

}



}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
