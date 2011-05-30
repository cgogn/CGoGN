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
void EmbeddedMap3<MAP3>::sewVolumes(Dart d, Dart e)
{
	unsigned int vEmb1 = EMBNULL ;

	if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
	{

	}

	//topological sewing
	MAP3::sewVolumes(d,e);

	//embed the new vertex orbit (with the new cell) with old vertex orbit value
	if (MAP3::isOrbitEmbedded(VERTEX_ORBIT))
	{
		Dart dd = d ;
		do {
			vEmb1 = MAP3::getEmbedding(VERTEX_ORBIT, d);
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

			vEmb1 = MAP3::getEmbedding(EDGE_ORBIT, dd);

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
		fEmb = MAP3::getEmbedding(VOLUME_ORBIT, d);
		if(fEmb != EMBNULL)
			MAP3::setDartEmbedding(VOLUME_ORBIT, d2, fEmb) ;
	}

	if (MAP3::isOrbitEmbedded(EDGE_ORBIT))
	{
		eEmb = MAP3::getEmbedding(EDGE_ORBIT, d);
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

	if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
	{
		MAP3::copyDartEmbedding(VERTEX_ORBIT, MAP3::phi2(MAP3::phi_1(d)),  d);
		MAP3::copyDartEmbedding(VERTEX_ORBIT, MAP3::phi2(MAP3::phi_1(e)),  e);

		if(MAP3::phi3(d) != d)
		{
			Dart d3 = MAP3::phi3(d);
			Dart e3 = MAP3::phi3(e);

			MAP3::copyDartEmbedding(VERTEX_ORBIT, MAP3::phi1(d3), MAP3::phi1(MAP3::phi2(MAP3::phi1(d3))));
			MAP3::copyDartEmbedding(VERTEX_ORBIT, MAP3::phi1(e3), MAP3::phi1(MAP3::phi2(MAP3::phi1(e3))));
		}

	}

}

template <typename MAP3>
void EmbeddedMap3<MAP3>::cutEdge(Dart d)
{

	MAP3::cutEdge(d);

}


template <typename MAP3>
int EmbeddedMap3<MAP3>::collapseEdge(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{

	unsigned int vEmb = EMBNULL ;
	if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
	{
		vEmb = MAP3::getEmbedding(VERTEX_ORBIT, d) ;
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


template <typename MAP3>
unsigned int EmbeddedMap3<MAP3>::closeHole(Dart d)
{
	unsigned int nbE = MAP3::closeHole(d);
	Dart dd = MAP3::phi2(d);
	Dart f = dd;

	do
	{
		if(MAP3::isOrbitEmbedded(VERTEX_ORBIT))
			MAP3::copyDartEmbedding(VERTEX_ORBIT,f, MAP3::phi1(MAP3::phi2(f)));
		if(MAP3::isOrbitEmbedded(EDGE_ORBIT))
			MAP3::copyDartEmbedding(EDGE_ORBIT, f, MAP3::phi2(f));

		f = MAP3::phi1(f);
	}
	while(dd != f);

	return nbE;
}


} // namespace CGoGN
