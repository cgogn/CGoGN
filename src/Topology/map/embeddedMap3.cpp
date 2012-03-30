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

#include "Topology/map/embeddedMap3.h"

namespace CGoGN
{

Dart EmbeddedMap3::deleteVertex(Dart d)
{
	Dart v = Map3::deleteVertex(d) ;
	if(v != NIL)
	{
		if (isOrbitEmbedded(VOLUME))
		{
			embedOrbit(VOLUME, v, getEmbedding(VOLUME, v)) ;
		}
	}
	return v ;
}

Dart EmbeddedMap3::cutEdge(Dart d)
{
	Dart nd = Map3::cutEdge(d);

	if(isOrbitEmbedded(EDGE))
	{
		// embed the new darts created in the cut edge
		embedOrbit(EDGE, d, getEmbedding(EDGE, d)) ;
		// embed a new cell for the new edge and copy the attributes' line (c) Lionel
		embedNewCell(EDGE, nd) ;
		copyCell(EDGE, nd, d) ;
	}

	if(isOrbitEmbedded(FACE2))
	{
		Dart f = d;
		do
		{
			Dart f1 = phi1(f) ;

			copyDartEmbedding(FACE2, f1, f);
			Dart e = phi3(f1);
			copyDartEmbedding(FACE2, phi1(e), e);
			f = alpha2(f);
		} while(f != d);
	}

	if(isOrbitEmbedded(FACE))
	{
		Dart f = d;
		do
		{
			unsigned int fEmb = getEmbedding(FACE, f) ;
			setDartEmbedding(FACE, phi1(f), fEmb);
			setDartEmbedding(FACE, phi3(f), fEmb);
			f = alpha2(f);
		} while(f != d);
	}

	if(isOrbitEmbedded(VOLUME))
	{
		Dart f = d;
		do
		{
			unsigned int vEmb = getEmbedding(VOLUME, f) ;
			setDartEmbedding(VOLUME, phi1(f), vEmb);
			setDartEmbedding(VOLUME, phi2(f), vEmb);
			f = alpha2(f);
		} while(f != d);
	}

	return nd ;
}

bool EmbeddedMap3::uncutEdge(Dart d)
{
	if(Map3::uncutEdge(d))
	{
		//embed all darts from the old two edges to one of the two edge embedding
		if(isOrbitEmbedded(EDGE))
		{
			embedOrbit(EDGE, d, getEmbedding(EDGE, d)) ;
		}
		return true ;
	}
	return false ;
}

Dart EmbeddedMap3::deleteEdge(Dart d)
{
	Dart v = Map3::deleteEdge(d) ;
	if(v != NIL)
	{
		if(isOrbitEmbedded(VOLUME))
		{
			embedOrbit(VOLUME, v, getEmbedding(VOLUME, v)) ;
		}
	}
	return v;
}

bool EmbeddedMap3::edgeCanCollapse(Dart d)
{
	//Criteres sur le bord
	if(isBoundaryEdge(d))
	{
		//fusion de deux bords

		//deconnection du bord
	}

	return false;
}

Dart EmbeddedMap3::collapseEdge(Dart d, bool delDegenerateVolumes)
{
	unsigned int vEmb = getEmbedding(VERTEX, d) ;

	Dart resV = Map3::collapseEdge(d, delDegenerateVolumes);

	if(resV != NIL)
	{
		if(isOrbitEmbedded(VERTEX))
		{
			embedOrbit(VERTEX,resV,vEmb);
		}
	}

	return resV;
}

//bool EmbeddedMap3::collapseDegeneratedFace(Dart d)
//{
//	return Map3::collapseDegeneratedFace(d);
//}

void EmbeddedMap3::splitFace(Dart d, Dart e)
{
	Dart dd = phi1(phi3(d));
	Dart ee = phi1(phi3(e));

	Map3::splitFace(d, e);

	if(isOrbitEmbedded(VERTEX))
	{
		unsigned int vEmb1 = getEmbedding(VERTEX, d) ;
		unsigned int vEmb2 = getEmbedding(VERTEX, e) ;
		setDartEmbedding(VERTEX, phi_1(e), vEmb1);
		setDartEmbedding(VERTEX, phi_1(ee), vEmb1);
		setDartEmbedding(VERTEX, phi_1(d), vEmb2);
		setDartEmbedding(VERTEX, phi_1(dd), vEmb2);
	}

	if(isOrbitEmbedded(FACE2))
	{
		copyDartEmbedding(FACE2, phi_1(d), d) ;
		embedNewCell(FACE2, e) ;
		copyCell(FACE2, e, d) ;

		copyDartEmbedding(FACE2, phi_1(dd), dd) ;
		embedNewCell(FACE2, ee) ;
		copyCell(FACE2, ee, dd) ;
	}

	if(isOrbitEmbedded(FACE))
	{
		unsigned int fEmb = getEmbedding(FACE, d) ;
		setDartEmbedding(FACE, phi_1(d), fEmb) ;
		setDartEmbedding(FACE, phi_1(ee), fEmb) ;
		embedNewCell(FACE, e);
		copyCell(FACE, e, d);
	}

	if(isOrbitEmbedded(VOLUME))
	{
		unsigned int vEmb1 = getEmbedding(VOLUME, d) ;
		setDartEmbedding(VOLUME, phi_1(d),  vEmb1);
		setDartEmbedding(VOLUME, phi_1(e),  vEmb1);

		unsigned int vEmb2 = getEmbedding(VOLUME, dd) ;
		setDartEmbedding(VOLUME, phi_1(dd),  vEmb2);
		setDartEmbedding(VOLUME, phi_1(ee),  vEmb2);
	}
}

//!
/*!
 *
 */
Dart EmbeddedMap3::collapseFace(Dart d, bool delDegenerateVolumes)
{
	unsigned int vEmb = getEmbedding(VERTEX, d) ;

	Dart resV = Map3::collapseFace(d, delDegenerateVolumes);

	if(resV != NIL)
	{
		if(isOrbitEmbedded(VERTEX))
		{
			embedOrbit(VERTEX,resV,vEmb);
		}
	}

	return resV;
}

void EmbeddedMap3::sewVolumes(Dart d, Dart e, bool withBoundary)
{
	if (!withBoundary)
	{
		Map3::sewVolumes(d, e, false) ;
		return ;
	}

	Map3::sewVolumes(d, e, withBoundary);

	// embed the vertex orbits from the oriented face with dart e
	// with vertex orbits value from oriented face with dart d
	if (isOrbitEmbedded(VERTEX))
	{
		Dart it = d ;
		do
		{
			embedOrbit(VERTEX, it, getEmbedding(VERTEX, it)) ;
			it = phi1(it) ;
		} while(it != d) ;
	}

	// embed the new edge orbit with the old edge orbit value
	// for all the face
	if (isOrbitEmbedded(EDGE))
	{
		Dart it = d ;
		do
		{
			embedOrbit(EDGE, it, getEmbedding(EDGE, it)) ;
			it = phi1(it) ;
		} while(it != d) ;
	}

	// embed the face orbit from the volume sewn
	if (isOrbitEmbedded(FACE))
	{
		embedOrbit(FACE, e, getEmbedding(FACE, d)) ;
	}
}

void EmbeddedMap3::unsewVolumes(Dart d)
{
	Dart dd = alpha1(d);

	unsigned int fEmb = EMBNULL ;
	if(isOrbitEmbedded(FACE))
		fEmb = getEmbedding(FACE, d) ;

	Map3::unsewVolumes(d);

	Dart dit = d;
	do
	{
		// embed the unsewn vertex orbit with the vertex embedding if it is deconnected
		if(isOrbitEmbedded(VERTEX))
		{
			if(!sameVertex(dit, dd))
			{
				embedOrbit(VERTEX, dit, getEmbedding(VERTEX, dit)) ;
				embedNewCell(VERTEX, dd);
				copyCell(VERTEX, dd, dit);
			}
			else
			{
				embedOrbit(VERTEX, dit, getEmbedding(VERTEX, dit)) ;
			}
		}

		dd = phi_1(dd);

		// embed the unsewn edge with the edge embedding if it is deconnected
		if(isOrbitEmbedded(EDGE))
		{
			if(!sameEdge(dit, dd))
			{
				embedNewCell(EDGE, dd);
				copyCell(EDGE, dd, dit);
				copyDartEmbedding(EDGE, phi3(dit), dit) ;
			}
			else
			{
				unsigned int eEmb = getEmbedding(EDGE, dit) ;
				setDartEmbedding(EDGE, phi3(dit), eEmb) ;
				setDartEmbedding(EDGE, alpha_2(dit), eEmb) ;
			}
		}

		if(isOrbitEmbedded(FACE))
		{
			setDartEmbedding(FACE, phi3(dit), fEmb) ;
		}

		dit = phi1(dit);
	} while(dit != d);

	// embed the unsewn face with the face embedding
	if (isOrbitEmbedded(FACE))
	{
		embedNewCell(FACE, dd);
		copyCell(FACE, dd, d);
	}
}

bool EmbeddedMap3::mergeVolumes(Dart d)
{
	Dart d2 = phi2(d);

	if(Map3::mergeVolumes(d))
	{
		if (isOrbitEmbedded(VOLUME))
		{
			embedOrbit(VOLUME, d2, getEmbedding(VOLUME, d2)) ;
		}
		return true;
	}
	return false;
}

void EmbeddedMap3::splitVolume(std::vector<Dart>& vd)
{
	Map3::splitVolume(vd);

	// follow the edge path a second time to embed the vertex, edge and volume orbits
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
	{
		Dart dit = *it;
		Dart dit23 = alpha2(dit);

		// embed the vertex embedded from the origin volume to the new darts
		if(isOrbitEmbedded(VERTEX))
		{
			copyDartEmbedding(VERTEX, dit23, dit);
			copyDartEmbedding(VERTEX, phi2(dit), phi1(dit));
		}

		// embed the edge embedded from the origin volume to the new darts
		if(isOrbitEmbedded(EDGE))
		{
			unsigned int eEmb = getEmbedding(EDGE, dit) ;
			setDartEmbedding(EDGE, dit23, eEmb);
			setDartEmbedding(EDGE, phi2(dit), eEmb);
		}

		// embed the volume embedded from the origin volume to the new darts
		if(isOrbitEmbedded(VOLUME))
		{
			copyDartEmbedding(VOLUME, phi2(dit), dit);
		}
	}

	if(isOrbitEmbedded(VOLUME))
	{
		Dart v = vd.front() ;
		Dart v23 = alpha2(v) ;
		embedNewCell(VOLUME, v23) ;
		copyCell(VOLUME, v23, v) ;
	}
}

Dart EmbeddedMap3::collapseVolume(Dart d, bool delDegenerateVolumes)
{
	unsigned int vEmb = getEmbedding(VERTEX, d) ;

	Dart resV = Map3::collapseVolume(d, delDegenerateVolumes);

	if(resV != NIL)
	{
		if(isOrbitEmbedded(VERTEX))
		{
			embedOrbit(VERTEX,resV,vEmb);
		}
	}

	return resV;
}

unsigned int EmbeddedMap3::closeHole(Dart d, bool forboundary)
{
	unsigned int nbF = Map3::closeHole(d, forboundary) ;

	DartMarkerStore mark(*this);	// Lock a marker

	std::vector<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.reserve(1024) ;
	visitedFaces.push_back(phi3(d));// Start with the face of d
	mark.markOrbit(FACE2, phi3(d)) ;

	// For every face added to the list
	for(unsigned int i = 0; i < visitedFaces.size(); ++i)
	{
		Dart it = visitedFaces[i] ;
		Dart f = it ;
		do
		{
			if(isOrbitEmbedded(VERTEX))
			{
				copyDartEmbedding(VERTEX, f, alpha1(f)) ;
			}
			if(isOrbitEmbedded(EDGE))
			{
				copyDartEmbedding(EDGE, f, phi3(f)) ;
			}
			if(isOrbitEmbedded(FACE))
			{
				copyDartEmbedding(FACE, f, phi3(f)) ;
			}

			Dart adj = phi2(f);	// Get adjacent face
			if (!mark.isMarked(adj))
			{
				visitedFaces.push_back(adj);	// Add it
				mark.markOrbit(FACE2, adj) ;
			}

			f = phi1(f) ;
		} while(f != it) ;
	}

	return nbF ;
}

bool EmbeddedMap3::check()
{
	bool topo = Map3::check() ;
	if (!topo)
		return false ;

	std::cout << "Check: embedding begin" << std::endl ;

    std::cout << "nb vertex orbits : " << getNbOrbits(VERTEX) << std::endl ;
    std::cout << "nb vertex cells : " << m_attribs[VERTEX].size() << std::endl ;

    std::cout << "nb edge orbits : " << getNbOrbits(EDGE) << std::endl ;
    std::cout << "nb edge cells : " << m_attribs[EDGE].size() << std::endl ;

    std::cout << "nb face orbits : " << getNbOrbits(FACE) << std::endl ;
    std::cout << "nb face cells : " << m_attribs[FACE].size() << std::endl ;

    std::cout << "nb volume orbits : " << getNbOrbits(VOLUME) << std::endl ;
    std::cout << "nb volume cells : " << m_attribs[VOLUME].size() << std::endl ;


	for(Dart d = begin(); d != end(); next(d))
	{
		if(isOrbitEmbedded(VERTEX))
		{
			if( getEmbedding(VERTEX, d) != getEmbedding(VERTEX, alpha1(d)))
			{
				std::cout << "Embedding Check : different embeddings on vertex (alpha1(d) != d)" << std::endl ;
				return false ;
			}
			if(getEmbedding(VERTEX, d) != getEmbedding(VERTEX, alpha2(d)) )
			{
				std::cout << "Embedding Check : different embeddings on vertex (alpha2(d) != d)" << std::endl ;
				return false ;
			}
		}

		if(isOrbitEmbedded(EDGE))
		{
			if( getEmbedding(EDGE, d) != getEmbedding(EDGE, phi2(d)) ||
					getEmbedding(EDGE, d) != getEmbedding(EDGE, phi3(d)) )
			{
				std::cout << "Embedding Check : different embeddings on edge" << std::endl ;
				return false ;
			}
		}

		if (isOrbitEmbedded(FACE2))
		{
			if (getEmbedding(FACE2, d) != getEmbedding(FACE2, phi1(d)))
			{
				CGoGNout << "Check: different embeddings on oriented face" << CGoGNendl ;
				return false ;
			}
		}

		if (isOrbitEmbedded(FACE))
		{
			if( getEmbedding(FACE, d) != getEmbedding(FACE, phi1(d)) ||
					getEmbedding(FACE, d) != getEmbedding(FACE, phi3(d)) )
			{
				CGoGNout << "Check: different embeddings on face" << CGoGNendl ;
				return false ;
			}
		}

		if (isOrbitEmbedded(VOLUME))
		{
			if( getEmbedding(VOLUME, d) != getEmbedding(VOLUME, phi1(d)) ||
					getEmbedding(VOLUME, d) != getEmbedding(VOLUME, phi2(d)) )
			{
				CGoGNout << "Check: different embeddings on volume" << CGoGNendl ;
				return false ;
			}
		}
	}

	std::cout << "Check: embedding ok" << std::endl ;

	return true ;
}

} // namespace CGoGN
