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
	Dart e = d;
	int nb = 0;
	Dart dNew;

	Dart dRing;
	Dart dRing2;

	//count the valence of the vertex
	do
	{
		nb++;
		e = map.phi1(map.phi2(e));
	} while (e != d);

	if(nb < 3)
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
			if(map.template phi<111>(e) != e)
				map.splitFace(map.phi_1(e), map.phi1(e));

			dRing = map.phi1(e);
			dRing2 = map.phi2(dRing);

			map.unsewFaces(dRing);

			e = dN;
		} while (e != d);

		map.closeHole(dRing);
		map.closeHole(dRing2);
		map.sewVolumes(map.phi2(dRing), map.phi2(dRing2));
	}

	return map.phi2(dRing);
}

//template <typename PFP, typename EMBV, typename EMB>
//void catmullClarkVol(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
//{
//	std::vector<Dart> l_centers;
//	std::vector<Dart> l_vertices;
//
//	DartMarkerNoUnmark mv(map);
//	CellMarkerNoUnmark me(map, EDGE);
//	CellMarker mf(map, FACE);
//
//	AutoAttributeHandler< EMB > attBary(map, VOLUME);
//	CellMarker vol(map, VOLUME);
//
//	//pre-computation : compute the centroid of all volume
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if(selected(d) && !vol.isMarked(d))
//		{
//			vol.mark(d);
//			attBary[d] = Algo::Geometry::volumeCentroidGen<PFP,EMBV,EMB>(map, d, attributs);
//		}
//	}
//
//	// first pass: cut edges
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		//memorize each vertices per volumes
//		if(selected(d) && !mv.isMarked(d))
//		{
//			l_vertices.push_back(d);
////			mv.markOrbitInParent<typename PFP::MAP>(VERTEX,d);
//			mv.markOrbit(PFP::MAP::ORBIT_IN_PARENT(VERTEX),d);
//		}
//
//		//cut edges
//		if (selected(d) && !me.isMarked(d))
//		{
//			Dart f = map.phi1(d);
//			map.cutEdge(d);
//			Dart e = map.phi1(d) ;
//
//			attributs[e] =  attributs[d];
//			attributs[e] += attributs[f];
//			attributs[e] *= 0.5;
//
//			me.mark(d);
//			me.mark(e);
//
//			//mark new vertices
//			mv.markOrbit<VERTEX>(e);
//
//			Dart dd = d;
//			do
//			{
//				mf.mark(dd) ;
//				mf.mark(map.phi2(dd));
//				dd = map.alpha2(dd);
//			} while(dd != d);
//		}
//	}
//
//	// second pass: quandrangule faces
//	std::map<Dart,Dart> toSew;
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		mv.unmark(d);
//
//		if (selected(d) && mf.isMarked(d)) // for each face not subdivided
//		{
//			mf.unmark(d);
//			// compute center skip darts of new vertices non embedded
//			EMB center = AttribOps::zero<EMB,PFP>();
//			unsigned int count = 0 ;
//			Dart it = d;
//
//			do
//			{
//				me.unmark(it);
//				me.unmark(map.phi1(it));
//
//				center += attributs[it];
//				++count ;
//
//				it = map.template phi<11>(it) ;
//			} while(it != d) ;
//
//			center /= double(count);
//
//			Dart cf = quadranguleFace<PFP>(map, d);	// quadrangule the face
//			attributs[cf] = center;					// affect the data to the central vertex
//		}
//	}
//
//	//third pass : create the inner faces
//	for (std::vector<Dart>::iterator it = l_vertices.begin(); it != l_vertices.end(); ++it)
//	{
//		Dart d = *it;
//		//unsew all around the vertex
//		//there are 2 links to unsew for each face around (-> quadrangulation)
//		std::vector<Dart> v;
//		do
//		{
//			v.push_back(map.phi1(map.phi1(d)));
//			v.push_back(map.phi1(d));
//
//			d = map.phi2(map.phi_1(d));
//		}
//		while(d != *it);
//
////		do
////		{
////			Dart dN = map.phi1(map.phi2(d));
////
////	 		Dart dRing = map.phi1(d);
////
////	 		if(map.phi2(dRing)!=dRing)
////	 		{
////	 			toSew.insert(std::pair<Dart,Dart>(dRing,map.phi2(dRing)));
////	 			v.push_back(dRing);
////	 		}
////
////	 		dRing = map.phi1(dRing);
////
////	 		if(map.phi2(dRing)!=dRing)
////	 		{
////	 			toSew.insert(std::pair<Dart,Dart>(dRing,map.phi2(dRing)));
////	 			v.push_back(dRing);
////	 		}
////
////			d = dN;
////		} while (d != *it);
//
////		//close the generated hole and create the central vertex
////		//unsigned int degree = map.closeHole(map.phi1(d));
//
//		//TODO : pb de face en trop avec splitVolume
//		//map.splitVolume(v);
//
//		//
//
////		Dart e = v.front();
////		for(std::vector<Dart>::iterator it = v.begin() ; it != v.end() ; ++it)
////			if(map.Map2::isBoundaryEdge(*it))
////				map.unsewFaces(*it);
//
////		Dart dd = map.phi1(map.phi2(map.phi1(d)));
////		map.splitFace(map.phi_1(dd),map.phi1(dd));
////		Dart dS = map.phi1(dd);
////		map.cutEdge(dS);
//
////		attributs[map.phi1(dS)] = attBary[d];
//
//
//		//TODO : test with vertices with degree higher than 3
////		for(unsigned int i=0; i < (degree/2)-2; ++i)
////		{
////			map.splitFace(map.phi2(dS),map.template phi<111>(map.phi2(dS)));
////			dS = map.template phi<111>(map.phi2(dS));
////		}
//	}
//
////	map.deleteVolume(map.phi3(map.phi2(map.phi1(l_vertices.front()))));
//
//	map.check();
//
//	//sew all faces leading to the central vertex
//	for (std::map<Dart,Dart>::iterator it = toSew.begin(); it != toSew.end(); ++it)
//	{
//
////		Dart f1 = map.phi2((*it).first);
////		Dart f2 = map.phi2((*it).second);
////		if(map.isBoundaryFace(f1) && map.isBoundaryFace(f2))
////		{
////			map.sewVolumes(f1, f2);
////		}
//
//		//Dart dT = map.phi2(it->first);
////		if(dT==map.phi3(dT))
////		{
////			map.sewVolumes(dT,map.phi2(it->second));
////		}
//	}
//}


template <typename PFP, typename EMBV, typename EMB>
void catmullClarkVol(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	//std::vector<Dart> l_centers;
	std::vector<Dart> l_vertices;

	//pre-computation : compute the centroid of all volume
	VolumeAutoAttribute<EMB> attBary(map);
	Algo::Geometry::computeCentroidVolumes<PFP>(map, const_cast<const EMBV&>(attributs), attBary, selected);

	//subdivision
	//1. cut edges
	DartMarkerNoUnmark mv(map);
	TraversorE<typename PFP::MAP> travE(map);
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
		//memorize each vertices per volumes
		if(selected(d) && !mv.isMarked(d))
		{
			l_vertices.push_back(d);
			mv.markOrbit<PFP::MAP::VERTEX_OF_PARENT>(d);
		}

		Dart f = map.phi1(d);
		map.cutEdge(d) ;
		Dart e = map.phi1(d) ;

		attributs[e] =  attributs[d];
		attributs[e] += attributs[f];
		attributs[e] *= 0.5;

		travE.skip(d) ;
		travE.skip(e) ;
	}

	//2. split faces - quadrangule faces
	TraversorF<typename PFP::MAP> travF(map) ;
	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
	{
		EMB center = Algo::Geometry::faceCentroidGen<PFP,EMBV,EMB>(map,d,attributs);

		Dart dd = map.phi1(d) ;
		Dart next = map.phi1(map.phi1(dd)) ;
		map.splitFace(dd, next) ;

		Dart ne = map.phi2(map.phi_1(dd)) ;
		map.cutEdge(ne) ;
		travF.skip(dd) ;

		attributs[map.phi1(ne)] = center;

		dd = map.phi1(map.phi1(next)) ;
		while(dd != ne)
		{
			Dart tmp = map.phi1(ne) ;
			map.splitFace(tmp, dd) ;
			travF.skip(tmp) ;
			dd = map.phi1(map.phi1(dd)) ;
		}

		travF.skip(ne) ;
	}

	//3. create inside volumes

	std::vector<std::pair<Dart, Dart> > subdividedFaces;
	subdividedFaces.reserve(2048);
	for (std::vector<Dart>::iterator it = l_vertices.begin(); it != l_vertices.end(); ++it)
	{
		Dart e = *it;
		std::vector<Dart> v ;

		do
		{
			v.push_back(map.phi1(e));
			v.push_back(map.phi1(map.phi1(e)));

			if(!map.PFP::MAP::ParentMap::isBoundaryEdge(map.phi1(e)))
				subdividedFaces.push_back(std::pair<Dart,Dart>(map.phi1(e),map.phi2(map.phi1(e))));

			if(!map.PFP::MAP::ParentMap::isBoundaryEdge(map.phi1(map.phi1(e))))
				subdividedFaces.push_back(std::pair<Dart,Dart>(map.phi1(map.phi1(e)),map.phi2(map.phi1(map.phi1(e)))));

			e = map.phi2(map.phi_1(e));
		}
		while(e != *it);

		//
		// SplitSurfaceInVolume
		//

		std::vector<Dart> vd2 ;
		vd2.reserve(v.size());

		// save the edge neighbors darts
		for(std::vector<Dart>::iterator it2 = v.begin() ; it2 != v.end() ; ++it2)
		{
			vd2.push_back(map.phi2(*it2));
		}

		assert(vd2.size() == v.size());

		map.PFP::MAP::ParentMap::splitSurface(v, true, false);

		// follow the edge path a second time to embed the vertex, edge and volume orbits
		for(unsigned int i = 0; i < v.size(); ++i)
		{
			Dart dit = v[i];
			Dart dit2 = vd2[i];

			// embed the vertex embedded from the origin volume to the new darts
			if(map.template isOrbitEmbedded<VERTEX>())
			{
				map.template copyDartEmbedding<VERTEX>(map.phi2(dit), map.phi1(dit));
				map.template copyDartEmbedding<VERTEX>(map.phi2(dit2), map.phi1(dit2));
			}

			// embed the edge embedded from the origin volume to the new darts
			if(map.template isOrbitEmbedded<EDGE>())
			{
				unsigned int eEmb = map.template getEmbedding<EDGE>(dit) ;
				map.template setDartEmbedding<EDGE>(map.phi2(dit), eEmb);
				map.template setDartEmbedding<EDGE>(map.phi2(dit2), eEmb);
			}

			// embed the volume embedded from the origin volume to the new darts
			if(map.template isOrbitEmbedded<VOLUME>())
			{
				map.template copyDartEmbedding<VOLUME>(map.phi2(dit), dit);
				map.template copyDartEmbedding<VOLUME>(map.phi2(dit2), dit2);
			}
		}

		//
		//
		//

		Dart dd = map.phi2(map.phi1(*it));
		Dart next = map.phi1(map.phi1(dd)) ;
		map.PFP::MAP::ParentMap::splitFace(dd, next);

		if (map.template isOrbitEmbedded<VERTEX>())
		{
			map.template copyDartEmbedding<VERTEX>(map.phi_1(next), dd) ;
			map.template copyDartEmbedding<VERTEX>(map.phi_1(dd), next) ;
		}

		Dart ne = map.phi2(map.phi_1(dd));
		map.PFP::MAP::ParentMap::cutEdge(ne);

//		dd = map.phi1(map.phi1(next)) ;
//		while(dd != ne)
//		{
//			Dart tmp = map.phi1(ne) ;
//			map.PFP::MAP::ParentMap::splitFace(tmp, dd);
//
//			if (map.isOrbitEmbedded<VERTEX>())
//			{
//				map.copyDartEmbedding<VERTEX>(map.phi_1(dd), tmp) ;
//				map.copyDartEmbedding<VERTEX>(map.phi_1(tmp), dd) ;
//			}
//
//			dd = map.phi1(map.phi1(dd)) ;
//		}
//
	}

//		setCurrentLevel(getMaxLevel()) ;
//		//4 couture des relations precedemment sauvegarde
//		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
//		{
//			Dart f1 = phi2((*it).first);
//			Dart f2 = phi2((*it).second);
//
//			//if(isBoundaryFace(f1) && isBoundaryFace(f2))
//			if(phi3(f1) == f1 && phi3(f2) == f2)
//				sewVolumes(f1, f2, false);
//		}
//		embedOrbit<VERTEX>(centralDart, getEmbedding<VERTEX>(centralDart));
		//attributs[map.phi1(ne)] = attBary[*it];
//
//		setCurrentLevel(getMaxLevel() - 1) ;
//	}
//
//	//A optimiser
//
//	TraversorE<typename PFP::MAP> travE2(map);
//	for (Dart d = travE2.begin(); d != travE2.end(); d = travE2.next())
//	{
//		map.embedOrbit<VERTEX>(map.phi1(d), map.getEmbedding<VERTEX>(map.phi1(d)));
//	}
//
//	TraversorF<typename PFP::MAP> travF2(map) ;
//	for (Dart d = travF2.begin(); d != travF2.end(); d = travF2.next())
//	{
//		map.embedOrbit<VERTEX>(map.phi2(map.phi1(d)), map.getEmbedding<VERTEX>(map.phi2(map.phi1(d))));
//	}


}

} //namespace Modelisation

} //namespace Algo

} //namespace CGoGN

