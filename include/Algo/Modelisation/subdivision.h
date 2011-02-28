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

#ifndef __SUBDIVISION_H__
#define __SUBDIVISION_H__

#include <math.h>
#include <vector>

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

/**
* Triangule a face with central vertex
* @param d dart of face
* @return a dart of the central vertex
*/
template <typename PFP>
Dart trianguleFace(typename PFP::MAP& map, Dart d);

/**
 * Triangule all the faces of the mesh
 */
template <typename PFP, typename EMBV, typename EMB>
void trianguleFaces(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = SelectorTrue()) ;

template <typename PFP>
void trianguleFaces(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected = SelectorTrue()) ;

/**
* Quadrangule a face with central vertex
* warning: edges are considered here as already cut !!
* A face with 2n edges give n quads
* @param d dart of face with belong to a vertex introduce when cutting the edge
* @return a dart of central point
*/
template <typename PFP>
Dart quadranguleFace(typename PFP::MAP& map, Dart d);

/**
 * Quadrangule all the faces of the mesh
 */
template <typename PFP, typename EMBV, typename EMB>
void quadranguleFaces(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = SelectorTrue()) ;

template <typename PFP>
void quadranguleFaces(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected = SelectorTrue()) ;

/**
 * Catmull-Clark subdivision scheme
 */
template <typename PFP, typename EMBV, typename EMB>
void CatmullClarkSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = SelectorTrue()) ;

template <typename PFP>
void CatmullClarkSubdivision(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected = SelectorTrue()) ;

/**
 * Loop subdivision scheme
 */
template <typename PFP, typename EMBV, typename EMB>
void LoopSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = SelectorTrue()) ;

template <typename PFP>
void LoopSubdivision(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected = SelectorTrue()) ;

/**
 * Reverse the orientation of the map
 */
template <typename PFP>
void reverseOrientation(typename PFP::MAP& map) ;

/**
 * Dual mesh computation
 */
template <typename PFP>
void computeDual(typename PFP::MAP& map, const FunctorSelect& selected = SelectorTrue()) ;

/**
 * Sqrt(3) subdivision scheme
 */
template <typename PFP>
void Sqrt3Subdivision(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected = SelectorTrue()) ;



/*
 * TODO quadrangule volume
 */

/**
 * Quadrangule all the faces of the mesh
 */
template <typename PFP, typename EMBV, typename EMB>
void quadranguleFacesVolume(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = SelectorTrue()) ;

template <typename PFP, typename EMBV, typename EMB>
void hexaCutVolume(typename PFP::MAP& map, Dart d, EMBV& attributs);

template <typename PFP, typename EMBV, typename EMB>
void dooSabinVolume(typename PFP::MAP& map, Dart d, EMBV& attributs);

/*
 * Quadrangule a Volume
 */
template <typename PFP, typename EMBV, typename EMB>
void quadranguleVolumes(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = SelectorTrue());


// TODO a mettre ailleurs ?
//
///**
// * create a tetra based on the two triangles that have a common dart and phi2(dart)
// * return a new dart inside the tetra
// */
//template<typename PFP>
//Dart extractTetra(typename PFP::MAP& the_map, Dart d)
//{
//	
//
//	Dart e = the_map.phi2(d);
//
//	//create the new faces
//	Dart dd = the_map.newFace(3);
//	Dart ee = the_map.newFace(3);
//
//	//update their sew
//	the_map.sewFaces(dd,ee);
//	the_map.sewFaces(the_map.phi3(dd),the_map.phi3(ee));
//
//	//add the two new faces in the mesh to obtain a tetra
//	Dart s2d = the_map.phi2(the_map.phi_1(d));
//	the_map.unsewFaces(the_map.phi_1(d));
//	the_map.sewFaces(the_map.phi_1(d),the_map.phi_1(dd));
//	the_map.sewFaces(s2d,the_map.phi3(the_map.phi_1(dd)));
//
//	Dart s2e = the_map.phi2(the_map.phi_1(e));
//	the_map.unsewFaces(the_map.phi_1(e));
//	the_map.sewFaces(the_map.phi_1(e),the_map.phi_1(ee));
//	the_map.sewFaces(s2e,the_map.phi3(the_map.phi_1(ee)));
//
//	Dart ss2d = the_map.phi2(the_map.phi1(d));
//	the_map.unsewFaces(the_map.phi1(d));
//	the_map.sewFaces(the_map.phi1(d),the_map.phi1(ee));
//	the_map.sewFaces(ss2d,the_map.phi3(the_map.phi1(ee)));
//
//	Dart ss2e = the_map.phi2(the_map.phi1(e));
//	the_map.unsewFaces(the_map.phi1(e));
//	the_map.sewFaces(the_map.phi1(e),the_map.phi1(dd));
//	the_map.sewFaces(ss2e,the_map.phi3(the_map.phi1(dd)));
//
//
//
//	//embed the coords
//	the_map.setVertexEmb(d,the_map.getVertexEmb(d));
//	the_map.setVertexEmb(e,the_map.getVertexEmb(e));
//	the_map.setVertexEmb(the_map.phi_1(d),the_map.getVertexEmb(the_map.phi_1(d)));
//	the_map.setVertexEmb(the_map.phi_1(e),the_map.getVertexEmb(the_map.phi_1(e)));
//
//	return dd;
//}
//
///**
// * tetrahedrization of the volume
// * @param the map
// * @param a dart of the volume
// * @param true if the faces are in CCW order
// * @return success of the tetrahedrization
// */
//template<typename PFP>
//bool smartVolumeTetrahedrization(typename PFP::MAP& the_map, Dart d, bool CCW=true)
//{
//	
//	typedef typename PFP::EMB EMB;
//
//	bool ret=true;
//
//	if (!the_map.isTetrahedron(d))
//	{
//		//only works on a 3-map
//		assert(Dart::nbInvolutions()>=2 || "cannot be applied on this map, nbInvolutions must be at least 2");
//
//		if (Geometry::isConvex<PFP>(the_map,d,CCW))
//		{
//			the_map.tetrahedrizeVolume(d);
//		}
//		else
//		{
//
//			//get all the dart of the volume
//			std::vector<Dart> vStore;
//			FunctorStore fs(vStore);
//			the_map.foreach_dart_of_volume(d,fs);
//
//			if (vStore.size()==0)
//			{
//				if (the_map.phi1(d)==d)
//					std::cout << "plop" << std::endl;
//				if (the_map.phi2(d)==d)
//					std::cout << "plip" << std::endl;
//
//				std::cout << the_map.getVertexEmb(d)->getPosition() << std::endl;
//				std::cout << "tiens tiens, c'est etrange" << std::endl;
//			}
//			//prepare the list of embeddings of the current volume
//			std::vector<EMB *> lstEmb;
//
//			//get a marker
//			DartMarker m(the_map);
//
//			//all the darts from a vertex that can generate a tetra (3 adjacent faces)
//			std::vector<Dart> allowTetra;
//
//			//all the darts that are not in otherTetra
//			std::vector<Dart> otherTetra;
//
//			//for each dart of the volume
//			for (typename std::vector<Dart>::iterator it = vStore.begin() ; it != vStore.end() ; ++it )
//			{
//				Dart e = *it;
//				//if the vertex is not treated
//				if (!m.isMarked(e))
//				{
//					//store the embedding
//					lstEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(e)));
//					Dart ee=e;
//
//					//count the number of adjacent faces and mark the darts
//					int nbe=0;
//					do
//					{
//						nbe++;
//						m.markOrbit(DART_ORBIT,e);
//						ee=the_map.phi1(the_map.phi2(ee));
//					}
//					while (ee!=e);
//
//					//if 3 adjacents faces, we can create a tetra on this vertex
//					if (nbe==3)
//						allowTetra.push_back(e);
//					else
//						otherTetra.push_back(e);
//				}
//			}
//
//			//we haven't created a tetra yet
//			bool decoupe=false;
//
//			//if we have vertex that can be base
//			if (allowTetra.size()!=0)
//			{
//				//foreach possible vertex while we haven't done any cut
//				for (typename std::vector<Dart>::iterator it=allowTetra.begin();it!=allowTetra.end() && !decoupe ;++it)
//				{
//					//get the dart
//					Dart s=*it;
//					//store the emb
//					std::vector<EMB*> lstCurEmb;
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(s)));
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(the_map.phi1(s))));
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(the_map.phi_1(s))));
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(the_map.phi_1(the_map.phi2(s)))));
//
//					//store the coords of the point
//					gmtl::Vec3f points[4];
//					for (int i=0;i<4;++i)
//					{
//						points[i] = lstCurEmb[i]->getPosition();
//					}
//
//					//test if the future tetra is well oriented (concave case)
//					if (Geometry::isTetrahedronWellOriented(points,CCW))
//					{
//						//test if we haven't any point inside the future tetra
//						bool isEmpty=true;
//						for (typename std::vector<EMB *>::iterator iter = lstEmb.begin() ; iter != lstEmb.end() && isEmpty ; ++iter)
//						{
//							//we don't test the vertex that composes the new tetra
//							if (std::find(lstCurEmb.begin(),lstCurEmb.end(),*iter)==lstCurEmb.end())
//							{
//								isEmpty = !Geometry::isPointInTetrahedron(points, (*iter)->getPosition(), CCW);
//							}
//						}
//
//						//if no point inside the new tetra
//						if (isEmpty)
//						{
//							//cut the spike to make a tet
//							Dart dRes = the_map.cutSpike(*it);
//							decoupe=true;
//							//and continue with the rest of the volume
//							ret = ret && smartVolumeTetrahedrization<PFP>(the_map,the_map.phi3(dRes),CCW);
//						}
//					}
//				}
//			}
//
//			if (!decoupe)
//			{
//				//foreach other vertex while we haven't done any cut
//				for (typename std::vector<Dart>::iterator it=otherTetra.begin();it!=otherTetra.end() && !decoupe ;++it)
//				{
//					//get the dart
//					Dart s=*it;
//					//store the emb
//					std::vector<EMB*> lstCurEmb;
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(s)));
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(the_map.phi1(s))));
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(the_map.phi_1(s))));
//					lstCurEmb.push_back(reinterpret_cast<EMB*>(the_map.getVertexEmb(the_map.phi_1(the_map.phi2(s)))));
//
//					//store the coords of the point
//					gmtl::Vec3f points[4];
//					for (int i=0;i<4;++i)
//					{
//						points[i] = lstCurEmb[i]->getPosition();
//					}
//
//					//test if the future tetra is well oriented (concave case)
//					if (Geometry::isTetrahedronWellOriented(points,CCW))
//					{
//						//test if we haven't any point inside the future tetra
//						bool isEmpty=true;
//						for (typename std::vector<EMB *>::iterator iter = lstEmb.begin() ; iter != lstEmb.end() && isEmpty ; ++iter)
//						{
//							//we don't test the vertex that composes the new tetra
//							if (std::find(lstCurEmb.begin(),lstCurEmb.end(),*iter)==lstCurEmb.end())
//							{
//								isEmpty = !Geometry::isPointInTetrahedron(points, (*iter)->getPosition(), CCW);
//							}
//						}
//
//						//if no point inside the new tetra
//						if (isEmpty)
//						{
//							//cut the spike to make a tet
//							Dart dRes = extractTetra<PFP>(the_map,*it);
//							decoupe=true;
//							//and continue with the rest of the volume
//							smartVolumeTetrahedrization<PFP>(the_map,the_map.phi3(dRes),CCW);
//						}
//					}
//				}
//			}
//
//			if (!decoupe)
//				ret=false;
//		}
//	}
//	return ret;
//}

} // namespace Modelisation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Modelisation/subdivision.hpp"

#endif
