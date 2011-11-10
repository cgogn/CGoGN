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

#include "Algo/Geometry/centroid.h"
#include "Algo/Modelisation/subdivision.h"
#include "Algo/Modelisation/extrusion.h"

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

template <typename PFP>
Dart cut3Ear(typename PFP::MAP& map, Dart d)
{
  Dart e=d;
  int nb=0;
  Dart dNew;

  Dart dRing;
  Dart dRing2;

  //count the valence of the vertex
  do
  {
    nb++;
    e=map.phi1(map.phi2(e));
  } while (e!=d);

  if(nb<3)
  {
	CGoGNout << "Warning : cannot cut 2 volumes without creating a degenerated face " << CGoGNendl;
	return d;
  }
  else
  {
	//triangulate around the vertex
	do
	{
		Dart dN = map.phi1(map.phi2(e));
		if(map.template phi<111>(e)!=e)
 			map.splitFace(map.phi_1(e), map.phi1(e));

 		dRing = map.phi1(e);
		dRing2 = map.phi2(dRing);

		map.unsewFaces(dRing);

		e= dN;
	} while (e!=d);

	map.closeHole(dRing);
	map.closeHole(dRing2);
	map.sewVolumes(map.phi2(dRing),map.phi2(dRing2));
  }

  return map.phi2(dRing);
}



template <typename PFP, typename EMBV, typename EMB>
void catmullClarkVol(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	std::vector<Dart> l_centers;
	std::vector<Dart> l_vertices;

	DartMarkerNoUnmark mv(map);
	CellMarkerNoUnmark me(map, EDGE);
	CellMarker mf(map, FACE);

	AutoAttributeHandler< EMB > attBary(map,VOLUME);
	CellMarker vol(map,VOLUME);

	//pre-computation : compute the centroid of all volume
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(selected(d) && !vol.isMarked(d))
		{
			vol.mark(d);
			attBary[d] = Algo::Geometry::volumeCentroidGen<PFP,EMBV,EMB>(map,d,attributs);
		}
	}

	// first pass: cut edges
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		//memorize each vertices per volumes
		if(selected(d) && !mv.isMarked(d))
		{
			std::cout << " d " << d << std::endl;
			l_vertices.push_back(d);
			mv.markOrbitInParent<typename PFP::MAP>(VERTEX,d);
		}

		//cut edges
		if (selected(d) && !me.isMarked(d))
		{
			Dart f = map.phi1(d);
			map.cutEdge(d);
			Dart e = map.phi1(d) ;

			attributs[e] =  attributs[d];
			attributs[e] += attributs[f];
			attributs[e] *= 0.5;

			me.mark(d);
			me.mark(e);

			//mark new vertices
			mv.markOrbit(VERTEX, e);

			Dart dd = d;
			do
			{
				mf.mark(dd) ;
				mf.mark(map.phi2(dd));
				dd = map.alpha2(dd);
			} while(dd != d);
		}
	}

	unsigned int nb_=0;
	for(unsigned int nb= attributs.begin() ; nb != attributs.end() ; attributs.next(nb))
		nb_++;

	std::cout << "first " << nb_ << std::endl;

	// second pass: quandrangule faces
	std::map<Dart,Dart> toSew;
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		mv.unmark(d);

		if (selected(d) && mf.isMarked(d)) // for each face not subdivided
		{
			mf.unmark(d);
			// compute center skip darts of new vertices non embedded
			EMB center = AttribOps::zero<EMB,PFP>();
			unsigned int count = 0 ;
			Dart it = d;

			do
			{
				me.unmark(it);
				me.unmark(map.phi1(it));

				center += attributs[it];
				++count ;

				it = map.template phi<11>(it) ;
			} while(it != d) ;

			center /= double(count);

			Dart cf = quadranguleFace<PFP>(map, d);	// quadrangule the face
			attributs[cf] = center;					// affect the data to the central vertex
		}
	}

	//third pass : create the inner faces
	for (std::vector<Dart>::iterator it = l_vertices.begin(); it != l_vertices.end(); ++it)
	{
		Dart d = *it;
		//unsew all around the vertex
		//there are 2 links to unsew for each face around (-> quadrangulation)
		do
		{
			Dart dN = map.phi1(map.phi2(d));

	 		Dart dRing = map.phi1(d);

	 		if(map.phi2(dRing)!=dRing)
	 		{
	 			toSew.insert(std::pair<Dart,Dart>(dRing,map.phi2(dRing)));
	 			map.unsewFaces(dRing);
	 		}

	 		dRing = map.phi1(dRing);

	 		if(map.phi2(dRing)!=dRing)
	 		{
	 			toSew.insert(std::pair<Dart,Dart>(dRing,map.phi2(dRing)));
	 			map.unsewFaces(dRing);
	 		}

			d= dN;
		} while (*it!=d);

		//close the generated hole and create the central vertex
		unsigned int degree = map.closeHole(map.phi1(d));

		Dart dd = map.phi1(map.phi2(map.phi1(d)));
		map.splitFace(map.phi_1(dd),map.phi1(dd));
		Dart dS = map.phi1(dd);
		map.cutEdge(dS);

		attributs[map.phi1(dS)] = attBary[d];

		//TODO : test with vertices with degree higher than 3
		for(unsigned int i=0; i < (degree/2)-2; ++i)
		{
			map.splitFace(map.phi2(dS),map.template phi<111>(map.phi2(dS)));
			dS = map.template phi<111>(map.phi2(dS));
		}
	}

	nb_=0;
	for(unsigned int nb= attributs.begin() ; nb != attributs.end() ; attributs.next(nb))
		nb_++;

	std::cout << "then " << nb_ << std::endl;

	map.check();

	//sew all faces leading to the central vertex
	for (std::map<Dart,Dart>::iterator it = toSew.begin(); it != toSew.end(); ++it)
	{
		Dart dT = map.phi2(it->first);
		if(dT==map.phi3(dT))
		{
			map.sewVolumes(dT,map.phi2(it->second));
		}
	}
}

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
//					CGoGNout << "plop" << CGoGNendl;
//				if (the_map.phi2(d)==d)
//					CGoGNout << "plip" << CGoGNendl;
//
//				CGoGNout << the_map.getVertexEmb(d)->getPosition() << CGoGNendl;
//				CGoGNout << "tiens tiens, c'est etrange" << CGoGNendl;
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
//						m.markOrbit(DART,e);
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

} //namespace Modelisation

} //namespace Algo

} //namespace CGoGN



