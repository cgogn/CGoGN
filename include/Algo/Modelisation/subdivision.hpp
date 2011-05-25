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
#include "Topology/generic/autoAttributeHandler.h"

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

template <typename PFP>
Dart trianguleFace(typename PFP::MAP& map, Dart d)
{
	Dart d1 = map.phi1(d);
	if (d1 == d)
		CGoGNout << "Warning: triangulation of a face with only one edge" << CGoGNendl;
	if (map.phi1(d1) == d)
		CGoGNout << "Warning: triangulation of a face with only two edges" << CGoGNendl;

	map.splitFace(d, d1) ;
	map.cutEdge(map.phi_1(d)) ;
	Dart x = map.phi2(map.phi_1(d)) ;
	Dart dd = map.template phi<111>(x) ;
	while(dd != x)
	{
		Dart next = map.phi1(dd) ;
		map.splitFace(dd, map.phi1(x)) ;
		dd = next ;
	}

	return map.phi2(x);	// Return a dart of the central vertex
}

template <typename PFP, typename EMBV, typename EMB>
void trianguleFaces(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	DartMarker m(map) ;
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (selected(d) && !m.isMarked(d))
		{
			EMB center = Algo::Geometry::faceCentroidGen<PFP,EMBV,EMB>(map, d, attributs);	// compute center
			Dart cd = trianguleFace<PFP>(map, d);	// triangule the face
			attributs[cd] = center;					// affect the data to the central vertex
			Dart fit = cd ;
			do
			{
				m.markOrbit(FACE_ORBIT, fit);
				fit = map.alpha1(fit);
			} while(fit != cd);
		}
	}
}

template <typename PFP>
void trianguleFaces(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected)
{
	trianguleFaces<PFP, typename PFP::TVEC3, typename PFP::VEC3>(map, position, selected) ;
}

template <typename PFP>
void trianguleFaces(
		typename PFP::MAP& map,
		typename PFP::TVEC3& position, typename PFP::TVEC3& positionF,
		const FunctorSelect& selected)
{
	assert(position.getOrbit() == VERTEX_ORBIT) ;
	assert(positionF.getOrbit() == FACE_ORBIT) ;

	DartMarker m(map) ;
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (selected(d) && !m.isMarked(d))
		{
			typename PFP::VEC3 p = positionF[d] ;
			Dart cd = trianguleFace<PFP>(map, d) ;	// triangule the face
			position[cd] = p ;						// affect the data to the central vertex
			Dart fit = cd ;
			do
			{
				m.markOrbit(FACE_ORBIT, fit);
				fit = map.alpha1(fit);
			} while(fit != cd);
		}
	}
}

template <typename PFP>
Dart quadranguleFace(typename PFP::MAP& map, Dart d)
{
	d = map.phi1(d) ;
	map.splitFace(d, map.template phi<11>(d)) ;
	map.cutEdge(map.phi_1(d)) ;
	Dart x = map.phi2(map.phi_1(d)) ;
	Dart dd = map.template phi<1111>(x) ;
	while(dd != x)
	{
		Dart next = map.template phi<11>(dd) ;
		map.splitFace(dd, map.phi1(x)) ;
		dd = next ;
	}

	return map.phi2(x);	// Return a dart of the central vertex
}

template <typename PFP, typename EMBV, typename EMB>
void quadranguleFaces(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	DartMarker me(map) ;
	DartMarker mf(map) ;

	// first pass: cut the edges
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (selected(d) && !me.isMarked(d))
		{
			Dart f = map.phi1(d);
			map.cutEdge(d);
			Dart e = map.phi1(d);
//			TODO trouver pourquoi lerp bug avec ECell
//			attributs[m] = AttribOps::lerp<EMB,PFP>(attributs[d],attributs[f], 0.5);
			attributs[e] = attributs[d];
			attributs[e] += attributs[f];
			attributs[e] *= 0.5;

			me.markOrbit(EDGE_ORBIT, d);
			me.markOrbit(EDGE_ORBIT, e);
			mf.markOrbit(VERTEX_ORBIT, e);
		}
	}

	// second pass: quandrangule faces
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (selected(d) && !mf.isMarked(d))
		{
			EMB center = Algo::Geometry::faceCentroidGen<PFP,EMBV,EMB>(map, d, attributs);	// compute center
			Dart cf = quadranguleFace<PFP>(map, d);	// quadrangule the face
			attributs[cf] = center;					// affect the data to the central vertex
			Dart e = cf;
			do
			{
				mf.markOrbit(FACE_ORBIT, e);
				e = map.alpha1(e);
			} while (e != cf);
		}
	}
}

template <typename PFP>
void quadranguleFaces(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected)
{
	quadranguleFaces<PFP, typename PFP::TVEC3, typename PFP::VEC3>(map, position, selected) ;
}

template <typename PFP, typename EMBV, typename EMB>
void CatmullClarkSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	std::vector<Dart> l_middles;
	std::vector<Dart> l_verts;

	CellMarkerNoUnmark m0(map, VERTEX_CELL);
	DartMarkerNoUnmark mf(map);
	DartMarkerNoUnmark me(map);

	// first pass: cut edges
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (selected(d) && !me.isMarked(d))
		{
			if (!m0.isMarked(d))
			{
				m0.mark(d);
				l_verts.push_back(d);
			}
			if (!m0.isMarked(map.phi2(d)))
			{
				m0.mark(map.phi2(d));
				l_verts.push_back(map.phi2(d));
			}

			Dart f = map.phi1(d);
			map.cutEdge(d);
			Dart e = map.phi1(d) ;

			attributs[e] =  attributs[d];
			attributs[e] += attributs[f];
			attributs[e] *= 0.5;

			me.markOrbit(EDGE_ORBIT, d);
			me.markOrbit(EDGE_ORBIT, e);

			mf.mark(d) ;
			mf.mark(map.phi2(e)) ;

			l_middles.push_back(e);
		}
	}

	// second pass: quandrangule faces
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (selected(d) && mf.isMarked(d)) // for each face not subdivided
		{
			// compute center skip darts of new vertices non embedded
			EMB center = AttribOps::zero<EMB,PFP>();
			unsigned int count = 0 ;
			Dart it = d;
			do
			{
				mf.unmark(it);
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

	// Compute edge points
	for(typename std::vector<Dart>::iterator mid = l_middles.begin(); mid != l_middles.end(); ++mid)
	{
		Dart x = *mid;
		// other side of the edge
		Dart y = map.phi2(x);
		if (y != x)
		{
			Dart f1 = map.phi_1(x);
			Dart f2 = map.phi2(map.phi1(y));
			EMB temp = AttribOps::zero<EMB,PFP>();
			temp =  attributs[f1];
			temp += attributs[f2];			// E' = (V0+V1+F1+F2)/4
			temp *= 0.25;
			attributs[x] *= 0.5;
			attributs[x] += temp;
		}
		// else nothing to do point already in the middle of segment
	}

	// Compute vertex points
	for(typename std::vector<Dart>::iterator vert = l_verts.begin(); vert != l_verts.end(); ++vert)
	{
		m0.unmark(*vert);

		EMB temp = AttribOps::zero<EMB,PFP>();
		EMB temp2 = AttribOps::zero<EMB,PFP>();

		unsigned int n = 0;
		Dart x = *vert;
		do
		{
			Dart m = map.phi1(x);
			Dart f = map.phi2(m);
			Dart v = map.template phi<11>(f);

			temp += attributs[f];
			temp2 += attributs[v];

			++n;
			x = map.alpha1(x);
		} while (x != *vert);

		EMB emcp = attributs[*vert];
		emcp *= double((n-2)*n);		// V' = (n-2)/n*V + 1/n2 *(F+E)
		emcp += temp;
		emcp += temp2;
		emcp /= double(n*n);

		attributs[*vert] = emcp ;
	}
}

template <typename PFP>
void CatmullClarkSubdivision(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected)
{
	CatmullClarkSubdivision<PFP, typename PFP::TVEC3, typename PFP::VEC3>(map, position, selected) ;
}

inline double betaF(unsigned int n)
{
	switch(n)
	{
		case 1: return 0.234375 ;
		case 2: return 0.609375 ;
		case 3: return 0.5625 ;
		case 4: return 0.484375 ;
		case 5: return 0.420466 ;
		case 6: return 0.375 ;
		case 7: return 0.343174 ;
		case 8: return 0.320542 ;
		case 9: return 0.304065 ;
		case 10: return 0.291778 ;
		case 11: return 0.282408 ;
		case 12: return 0.27512 ;
		case 13: return 0.26935 ;
		case 14: return 0.264709 ;
		default:
			double t = 3.0 + 2.0 * cos((2.0*M_PI)/double(n)) ;
			return 5.0/8.0 - (t * t) / 64.0 ;
	}
}

template <typename PFP, typename EMBV, typename EMB>
void LoopSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	std::vector<Dart> l_middles;
	std::vector<Dart> l_verts;

	CellMarkerNoUnmark m0(map, VERTEX_CELL);
	DartMarkerNoUnmark mv(map);
	DartMarkerNoUnmark me(map);

	// first pass cut edges
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (selected(d) && !me.isMarked(d))
		{
			if (!m0.isMarked(d))
			{
				m0.mark(d);
				l_verts.push_back(d);
			}
			if (!m0.isMarked(map.phi2(d)))
			{
				m0.mark(map.phi2(d));
				l_verts.push_back(map.phi2(d));
			}

			Dart f = map.phi1(d);

			map.cutEdge(d);
			Dart e = map.phi1(d) ;

			attributs[e] =  attributs[d];
			attributs[e] += attributs[f];
			attributs[e] *= 0.5;

			me.markOrbit(EDGE_ORBIT, d);
			me.markOrbit(EDGE_ORBIT, e);

			mv.markOrbit(VERTEX_ORBIT, e);

			l_middles.push_back(e);
		}
	}

	// Compute edge points
	for(typename std::vector<Dart>::iterator mid = l_middles.begin(); mid != l_middles.end(); ++mid)
	{
		Dart d = *mid;
		Dart dd = map.phi2(d);
		if (dd != d)
		{
			attributs[d] *= 0.75;
			Dart e1 = map.template phi<111>(d);
			EMB temp = AttribOps::zero<EMB,PFP>();
			temp = attributs[e1];
			e1 = map.phi_1(map.phi_1(dd));
			temp += attributs[e1];
			temp *= 1.0/8.0;
			attributs[d] += temp;
		}
		// else nothing to do point already in the middle of segment
	}

	// Compute vertex points
	for(typename std::vector<Dart>::iterator vert = l_verts.begin(); vert != l_verts.end(); ++vert)
	{
		m0.unmark(*vert);

		EMB temp = AttribOps::zero<EMB,PFP>();
		int n = 0;
		Dart x = *vert;
		do
		{
			Dart y = map.phi1(map.phi1(x));
			temp += attributs[y];
			++n;
			x = map.alpha1(x);
		} while ((x != *vert));
		EMB emcp = attributs[*vert];
		if (n == 6)
		{
			temp /= 16.0;
			emcp *= 10.0/16.0;
			emcp += temp;
		}
		else
		{
			double beta = betaF(n) ;
			temp *= (beta / double(n));
			emcp *= (1.0 - beta);
			emcp += temp;
		}
		attributs[*vert] = emcp;
	}

	// insert new edges
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (mv.isMarked(d))
		{
			// unmark the darts of the face
			Dart fit = d ;
			do
			{
				mv.unmark(fit) ;
				me.unmark(fit) ;
				me.unmark(map.phi_1(fit)) ;
				fit = map.template phi<11>(fit) ;
			} while(fit != d) ;

			Dart dd = d;
			Dart e = map.template phi<11>(dd) ;
			map.splitFace(dd, e);

			dd = e;
			e = map.template phi<11>(dd) ;
			map.splitFace(dd, e);

			dd = e;
			e = map.template phi<11>(dd) ;
			map.splitFace(dd, e);
		}
	}
}

template <typename PFP, typename EMBV, typename EMB>
void TwoNPlusOneSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{
	AutoAttributeHandler< Dart > tablePred(map,EDGE_ORBIT);
	CellMarker m0(map, EDGE_CELL);

	std::vector<Dart> dOrig;
	std::vector<EMB> cOrig;

	//first pass cut edge
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(selected(d))
		{
			if(!m0.isMarked(d)) {
				dOrig.push_back(d);

				Dart dd = d;
				do {
					if(!m0.isMarked(dd)) {
						EMB e1 = attributs[dd];
						EMB e2 = attributs[map.phi1(dd)];
						map.cutEdge(dd);
						attributs[map.phi1(dd)] = e1*2.0f/3.0f+e2/3.0f;
						map.cutEdge(map.phi1(dd));
						attributs[map.phi1(map.phi1(dd))] = e2*2.0f/3.0f+e1/3.0f;
						m0.mark(dd);
						m0.mark(map.phi1(dd));
						m0.mark(map.template phi<11>(dd));
					}
					dd = map.template phi<111>(dd);
				} while(dd!=d);

			}
		}
	}

	CGoGNout << "nb orig : " << dOrig.size() << CGoGNendl;

	DartMarkerNoUnmark mCorner(map);
//	//second pass create corner face
	for (std::vector<Dart>::iterator it = dOrig.begin(); it != dOrig.end(); ++it)
	{
		EMB c = Algo::Geometry::faceCentroid<PFP>(map,*it,attributs);
		Dart dd = *it;
		do
		{
			map.splitFace(map.phi1(dd),map.phi_1(dd));
			map.cutEdge(map.phi1(dd));
			mCorner.mark(map.phi2(map.phi1(dd)));
			attributs[map.template phi<11>(dd)] = c*2.0/3.0f + attributs[dd]/3.0f;
			dd = map.phi1(map.phi1(map.phi1(map.phi2(map.phi1(dd)))));
		} while(!mCorner.isMarked(dd));
	}

	//third pass create center face
	for (std::vector<Dart>::iterator it = dOrig.begin(); it != dOrig.end(); ++it)
	{
		Dart dd = map.phi2(map.phi1(*it));
		do {
			mCorner.unmark(dd);
			Dart dNext = map.phi1(map.phi1(map.phi1(dd)));
			map.splitFace(dd,dNext);
			dd = dNext;
		} while(mCorner.isMarked(dd));
	}
}

template <typename PFP>
void LoopSubdivision(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected)
{
	LoopSubdivision<PFP, typename PFP::TVEC3, typename PFP::VEC3>(map, position, selected) ;
}

template <typename PFP>
void reverseOrientation(typename PFP::MAP& map)
{
	AttributeHandler<unsigned int> emb0(&map, map.getEmbeddingAttributeVector(VERTEX_ORBIT)) ;
	if(emb0.isValid())
	{
		AttributeHandler<unsigned int> new_emb0 = map.template addAttribute<unsigned int>(DART_ORBIT, "new_EMB_0") ;
		for(Dart d = map.begin(); d != map.end(); map.next(d))
			new_emb0[d] = emb0[map.phi1(d)] ;
		map.template swapAttributes<unsigned int>(emb0, new_emb0) ;
		map.template removeAttribute<unsigned int>(new_emb0) ;
	}

	AttributeHandler<Dart> phi1 = map.template getAttribute<Dart>(DART_ORBIT, "phi1") ;
	AttributeHandler<Dart> phi_1 = map.template getAttribute<Dart>(DART_ORBIT, "phi_1") ;
	map.template swapAttributes<Dart>(phi1, phi_1) ;
}

template <typename PFP>
void computeDual(typename PFP::MAP& map, const FunctorSelect& selected)
{
	AttributeHandler<Dart> phi1 = map.template getAttribute<Dart>(DART_ORBIT, "phi1") ;
	AttributeHandler<Dart> phi_1 = map.template getAttribute<Dart>(DART_ORBIT, "phi_1") ;
	AttributeHandler<Dart> new_phi1 = map.template addAttribute<Dart>(DART_ORBIT, "new_phi1") ;
	AttributeHandler<Dart> new_phi_1 = map.template addAttribute<Dart>(DART_ORBIT, "new_phi_1") ;

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		Dart dd = map.alpha_1(d) ;
		new_phi1[d] = dd ;
		new_phi_1[dd] = d ;
	}

	map.template swapAttributes<Dart>(phi1, new_phi1) ;
	map.template swapAttributes<Dart>(phi_1, new_phi_1) ;

	map.template removeAttribute<Dart>(new_phi1) ;
	map.template removeAttribute<Dart>(new_phi_1) ;

	map.swapEmbeddingContainers(VERTEX_ORBIT, FACE_ORBIT) ;

	reverseOrientation<PFP>(map) ;
}

inline double sqrt3_K(unsigned int n)
{
	switch(n)
	{
		case 1: return 0.333333 ;
		case 2: return 0.555556 ;
		case 3: return 0.5 ;
		case 4: return 0.444444 ;
		case 5: return 0.410109 ;
		case 6: return 0.388889 ;
		case 7: return 0.375168 ;
		case 8: return 0.365877 ;
		case 9: return 0.359328 ;
		case 10: return 0.354554 ;
		case 11: return 0.350972 ;
		case 12: return 0.348219 ;
		default:
			double t = cos((2.0*M_PI)/double(n)) ;
			return (4.0 - t) / 9.0 ;
	}
}

template <typename PFP>
void Sqrt3Subdivision(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	AttributeHandler<VEC3> positionF = map.template getAttribute<VEC3>(FACE_ORBIT, "position") ;
	if(!positionF.isValid())
		positionF = map.template addAttribute<VEC3>(FACE_ORBIT, "position") ;
	Algo::Geometry::computeCentroidFaces<PFP>(map, position, positionF) ;

	computeDual<PFP>(map, selected);

	AttributeHandler<VEC3> tmp = position ;
	position = positionF ;
	positionF = tmp ;

	CellMarker m(map, VERTEX_ORBIT) ;
	m.markAll() ;

	trianguleFaces<PFP>(map, position, positionF, selected);

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.mark(d) ;
			VEC3 P = position[d] ;
			VEC3 newP(0) ;
			unsigned int val = 0 ;
			Dart vit = d ;
			do
			{
				newP += position[map.phi2(vit)] ;
				++val ;
				vit = map.alpha1(vit) ;
			} while(vit != d) ;
			REAL K = sqrt3_K(val) ;
			newP *= REAL(3) ;
			newP -= REAL(val) * P ;
			newP *= K / REAL(2 * val) ;
			newP += (REAL(1) - K) * P ;
			position[d] = newP ;
		}
	}
}

template <typename PFP, typename EMBV, typename EMB>
void quadranguleFacesVolume(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected)
{

}

template <typename PFP, typename EMBV, typename EMB>
void hexaCutVolume(typename PFP::MAP& map, Dart d, EMBV& attributs)
{
	DartMarker mf(map) ; //mark face
	DartMarker me(map) ; //mark edge

	DartMarkerStore mark(map);		// Lock a marker

	// compute volume centroid
	EMB volCenter = Algo::Geometry::volumeCentroidGen<PFP,EMBV,EMB>(map, d, attributs);

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(100);
	visitedFaces.push_back(d);
	std::vector<Dart>::iterator face;

	//Store the edges before the cutEdge
	std::vector<Dart> oldEdges;
	oldEdges.reserve(100);
	std::vector<Dart>::iterator edge;

	//Store a dart from a each face
	std::vector<Dart> faces;
	faces.reserve(100);

	//Store the darts from quadrangulated faces
	std::vector<Dart> quadfaces;
	quadfaces.reserve(100);

	// First pass : for every face added to the list save a dart
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))		// Face has not been visited yet
		{

			faces.push_back(*face);

			Dart dNext = *face ;
			do
			{
				mark.mark(dNext);					// Mark
				oldEdges.push_back(dNext);
				Dart adj = map.phi2(dNext);				// Get adjacent face
				if (adj != dNext && !mark.isMarked(adj))
					visitedFaces.push_back(adj);	// Add it
				dNext = map.phi1(dNext);
			} while(dNext != *face);
		}
	}

	//Second pass : cut the edges && quadrangule the faces
	for (face = faces.begin(); face != faces.end(); ++face)
	{
		Dart dNext = *face;

		//parcours de la face pour couper les aretes
		do
		{
			Dart d = dNext;
			dNext = map.phi1(dNext);

			if(!me.isMarked(d))
			{
				Dart f = map.phi1(d);
				map.cutEdge(d);
				Dart e = map.phi1(d);
				attributs[e] = attributs[d];
				attributs[e] += attributs[f];
				attributs[e] *= 0.5;

				me.markOrbit(EDGE_ORBIT, d);
				me.markOrbit(EDGE_ORBIT, e);
			}

		}while (dNext != *face);

		//quadrangulation
		EMB center = Algo::Geometry::faceCentroidGen<PFP,EMBV,EMB>(map, *face, attributs);	// compute center
		Dart cf = Algo::Modelisation::quadranguleFace<PFP>(map, *face);	// quadrangule the face
		attributs[cf] = center;	// affect the data to the central vertex

		Dart e = cf;
		do
		{
			quadfaces.push_back(e);
			quadfaces.push_back(map.phi2(e));
			e = map.phi2(map.phi_1(e));
		}while (e != cf);
	}

	DartMarker moe(map) ; //mark edge

	//Third pass : deconnect the corners
	for (edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{

		map.unsewFaces(map.phi1(*edge));
		moe.markOrbit(DART_ORBIT,map.phi1(*edge));
	}

	//Thourth pass : close the hole
	for (edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
	{
		if(moe.isMarked(map.phi1(*edge)))
		{
			map.closeHole(map.phi1(*edge));
			moe.unmark(map.phi1(*edge));
			moe.unmark(map.phi1(map.phi2(map.phi_1(*edge))));
			moe.unmark(map.phi1(map.phi1(map.phi2(*edge))));

			Dart cf = Algo::Modelisation::quadranguleFace<PFP>(map,  map.phi1(map.phi2(map.phi1(*edge))));	// quadrangule the face
			attributs[cf] = volCenter;	// affect the data to the central vertex
		}
	}

	moe.unmarkAll();

	//Fifth pass : traversal to phi3 sewing
	std::vector<Dart>::iterator nvol;
	for (nvol = quadfaces.begin(); nvol != quadfaces.end(); nvol = nvol + 2)
	{
		map.sewVolumes(map.phi2(*nvol), map.phi2(*(nvol+1)));
	}

//	// Compute edge points
//	for (edge = oldEdges.begin(); edge != oldEdges.end(); ++edge)
//	{
//		Dart x = *edge;
//		// other side of the edge
//		Dart y = map.phi2(x);
//		if (y != x)
//		{
//			Dart f1 = map.phi_1(x);
//			Dart f2 = map.phi2(map.phi1(y));
//			EMB temp = AttribOps::zero<EMB,PFP>();
//			temp =  attributs[f1];
//			temp += attributs[f2];			// E' = (V0+V1+F1+F2)/4
//			temp *= 0.25;
//			attributs[x] *= 0.5;
//			attributs[x] += temp;
//		}
//		// else nothing to do point already in the middle of segment
//	}
//
//	// Compute vertex points
//	for (face = faces.begin(); face != faces.end(); ++face)
//	{
//		//m0.unmark(*face);
//
//		EMB temp = AttribOps::zero<EMB,PFP>();
//		EMB temp2 = AttribOps::zero<EMB,PFP>();
//
//		unsigned int n = 0;
//		Dart x = *face;
//		do
//		{
//			Dart m = map.phi1(x);
//			Dart f = map.phi2(m);
//			Dart v = map.template phi<11>(f);
//
//			temp += attributs[f];
//			temp2 += attributs[v];
//
//			++n;
//			x = map.alpha1(x);
//		} while (x != *face);
//
//		EMB emcp = attributs[*face];
//		emcp *= double((n-2)*n);		// V' = (n-2)/n*V + 1/n2 *(F+E)
//		emcp += temp;
//		emcp += temp2;
//		emcp /= double(n*n);
//
//		attributs[*face] = emcp ;
//	}
}

template <typename PFP, typename EMBV, typename EMB>
void dooSabinVolume(typename PFP::MAP& map, Dart d, EMBV& attributs)
{
	DartMarker mf(map) ; //mark face
	DartMarker me(map) ; //mark edge

	DartMarkerStore mark(map);		// Lock a marker

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(100);
	visitedFaces.push_back(d);
	std::vector<Dart>::iterator face;

	//Store a dart from a each face
	std::vector<Dart> faces;
	faces.reserve(100);

	// First pass : for every face added to the list save a dart
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))		// Face has not been visited yet
		{

			faces.push_back(*face);

			Dart dNext = *face ;
			do
			{
				mark.mark(dNext);					// Mark
				Dart adj = map.phi2(dNext);				// Get adjacent face
				if (adj != dNext && !mark.isMarked(adj))
					visitedFaces.push_back(adj);	// Add it
				dNext = map.phi1(dNext);
			} while(dNext != *face);
		}
	}

	// Second pass : for every edges insert a face
	for(face = faces.begin() ; face != faces.end() ; ++face)
	{
		Dart e = *face;
		do
		{
			if(!me.isMarked(e))
			{
				//insertion d'une face dans l'arete
				Dart e2 = map.phi2(e);
				map.unsewFaces(e);
				Dart ne = map.newOrientedFace(4);

				map.sewFaces(e,ne);
				map.sewFaces(e2, map.phi1(map.phi1(ne)));

				//marquage de l'orbite arete
				me.markOrbit(EDGE_ORBIT,e);
			}

			e = map.phi1(e);
		}while (e != *face);
	}

//	Dart f = map.phi1(d);
//	map.cutEdge(d);
//	Dart e = map.phi1(d);
//	attributs[e] = attributs[d];
//	attributs[e] += attributs[f];
//	attributs[e] *= 0.5;
//
//	Dart dPrev = d;
//	Dart ePrev = map.phi2(d);
//	map.splitFace(dPrev,ePrev);
//
//	attributs[map.phi_1(d)] = attributs[map.phi1(ePrev)];

}

} // namespace Modelisation

} // namespace Algo

} // namespace CGoGN
