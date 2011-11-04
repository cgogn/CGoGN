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

#include <vector>
#include <algorithm>

#include "Topology/gmap/embeddedGMap3.h"

namespace CGoGN
{

void EmbeddedGMap3::sewVolumes(Dart d, Dart e)
{
	//topological sewing
	GMap3::sewVolumes(d,e);

	//embed the vertex orbits from the oriented face with dart e
	//with vertex orbits value from oriented face with dart d
	if (isOrbitEmbedded(VERTEX))
	{
		unsigned int vEmb1 = EMBNULL ;
		Dart dd = d ;
		do {
			vEmb1 = getEmbedding(VERTEX, dd);
			embedOrbit(VERTEX, dd, vEmb1) ;
			dd = phi1(dd) ;
		} while(dd != d) ;
	}

	//embed the new edge orbit with the old edge orbit value
	//for all the face
	if (isOrbitEmbedded(EDGE))
	{
		unsigned int vEmb1 = EMBNULL ;
		Dart dd = d ;
		do {
			vEmb1 = getEmbedding(EDGE, d);
			embedOrbit(EDGE, d, vEmb1) ;
			dd = phi1(dd) ;
		} while(dd != d) ;
	}

	//embed the face orbit from the volume sewn
	if (isOrbitEmbedded(FACE))
		copyDartEmbedding(FACE, e, d) ;
}

void EmbeddedGMap3::unsewVolumes(Dart d)
{
//	Dart d3 = phi3(d);
//
//	bool boundaryD = false;
//	bool boundaryE = false;
//
//	if(isOrbitEmbedded(VERTEX))
//	{
//		if(isBoundaryVertex(d))
//			boundaryD = true;
//		if(isBoundaryVertex(phi1(d)))
//			boundaryE = true;
//	}
//
	GMap3::unsewVolumes(d);
//
//	Dart dd = d;
//	Dart dd3 = d3;
//
//	do
//	{
//		if(isOrbitEmbedded(VERTEX))
//			copyCell(VERTEX, dd3, phi1(dd));
//
//		if(isOrbitEmbedded(EDGE))
//
//
//		if(isOrbitEmbedded(FACE))
//
//
//		dd = phi1(dd) ;
//	}while( dd != d );
}

bool EmbeddedGMap3::mergeVolumes(Dart d)
{
	Dart d2 = phi2(d);
	Dart a_2 = phi3(phi2(d));

	if(GMap3::mergeVolumes(d))
	{
		if (isOrbitEmbedded(VOLUME))
		{
				unsigned int vEmb = getEmbedding(VOLUME, d2);
				embedOrbit(VOLUME, a_2, vEmb) ;
		}
		return true;
	}
	return false;
}

void EmbeddedGMap3::splitFace(Dart d, Dart e)
{
	GMap3::splitFace(d,e);

	if(isOrbitEmbedded(VERTEX))
	{
		copyDartEmbedding(VERTEX, phi2(phi_1(d)), d);
		copyDartEmbedding(VERTEX, phi2(phi_1(e)), e);

		if(phi3(d) != d)
		{
			Dart d3 = phi3(d);
			Dart e3 = phi3(e);

			copyDartEmbedding(VERTEX, phi1(d3), phi1(phi2(phi1(d3))));
			copyDartEmbedding(VERTEX, phi1(e3), phi1(phi2(phi1(e3))));
		}
	}

	if(isOrbitEmbedded(FACE))
	{
		embedNewCell(FACE, phi2(phi_1(d)));
		copyCell(FACE, phi2(phi_1(d)), d);
	}

	if(isOrbitEmbedded(VOLUME))
	{
		copyDartEmbedding(VOLUME, phi_1(d),  d);
		copyDartEmbedding(VOLUME, phi2(phi_1(d)),  d);

		if(phi3(d) != d)
		{
			Dart d3 = phi3(d);

			copyDartEmbedding(VOLUME, phi1(d3), d3);
			copyDartEmbedding(VOLUME, phi2(phi1(d3)), d3);
		}
	}
}

void EmbeddedGMap3::cutEdge(Dart d)
{
	GMap3::cutEdge(d);

	if(isOrbitEmbedded(EDGE))
	{
		Dart nd = phi1(d) ;

		embedNewCell(EDGE, nd) ;
		copyCell(EDGE, nd, d) ;

		unsigned int vEmb = getEmbedding(EDGE, d);
		embedOrbit(EDGE, d, vEmb) ;
	}

	if(isOrbitEmbedded(FACE))
	{
		Dart f = d;
		do
		{
			Dart nd = phi1(f) ;
			copyDartEmbedding(FACE, nd, f);

			Dart f2 = phi2(nd);
			if(f2!=nd)
			{
				Dart nd2 = phi2(f);
				copyDartEmbedding(FACE, nd2, f2);
			}

			f = alpha2(f);
		} while(f != d);
	}

	if(isOrbitEmbedded(VOLUME))
	{
		Dart f = d;
		do
		{
			Dart nd = phi1(f) ;
			copyDartEmbedding(VOLUME, nd, f);

			Dart nd2 = phi2(f);
			if(f!=nd2)
				copyDartEmbedding(VOLUME, nd2, f);

			f = alpha2(f);
		} while(f != d);
	}
}

//int EmbeddedGMap3::collapseEdge(Dart d, bool delDegenerateFaces,
//		bool delDegenerateVolumes)
//{
//	unsigned int vEmb = EMBNULL ;
//	if(isOrbitEmbedded(VERTEX))
//	{
//		vEmb = getEmbedding(VERTEX, d) ;
//		embedOrbit(VERTEX,d,vEmb);
//		embedOrbit(VERTEX,phi2(d),vEmb);
//	}
//
//	int nbCol = GMap3::collapseEdge(d,delDegenerateFaces,delDegenerateVolumes);
//
//	return nbCol;
//}

//void EmbeddedGMap3::collapseFace(Dart d, bool delDegenerateFaces,
//		bool delDegenerateVolumes)
//{
//	//unsigned int degree = faceDegree(d);
//	//Dart dsave;
//
////	//si degree face = 3
////	if(degree == 3)
////	{
////		dsave = phi3(phi2(phi1(d)));
////		dsave = phi3(phi2(phi1(dsave)));
////		//ATTENTION : il faut trouver un brin de l'orbite sommet de d non modifie par l'operation !!
////	}
////	else if(degree > 3)
////	{
////		dsave = phi1(phi2(phi1(d)));
////		dsave = phi1(phi2(phi1(dsave)));
////	}
////	else
////		return;
//
//
////	unsigned int vEmb = EMBNULL ;
////	if(isOrbitEmbedded(VERTEX))
////	{
////		vEmb = getEmbedding(d, VERTEX) ;
////	}
//
//	GMap3::collapseFace(d,delDegenerateFaces,delDegenerateVolumes);
//
////	if(isOrbitEmbedded(VERTEX))
////	{
////		embedOrbit(VERTEX,dsave,vEmb);
////	}
//}

//TODO collapseVolume

//Dart EmbeddedGMap3::cutSpike(Dart d)
//{
//	Dart e = GMap3::cutSpike(d);
//	return e;
//}

unsigned int EmbeddedGMap3::closeHole(Dart d)
{
	unsigned int nbE = GMap3::closeHole(d);
	Dart dd = phi2(d);
	Dart f = dd;

	do
	{
		if(isOrbitEmbedded(VERTEX))
			copyDartEmbedding(VERTEX,f, phi1(phi2(f)));
		if(isOrbitEmbedded(EDGE))
			copyDartEmbedding(EDGE, f, phi2(f));
		if(isOrbitEmbedded(VOLUME))
			copyDartEmbedding(VOLUME, f, phi2(f));

		f = phi1(f);
	}
	while(dd != f);

	return nbE;
}

void EmbeddedGMap3::closeMap(DartMarker &marker)
{
	GMap3::closeMap(marker);

	for(Dart d = begin() ; d != end() ; next(d))
	{
		if(marker.isMarked(d))
		{
			if(isOrbitEmbedded(VERTEX))
			{
				copyDartEmbedding(VERTEX, d, phi1(phi3(d)));
			}
		}
	}
}

bool EmbeddedGMap3::check()
{
	bool topo = GMap3::check() ;
	if (!topo)
		return false ;

	CGoGNout << "Check: embedding begin" << CGoGNendl ;
	for(Dart d = begin(); d != end(); next(d))
	{
		if (isOrbitEmbedded(VERTEX))
		{
			if (phi2(phi_1(d)) != phi_1(d) && getEmbedding(VERTEX, d) != getEmbedding(VERTEX, phi2(phi_1(d))))
			{
				CGoGNout << "Check: different embeddings on vertex" << CGoGNendl ;
				return false ;
			}

			 if(phi3(d) != d && getEmbedding(VERTEX, d) != getEmbedding(VERTEX, phi1(phi3(d))))
			 {
					CGoGNout << "Check: different embeddings on vertex in the 2 oriented faces" << CGoGNendl ;
					std::cout << "Dart #" << d << std::endl;
					std::cout << "Emb(d) = " << getEmbedding(VERTEX, d) << std::endl;
					std::cout << "Emb(phi32(d)) = " << getEmbedding(VERTEX, phi3(phi2(d))) << std::endl;
					return false ;
			 }

		}

//		if (isOrbitEmbedded(EDGE))
//		{
//			if (getEmbedding(EDGE, d) != getEmbedding(EDGE, phi2(d)))
//			{
//				CGoGNout << "Check: different embeddings on edge" << CGoGNendl ;
//				return false ;
//			}
//		}
//
//		if (isOrbitEmbedded(FACE))
//		{
//			if (getEmbedding(FACE, d) != getEmbedding(FACE, phi1(d)))
//		{
//				CGoGNout << "Check: different embeddings on face" << CGoGNendl ;
//				return false ;
//			}
//		}
	}
	CGoGNout << "Check: embedding ok" << CGoGNendl ;
	return true ;
}

} // namespace CGoGN
