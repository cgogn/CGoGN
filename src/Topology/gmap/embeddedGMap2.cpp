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

#include "Topology/gmap/embeddedGMap2.h"

namespace CGoGN
{

void EmbeddedGMap2::splitVertex(Dart d, Dart e)
{
	Dart dd = phi2(d) ;
	Dart ee = phi2(e) ;

	GMap2::splitVertex(d, e) ;

	if (isOrbitEmbedded(VERTEX))
	{
		copyDartEmbedding(VERTEX, phi1(dd), d) ;
		copyDartEmbedding(VERTEX, beta2(phi1(dd)), beta2(d)) ;
		copyDartEmbedding(VERTEX, phi1(ee), e) ;
		copyDartEmbedding(VERTEX, beta2(phi1(ee)), beta2(e)) ;
		embedNewCell(VERTEX, e) ;
		copyCell(VERTEX, e, d) ;
	}

	if(isOrbitEmbedded(FACE))
	{
		copyDartEmbedding(FACE, phi1(dd), dd) ;
		copyDartEmbedding(FACE, beta0(phi1(dd)), beta0(dd)) ;
		copyDartEmbedding(FACE, phi1(ee), ee) ;
		copyDartEmbedding(FACE, beta0(phi1(ee)), beta0(ee)) ;
	}
}

bool EmbeddedGMap2::deleteVertex(Dart d)
{
	Dart f = phi1(d) ;
	if(GMap2::deleteVertex(d))
	{
		if (isOrbitEmbedded(FACE))
		{
			embedOrbit(FACE, f, getEmbedding(FACE, f)) ;
		}
		return true ;
	}
	return false ;
}

void EmbeddedGMap2::linkVertices(Dart d, Dart e)
{
	Dart dNext = phi1(d) ;

	GMap2::linkVertices(d,e);

	if (isOrbitEmbedded(VERTEX))
	{
		copyDartEmbedding(VERTEX, phi_1(e), d) ;
		copyDartEmbedding(VERTEX, beta2(phi_1(e)), d) ;
		copyDartEmbedding(VERTEX, phi_1(d), e) ;
		copyDartEmbedding(VERTEX, beta2(phi_1(d)), e) ;
	}

	if (isOrbitEmbedded(FACE))
	{
		embedOrbit(FACE, dNext, getEmbedding(FACE, dNext)) ;
	}
}

void EmbeddedGMap2::cutEdge(Dart d)
{
	GMap2::cutEdge(d) ;

	Dart nd = phi1(d) ;

	if (isOrbitEmbedded(EDGE))
	{
		embedNewCell(EDGE, nd) ;
		copyCell(EDGE, nd, d) ;
	}

	if(isOrbitEmbedded(FACE))
	{
		copyDartEmbedding(FACE, phi1(d), d) ;
		copyDartEmbedding(FACE, beta0(phi1(d)), beta0(d)) ;
		Dart e = phi2(nd) ;
		if(e != nd)
		{
			copyDartEmbedding(FACE, phi1(e), e) ;
			copyDartEmbedding(FACE, beta0(phi1(e)), beta0(e)) ;
		}
	}
}

void EmbeddedGMap2::uncutEdge(Dart d)
{
	bool doSomethg = (d != phi2(d)) ;

	GMap2::uncutEdge(d) ;

	if(doSomethg)
		if(isOrbitEmbedded(EDGE))
			embedOrbit(EDGE, d, getEmbedding(EDGE, d)) ;
}

bool EmbeddedGMap2::edgeCanCollapse(Dart d)
{
	if(phi2(d) == d)
		return false ;

	if(isBoundaryVertex(d) || isBoundaryVertex(phi1(d)))
		return false ;

	unsigned int val_v1 = vertexDegree(d) ;
	unsigned int val_v2 = vertexDegree(phi1(d)) ;

	if(val_v1 + val_v2 < 8 || val_v1 + val_v2 > 14)
		return false ;

	if(isFaceTriangle(d))
	{
		if(vertexDegree(phi_1(d)) < 4)
			return false ;
	}

	Dart dd = phi2(d) ;
	if(isFaceTriangle(dd))
	{
		if(vertexDegree(phi_1(dd)) < 4)
			return false ;
	}

	// Check vertex sharing condition
	std::vector<unsigned int> vu1 ;
	vu1.reserve(32) ;
	Dart vit1 = alpha1(alpha1(d)) ;
	Dart end = phi1(dd) ;
	do
	{
		unsigned int ve = getEmbedding(VERTEX, phi2(vit1)) ;
		vu1.push_back(ve) ;
		vit1 = alpha1(vit1) ;
	} while(vit1 != end) ;
	end = phi1(d) ;
	Dart vit2 = alpha1(alpha1(dd)) ;
	do
	{
		unsigned int ve = getEmbedding(VERTEX, phi2(vit2)) ;
		std::vector<unsigned int>::iterator it = std::find(vu1.begin(), vu1.end(), ve) ;
		if(it != vu1.end())
			return false ;
		vit2 = alpha1(vit2) ;
	} while(vit2 != end) ;

	return true ;
}

Dart EmbeddedGMap2::collapseEdge(Dart d, bool delDegenerateFaces)
{
	unsigned int vEmb = EMBNULL ;
	if (isOrbitEmbedded(VERTEX))
	{
		vEmb = getEmbedding(VERTEX, d) ;
	}

	Dart dV = GMap2::collapseEdge(d, delDegenerateFaces);

	if (isOrbitEmbedded(VERTEX))
	{
		embedOrbit(VERTEX, dV, vEmb) ;
	}

	return dV ;
}

bool EmbeddedGMap2::flipEdge(Dart d)
{
	if(GMap2::flipEdge(d))
	{
		Dart e = phi2(d) ;

		if (isOrbitEmbedded(VERTEX))
		{
			copyDartEmbedding(VERTEX, d, phi1(e)) ;
			copyDartEmbedding(VERTEX, beta2(d), beta2(phi1(e))) ;
			copyDartEmbedding(VERTEX, e, phi1(d)) ;
			copyDartEmbedding(VERTEX, beta2(e), beta2(phi1(d))) ;
		}
		if (isOrbitEmbedded(FACE))
		{
			copyDartEmbedding(FACE, phi_1(d), d) ;
			copyDartEmbedding(FACE, beta0(phi_1(d)), beta0(d)) ;
			copyDartEmbedding(FACE, phi_1(e), e) ;
			copyDartEmbedding(FACE, beta0(phi_1(e)), beta0(e)) ;
		}
		return true ;
	}
	return false ;
}

bool EmbeddedGMap2::flipBackEdge(Dart d)
{
	if(GMap2::flipBackEdge(d))
	{
		Dart e = phi2(d) ;

		if (isOrbitEmbedded(VERTEX))
		{
			copyDartEmbedding(VERTEX, d, phi1(e)) ;
			copyDartEmbedding(VERTEX, beta2(d), beta2(phi1(e))) ;
			copyDartEmbedding(VERTEX, e, phi1(d)) ;
			copyDartEmbedding(VERTEX, beta2(e), beta2(phi1(d))) ;
		}

		if (isOrbitEmbedded(FACE))
		{
			copyDartEmbedding(FACE, phi1(d), d) ;
			copyDartEmbedding(FACE, beta0(phi1(d)), beta0(d)) ;
			copyDartEmbedding(FACE, phi1(e), e) ;
			copyDartEmbedding(FACE, beta0(phi1(e)), beta0(e)) ;
		}
		return true ;
	}
	return false ;
}

void EmbeddedGMap2::insertEdgeInVertex(Dart d, Dart e)
{
	GMap2::insertEdgeInVertex(d, e);

	if (isOrbitEmbedded(VERTEX))
	{
		copyDartEmbedding(VERTEX, e, d) ;
		copyDartEmbedding(VERTEX, beta2(e), d) ;
	}

	if (isOrbitEmbedded(FACE))
	{
		if(!sameFace(d,e))
		{
			embedNewCell(FACE, e);
			copyCell(FACE, e, d) ;
		}
		else
		{
			embedOrbit(FACE, d, getEmbedding(FACE, d)) ;
		}
	}
}

void EmbeddedGMap2::removeEdgeFromVertex(Dart d)
{
	Dart dPrev = alpha_1(d);

	GMap2::removeEdgeFromVertex(d);

	if (isOrbitEmbedded(VERTEX))
	{
		embedNewCell(VERTEX, d);
		copyCell(VERTEX, d, dPrev);
	}

	if (isOrbitEmbedded(FACE))
	{
		if(!sameFace(d, dPrev))
		{
			embedNewCell(FACE, d);
			copyCell(FACE, d, dPrev) ;
		}
		else
		{
			embedOrbit(FACE, d, getEmbedding(FACE, d)) ;
		}
	}
}

void EmbeddedGMap2::sewFaces(Dart d, Dart e)
{
	unsigned int vEmb1 = EMBNULL ;
	unsigned int vEmb2 = EMBNULL ;
	if (isOrbitEmbedded(VERTEX))
	{
		vEmb1 = getEmbedding(VERTEX, d) ;
		vEmb2 = getEmbedding(VERTEX, phi1(d)) ;
	}

	GMap2::sewFaces(d, e) ;

	if (isOrbitEmbedded(VERTEX))
	{
		embedOrbit(VERTEX, d, vEmb1) ;
		embedOrbit(VERTEX, e, vEmb2) ;
	}

	if (isOrbitEmbedded(EDGE))
	{
		vEmb1 = getEmbedding(EDGE, d) ;
		embedOrbit(EDGE, e, vEmb1) ;
	}
}

void EmbeddedGMap2::unsewFaces(Dart d)
{
	Dart e = beta2(d);
	GMap2::unsewFaces(d);

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
}

bool EmbeddedGMap2::collapseDegeneratedFace(Dart d)
{
	Dart e = phi2(d) ;

	if(GMap2::collapseDegeneratedFace(d))
	{
		if (isOrbitEmbedded(EDGE))
		{
			copyDartEmbedding(EDGE, phi2(e), e) ;
			copyDartEmbedding(EDGE, beta0(phi2(e)), beta0(e)) ;
		}
		return true ;
	}
	return false ;
}

void EmbeddedGMap2::splitFace(Dart d, Dart e)
{
	GMap2::splitFace(d, e) ;

	if (isOrbitEmbedded(VERTEX))
	{
		copyDartEmbedding(VERTEX, phi_1(e), d) ;
		copyDartEmbedding(VERTEX, beta2(phi_1(e)), beta2(d)) ;
		copyDartEmbedding(VERTEX, phi_1(d), e) ;
		copyDartEmbedding(VERTEX, beta2(phi_1(d)), beta2(e)) ;
	}
	if (isOrbitEmbedded(FACE))
	{
		embedNewCell(FACE, e) ;
		copyCell(FACE, e, d) ;
	}
}

bool EmbeddedGMap2::mergeFaces(Dart d)
{
	Dart dNext = phi1(d) ;

	if(GMap2::mergeFaces(d))
	{
		if (isOrbitEmbedded(FACE))
		{
			embedOrbit(FACE, dNext, getEmbedding(FACE, dNext)) ;
		}
		return true ;
	}
	return false ;
}

bool EmbeddedGMap2::mergeVolumes(Dart d, Dart e)
{
	std::vector<Dart> darts ;
	std::vector<unsigned int> vEmb ;
	std::vector<unsigned int> eEmb ;
	Dart fit = d ;
	do
	{
		darts.push_back(phi2(fit)) ;

		if (isOrbitEmbedded(VERTEX))
		{
			vEmb.push_back(getEmbedding(VERTEX, phi2(fit))) ;
		}

		if (isOrbitEmbedded(EDGE))
		{
			eEmb.push_back(getEmbedding(EDGE, fit)) ;
		}

		fit = phi1(fit) ;
	} while (fit != d) ;

	if(GMap2::mergeVolumes(d, e))
	{
		for(unsigned int i = 0; i < darts.size(); ++i)
		{
			if (isOrbitEmbedded(VERTEX))
			{
				embedOrbit(VERTEX, darts[i], vEmb[i]) ;
			}

			if (isOrbitEmbedded(EDGE))
			{
				embedOrbit(EDGE, darts[i], eEmb[i]) ;
			}
		}
		return true ;
	}
	return false ;
}

unsigned int EmbeddedGMap2::closeHole(Dart d)
{
	unsigned int nbE = GMap2::closeHole(d) ;
	Dart dd = phi2(d) ;
	Dart f = dd ;
	do
	{
		if (isOrbitEmbedded(VERTEX))
		{
			copyDartEmbedding(VERTEX, f, alpha1(f)) ;
			copyDartEmbedding(VERTEX, beta2(f), beta2(alpha1(f))) ;
		}
		if (isOrbitEmbedded(EDGE))
		{
			copyDartEmbedding(EDGE, f, phi2(f)) ;
			copyDartEmbedding(EDGE, beta0(f), beta0(phi2(f))) ;
		}
		f = phi1(f) ;
	} while(dd != f) ;
	return nbE ;
}

bool EmbeddedGMap2::check()
{
	bool topo = GMap2::check() ;
	if (!topo)
		return false ;

	CGoGNout << "Check: embedding begin" << CGoGNendl ;
	for(Dart d = begin(); d != end(); next(d))
	{
		if (isOrbitEmbedded(VERTEX))
		{
			if (getEmbedding(VERTEX, d) != getEmbedding(VERTEX, alpha1(d)))
			{
				CGoGNout << "Check: different embeddings on vertex" << CGoGNendl ;
				return false ;
			}
		}

		if (isOrbitEmbedded(EDGE))
		{
			if (getEmbedding(EDGE, d) != getEmbedding(EDGE, phi2(d)))
			{
				CGoGNout << "Check: different embeddings on edge" << CGoGNendl ;
				return false ;
			}
		}

		if (isOrbitEmbedded(FACE))
		{
			if (getEmbedding(FACE, d) != getEmbedding(FACE, phi1(d)))
		{
				CGoGNout << "Check: different embeddings on face" << CGoGNendl ;
				return false ;
			}
		}
	}
	CGoGNout << "Check: embedding ok" << CGoGNendl ;
	return true ;
}

} // namespace CGoGN
