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
#include <cmath>

#include "Geometry/vector_gen.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

inline explodeVolume_VBORender::explodeVolume_VBORender()
{
	glGenBuffersARB(2, m_VBOBuffers);
}


explodeVolume_VBORender::~explodeVolume_VBORender()
{
	glDeleteBuffersARB(2, m_VBOBuffers);
}

template<typename PFP>
void explodeVolume_VBORender::updateData(typename PFP::MAP& map, const FunctorSelect& good, const typename PFP::TVEC3& positions,float kf, float kv)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	m_nbTris = 0;

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
					unsigned int nbe=0;
					do
					{
						if (!markVert.isMarked(dNext))
						{
							markVert.mark(dNext);
							center += positions[dNext];
							nbv++;
						}
						mark.mark(dNext);					// Mark
						nbe++;
						Dart adj = map.phi2(dNext);				// Get adjacent face
						if (adj != dNext && !mark.isMarked(adj))
							visitedFaces.push_back(adj);	// Add it
						dNext = map.phi1(dNext);
					} while(dNext != *face);
					m_nbTris += nbe-2;
				}
			}
			center /= typename PFP::REAL(nbv);
			vecCenters.push_back(center);
			vecNbFaces.push_back(nbf);
		}
	}

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
	glBufferDataARB(GL_ARRAY_BUFFER, m_nbTris*3*3*sizeof(GLfloat), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* positionBuf = reinterpret_cast<VEC3*>(PositionBuffer);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
	glBufferDataARB(GL_ARRAY_BUFFER, m_nbTris*3*3*sizeof(GLfloat), 0, GL_STREAM_DRAW);
	GLvoid* NormalBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);
	VEC3* normalBuf = reinterpret_cast<VEC3*>(NormalBuffer);


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

			// compute Normal
			unsigned int angle = 1;
			VEC3 N;
			do{
				VEC3 V =  vecPos[angle+1] -  vecPos[angle];
				VEC3 U =  vecPos[angle] -  vecPos[angle-1];
				N = U^V;
				angle++;
			}while (std::isnan(N[0]) && (angle < nb));

			N.normalize();

			// compute position of points to use for drawing topo
			for (unsigned int i = 2; i < nb; ++i)
			{
				*positionBuf++ =  vecPos[0];
				*normalBuf++ = N;
				*positionBuf++ =  vecPos[i-1];
				*normalBuf++ = N;
				*positionBuf++ =  vecPos[i];
				*normalBuf++ = N;
				d = map.phi1(d);
			}

		}
	}

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[0]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_VBOBuffers[1]);
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER);

}


inline void explodeVolume_VBORender::drawFaces()
{
	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER, m_VBOBuffers[1]);
	glNormalPointer(GL_FLOAT, 0, 0);
	glEnableClientState(GL_NORMAL_ARRAY);

	glDrawArrays(GL_TRIANGLES, 0, m_nbTris*3);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}


}//end namespace VBO

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN
