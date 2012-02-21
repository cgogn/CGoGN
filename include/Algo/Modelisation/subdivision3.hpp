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
#include "Geometry/intersection.h"

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

template <typename PFP>
Dart sliceConvexVolume(typename PFP::MAP& map, typename PFP::TVEC3& position, Dart d, Geom::Plane3D<typename PFP::REAL > pl)
{
	Dart dRes=NIL;
	unsigned int nbInter = 0;
	unsigned int nbVertices = 0;
	CellMarkerStore vs(map, VERTEX);			//marker for new vertices from edge cut
	CellMarkerStore cf(map, FACE);
	Dart dPath;

	MarkerForTraversor<typename PFP::MAP::ParentMap > mte(map,EDGE);
	MarkerForTraversor<typename PFP::MAP::ParentMap > mtf(map,FACE);

	//search edges and vertices crossing the plane
	Traversor3WE<typename PFP::MAP::ParentMap > te(map,d);
	for(Dart dd = te.begin() ;dd != te.end() ; dd = te.next())
	{
		if(!mte.isMarked(dd))
		{
			if(fabs(pl.distance(position[dd]))<0.000001f)
			{
				nbVertices++;
				vs.mark(dd); //mark vertex on slicing path
				mte.mark(dd);
			}
			else
			{
				typename PFP::VEC3 interP;
				typename PFP::VEC3 vec(Algo::Geometry::vectorOutOfDart<PFP>(map,dd,position));
				Geom::Intersection inter = Geom::intersectionLinePlane<typename PFP::VEC3, typename Geom::Plane3D<typename PFP::REAL > >(position[dd],vec,pl,interP);

				if(inter==Geom::FACE_INTERSECTION)
				{
					Dart dOp = map.phi1(dd);
					typename PFP::VEC3 v2(interP-position[dd]);
					typename PFP::VEC3 v3(interP-position[dOp]);
					if(vec.norm2()>v2.norm2() && vec.norm2()>v3.norm2())
					{
						nbInter++;

						cf.mark(dd);			//mark face and opposite face to split
						cf.mark(map.phi2(dd));

						map.cutEdge(dd);
						Dart dN = map.phi1(dd);

						mte.mark(dN);

						vs.mark(dN);			//mark vertex for split
						position[dN] = interP; 	//place
					}
				}
			}
		}
	}

//	std::cout << "edges cut: " << nbInter << std::endl;
	unsigned int nbSplit=0;

	//slice when at least two edges are concerned
	if(nbInter>1)
	{
		Traversor3WF<typename PFP::MAP::ParentMap > tf(map,d);
		for(Dart dd = tf.begin() ; dd != tf.end() ; dd = tf.next())
		{
			//for faces with a new vertex
			if(cf.isMarked(dd))
			{
				cf.unmark(dd);

				Dart dS = dd;
				bool split=false;

				do {
					//find the new vertex
					if(vs.isMarked(dS))
					{
						Dart dSS = map.phi1(dS);
						//search an other new vertex (or an existing vertex intersected with the plane) in order to split the face
						do {
							if(vs.isMarked(dSS))
							{
								nbSplit++;
								map.splitFace(dS,dSS);
								dPath=map.phi_1(dS);
								split=true;
							}
							dSS = map.phi1(dSS);
						} while(!split && dSS!=dS);
					}
					dS = map.phi1(dS);
				} while(!split && dS!=dd);
			}
		}

//		std::cout << "face split " << nbSplit << std::endl;

		//define the path to split
		std::vector<Dart> vPath;
		vPath.reserve((nbSplit+nbVertices)+1);
		vPath.push_back(dPath);
		for(std::vector<Dart>::iterator it = vPath.begin() ;it != vPath.end() ; ++it)
		{
			Dart dd = map.phi1(*it);

			Dart ddd = map.phi1(map.phi2(dd));

			while(!vs.isMarked(map.phi1(ddd)) && ddd!=dd)
				ddd = map.phi1(map.phi2(ddd));

			if(vs.isMarked(map.phi1(ddd)) && !map.sameVertex(ddd,*vPath.begin()))
				vPath.push_back(ddd);
		}

		assert(vPath.size()>2);
		map.splitVolume(vPath);
		dRes = map.phi2(*vPath.begin());
	}

	return dRes;
}

template <typename PFP, typename EMBV, typename EMB>
void catmullClarkVol(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	std::vector<Dart> l_centers;
	std::vector<Dart> l_vertices;

	DartMarkerNoUnmark mv(map);
	CellMarkerNoUnmark me(map, EDGE);
	CellMarker mf(map, FACE);

	AutoAttributeHandler< EMB > attBary(map, VOLUME);
	CellMarker vol(map, VOLUME);

	//pre-computation : compute the centroid of all volume
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(selected(d) && !vol.isMarked(d))
		{
			vol.mark(d);
			attBary[d] = Algo::Geometry::volumeCentroidGen<PFP,EMBV,EMB>(map, d, attributs);
		}
	}

	// first pass: cut edges
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		//memorize each vertices per volumes
		if(selected(d) && !mv.isMarked(d))
		{
			l_vertices.push_back(d);
//			mv.markOrbitInParent<typename PFP::MAP>(VERTEX,d);
			mv.markOrbit(PFP::MAP::ORBIT_IN_PARENT(VERTEX),d);
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

			d = dN;
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

} //namespace Modelisation

} //namespace Algo

} //namespace CGoGN

