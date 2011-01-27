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

//#include "Topology/map/emap3.h"

namespace CGoGN
{

//Dart EMap3::edgeCollapse(Dart d,bool delDegenerateFaces,bool delDegenerateVolumes)
//{
//	Dart dd = phi2(phi_1(d));
//	Map3::edgeCollapse(d,delDegenerateFaces,delDegenerateVolumes);
//
//	if (this->isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		embedNewCell(VERTEX_ORBIT,dd);
//	}
//
//	return dd;
//}
//
//Dart EMap3::cutFace(Dart d, Dart e)
//{
//	Dart nd = Map3::cutFace(d,e);
//	Dart ne = phi2(nd);
//
//	if (this->isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		// nd & phi3(ne) are in vertex of e
//		unsigned int em =  getEmbedding(e,VERTEX_ORBIT);
//		setDartEmbedding(nd, VERTEX_ORBIT, em);
//		setDartEmbedding(phi3(ne), VERTEX_ORBIT, em);
//
//		// ne & phi3(nd) are in vertex of d
//
//		copyEmbedding(ne, d, VERTEX_ORBIT);
//		copyEmbedding(phi3(nd), d, VERTEX_ORBIT);
//	}
//
//	if (this->isOrbitEmbedded(EDGE_ORBIT))
//	{
//		embedNewCell(EDGE_ORBIT,nd);
//	}
//
//	if (this->isOrbitEmbedded(FACE_ORBIT))
//	{
//		embedNewCell(FACE_ORBIT,nd);
//		embedNewCell(FACE_ORBIT,ne);
//	}
//
//	if (this->isOrbitEmbedded(VOLUME_ORBIT))
//	{
//		// nd share volume embedding with d
//		setDartEmbedding(nd, VOLUME_ORBIT, getEmbedding(d,VOLUME_ORBIT));
//		// phi3(nd) share volume embedding with phi3(d)
//		setDartEmbedding(phi3(nd), VOLUME_ORBIT, getEmbedding(phi3(d),VOLUME_ORBIT));
//		// ne share volume embedding with e
//		setDartEmbedding(ne, VOLUME_ORBIT, getEmbedding(e,VOLUME_ORBIT));
//		// phi3(ne) share volume embedding with phi3(e)
//		setDartEmbedding(phi3(ne), VOLUME_ORBIT, getEmbedding(phi3(e),VOLUME_ORBIT));
//	}
//
//	return nd;
//}
//
//void EMap3::embeddxxguledFace(Dart d, Dart n)
//{
//	// create embedding for the center (if necessary)
//	if (this->isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		embedNewCell(VERTEX_ORBIT,n);
//	}
//
//	// create embedding for new edges
//	if (this->isOrbitEmbedded(EDGE_ORBIT))
//	{
//		Dart nn = n;
//		do
//		{
//			embedNewCell(EDGE_ORBIT,nn);
//			nn = alpha1(nn);
//		} while (nn!=n);
//	}
//
//	// create embedding for new faces
//	if (this->isOrbitEmbedded(FACE_ORBIT))
//	{
//		Dart nn = n;
//		do
//		{
//			embedNewCell(FACE_ORBIT,nn);
//			nn = alpha1(nn);
//		} while (nn!=n);
//	}
//
//	// embed new darts with volume embedding if necessary
//	if (this->isOrbitEmbedded(VOLUME_ORBIT))
//	{
//		unsigned int v1 = getEmbedding(d,VOLUME_ORBIT);
//		unsigned int v2 = getEmbedding(phi3(d),VOLUME_ORBIT);
//		Dart nn = n;
//		do
//		{
//			Dart xx = phi2(nn);
//			setDartEmbedding(nn, VOLUME_ORBIT, v1);
//			setDartEmbedding(xx, VOLUME_ORBIT, v1);
//			setDartEmbedding(phi3(nn), VOLUME_ORBIT, v2);
//			setDartEmbedding(phi3(xx), VOLUME_ORBIT, v2);
//			nn = alpha1(nn);
//		} while (nn!=n);
//	}
//}
//
//Dart EMap3::trianguleFace(Dart d)
//{
//	Dart n = trianguleFace(d);
//	EMap3::embeddxxguledFace(d,n);
//	return n;
//}
//
//Dart EMap3::quadranguleFace(Dart d)
//{
//	Dart n = quadranguleFace(d);
//	EMap3::embeddxxguledFace(d,n);
//	return n;
//}
//
//Dart EMap3::cutEdge(Dart d)
//{
//	Dart dd=Map3::cutEdge(d);
//
//	// create embedding for new vertex
//	if (this->isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		embedNewCell(VERTEX_ORBIT,dd);
//	}
//
//	// create embedding for new edges
//	if (this->isOrbitEmbedded(EDGE_ORBIT))
//	{
//		embedNewCell(EDGE_ORBIT,d);
//		embedNewCell(EDGE_ORBIT,dd);
//	}
//
//	// embed new darts with face & volume embeddings if necessary
//	Dart dNext = d;
//	do
//	{
//		if (this->isOrbitEmbedded(FACE_ORBIT))
//			copyEmbedding(phi1(dNext), dNext, FACE_ORBIT);
//		if (this->isOrbitEmbedded(VOLUME_ORBIT))
//			copyEmbedding(phi1(dNext), dNext, VOLUME_ORBIT);
//		dNext = alpha2(dNext);
//	} while (dNext != d);
//
//	return dd;
//
//}
//
//Dart EMap3::tetrahedrizeVolume(Dart d)
//{
//	//tetrahedrize the volume
//	Dart nd = Map3::tetrahedrizeVolume(d);
//
//	// create embedding for new vertex
//	if (this->isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		embedNewCell(VERTEX_ORBIT,nd);
//	}
//
//	// create embedding for new edges
//	if (this->isOrbitEmbedded(EDGE_ORBIT))
//	{
//
//	}
//
//	if (this->isOrbitEmbedded(FACE_ORBIT))
//	{
//
//	}
//
//	if (this->isOrbitEmbedded(VOLUME_ORBIT))
//	{
//
//	}
//
//	return nd;
//}
//
//bool EMap3::check()
//{
//	bool topo = Map3::check();
//	if (!topo) return false;
//	std::cout << "Check: embedding begin" << std::endl;
//	for(Dart d = this->begin(); d != this->end(); ++d)
//	{
//		if (isOrbitEmbedded(VERTEX_ORBIT))
//			if (getEmbedding(d,VERTEX_ORBIT) != getEmbedding(alpha1(d),VERTEX_ORBIT)) {
//				std::cout << "Check: different embeddings on vertex" << std::endl;
//				return false;
//			}
//	}
//	std::cout << "Check: embedding ok" << std::endl;
//	return true;
//}
//
//void EMap3::flipEdge(Dart d)
//{
//	Map3::flipEdge(d);
//	Dart e= phi2(d);
//
//	if (this->isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		copyEmbedding(d,  phi1(phi2(d)), VERTEX_ORBIT);
//		copyEmbedding(e,  phi1(d), VERTEX_ORBIT);
//	}
//
//	if (this->isOrbitEmbedded(FACE_ORBIT))
//	{
//		embedNewCell(FACE_ORBIT,d);
//		embedNewCell(FACE_ORBIT,e);
//	}
//
//	if (this->isOrbitEmbedded(VOLUME_ORBIT))
//	{
//
//	}
//}

} // namespace CGoGN
