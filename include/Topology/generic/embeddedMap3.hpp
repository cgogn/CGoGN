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

#include <vector>
#include <algorithm>

namespace CGoGN
{

template <typename MAP3>
void EmbeddedMap3<MAP3>::deleteOrientedVolume(Dart d)
{
//	DartMarkerStore mark(*this);		// Lock a marker
//
//	std::vector<Dart> visitedFaces;		// Faces that are traversed
//	visitedFaces.reserve(16);
//	visitedFaces.push_back(d);			// Start with the face of d
//	std::vector<Dart>::iterator face;
//
//	//For every face added to the list
//	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
//	{
//		if (!mark.isMarked(*face))		// Face has not been visited yet
//		{
//			Dart dNext = *face ;
//
//			do
//			{
//				mark.mark(dNext);					// Mark
//
//				unsigned int vEmb1 = EMBNULL;		//Manage the embedding
//				if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
//				{
//					vEmb1 = MAP3::getDartEmbedding(VERTEX_ORBIT, dNext);
//					MAP3::embedOrbit(VERTEX_ORBIT, d, vEmb1) ; //alpha1 peut ne pas fonctionner
//				}
//
//				if(MAP3::isOrbitEmbedded(EDGE_ORBIT))
//				{
//					vEmb1 = MAP3::getDartEmbedding(EDGE_ORBIT, dNext);
//					MAP3::setDartEmbedding(VERTEX_ORBIT, MAP3::alpha2(dNext), vEmb1);
//				}
//
//				Dart adj = MAP3::phi2(dNext);				// Get adjacent face
//				if (adj != dNext && !mark.isMarked(adj))
//					visitedFaces.push_back(adj);	// Add it
//				dNext = MAP3::phi1(dNext);
//			} while(dNext != *face);
//		}
//	}

	MAP3::deleteOrientedVolume(d);
}

template <typename MAP3>
void EmbeddedMap3<MAP3>::sewVolumes(Dart d, Dart e)
{
	unsigned int vEmb1 = EMBNULL ;

	//topological sewing
	MAP3::sewVolumes(d,e);

	//embed the new vertex orbit (with the new cell) with old vertex orbit value
	if (MAP3::isOrbitEmbedded(VERTEX_ORBIT))
	{
		Dart dd = d ;
		do {
			vEmb1 = MAP3::getEmbedding(d, VERTEX_ORBIT);
			MAP3::embedOrbit(VERTEX_ORBIT, d, vEmb1) ;
			dd = MAP3::phi1(dd) ;
		} while(dd != d) ;
	}

	//embed the new edge orbit with the old edge orbit value
	if (MAP3::isOrbitEmbedded(EDGE_ORBIT))
	{
		Dart dd = d ;
		do {
			MAP3::copyDartEmbedding(EDGE_ORBIT, e, d) ;
			dd = MAP3::phi1(dd) ;
		} while(dd != d) ;
	}

}

template <typename MAP3>
void EmbeddedMap3<MAP3>::unsewVolumes(Dart d)
{
	unsigned int vEmb1 = EMBNULL ;

	Dart dd = d ;
	do {
		//embed the vertex in the dart of the two volumes for the face
//		if (MAP3::isOrbitEmbedded(VERTEX_ORBIT))
//		{
//			Dart e = MAP3::alpha1(dd);
//
//			vEmb1 = MAP3::getEmbedding(dd, VERTEX_ORBIT) ;
//			MAP3::setDartEmbedding(VERTEX_ORBIT, dd, vEmb1);
//
//			if(e != MAP3::phi1(dd))
//			{
//				MAP3::embedNewCell(VERTEX_ORBIT, e);
//				MAP3::copyCell(VERTEX_ORBIT, e, dd);
//			}
//		}

		if(MAP3::isOrbitEmbedded(EDGE_ORBIT))
		{
			Dart e = MAP3::phi3(dd);

			vEmb1 = MAP3::getEmbedding(dd, EDGE_ORBIT);

			MAP3::setDartEmbedding(EDGE_ORBIT, dd, vEmb1) ;

			if(e != dd)
			{
				MAP3::embedNewCell(EDGE_ORBIT, e);
				MAP3::copyCell(EDGE_ORBIT, e, d);
			}
		}

		dd = MAP3::phi1(dd) ;
	} while(dd != d) ;

	MAP3::unsewVolumes(d);
}

template <typename MAP3>
bool EmbeddedMap3<MAP3>::mergeVolumes(Dart d)
{
	unsigned int fEmb = EMBNULL;
	unsigned int eEmb = EMBNULL;

	Dart d2 = MAP3::phi2(d);
	Dart a2 = MAP3::alpha2(d);

	if (MAP3::isOrbitEmbedded(VOLUME_ORBIT))
	{
		fEmb = MAP3::getEmbedding(d, VOLUME_ORBIT);
		if(fEmb != EMBNULL)
			MAP3::setDartEmbedding(VOLUME_ORBIT, d2, fEmb) ;
	}

	if (MAP3::isOrbitEmbedded(EDGE_ORBIT))
	{
		eEmb = MAP3::getEmbedding(d, EDGE_ORBIT);
		if(eEmb != EMBNULL)
			MAP3::setDartEmbedding(EDGE_ORBIT, a2, eEmb);
	}


	if(MAP3::mergeVolumes(d))
	{
		if (MAP3::isOrbitEmbedded(VOLUME_ORBIT))
		{
			MAP3::embedOrbit(VOLUME_ORBIT, d2, fEmb) ;
		}

		if (MAP3::isOrbitEmbedded(EDGE_ORBIT))
		{
			MAP3::embedOrbit(EDGE_ORBIT, a2, eEmb);
		}

		return true;
	}
	return false;
}

template <typename MAP3>
void EmbeddedMap3<MAP3>::splitFace(Dart d, Dart e)
{

	MAP3::splitFace(d,e);
}

template <typename MAP3>
void EmbeddedMap3<MAP3>::cutEdge(Dart d)
{
//	unsigned int dEmb = EMBNULL ;
//	unsigned int fEmb = EMBNULL;
//
//	if (MAP3::isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		dEmb = MAP3::getEmbedding(d, VERTEX_ORBIT) ;
//	}
//
//	Dart f = MAP3::phi1(d) ;
	MAP3::cutEdge(d);
//	Dart e = MAP3::phi1(d);
//
//	if (MAP3::isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		fEmb = MAP3::getEmbedding(f, VERTEX_ORBIT);
//
//
//		MAP3::setDartEmbedding(EDGE_ORBIT, d, dEmb) ;
//		MAP3::embedNewCell(EDGE_ORBIT, e) ;
//		MAP3::copyCell(EDGE_ORBIT, e, d) ;
//	}
}

//template <typename MAP3>
//bool EmbeddedMap3<MAP3>::flipEdge(Dart d)
//{
//	unsigned int eEmbd = EMBNULL;;
//	unsigned int eEmbd2 = EMBNULL;;
//
//	bool res = MAP3::flipEdge(d);
//
//	if(MAP3::isOrbitEmbedded(EDGE_ORBIT))
//	{
//		eEmbd = MAP3::getEmbedding(d, EDGE_ORBIT);
//		eEmbd2 = MAP3::getEmbedding(MAP3::phi2(d), EDGE_ORBIT);
//	}
//
//	return res;
//}
//
//
//template <typename MAP3>
//bool EmbeddedMap3<MAP3>::flipBackEdge(Dart d)
//{
//
//	bool res = false;
//
//
//	return res;
//}

template <typename MAP3>
int EmbeddedMap3<MAP3>::collapseEdge(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{

	unsigned int vEmb = EMBNULL ;
	if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
	{
		vEmb = MAP3::getEmbedding(d, VERTEX_ORBIT) ;
		MAP3::embedOrbit(VERTEX_ORBIT,d,vEmb);
		MAP3::embedOrbit(VERTEX_ORBIT,MAP3::phi2(d),vEmb);
	}

	int nbCol = MAP3::collapseEdge(d,delDegenerateFaces,delDegenerateVolumes);

	return nbCol;
}

template <typename MAP3>
void EmbeddedMap3<MAP3>::collapseFace(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{
	//unsigned int degree = MAP3::faceDegree(d);
	//Dart dsave;

//	//si degree face = 3
//	if(degree == 3)
//	{
//		dsave = MAP3::phi3(MAP3::phi2(MAP3::phi1(d)));
//		dsave = MAP3::phi3(MAP3::phi2(MAP3::phi1(dsave)));
//		//ATTENTION : il faut trouver un brin de l'orbite sommet de d non modifie par l'operation !!
//	}
//	else if(degree > 3)
//	{
//		dsave = MAP3::phi1(MAP3::phi2(MAP3::phi1(d)));
//		dsave = MAP3::phi1(MAP3::phi2(MAP3::phi1(dsave)));
//	}
//	else
//		return;


//	unsigned int vEmb = EMBNULL ;
//	if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		vEmb = MAP3::getEmbedding(d, VERTEX_ORBIT) ;
//	}

	MAP3::collapseFace(d,delDegenerateFaces,delDegenerateVolumes);

//	if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
//	{
//		MAP3::embedOrbit(VERTEX_ORBIT,dsave,vEmb);
//	}

}

template <typename MAP3>
Dart EmbeddedMap3<MAP3>::cutSpike(Dart d)
{

	Dart e = MAP3::cutSpike(d);

	return e;
}




} // namespace CGoGN
