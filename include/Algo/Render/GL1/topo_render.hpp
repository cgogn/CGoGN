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

#include <limits>
#include "Topology/generic/autoAttributeHandler.h"

#include "Geometry/vector_gen.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL1
{
	
//template<typename PFP>
//class FunctorGM2Face: public FunctorMap<typename PFP::MAP>
//{
//	typedef typename PFP::MAP MAP;
//	typedef typename PFP::EMB EMB;
//
//
//protected:
//	std::vector<gmtl::Vec3f>& m_vf1;
//	std::vector<gmtl::Vec3f>& m_vf2;
//	int m_lab;
//	Marker m_mark;
//
//public:
//	FunctorGM2Face(MAP& map, std::vector<gmtl::Vec3f>& vf1, std::vector<gmtl::Vec3f>& vf2, Marker m):
//		FunctorMap<MAP>(map), m_vf1(vf1), m_vf2(vf2), m_lab(0), m_mark(m)
//	{}
//
//	bool operator()(Dart d)
//	{
//		std::vector<gmtl::Vec3f> vecPos;
//		vecPos.reserve(16);
//
//		Dart dd = d;
//		int nb=0;
//		gmtl::Vec3f center = Geometry::faceCentroid<PFP>(this->m_map, d);
//		do
//		{
//			const gmtl::Vec3f& P = this->m_map.getVertexEmb(d)->getPosition();
//			vecPos.push_back(P);
//			nb++;
//			d->setLabel(m_lab++);
//			d = this->m_map.alpha(0,d);
//			d->setLabel(m_lab++);
//			d = this->m_map.alpha1(d);
//		} while (d!=dd);
//
//		for (int i=0; i<nb; ++i)
//		{
//			vecPos[i] = center*0.05f + vecPos[i]*0.95f;
//		}
//		vecPos.push_back(vecPos.front());
//
//		for (int i=0; i<nb; ++i)
//		{
//			if (this->m_map.isMarkedDart(d,m_mark))
//				glColor3f(1.,1.,0.);
//			else
//				glColor3f(1.,1.,1.);
//			// pour d
//			gmtl::Vec3f P = vecPos[i]*0.9f + vecPos[i+1]*0.1f;
//			gmtl::Vec3f M = vecPos[i]*0.55f + vecPos[i+1]*0.45f;
//			glVertex3fv(P.getData());
//			glVertex3fv(M.getData());
//			gmtl::Vec3f f = vecPos[i]*0.8f + vecPos[i+1]*0.2f;
//			m_vf1.push_back(f);
//			f = vecPos[i]*0.6f + vecPos[i+1]*0.4f;
//			m_vf2.push_back(f);
//
//			d = this->m_map.alpha(0,d);
//
//			if (this->m_map.isMarkedDart(d,m_mark))
//				glColor3f(1.,1.,0.);
//			else
//				glColor3f(1.,1.,1.);
//			// pour alpha(0,d)
//			P = vecPos[i+1]*0.9f + vecPos[i]*0.1f;
//			M = vecPos[i+1]*0.55f + vecPos[i]*0.45f;
//			glVertex3fv(P.getData());
//			glVertex3fv(M.getData());
//			f = vecPos[i+1]*0.8f + vecPos[i]*0.2f;
//			m_vf1.push_back(f);
//			f = vecPos[i+1]*0.6f + vecPos[i]*0.4f;
//			m_vf2.push_back(f);
//
//			d = this->m_map.alpha1(d);
//		}
//		return false;
//	}
//};
//
//template <typename PFP>
//void renderTopoGM2(typename PFP::MAP& map, Marker m)
//{
//
//
//	std::vector<gmtl::Vec3f> fv1;
//	std::vector<gmtl::Vec3f> fv2;
//	fv1.reserve(map.getNbDarts());
//	fv2.reserve(map.getNbDarts());
//
//	// first pass render the triangles
//	FunctorGM2Face<PFP> fgl_gc(map,fv1,fv2,m);
//
//	glLineWidth(1.0f);
//	glBegin(GL_LINES);
//	glColor3f(1.0,1.0,1.0);
//
//	map.foreach_orbit(2,fgl_gc);
//	glEnd();
//	glLineWidth(2.0f);
//	glBegin(GL_LINES);
//	for(Dart d = map.begin(); d!= map.end(); map.next(d))
//	{
//		glColor3f(0.0,1.0,1.0);
//		int i=d->getLabel();
//		int j=(map.alpha1(d))->getLabel();
//		glVertex3fv(fv1[i].getData());
//		glVertex3fv(fv1[j].getData());
//		glColor3f(1.0,0.0,0.0);
//		j=(map.alpha(2,d))->getLabel();
//		glVertex3fv(fv2[i].getData());
//		glVertex3fv(fv2[j].getData());
//	}
//	glEnd();
//}



template <typename PFP>
void renderTopoMD2(typename PFP::MAP& map, const typename PFP::TVEC3& positions, bool drawPhi1, bool drawPhi2, float ke, float kf)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;
	
	AutoAttributeHandler<Geom::Vec3f> fv1(map, DART);
	AutoAttributeHandler<Geom::Vec3f> fv11(map, DART);
	AutoAttributeHandler<Geom::Vec3f> fv2(map, DART);
	AutoAttributeHandler<Geom::Vec3f> vert(map, DART);

	glLineWidth(2.0f);
	glColor3f(0.9f,0.9f,0.9f);
	glBegin(GL_LINES);

	DartMarker mf(map);
	for(Dart d = map.begin(); d!= map.end(); map.next(d))
	{
		if (!mf.isMarked(d))
		{
			std::vector<VEC3> vecPos;
			vecPos.reserve(16);

			std::vector<VEC3> vecF1;
			vecF1.reserve(16);

			// store the face & center
			VEC3 center(0.0f,0.0f,0.0f);
			Dart dd = d;
			do
			{
				const VEC3& P = positions[d];
				vecPos.push_back(positions[d]);
				center += P;
				d = map.phi1(d);
			}while (d!=dd);
			center /= REAL(vecPos.size());

			//shrink the face
			unsigned int nb = vecPos.size();
			float k = 1.0f - kf;
			for (unsigned int i=0; i<nb; ++i)
			{
				vecPos[i] = center*k + vecPos[i]*kf;
			}
			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

			k = 1.0f - ke;
			for (unsigned int i=0; i<nb; ++i)
			{
				VEC3 P = vecPos[i]*ke + vecPos[i+1]*k;
				VEC3 Q = vecPos[i+1]*ke + vecPos[i]*k;
				glVertex3fv(P.data());
				glVertex3fv(Q.data());
				vert[d] = P;
				VEC3 f = P*0.5f + Q*0.5f;
				fv2[d] = f;
				f = P*0.1f + Q*0.9f;
				fv1[d] = f;
				f = P*0.9f + Q*0.1f;
				fv11[d] = f;
				d = map.phi1(d);
			}
			mf.markOrbit(FACE, d);
		}
	}

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		Dart e = map.phi2(d);
		if ((d<e) && drawPhi2)
		{
			glColor3f(1.0,0.0,0.0);
			glVertex3fv(fv2[d].data());
			glVertex3fv(fv2[e].data());
		}
		if (drawPhi1)
		{
			e = map.phi1(d);
			glColor3f(0.0f,1.0f,1.0f);
			glVertex3fv(fv1[d].data());
			glVertex3fv(fv11[e].data());
		}
	}
	
	glEnd(); // LINES

	glPointSize(5.0f);
	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_POINTS);
	for(Dart d = map.begin(); d!= map.end(); map.next(d))
	{
		glVertex3fv(vert[d].data());
	}
	glEnd();

}



//template<typename PFP>
//class FunctorMD3Face : public FunctorMap<typename PFP::MAP>
//{
//	typedef typename PFP::MAP MAP;
//	typedef typename PFP::VEC3 VEC3;
//
//
//	protected:
//		std::vector<VEC3>& m_vf1;
//
//		VEC3 m_volCenter;
//		int m_lab;
//		float m_ke;
//		float m_kf;
//		float m_kv;
//		DartMarkerStore m_mark;
//		bool m_drawPhi1;
//
//		typename PFP::TVEC3 m_positions;
//
//	public:
//	FunctorMD3Face(MAP& map, typename PFP::TVEC3& positions, std::vector<VEC3>& vf1, float ke, float kf, float kv, Marker m, bool df1):
//		FunctorMap<MAP>(map), m_positions(positions), m_vf1(vf1), m_lab(0), m_ke(ke), m_kf(kf), m_kv(kv), m_mark(m),m_drawPhi1(df1)
//	{}
//
//	void setVolCenter(const VEC3& vc) { m_volCenter = vc;}
//
//	bool operator()(Dart d)
//	{
//		this->m_mark.markDart(d);
//
//		VEC3 vPosPhi1[256];	// max 256 sides per face
//
//		std::vector<VEC3> vecPos;
//		vecPos.reserve(16);
//		VEC3 centerFace(0.0f,0.0f,0.0f);
//
//		float k = 1.0f - m_kv;
//
//		Dart dd = d;
//		do
//		{
//			VEC3 P = m_positions[dd];//this->m_map.getEmbedding(VERTEX,dd)->getPosition();
//			VEC3 Q = m_volCenter*k + m_kv*P;
//			centerFace += Q;
//			vecPos.push_back(Q);
//			dd = m_lab++;
//			dd = this->m_map.phi1(dd);
//		}while (dd!=d);
//
//		int nb=vecPos.size();
//
//		centerFace /= float(nb);
//
//		k = 1.0f - m_kf;
//		for (int i=0; i<nb; ++i)
//		{
//			vecPos[i] = centerFace*k + vecPos[i]*m_kf;
//		}
//		vecPos.push_back(vecPos.front());
//
//		glTexCoord3f(m_volCenter[0],m_volCenter[1],m_volCenter[2]);
//
//		k = (1.0f - m_ke)/2.0f;
//		float ke = 1.0f - k;
//
//		for (int i=0; i<nb; ++i)
//		{
//			VEC3 P = vecPos[i];
//			VEC3 Q = vecPos[i+1]*ke + vecPos[i]*k;
//			glColor3f(1.0f,1.0f,0.0f);
//			glVertex3fv(P);
//			glColor3f(1.0f,1.0f,1.0f);
//			glVertex3fv(Q);
//			VEC3 f = P*0.5f + Q*0.5f;
//			m_vf1.push_back(f);
//			f = P*0.9f + Q*0.1f;
//			vPosPhi1[i] = f;
//			d = this->m_map.phi1(d);
//		}
//
//		if (m_drawPhi1)
//		{
//			for (int i=0; i<nb; ++i)
//			{
//				glColor3f(0.0f,1.0f,1.0f);
//				int k=d;
//				int j=(i+1)%nb;
//				VEC3 fv3 = 2.0f*m_vf1[k] - vPosPhi1[i];
//				glVertex3fv(fv3);
//				glVertex3fv(vPosPhi1[j]);
//				d = this->m_map.phi1(d);
//			}
//		}
//		return false;
//	}
//};



//template <typename PFP>
//void renderTopoMD3(typename PFP::MAP& map, typename PFP::TVEC3& positions, bool drawPhi1, bool drawPhi2, bool drawPhi3, float ke, float kf, float kv, FunctorType& good)
//{
//	typedef typename PFP::VEC3 VEC3;
//	typedef typename PFP::REAL REAL;
//
//	std::vector<VEC3> vPosPhi23;
//
//	vPosPhi23.reserve(map.getNbDarts());
//
// 	glLineWidth(1.0f);
//	glBegin(GL_LINES);
//	glColor3f(1.0f,1.0f,1.0f);
//
//	std::vector<Dart> vecDarts;
//	vecDarts.reserve(50);
//	FunctorStore fvol(vecDarts);
//
//	DartMarkerStore markerFace(map);
//
//	FunctorMD3Face<PFP> fgl_cd(map,positions,vPosPhi23,ke,kf,kv,markerFace,drawPhi1);
//
//	// Scan all darts of the map
//	for(Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if ( good(d) && (!markerFace.isMarked(d))) // foreach volume (not already marked)
//		{
//			VEC3 centerVol= Algo::Geometry::volumeCentroid<PFP>(map,d, positions) ; // compute center
//			fgl_cd.setVolCenter(centerVol);		//set center of volume in face drawing fonctor
//
//			vecDarts.clear();
//			map.foreach_dart_of_oriented_volume(d,fvol);	// get all darts of voluùe
//
//			for (typename std::vector<Dart>::iterator it = vecDarts.begin(); it != vecDarts.end(); ++it)
//			{
//				if (!markerFace.isMarked(*it))	// for all face of the volume
//				{
//					fgl_cd(*it);
//				}
//			}
//		}
//	 }
//
//	markerFace.unmarkAll();
//
// 	glEnd();
// 	glLineWidth(2.0f);
// 	glBegin(GL_LINES);
//
//	// si on doit tracer phi2 & phi3 : unes seule passe
//	if ( drawPhi2 && drawPhi3)
//	{
//		for(Dart d = map.begin(); d!= map.end(); map.next(d))
//		{
//			if (good(d))
//			{
//				int i=d;
//				glTexCoord3f(vPosPhi23[i][0],vPosPhi23[i][1],vPosPhi23[i][2]);
//
//				if (good(map.phi2(d)))
//				{
//					glColor3f(1.0,0.0,0.0);
//					int j=(map.phi2(d));
//					glVertex3fv(vPosPhi23[i].getData());
//					glVertex3fv(vPosPhi23[j].getData());
//				}
//				if (good(map.phi3(d)))
//				{
//					glColor3f(1.0,0.0,1.0);
//					int j=(map.phi3(d));
//					glVertex3fv(vPosPhi23[i].getData());
//					glVertex3fv(vPosPhi23[j].getData());
//				}
//			}
//		}
//		glEnd();
//		return;	// on sort pour ne pas repasser dans les 2 if (pas très grave)
//	}
//
//	if (drawPhi2)
//	{
//		for(Dart d = map.begin(); d!= map.end(); map.next(d))
//		{
//			if (good(d) && good(map.phi2(d)))
//			{
//				glColor3f(1.0,0.0,0.0);
//				int i=d;
//				glTexCoord3f(vPosPhi23[i][0],vPosPhi23[i][1],vPosPhi23[i][2]);
//				int	j=(map.phi2(d));
//				glVertex3fv(vPosPhi23[i].getData());
//				glVertex3fv(vPosPhi23[j].getData());
//			}
//		}
//	}
//
//	if (drawPhi3)
//	{
//		for(Dart d = map.begin(); d!= map.end(); map.next(d))
//		{
//			if (good(d) && good(map.phi3(d)))
//			{
//				glColor3f(1.0,0.0,1.0);
//				int i=d;
//				glTexCoord3f(vPosPhi23[i][0],vPosPhi23[i][1],vPosPhi23[i][2]);
//				int	j=(map.phi3(d));
//				glVertex3fv(vPosPhi23[i].getData());
//				glVertex3fv(vPosPhi23[j].getData());
//			}
//		}
//	}
//
//	glEnd();	// end Lines for phi
//
//}

template <typename PFP>
void renderTopoMD3(typename PFP::MAP& map, typename PFP::TVEC3& positions, bool drawPhi1, bool drawPhi2, bool drawPhi3, float ke, float kf, float kv)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;


	AutoAttributeHandler<Geom::Vec3f> fv1(map, DART);
	AutoAttributeHandler<Geom::Vec3f> fv11(map, DART);
	AutoAttributeHandler<Geom::Vec3f> fv2(map, DART);
	AutoAttributeHandler<Geom::Vec3f> fv2x(map, DART);
	AutoAttributeHandler<Geom::Vec3f> vert(map, DART);


	int m_nbDarts = 0;

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

 	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f,1.0f,1.0f);

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
				//m_attIndex[d] = posDBI;
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

				vert[d] = P;

				glVertex3fv(P.data());
				glVertex3fv(Q.data());

				fv1[d] = P*0.1f + Q*0.9f;
				fv11[d] = P*0.9f + Q*0.1f;

				fv2[d] = P*0.52f + Q*0.48f;
				fv2x[d] = P*0.48f + Q*0.52f;

				d = map.phi1(d);
			}

		}
	}


	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		Dart e = map.phi2(d);
		if ((d<e) && drawPhi2)
		{
			glColor3f(1.0,0.0,0.0);
			glVertex3fv(fv2[d].data());
			glVertex3fv(fv2[e].data());
		}

		e = map.phi3(d);
		if ((d<e) && drawPhi3)
		{
			glColor3f(1.0,1.0,0.0);
			glVertex3fv(fv2[d].data());
			glVertex3fv(fv2[e].data());
		}
		if (drawPhi1)
		{
			e = map.phi1(d);
			glColor3f(0.0f,1.0f,1.0f);
			glVertex3fv(fv1[d].data());
			glVertex3fv(fv11[e].data());
		}
	}

	glEnd(); // LINES

	glPointSize(5.0f);
	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_POINTS);
	for(Dart d = map.begin(); d!= map.end(); map.next(d))
	{
		glVertex3fv(vert[d].data());
	}
	glEnd();

}


} // endnamespace
} // endnamespace
} // endnamespace
} // endnamespace

