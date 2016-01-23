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
#include "Topology/generic/parameters.h"

#include "Topology/map/embeddedMap2.h"
#include "Topology/gmap/embeddedGMap2.h"

#include "Algo/Geometry/basic.h"
#include "Geometry/distances.h"
#include "Algo/Geometry/centroid.h"
#include "Algo/Geometry/normal.h"

#include "Container/containerBrowser.h"
#include "Utils/Shaders/shaderColorDarts.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

template<typename MAP>
void TopoRender::overdrawDart(MAP& map, Dart d, float width, float r, float g, float b)
{
	if (d == NIL)
		return;

	DartAttribute<unsigned int, MAP> attIndex = map.template getAttribute<unsigned int, DART, MAP>(m_nameIndex);
	if (!attIndex.isValid())
		attIndex  = map.template addAttribute<unsigned int, DART, MAP>(m_nameIndex);

	unsigned int indexDart = attIndex[d];

	m_shader1->setColor(Geom::Vec4f(r,g,b,0.0f));
	m_shader1->setLineWidth(width);
	m_shader1->enableVertexAttribs();
	glDrawArrays(GL_LINES, indexDart, 2);
	m_shader1->disableVertexAttribs();
}


template<typename MAP>
void TopoRender::drawColoredDarts(MAP& map)
{
	DartAttribute<unsigned int, MAP> attIndex = map.template getAttribute<unsigned int, DART, MAP>(m_nameIndex);
	if (!attIndex.isValid())
		return;

	m_shader1->enableVertexAttribs();
	for (auto it=m_coloredDarts.begin(); it != m_coloredDarts.end(); ++it)
	{
		unsigned int indexDart = attIndex[it->d];

		m_shader1->directColor(Geom::Vec4f(it->r,it->g,it->b,0.0f));
//		m_shader1->bind(); // because of unbind, do a set color without bind/unbind
		glDrawArrays(GL_LINES, indexDart, 2);
	}
	m_shader1->disableVertexAttribs();
}



template<typename PFP>
void TopoRender::updateDataBoundary(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions)
{
	updateDataBoundary<PFP>(map,positions,m_ke,m_kf,m_ns);
}


template<typename PFP>
void TopoRender::updateDataBoundary(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, float ke, float kf, float ns)
{
	m_normalShift = ns;
	SelectorDartBoundary<typename PFP::MAP> sdb(map);
	DartContainerBrowserSelector<typename PFP::MAP> browser(map, sdb);
	browser.enable();
	this->updateData<PFP>(map, positions, ke, kf, false,true); // false,true because we are drawing the boundary of a 3map
	browser.disable();
	m_normalShift = 0.0f;
}


template<typename PFP>
void TopoRender::updateData(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, bool onlyBoundary)
{
	updateData<PFP>(map,positions,m_ke,m_kf,m_wb,onlyBoundary);
}

template<typename PFP>
void TopoRender::updateData(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, float ke, float kf, bool withBoundary, bool onlyBoundary)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::REAL REAL;

	std::string name_index(this->m_nameIndex);
	if (onlyBoundary)
		name_index = std::string("dart_boundary_index2");

	std::vector<Dart> vecDarts;
	vecDarts.reserve(map.getNbDarts());  // no problem dart is int: no problem of memory

	DartAttribute<unsigned int, MAP> attIndex = map.template getAttribute<unsigned int, DART, MAP>(name_index);
	if (!attIndex.isValid())
		attIndex  = map.template addAttribute<unsigned int, DART, MAP>(name_index);


	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (withBoundary || !map.template isBoundaryMarked<2>(d))
			vecDarts.push_back(d);
	}
	this->m_nbDarts = GLuint(vecDarts.size());

	// debut phi1
	DartAutoAttribute<VEC3, MAP> fv1(map);
	// fin phi1
	DartAutoAttribute<VEC3, MAP> fv11(map);
	// phi2
	DartAutoAttribute<VEC3, MAP> fv2(map);

	if (this->m_bufferDartPosition!=NULL)
		delete this->m_bufferDartPosition;
	this->m_bufferDartPosition = new Geom::Vec3f[2*this->m_nbDarts];
	Geom::Vec3f* positionDartBuf = reinterpret_cast<Geom::Vec3f*>(this->m_bufferDartPosition);

	std::vector<VEC3> vecPos;
	vecPos.reserve(16);

	unsigned int indexDC = 0;

	DartMarker<MAP> mf(map);
	for(std::vector<Dart>::iterator id = vecDarts.begin(); id != vecDarts.end(); id++)
	{
		Dart d = *id;
		if (!mf.isMarked(d))
		{
			vecPos.clear();
			if (!map.template isBoundaryMarked<2>(d))
			{
				//VEC3 center = Algo::Surface::Geometry::faceCentroidELW<PFP>(map,d,positions);
				VEC3 center = Algo::Surface::Geometry::faceCentroid<PFP>(map,d,positions);
				float k = 1.0f - kf;
				Dart dd = d;
				do
				{
					vecPos.push_back(center*k + positions[dd]*kf);
					dd = map.phi1(dd);
				} while (dd != d);

				if (this->m_normalShift > 0.0f)
				{
					VEC3 normal = Algo::Surface::Geometry::newellNormal<PFP>(map,d,positions);
					for (typename std::vector<VEC3>::iterator pit = vecPos.begin(); pit != vecPos.end(); ++pit)
					{
						*pit -= normal * this->m_normalShift;
					}
				}

				unsigned int nb = uint32(vecPos.size());
				vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

				k = 1.0f - ke;
				for (unsigned int i = 0; i < nb; ++i)
				{

					VEC3 P = vecPos[i]*ke + vecPos[i+1]*k;
					VEC3 Q = vecPos[i+1]*ke + vecPos[i]*k;

					attIndex[d] = indexDC;
					indexDC+=2;
					*positionDartBuf++ = PFP::toVec3f(P);
					*positionDartBuf++ = PFP::toVec3f(Q);
					VEC3 f = P*0.5f + Q*0.5f;
					fv2[d] = f;
					f = P*0.1f + Q*0.9f;
					fv1[d] = f;
					f = P*0.9f + Q*0.1f;
					fv11[d] = f;
					d = map.phi1(d);
				}
				mf.template markOrbit<FACE>(d);
			}
			else if (withBoundary)
			{
				Dart dd = d;
				do
				{
					Dart ee = map.phi2(dd);
					VEC3 normal = Algo::Surface::Geometry::newellNormal<PFP>(map,ee,positions);
					VEC3 vd = Algo::Geometry::vectorOutOfDart<PFP>(map,ee,positions);
					VEC3 v = vd ^ normal;
					v.normalize();
					VEC3 P = positions[map.phi1(ee)] + v* this->m_boundShift;
					vecPos.push_back(P);
					dd = map.phi1(dd);
					ee = map.phi2(dd);
					P = positions[map.phi1(ee)] + v* this->m_boundShift;
					vecPos.push_back(P);
				} while (dd != d);

				unsigned int nb = uint32(vecPos.size()/2);
				float k = 1.0f - ke;
				for (unsigned int i = 0; i < nb; ++i)
				{

					VEC3 P = vecPos[2*i]*ke + vecPos[2*i+1]*k;
					VEC3 Q = vecPos[2*i+1]*ke + vecPos[2*i]*k;

					attIndex[d] = indexDC;
					indexDC+=2;
					*positionDartBuf++ = PFP::toVec3f(P);
					*positionDartBuf++ = PFP::toVec3f(Q);
					VEC3 f = P*0.5f + Q*0.5f;
					fv2[d] = f;
					f = P*0.1f + Q*0.9f;
					fv1[d] = f;
					f = P*0.9f + Q*0.1f;
					fv11[d] = f;
					d = map.phi1(d);
				}
				mf.template markOrbit<FACE>(d);
			}
		}
	}

	this->m_vbo0->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*this->m_nbDarts*sizeof(Geom::Vec3f), this->m_bufferDartPosition, GL_STREAM_DRAW);


	this->m_vbo1->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*this->m_nbDarts*sizeof(Geom::Vec3f), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer1 = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	this->m_vbo2->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*this->m_nbDarts*sizeof(Geom::Vec3f), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer2 = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	Geom::Vec3f* positionF1 = reinterpret_cast<Geom::Vec3f*>(PositionBuffer1);
	Geom::Vec3f* positionF2 = reinterpret_cast<Geom::Vec3f*>(PositionBuffer2);

	this->m_nbRel2 = 0;
	for(std::vector<Dart>::iterator id = vecDarts.begin(); id!= vecDarts.end(); id++)
	{
		Dart d = *id;

		Dart e = map.phi2(d);

//		if (good(e) && (e.index > d.index))
		if ( (withBoundary || !map.template isBoundaryMarked<2>(e)) && (e.index > d.index))
		{
			*positionF2++ = PFP::toVec3f(fv2[d]);
			*positionF2++ = PFP::toVec3f(fv2[e]);
			this->m_nbRel2++;
		}

		e = map.phi1(d);
		*positionF1++ = PFP::toVec3f(fv1[d]);
		*positionF1++ = PFP::toVec3f(fv11[e]);
	}
	this->m_nbRel1 = GLuint(vecDarts.size());

	this->m_vbo1->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	this->m_vbo2->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
}


template<typename PFP>
void TopoRender::updateDataGMap(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, bool onlyBoundary)
{
	updateDataGMap<PFP>(map,positions,m_ke,m_kf,m_wb,onlyBoundary);
}

template<typename PFP>
void TopoRender::updateDataGMap(typename PFP::MAP& dmap, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& positions, float ke, float kf, bool withBoundary, bool onlyBoundary)
{

	// compilation trick to bypass compilation problems

	struct GPFP:public PFP
	{
		typedef EmbeddedGMap2 MAP;
	};

	typedef typename GPFP::MAP MAP;
	typedef typename GPFP::VEC3 VEC3;
	typedef typename GPFP::REAL REAL;


	MAP& map = reinterpret_cast<MAP&>(dmap);

	std::vector<Dart> vecDarts;
	vecDarts.reserve(map.getNbDarts()); // no problem dart is int: no problem of memory

	std::string name_index(this->m_nameIndex);
	if (onlyBoundary)
		name_index = std::string("dart_boundary_index2");


	DartAttribute<unsigned int, MAP> attIndex = map.template getAttribute<unsigned int, DART, MAP>(name_index);
	if (!attIndex.isValid())
		attIndex  = map.template addAttribute<unsigned int, DART, MAP>(name_index);


	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (withBoundary || !map.template isBoundaryMarked<2>(d))
			vecDarts.push_back(d);
	}
	this->m_nbDarts = uint32(vecDarts.size());

	// debut phi1
	DartAutoAttribute<VEC3, MAP> fv1(map);
	// fin phi1
	DartAutoAttribute<VEC3, MAP> fv11(map);
	// phi2
	DartAutoAttribute<VEC3, MAP> fv2(map);

	this->m_vbo0->bind();
	glBufferData(GL_ARRAY_BUFFER, 4*this->m_nbDarts*sizeof(Geom::Vec3f), 0, GL_STREAM_DRAW);
	GLvoid* PositionDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	Geom::Vec3f* positionDartBuf = reinterpret_cast<Geom::Vec3f*>(PositionDartsBuffer);

	std::vector<VEC3> vecPos;
	vecPos.reserve(16);

	unsigned int indexDC = 0;

	DartMarker<MAP> mf(map);
	for(std::vector<Dart>::iterator id = vecDarts.begin(); id!= vecDarts.end(); id++)
	{
		Dart d = *id;

		if (!mf.isMarked(d))
		{
			vecPos.clear();
			VEC3 center = Algo::Surface::Geometry::faceCentroidELW<GPFP>(map, d, positions);
			
			float k = 1.0f - kf;
			Dart dd = d;
			do
			{
				vecPos.push_back(center*k + positions[dd]*kf);
				dd = map.phi1(dd);
			} while (dd != d);


			if (this->m_normalShift > 0.0f)
			{
				VEC3 normal = Algo::Surface::Geometry::newellNormal<GPFP>(map, d, positions);
				for (typename std::vector<VEC3>::iterator pit = vecPos.begin(); pit != vecPos.end(); ++pit)
				{
					*pit -= normal * this->m_normalShift;
				}
			}
			unsigned int nb = uint32(vecPos.size());
			vecPos.push_back(vecPos.front()); // copy the first for easy computation on next loop

			k = 1.0f - ke;
			for (unsigned int i = 0; i < nb; ++i)
			{
				VEC3 P = vecPos[i]*ke + vecPos[i+1]*k;
				VEC3 Q = vecPos[i+1]*ke + vecPos[i]*k;
				VEC3 PP = REAL(0.52)*P + REAL(0.48)*Q;
				VEC3 QQ = REAL(0.52)*Q + REAL(0.48)*P;

				attIndex[d] = indexDC;
				indexDC+=2;
				*positionDartBuf++ = GPFP::toVec3f(P);
				*positionDartBuf++ = GPFP::toVec3f(PP);
				*positionDartBuf++ = GPFP::toVec3f(Q);
				*positionDartBuf++ = GPFP::toVec3f(QQ);
				VEC3 f = P*0.5f + PP*0.5f;
				fv2[d] = f;
				f = P*0.9f + PP*0.1f;
				fv1[d] = f;

				dd = map.beta0(d);
				f = Q*0.5f + QQ*0.5f;
				fv2[dd] = f;
				f = Q*0.9f + QQ*0.1f;
				fv1[dd] = f;
				attIndex[dd] = indexDC;
				indexDC+=2;

				d = map.phi1(d);
			}
			mf.template markOrbit<FACE>(d);
		}
	}

	this->m_vbo0->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	this->m_vbo1->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*this->m_nbDarts*sizeof(Geom::Vec3f), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer1 = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	this->m_vbo2->bind();
	glBufferData(GL_ARRAY_BUFFER, 2*this->m_nbDarts*sizeof(Geom::Vec3f), 0, GL_STREAM_DRAW);
	GLvoid* PositionBuffer2 = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	Geom::Vec3f* positionF1 = reinterpret_cast<Geom::Vec3f*>(PositionBuffer1);
	Geom::Vec3f* positionF2 = reinterpret_cast<Geom::Vec3f*>(PositionBuffer2);

	this->m_nbRel2 = 0;
	for(std::vector<Dart>::iterator id = vecDarts.begin(); id!= vecDarts.end(); id++)
	{
		Dart d = *id;
		Dart e = map.beta2(d);
//		if (d < e )
		if ( (withBoundary || !map.template isBoundaryMarked<2>(e)) && (d < e ))
		{
			*positionF2++ = GPFP::toVec3f(fv2[d]);
			*positionF2++ = GPFP::toVec3f(fv2[e]);
			this->m_nbRel2++;
		}

		e = map.beta1(d);
		*positionF1++ = GPFP::toVec3f(fv1[d]);
		*positionF1++ = GPFP::toVec3f(fv1[e]);
	}
	this->m_nbRel1 = uint32(vecDarts.size()/2);

	this->m_vbo1->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);

	this->m_vbo2->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
}



template<typename MAP>
Dart TopoRender::picking(MAP& map,int x, int y, bool withBoundary)
{

	Utils::VBO vboCol;
	vboCol.setDataSize(3);

	vboCol.bind();
	glBufferData(GL_ARRAY_BUFFER, 2*this->m_nbDarts*sizeof(Geom::Vec3f), NULL, GL_STREAM_DRAW);
	float* colorBuffer = reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
	unsigned int nb = 0;

	DartAttribute<unsigned int, MAP> attIndex = map.template getAttribute<unsigned int, DART, MAP>(m_nameIndex);
	if (!attIndex.isValid())
	{
		CGoGNerr << "Error attribute_dartIndex does not exist during TopoRender<PFP>::picking" << CGoGNendl;
		return NIL;
	}

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (withBoundary || !map.template isBoundaryMarked<2>(d))
		{

			if (nb < m_nbDarts)
			{
				float r,g,b;
				dartToCol(d, r,g,b);
				float* local = colorBuffer+3*attIndex[d]; // get the right position in VBO
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
				CGoGNerr << "NB = " << nb << "   NBDARTs = "<< m_nbDarts<<CGoGNendl;
				break;
			}
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	Utils::ShaderColorDarts shaderCol;
	shaderCol.setAttributePosition(m_vbo0);
	shaderCol.setAttributeColor(&vboCol);
	shaderCol.updateMatrices(m_shader1);

	// save clear color and set to zero
	float cc[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE,cc);

	GLboolean multi = glIsEnabled(GL_MULTISAMPLE);
	if (multi)
		glDisable(GL_MULTISAMPLE);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderCol.setLineWidth(m_topo_dart_width + 2);
	shaderCol.enableVertexAttribs();
	glDrawArrays(GL_LINES, 0, m_nbDarts*2);
	shaderCol.disableVertexAttribs();

	// read the pixel under the mouse in back buffer
	glReadBuffer(GL_BACK);
	float color[3];
	glReadPixels(x,y,1,1,GL_RGB,GL_FLOAT,color);

	if (multi)
		glEnable(GL_MULTISAMPLE);

	glClearColor(cc[0], cc[1], cc[2], cc[3]);

	Dart d = colToDart(color);
	return d;

}


























//template<typename PFP>
template<typename MAP>
Dart TopoRender::coneSelection(MAP& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float angle)
{
	DartAttribute<unsigned int, MAP> attIndex = map.template getAttribute<unsigned int, DART, MAP>(m_nameIndex);
	if (!attIndex.isValid())
		attIndex  = map.template addAttribute<unsigned int, DART, MAP>(m_nameIndex);

	float AB2 = rayAB*rayAB;
	Dart dFinal;
	double sin2 = sin(M_PI/180.0 * angle);
	sin2 = sin2*sin2;
	double dist2 = std::numeric_limits<double>::max();

	for(Dart d = map.begin(); d!=map.end(); map.next(d))
	{
		// get back position of segment PQ
		const Geom::Vec3f& P = m_bufferDartPosition[attIndex[d]];
		const Geom::Vec3f& Q =m_bufferDartPosition[attIndex[d]+1];
		float ld2 = Geom::squaredDistanceLine2Seg(rayA, rayAB, AB2, P, Q);
		Geom::Vec3f V = (P+Q)/2.0f - rayA;
		double d2 = double(V*V);
		double s2 = double(ld2) / d2;
		if (s2 < sin2)
		{
			if (d2<dist2)
			{
				dist2 = d2;
				dFinal = d;
			}
		}
	}
	return dFinal;
}

//template<typename PFP>
template<typename MAP>
Dart TopoRender::raySelection(MAP& map, const Geom::Vec3f& rayA, const Geom::Vec3f& rayAB, float dmax)
{
	DartAttribute<unsigned int, MAP> attIndex = map.template getAttribute<unsigned int, DART, MAP>(m_nameIndex);
	if (!attIndex.isValid())
		attIndex  = map.template addAttribute<unsigned int, DART, MAP>(m_nameIndex);

	float AB2 = rayAB*rayAB;
	Dart dFinal;
	float dm2 = dmax*dmax;
	double dist2 = std::numeric_limits<double>::max();

	for(Dart d = map.begin(); d!=map.end(); map.next(d))
		if (!map.isBoundaryMarkedCurrent(d))
		{
			// get back position of segment PQ
			const Geom::Vec3f& P = m_bufferDartPosition[attIndex[d]];
			const Geom::Vec3f& Q =m_bufferDartPosition[attIndex[d]+1];
			float ld2 = Geom::squaredDistanceLine2Seg(rayA, rayAB, AB2, P, Q);
			if (ld2<dm2)
			{
				Geom::Vec3f V = (P+Q)/2.0f - rayA;
				double d2 = double(V*V);
				if (d2<dist2)
				{
					dist2 = d2;
					dFinal = d;
				}
			}
		}
	return dFinal;
}

template <typename PFP>
void TopoRender::computeDartMiddlePositions( typename PFP::MAP& map, DartAttribute< typename PFP::VEC3, typename PFP::MAP>& posExpl)
{
	DartAttribute<unsigned int, typename PFP::MAP> attIndex = map.template getAttribute<unsigned int, DART, typename PFP::MAP>(m_nameIndex);
	if (!attIndex.isValid())
		attIndex  = map.template addAttribute<unsigned int, DART, typename PFP::MAP>(m_nameIndex);

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if( attIndex[ d ] < 2 * this->m_nbDarts )
		{
			const Geom::Vec3f& P = m_bufferDartPosition[attIndex[d]];
			const Geom::Vec3f& Q = m_bufferDartPosition[attIndex[d]+1];
			posExpl[d] = (P + Q)*0.5f;
		}
	}
}


} // namespace GL2

} // namespace Algo

} // namespace Render

} // namespace CGoGN
