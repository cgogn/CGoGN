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

namespace CGoGN
{

template <typename MAP3>
void EmbeddedMap3<MAP3>::sewVolumes(Dart d, Dart e)
{
	//topological sewing
	MAP3::sewVolumes(d,e);

	//embed the vertex orbits from the oriented face with dart e
	//with vertex orbits value from oriented face with dart d
	if (MAP3::isOrbitEmbedded(VERTEX))
	{
		unsigned int vEmb1 = EMBNULL ;
		Dart dd = d ;
		do {
			vEmb1 = MAP3::getEmbedding(VERTEX, dd);
			MAP3::embedOrbit(VERTEX, dd, vEmb1) ;
			dd = MAP3::phi1(dd) ;
		} while(dd != d) ;
	}

	//embed the new edge orbit with the old edge orbit value
	//for all the face
	if (MAP3::isOrbitEmbedded(EDGE))
	{
		unsigned int vEmb1 = EMBNULL ;
		Dart dd = d ;
		do {
			vEmb1 = MAP3::getEmbedding(EDGE, d);
			MAP3::embedOrbit(EDGE, d, vEmb1) ;
			dd = MAP3::phi1(dd) ;
		} while(dd != d) ;
	}

	//embed the face orbit from the volume sewn
	if (MAP3::isOrbitEmbedded(FACE))
	{
		unsigned int vEmb1 = MAP3::getEmbedding(FACE, d);
		MAP3::embedOrbit(FACE, e, vEmb1) ;
	}
}

template <typename MAP3>
void EmbeddedMap3<MAP3>::unsewVolumes(Dart d)
{
	Dart dd = MAP3::phi1(MAP3::phi3(d));

	MAP3::unsewVolumes(d);

	Dart ddd = d;
	do
	{
		if(MAP3::isOrbitEmbedded(VERTEX))
		{
			if(!MAP3::sameVertex(d,dd))
			{
				MAP3::embedNewCell(VERTEX, dd);
				MAP3::copyCell(VERTEX, dd, d);
			}
		}

		dd = MAP3::phi_1(dd);

		if(MAP3::isOrbitEmbedded(EDGE))
		{
			if(!MAP3::sameEdge(d,dd))
			{
				MAP3::embedNewCell(EDGE, dd);
				MAP3::copyCell(VERTEX, dd, d);
			}
		}

		ddd = MAP3::phi1(ddd);
	} while(ddd!=d);


	if (MAP3::isOrbitEmbedded(FACE))
	{
		MAP3::embedNewCell(FACE, dd);
		MAP3::copyCell(FACE, dd, d);
	}
}

template <typename MAP3>
bool EmbeddedMap3<MAP3>::mergeVolumes(Dart d)
{
	Dart d2 = MAP3::phi2(d);
	Dart a_2 = MAP3::phi3(MAP3::phi2(d));

	if(MAP3::mergeVolumes(d))
	{
		if (MAP3::isOrbitEmbedded(VOLUME))
		{
				unsigned int vEmb = MAP3::getEmbedding(VOLUME, d2);
				MAP3::embedOrbit(VOLUME, a_2, vEmb) ;
		}
		return true;
	}
	return false;
}

template <typename MAP3>
void EmbeddedMap3<MAP3>::splitFace(Dart d, Dart e)
{
	MAP3::splitFace(d,e);

	if(MAP3::isOrbitEmbedded(VERTEX))
	{
		MAP3::copyDartEmbedding(VERTEX, MAP3::phi2(MAP3::phi_1(d)), d);
		MAP3::copyDartEmbedding(VERTEX, MAP3::phi2(MAP3::phi_1(e)), e);

		if(MAP3::phi3(d) != d)
		{
			Dart d3 = MAP3::phi3(d);
			Dart e3 = MAP3::phi3(e);

			MAP3::copyDartEmbedding(VERTEX, MAP3::phi1(d3), MAP3::phi1(MAP3::phi2(MAP3::phi1(d3))));
			MAP3::copyDartEmbedding(VERTEX, MAP3::phi1(e3), MAP3::phi1(MAP3::phi2(MAP3::phi1(e3))));
		}

	}

	if(MAP3::isOrbitEmbedded(FACE))
	{
		MAP3::embedNewCell(FACE, MAP3::phi2(MAP3::phi_1(d)));
		MAP3::copyCell(FACE, MAP3::phi2(MAP3::phi_1(d)), d);
	}

	if(MAP3::isOrbitEmbedded(VOLUME))
	{
		MAP3::copyDartEmbedding(VOLUME, MAP3::phi_1(d),  d);
		MAP3::copyDartEmbedding(VOLUME, MAP3::phi2(MAP3::phi_1(d)),  d);

		if(MAP3::phi3(d) != d)
		{
			Dart d3 = MAP3::phi3(d);

			MAP3::copyDartEmbedding(VOLUME, MAP3::phi1(d3), d3);
			MAP3::copyDartEmbedding(VOLUME, MAP3::phi2(MAP3::phi1(d3)), d3);
		}

	}

}

template <typename MAP3>
void EmbeddedMap3<MAP3>::cutEdge(Dart d)
{
	MAP3::cutEdge(d);

	if(MAP3::isOrbitEmbedded(VERTEX))
	{
		Dart nd = MAP3::phi1(d) ;

		MAP3::embedNewCell(VERTEX, nd) ;
	}

	if(MAP3::isOrbitEmbedded(EDGE))
	{
		Dart nd = MAP3::phi1(d) ;

		MAP3::embedNewCell(EDGE, nd) ;
		MAP3::copyCell(EDGE, nd, d) ;

		unsigned int vEmb = MAP3::getEmbedding(EDGE, d);
		MAP3::embedOrbit(EDGE, d, vEmb) ;
	}

	if(MAP3::isOrbitEmbedded(FACE))
	{
		Dart f = d;
		do
		{
			Dart nd = MAP3::phi1(f) ;
			MAP3::copyDartEmbedding(FACE, nd, f);

			Dart f2 = MAP3::phi2(nd);
			if(f2!=nd)
			{
				Dart nd2 = MAP3::phi2(f);
				MAP3::copyDartEmbedding(FACE, nd2, f2);
			}

			f = MAP3::alpha2(f);
		} while(f != d);
	}

	if(MAP3::isOrbitEmbedded(VOLUME))
	{
		Dart f = d;
		do
		{
			Dart nd = MAP3::phi1(f) ;
			MAP3::copyDartEmbedding(VOLUME, nd, f);

			Dart nd2 = MAP3::phi2(f);
			if(f!=nd2)
				MAP3::copyDartEmbedding(VOLUME, nd2, f);

			f = MAP3::alpha2(f);
		} while(f != d);
	}

}


template <typename MAP3>
int EmbeddedMap3<MAP3>::collapseEdge(Dart d, bool delDegenerateFaces,
		bool delDegenerateVolumes)
{

	unsigned int vEmb = EMBNULL ;
	if(MAP3::isOrbitEmbedded(VERTEX))
	{
		vEmb = MAP3::getEmbedding(VERTEX, d) ;
		MAP3::embedOrbit(VERTEX,d,vEmb);
		MAP3::embedOrbit(VERTEX,MAP3::phi2(d),vEmb);
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
//	if(MAP3::isOrbitEmbedded(VERTEX))
//	{
//		vEmb = MAP3::getEmbedding(d, VERTEX) ;
//	}

	MAP3::collapseFace(d,delDegenerateFaces,delDegenerateVolumes);

//	if(MAP3::isOrbitEmbedded(VERTEX))
//	{
//		MAP3::embedOrbit(VERTEX,dsave,vEmb);
//	}

}

//TODO collapseVolume

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
		if(MAP3::isOrbitEmbedded(VERTEX))
			MAP3::copyDartEmbedding(VERTEX,f, MAP3::phi1(MAP3::phi2(f)));
		if(MAP3::isOrbitEmbedded(EDGE))
			MAP3::copyDartEmbedding(EDGE, f, MAP3::phi2(f));
		if(MAP3::isOrbitEmbedded(VOLUME))
			MAP3::copyDartEmbedding(VOLUME, f, MAP3::phi2(f));

		f = MAP3::phi1(f);
	}
	while(dd != f);

	return nbE;
}


template <typename MAP3>
void EmbeddedMap3<MAP3>::closeMap(DartMarker &marker)
{
	MAP3::closeMap(marker);

	for(Dart d = MAP3::begin() ; d != MAP3::end() ; MAP3::next(d))
	{
		if(marker.isMarked(d))
		{
			if(MAP3::isOrbitEmbedded(VERTEX))
			{
				MAP3::copyDartEmbedding(VERTEX, d, MAP3::phi1(MAP3::phi3(d)));
			}
		}
	}

}

template <typename MAP3>
bool EmbeddedMap3<MAP3>::check()
{
	bool topo = MAP3::check() ;
	if (!topo)
		return false ;

	CGoGNout << "Check: embedding begin" << CGoGNendl ;
	for(Dart d = MAP3::begin(); d != MAP3::end(); MAP3::next(d))
	{
		if (MAP3::isOrbitEmbedded(VERTEX))
		{
			if (MAP3::phi2(MAP3::phi_1(d)) != MAP3::phi_1(d) && MAP3::getEmbedding(VERTEX, d) != MAP3::getEmbedding(VERTEX, MAP3::phi2(MAP3::phi_1(d))))
			{
				CGoGNout << "Check: different embeddings on vertex" << CGoGNendl ;
				return false ;
			}

			if(MAP3::phi3(d) != d && MAP3::getEmbedding(VERTEX, d) != MAP3::getEmbedding(VERTEX, MAP3::phi1(MAP3::phi3(d))))
			{
				CGoGNout << "Check: different embeddings on vertex in the 2 oriented faces" << CGoGNendl ;
				CGoGNout << "Dart #" << d << CGoGNendl;
				CGoGNout << "Emb(d) = " << MAP3::getEmbedding(VERTEX, d) << CGoGNendl;
				CGoGNout << "Emb(phi32(d)) = " << MAP3::getEmbedding(VERTEX, MAP3::phi3(MAP3::phi2(d))) << CGoGNendl;
				return false ;
			}

		}

		if (MAP3::isOrbitEmbedded(EDGE))
		{
			if (MAP3::getEmbedding(EDGE, d) != MAP3::getEmbedding(EDGE, MAP3::phi2(d)))
			{
				CGoGNout << "Check: different embeddings on edge" << CGoGNendl ;
				return false ;
			}

			if (MAP3::getEmbedding(EDGE, d) != MAP3::getEmbedding(EDGE, MAP3::phi3(d)))
			{
				CGoGNout << "Check: different embeddings on edge" << CGoGNendl ;
				return false ;
			}
		}

		if (MAP3::isOrbitEmbedded(FACE))
		{
			if (MAP3::getEmbedding(FACE, d) != MAP3::getEmbedding(FACE, MAP3::phi1(d)))
			{
				CGoGNout << "Check: different embeddings on face" << CGoGNendl ;
				return false ;
			}

			if (MAP3::getEmbedding(FACE, d) != MAP3::getEmbedding(FACE, MAP3::phi3(d)))
			{
				CGoGNout << "Check: different embeddings on face" << CGoGNendl ;
				return false ;
			}
		}

		if (MAP3::isOrbitEmbedded(VOLUME))
		{
			if (MAP3::getEmbedding(VOLUME, d) != MAP3::getEmbedding(VOLUME, MAP3::phi1(d)))
			{
				CGoGNout << "Check: different embeddings in volume" << CGoGNendl ;
				return false ;
			}

			if (MAP3::getEmbedding(VOLUME, d) != MAP3::getEmbedding(VOLUME, MAP3::phi2(d)))
			{
				CGoGNout << "Check: different embeddings in volume" << CGoGNendl ;
				return false ;
			}
		}
	}
	CGoGNout << "Check: embedding ok" << CGoGNendl ;
	return true ;
}

} // namespace CGoGN
