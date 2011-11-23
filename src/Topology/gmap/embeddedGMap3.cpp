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

void EmbeddedGMap3::cutEdge(Dart d)
{
	GMap3::cutEdge(d);

	if(isOrbitEmbedded(EDGE))
	{
		Dart nd = phi1(d);

		embedNewCell(EDGE, nd);
		copyCell(EDGE, nd, d);

		unsigned int vEmb = getEmbedding(EDGE, d);
		embedOrbit(EDGE, d, vEmb);
	}

	if(isOrbitEmbedded(FACE))
	{
		Dart f = d;
		do
		{
			Dart nd = phi1(f);
			unsigned int fEmb = getEmbedding(FACE, f);
			embedOrbit(FACE, nd, fEmb);

			Dart f2 = phi2(nd);
			if(f2!=nd)
			{
				Dart nd2 = phi2(f);
				unsigned int fEmb2 = getEmbedding(FACE, f2);
				embedOrbit(FACE, nd2, fEmb2);
			}

			f = alpha2(f);
		} while(f != d);
	}

	if(isOrbitEmbedded(VOLUME))
	{
		Dart f = d;
		do
		{
			Dart nd = phi1(f);
			copyDartEmbedding(VOLUME, nd, f);
			copyDartEmbedding(VOLUME, beta1(nd), f);
			copyDartEmbedding(VOLUME, beta2(nd), f);
			copyDartEmbedding(VOLUME, beta1(beta2(nd)), f);

			f = alpha2(f);
		} while(f != d);
	}
}

void EmbeddedGMap3::sewFaces(Dart d, Dart e)
{
	GMap3::sewFaces(d,e);

	unsigned int vEmb;
	if (isOrbitEmbedded(VERTEX))
	{
		vEmb = getEmbedding(VERTEX, d);
		embedOrbit(VERTEX, d, vEmb) ;

		vEmb = getEmbedding(VERTEX, beta0(d));
		embedOrbit(VERTEX, beta0(d), vEmb) ;
	}

	if (isOrbitEmbedded(EDGE))
	{
		vEmb = getEmbedding(EDGE, d);
		embedOrbit(EDGE, e, vEmb) ;
	}

	if (isOrbitEmbedded(VOLUME))
	{
		vEmb = getEmbedding(VOLUME, d);
		embedOrbit(VOLUME, d, vEmb) ;
	}
}

void EmbeddedGMap3::unsewFaces(Dart d)
{
	Dart e = beta2(d);
	GMap3::unsewFaces(d);

	if (isOrbitEmbedded(VERTEX))
	{
		if(!sameVertex(d,e))
		{
			embedNewCell(VERTEX, e);
			copyCell(VERTEX, e, d);
		}

		d = beta0(d);
		e = beta0(e);

		if(!sameVertex(d,e))
		{
			embedNewCell(VERTEX, e);
			copyCell(VERTEX, e, d);
		}
	}

	if (isOrbitEmbedded(EDGE))
	{
		embedNewCell(EDGE, e);
		copyCell(EDGE, e, d);
	}

	if (isOrbitEmbedded(VOLUME))
	{
		if(!sameVolume(d,e))
		{
			embedNewCell(VOLUME, e);
			copyCell(VOLUME, e, d);
		}
	}
}

void EmbeddedGMap3::splitFace(Dart d, Dart e)
{
	GMap3::splitFace(d,e);

	if(isOrbitEmbedded(VERTEX))
	{
		unsigned int vEmb = getEmbedding(VERTEX, d);
		embedOrbit(VERTEX,phi2(phi_1(d)),vEmb);

		vEmb = getEmbedding(VERTEX, e);
		embedOrbit(VERTEX,phi2(phi_1(e)),vEmb);
	}

	if(isOrbitEmbedded(FACE))
	{
		embedNewCell(FACE, phi2(phi_1(d)));
		copyCell(FACE, phi2(phi_1(d)), d);

		copyDartEmbedding(FACE, phi_1(d),  d);
		copyDartEmbedding(FACE, beta0(phi_1(d)),  beta0(d));

		copyDartEmbedding(FACE, beta3(phi_1(d)),  beta3(d));
		copyDartEmbedding(FACE, beta3(beta0(phi_1(d))),  beta3(beta0(d)));
	}

	if(isOrbitEmbedded(VOLUME))
	{
		copyDartEmbedding(VOLUME, phi_1(d),  d);
		copyDartEmbedding(VOLUME, beta0(phi_1(d)),  beta0(d));
		copyDartEmbedding(VOLUME, phi2(phi_1(d)),  d);
		copyDartEmbedding(VOLUME, beta0(phi2(phi_1(d))),  beta0(d));

		if(phi3(d) != d)
		{
			Dart d3 = phi3(d);

			copyDartEmbedding(VOLUME, phi1(d3), d3);
			copyDartEmbedding(VOLUME, beta0(phi1(d3)), beta0(d3));
			copyDartEmbedding(VOLUME, phi2(phi1(d3)), d3);
			copyDartEmbedding(VOLUME, beta0(phi2(phi1(d3))), beta0(d3));
		}
	}
}

void EmbeddedGMap3::sewVolumes(Dart d, Dart e)
{
	//topological sewing
	GMap3::sewVolumes(d,e);

	unsigned int vEmb = EMBNULL ;

	//embed the vertex orbits from the oriented face with dart e
	//with vertex orbits value from oriented face with dart d
	if (isOrbitEmbedded(VERTEX))
	{
		Dart dd = d ;
		do
		{
			vEmb = getEmbedding(VERTEX, dd);
			embedOrbit(VERTEX, dd, vEmb) ;
			dd = phi1(dd) ;
		} while(dd != d) ;
	}

	//embed the new edge orbit with the old edge orbit value
	//for all the face
	if (isOrbitEmbedded(EDGE))
	{
		Dart dd = d ;
		do
		{
			vEmb = getEmbedding(EDGE, dd);
			embedOrbit(EDGE, dd, vEmb) ;
			dd = phi1(dd) ;
		} while(dd != d) ;
	}

	//embed the face orbit from the volume sewn
	if (isOrbitEmbedded(FACE))
	{
		vEmb = getEmbedding(FACE, d);
		embedOrbit(FACE, e, vEmb) ;
	}
}

void EmbeddedGMap3::unsewVolumes(Dart d)
{
	Dart dd = phi1(phi3(d));

	if(beta3(d)!=d)
	{
		GMap3::unsewVolumes(d);

		Dart ddd = d;
		do
		{
			if(isOrbitEmbedded(VERTEX))
			{
				if(!sameVertex(ddd,dd))
				{
					embedNewCell(VERTEX, ddd);
					copyCell(VERTEX, ddd, dd);
				}
			}

			dd = phi_1(dd);

			if(isOrbitEmbedded(EDGE))
			{
				if(!sameEdge(ddd,dd))
				{
					embedNewCell(EDGE, dd);
					copyCell(EDGE, dd, ddd);
				}
			}

			ddd = phi1(ddd);
		} while(ddd!=d);


		if (isOrbitEmbedded(FACE))
		{
			embedNewCell(FACE, dd);
			copyCell(FACE, dd, d);
		}
	}
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

unsigned int EmbeddedGMap3::closeHole(Dart d)
{
	unsigned int nbE = GMap3::closeHole(d);
	Dart dd = phi2(d);
	Dart f = dd;

	do
	{
		if(isOrbitEmbedded(VERTEX))
		{
			copyDartEmbedding(VERTEX,f, phi1(phi2(f)));
			copyDartEmbedding(VERTEX,beta1(f), beta1(phi1(phi2(f))));
		}
		if(isOrbitEmbedded(EDGE))
		{
			copyDartEmbedding(EDGE, f, phi2(f));
			copyDartEmbedding(EDGE, beta0(f), beta0(phi2(f)));
		}
		if(isOrbitEmbedded(VOLUME))
		{
			copyDartEmbedding(VOLUME, f, phi2(f));
			copyDartEmbedding(VOLUME, beta0(f), beta0(phi2(f)));
		}

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
				copyDartEmbedding(VERTEX, beta0(d), beta0(phi1(phi3(d))));
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

	DartMarker * dvMark=NULL, * deMark=NULL, * dfMark=NULL, * dvolMark=NULL;
	CellMarker * vMark=NULL, * eMark=NULL, * fMark=NULL, * volMark=NULL;

	if (isOrbitEmbedded(VERTEX))
	{
		dvMark = new DartMarker(*this);
		vMark = new CellMarker(*this, VERTEX);
	}

	if (isOrbitEmbedded(EDGE))
	{
		deMark = new DartMarker(*this);
		eMark = new CellMarker(*this, EDGE);
	}

	if (isOrbitEmbedded(FACE))
	{
		dfMark = new DartMarker(*this);
		fMark = new CellMarker(*this, FACE);
	}

	if (isOrbitEmbedded(VOLUME))
	{
		dvolMark = new DartMarker(*this);
		volMark = new CellMarker(*this, VOLUME);
	}

	for(Dart d = begin(); d != end(); next(d))
	{
		if (isOrbitEmbedded(VERTEX))
		{
			if(dvMark->isMarked(d) != vMark->isMarked(d))
				CGoGNout << "Check: warning different orbits share same vertex attribute" << CGoGNendl ;
			vMark->mark(d);
			dvMark->markOrbit(VERTEX,d);

			if (getEmbedding(VERTEX, d) != getEmbedding(VERTEX, beta1(d)))
			{
				CGoGNout << "Check: different embeddings on vertex (beta1)" << CGoGNendl ;
				return false ;
			}

			if (getEmbedding(VERTEX, d) != getEmbedding(VERTEX, beta2(d)))
			{
				CGoGNout << "Check: different embeddings on vertex (beta2)" << CGoGNendl ;
				return false ;
			}

			if (getEmbedding(VERTEX, d) != getEmbedding(VERTEX, beta3(d)))
			{
				CGoGNout << "Check: different embeddings on vertex (beta3)" << CGoGNendl ;
				return false ;
			}
		}

		if (isOrbitEmbedded(EDGE))
		{
			if(deMark->isMarked(d) != eMark->isMarked(d))
				CGoGNout << "Check: warning different orbits share same edge attribute" << CGoGNendl ;
			eMark->mark(d);
			deMark->markOrbit(EDGE,d);

			if (getEmbedding(EDGE, d) != getEmbedding(EDGE, beta0(d)))
			{
				CGoGNout << "Check: different embeddings on edge (beta0)" << CGoGNendl ;
				return false ;
			}

			if (getEmbedding(EDGE, d) != getEmbedding(EDGE, beta2(d)))
			{
				CGoGNout << "Check: different embeddings on edge (beta2)" << CGoGNendl ;
				return false ;
			}

			if (getEmbedding(EDGE, d) != getEmbedding(EDGE, beta3(d)))
			{
				CGoGNout << "Check: different embeddings on edge (beta3)" << CGoGNendl ;
//				return false ;
			}
		}

		if (isOrbitEmbedded(FACE))
		{
			if(dfMark->isMarked(d) != fMark->isMarked(d))
				CGoGNout << "Check: warning different orbits share same face attribute" << CGoGNendl ;
			fMark->mark(d);
			dfMark->markOrbit(FACE,d);

			if (getEmbedding(FACE, d) != getEmbedding(FACE, beta0(d)))
			{
				CGoGNout << "Check: different embeddings on face (beta0)" << CGoGNendl ;
				return false ;
			}

			if (getEmbedding(FACE, d) != getEmbedding(FACE, beta1(d)))
			{
				CGoGNout << "Check: different embeddings on face (beta1)" << CGoGNendl ;
				return false ;
			}
		}

		if (isOrbitEmbedded(VOLUME))
		{
			if(dvolMark->isMarked(d) != volMark->isMarked(d))
				CGoGNout << "Check: warning different orbits share same volume attribute" << CGoGNendl ;
			volMark->mark(d);
			dvolMark->markOrbit(VOLUME,d);

			if (getEmbedding(VOLUME, d) != getEmbedding(VOLUME, beta0(d)))
			{
				CGoGNout << "Check: different embeddings in volume (beta0)" << CGoGNendl ;
				return false ;
			}

			if (getEmbedding(VOLUME, d) != getEmbedding(VOLUME, beta1(d)))
			{
				CGoGNout << "Check: different embeddings in volume (beta1)" << CGoGNendl ;
				return false ;
			}

			if (getEmbedding(VOLUME, d) != getEmbedding(VOLUME, beta2(d)))
			{
				CGoGNout << "Check: different embeddings in volume (beta2)" << CGoGNendl ;
				return false ;
			}
		}
	}

	CGoGNout << "Check: embedding ok" << CGoGNendl ;
	return true ;
}

} // namespace CGoGN
