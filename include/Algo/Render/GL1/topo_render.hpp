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

#include <limits>
#include "Topology/generic/autoAttributeHandler.h"

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
//void renderTopoGM2(typename PFP::MAP& the_map, Marker m)
//{
//
//
//	std::vector<gmtl::Vec3f> fv1;
//	std::vector<gmtl::Vec3f> fv2;
//	fv1.reserve(the_map.getNbDarts());
//	fv2.reserve(the_map.getNbDarts());
//
//	// first pass render the triangles
//	FunctorGM2Face<PFP> fgl_gc(the_map,fv1,fv2,m);
//
//	glLineWidth(1.0f);
//	glBegin(GL_LINES);
//	glColor3f(1.0,1.0,1.0);
//
//	the_map.foreach_orbit(2,fgl_gc);
//	glEnd();
//	glLineWidth(2.0f);
//	glBegin(GL_LINES);
//	for(Dart d = the_map.begin(); d!= the_map.end(); the_map.next(d))
//	{
//		glColor3f(0.0,1.0,1.0);
//		int i=d->getLabel();
//		int j=(the_map.alpha1(d))->getLabel();
//		glVertex3fv(fv1[i].getData());
//		glVertex3fv(fv1[j].getData());
//		glColor3f(1.0,0.0,0.0);
//		j=(the_map.alpha(2,d))->getLabel();
//		glVertex3fv(fv2[i].getData());
//		glVertex3fv(fv2[j].getData());
//	}
//	glEnd();
//}



template <typename PFP>
void renderTopoMD2(typename PFP::MAP& the_map, const typename PFP::TVEC3& positions, bool drawPhi1, bool drawPhi2, float ke, float kf)
{
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;
	
	AutoAttributeHandler<Geom::Vec3f> fv1(the_map, DART_ORBIT);
	AutoAttributeHandler<Geom::Vec3f> fv11(the_map, DART_ORBIT);
	AutoAttributeHandler<Geom::Vec3f> fv2(the_map, DART_ORBIT);
	AutoAttributeHandler<Geom::Vec3f> vert(the_map, DART_ORBIT);

	glLineWidth(2.0f);
	glColor3f(0.9f,0.9f,0.9f);
	glBegin(GL_LINES);

	DartMarker mf(the_map);
	for(Dart d = the_map.begin(); d!= the_map.end(); the_map.next(d))
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
				d = the_map.phi1(d);
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
				d = the_map.phi1(d);
			}
			mf.markOrbit(FACE_ORBIT, d);
		}
	}

	for(Dart d = the_map.begin(); d != the_map.end(); the_map.next(d))
	{
		Dart e = the_map.phi2(d);
		if ((d<e) && drawPhi2)
		{
			glColor3f(1.0,0.0,0.0);
			glVertex3fv(fv2[d].data());
			glVertex3fv(fv2[e].data());
		}
		if (drawPhi1)
		{
			e = the_map.phi1(d);
			glColor3f(0.0f,1.0f,1.0f);
			glVertex3fv(fv1[d].data());
			glVertex3fv(fv11[e].data());
		}
	}
	
	glEnd(); // LINES

	glPointSize(5.0f);
	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_POINTS);
	for(Dart d = the_map.begin(); d!= the_map.end(); the_map.next(d))
	{
		glVertex3fv(vert[d].data());
	}
	glEnd();

}

//
//
//template<typename PFP>
//class FunctorMD3Face : public FunctorMap<typename PFP::MAP>
//{
//	typedef typename PFP::MAP MAP;
//	typedef typename PFP::EMB EMB;
//
//
//	protected:
//		std::vector<gmtl::Vec3f>& m_vf1;
//
//		gmtl::Vec3f m_volCenter;
//		int m_lab;
//		float m_ke;
//		float m_kf;
//		float m_kv;
//		Marker m_mark;
//		bool m_drawPhi1;
//
//
//	public:
//	FunctorMD3Face(MAP& map, std::vector<gmtl::Vec3f>& vf1, float ke, float kf, float kv, Marker m, bool df1):
//		FunctorMap<MAP>(map), m_vf1(vf1), m_lab(0), m_ke(ke), m_kf(kf), m_kv(kv), m_mark(m),m_drawPhi1(df1)
//	{}
//
//	void setVolCenter(const gmtl::Vec3f& vc) { m_volCenter = vc;}
//
//	bool operator()(Dart d)
//	{
//		this->m_map.markDart(d,m_mark);
//
//		gmtl::Vec3f vPosPhi1[256];	// max 256 sides per face
//
//		std::vector<gmtl::Vec3f> vecPos;
//		vecPos.reserve(16);
//		gmtl::Vec3f centerFace(0.0f,0.0f,0.0f);
//
//		float k = 1.0f - m_kv;
//
//		Dart dd = d;
//		do
//		{
//			const gmtl::Vec3f& P = this->m_map.getVertexEmb(dd)->getPosition();
//			gmtl::Vec3f Q = m_volCenter*k + m_kv*P;
//			centerFace += Q;
//			vecPos.push_back(Q);
//			dd->setLabel(m_lab++);
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
//			const gmtl::Vec3f& P = vecPos[i];
//			gmtl::Vec3f Q = vecPos[i+1]*ke + vecPos[i]*k;
//			glColor3f(1.0f,1.0f,0.0f);
//			glVertex3fv(P.getData());
//			glColor3f(1.0f,1.0f,1.0f);
//			glVertex3fv(Q.getData());
//			gmtl::Vec3f f = P*0.5f + Q*0.5f;
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
//				int k=d->getLabel();
//				int j=(i+1)%nb;
//				gmtl::Vec3f fv3 = 2.0f*m_vf1[k] - vPosPhi1[i];
//				glVertex3fv(fv3.getData());
//				glVertex3fv(vPosPhi1[j].getData());
//				d = this->m_map.phi1(d);
//			}
//		}
//		return false;
//	}
//};
//
//
//
//template <typename PFP>
//void renderTopoMD3(typename PFP::MAP& the_map, bool drawPhi1, bool drawPhi2, bool drawPhi3, float ke, float kf, float kv, FunctorType& good)
//{
//
//
//	std::vector<gmtl::Vec3f> vPosPhi23;
//
//	vPosPhi23.reserve(the_map.getNbDarts());
//
// 	glLineWidth(1.0f);
//	glBegin(GL_LINES);
//	glColor3f(1.0f,1.0f,1.0f);
//
//	std::vector<Dart> vecDarts;
//	vecDarts.reserve(50);
//	FunctorStore fvol(vecDarts);
//
//	Marker markerFace = the_map.getNewMarker(DART_ORBIT);
//	FunctorMD3Face<PFP> fgl_cd(the_map,vPosPhi23,ke,kf,kv,markerFace,drawPhi1);
//
//	// Scan all darts of the map
//	for(Dart d = the_map.begin(); d != the_map.end(); the_map.next(d))
//	{
//		if ( good(d) && (!the_map.isMarkedDart(d,markerFace))) // foreach volume (not already marked)
//		{
//			gmtl::Vec3f centerVol= Geometry::volumeCentroid<PFP>(the_map,d) ; // compute center
//			fgl_cd.setVolCenter(centerVol);		//set center of volume in face drawing fonctor
//
//			vecDarts.clear();
//			the_map.foreach_dart_of_oriented_volume(d,fvol);	// get all darts of voluùe
//
//			for (typename std::vector<Dart>::iterator it = vecDarts.begin(); it != vecDarts.end(); ++it)
//			{
//				if (!the_map.isMarkedDart(*it,markerFace))	// for all face of the volume
//				{
//					fgl_cd(*it);
//				}
//			}
//		}
//	 }
//
//	the_map.unmarkAll(markerFace);
//	the_map.releaseMarker(markerFace);
//
// 	glEnd();
// 	glLineWidth(2.0f);
// 	glBegin(GL_LINES);
//
//	// si on doit tracer phi2 & phi3 : unes seule passe
//	if ( drawPhi2 && drawPhi3)
//	{
//		for(Dart d = the_map.begin(); d!= the_map.end(); the_map.next(d))
//		{
//			if (good(d))
//			{
//				int i=d->getLabel();
//				glTexCoord3f(vPosPhi23[i][0],vPosPhi23[i][1],vPosPhi23[i][2]);
//
//				if (good(the_map.phi2(d)))
//				{
//					glColor3f(1.0,0.0,0.0);
//					int j=(the_map.phi2(d))->getLabel();
//					glVertex3fv(vPosPhi23[i].getData());
//					glVertex3fv(vPosPhi23[j].getData());
//				}
//				if (good(the_map.phi3(d)))
//				{
//					glColor3f(1.0,0.0,1.0);
//					int j=(the_map.phi3(d))->getLabel();
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
//		for(Dart d = the_map.begin(); d!= the_map.end(); the_map.next(d))
//		{
//			if (good(d) && good(the_map.phi2(d)))
//			{
//				glColor3f(1.0,0.0,0.0);
//				int i=d->getLabel();
//				glTexCoord3f(vPosPhi23[i][0],vPosPhi23[i][1],vPosPhi23[i][2]);
//				int	j=(the_map.phi2(d))->getLabel();
//				glVertex3fv(vPosPhi23[i].getData());
//				glVertex3fv(vPosPhi23[j].getData());
//			}
//		}
//	}
//
//	if (drawPhi3)
//	{
//		for(Dart d = the_map.begin(); d!= the_map.end(); the_map.next(d))
//		{
//			if (good(d) && good(the_map.phi3(d)))
//			{
//				glColor3f(1.0,0.0,1.0);
//				int i=d->getLabel();
//				glTexCoord3f(vPosPhi23[i][0],vPosPhi23[i][1],vPosPhi23[i][2]);
//				int	j=(the_map.phi3(d))->getLabel();
//				glVertex3fv(vPosPhi23[i].getData());
//				glVertex3fv(vPosPhi23[j].getData());
//			}
//		}
//	}
//
//	glEnd();	// end Lines for phi
//
//}


} // endnamespace
} // endnamespace
} // endnamespace
} // endnamespace

