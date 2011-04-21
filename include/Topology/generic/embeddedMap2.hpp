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

template <typename MAP2>
void EmbeddedMap2<MAP2>::deleteFace(Dart d)
{
	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		Dart dd = d ;
		do
		{
			unsigned int vEmb = MAP2::getDartEmbedding(VERTEX_ORBIT, dd) ;
			if(vEmb != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, MAP2::alpha1(d), vEmb) ;
			dd = MAP2::phi1(dd) ;
		} while(dd != d) ;
	}

	if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
	{
		Dart dd = d ;
		do
		{
			unsigned int eEmb = MAP2::getDartEmbedding(EDGE_ORBIT, dd) ;
			if(eEmb != EMBNULL)
				MAP2::setDartEmbedding(EDGE_ORBIT, MAP2::phi2(d), eEmb) ;
			dd = MAP2::phi1(dd) ;
		} while(dd != d) ;
	}

	MAP2::deleteFace(d) ;
}

template <typename MAP2>
void EmbeddedMap2<MAP2>::splitVertex(Dart d, Dart e)
{
	unsigned int vEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		vEmb = MAP2::getEmbedding(d, VERTEX_ORBIT) ;
	}

	MAP2::splitVertex(d, e) ;

	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		MAP2::setDartEmbedding(VERTEX_ORBIT, d, vEmb) ;
		MAP2::embedNewCell(VERTEX_ORBIT, e) ;
		MAP2::copyCell(VERTEX_ORBIT, e, d) ;
	}
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::deleteVertex(Dart d)
{
	if(MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		Dart vit = d ;
		do
		{
			unsigned int vEmb = MAP2::getDartEmbedding(VERTEX_ORBIT, MAP2::phi2(vit)) ;
			if(vEmb != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, MAP2::phi1(vit), vEmb) ;
			vit = MAP2::alpha1(vit) ;
		} while(vit != d) ;
	}

	Dart f = MAP2::phi1(d) ;
	unsigned int fEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(FACE_ORBIT))
	{
		fEmb = MAP2::getEmbedding(d, FACE_ORBIT) ;
	}

	if(MAP2::deleteVertex(d))
	{
		if (MAP2::isOrbitEmbedded(FACE_ORBIT))
		{
			MAP2::embedOrbit(FACE_ORBIT, f, fEmb) ;
		}
		return true ;
	}
	return false ;
}

template <typename MAP2>
void EmbeddedMap2<MAP2>::cutEdge(Dart d)
{
	unsigned int eEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
	{
		eEmb = MAP2::getEmbedding(d, EDGE_ORBIT) ;
	}

	MAP2::cutEdge(d) ;

	if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
	{
		Dart e = MAP2::phi1(d) ;
		MAP2::setDartEmbedding(EDGE_ORBIT, d, eEmb) ;
		MAP2::embedNewCell(EDGE_ORBIT, e) ;
		MAP2::copyCell(EDGE_ORBIT, e, d) ;
	}
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::edgeCanCollapse(Dart d)
{
	if(MAP2::phi2(d) == d)
		return false ;

	if(MAP2::isBoundaryVertex(d) || MAP2::isBoundaryVertex(MAP2::phi1(d)))
		return false ;

	unsigned int val_v1 = MAP2::vertexDegree(d) ;
	unsigned int val_v2 = MAP2::vertexDegree(MAP2::phi1(d)) ;

	if(val_v1 + val_v2 < 8 || val_v1 + val_v2 > 14)
		return false ;

	if(MAP2::isFaceTriangle(d))
	{
		if(MAP2::vertexDegree(MAP2::phi_1(d)) < 4)
			return false ;
	}

	Dart dd = MAP2::phi2(d) ;
	if(MAP2::isFaceTriangle(dd))
	{
		if(MAP2::vertexDegree(MAP2::phi_1(dd)) < 4)
			return false ;
	}

	// Check vertex sharing condition
	std::vector<unsigned int> vu1 ;
	vu1.reserve(32) ;
	Dart vit1 = MAP2::alpha1(MAP2::alpha1(d)) ;
	Dart end = MAP2::phi1(dd) ;
	do
	{
		unsigned int ve = MAP2::getEmbedding(MAP2::phi2(vit1), VERTEX_ORBIT) ;
		vu1.push_back(ve) ;
		vit1 = MAP2::alpha1(vit1) ;
	} while(vit1 != end) ;
	end = MAP2::phi1(d) ;
	Dart vit2 = MAP2::alpha1(MAP2::alpha1(dd)) ;
	do
	{
		unsigned int ve = MAP2::getEmbedding(MAP2::phi2(vit2), VERTEX_ORBIT) ;
		std::vector<unsigned int>::iterator it = std::find(vu1.begin(), vu1.end(), ve) ;
		if(it != vu1.end())
			return false ;
		vit2 = MAP2::alpha1(vit2) ;
	} while(vit2 != end) ;

	return true ;
}

template <typename MAP2>
Dart EmbeddedMap2<MAP2>::collapseEdge(Dart d, bool delDegenerateFaces)
{
	unsigned int vEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		vEmb = MAP2::getEmbedding(d, VERTEX_ORBIT) ;
	}

	Dart dV = MAP2::collapseEdge(d, delDegenerateFaces);

	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		MAP2::embedOrbit(VERTEX_ORBIT, dV, vEmb) ;
	}

	return dV ;
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::flipEdge(Dart d)
{
	if(MAP2::flipEdge(d))
	{
		Dart e = MAP2::phi2(d) ;

		if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
		{
			unsigned int dVEmb = MAP2::getDartEmbedding(VERTEX_ORBIT, d) ;
			if(dVEmb != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, MAP2::phi_1(d), dVEmb) ;
			MAP2::setDartEmbedding(VERTEX_ORBIT, d, EMBNULL) ;

			unsigned int eVEmb = MAP2::getDartEmbedding(VERTEX_ORBIT, e) ;
			if(eVEmb != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, MAP2::phi_1(e), eVEmb) ;
			MAP2::setDartEmbedding(VERTEX_ORBIT, e, EMBNULL) ;
		}
		if (MAP2::isOrbitEmbedded(FACE_ORBIT))
		{
			unsigned int dFEmb = MAP2::getDartEmbedding(FACE_ORBIT, MAP2::phi_1(e)) ;
			if(dFEmb != EMBNULL)
				MAP2::setDartEmbedding(FACE_ORBIT, d, dFEmb) ;
			MAP2::setDartEmbedding(FACE_ORBIT, MAP2::phi_1(e), EMBNULL) ;

			unsigned int eFEmb = MAP2::getDartEmbedding(FACE_ORBIT, MAP2::phi_1(d)) ;
			if(eFEmb != EMBNULL)
				MAP2::setDartEmbedding(FACE_ORBIT, e, eFEmb) ;
			MAP2::setDartEmbedding(FACE_ORBIT, MAP2::phi_1(d), EMBNULL) ;
		}
		return true ;
	}
	return false ;
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::flipBackEdge(Dart d)
{
	if(MAP2::flipBackEdge(d))
	{
		Dart e = MAP2::phi2(d) ;

		if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
		{
			unsigned int dVEmb = MAP2::getDartEmbedding(VERTEX_ORBIT, d) ;
			if(dVEmb != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, MAP2::template phi<11>(e), dVEmb) ;
			MAP2::setDartEmbedding(VERTEX_ORBIT, d, EMBNULL) ;

			unsigned int eVEmb = MAP2::getDartEmbedding(VERTEX_ORBIT, e) ;
			if(eVEmb != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, MAP2::template phi<11>(d), eVEmb) ;
			MAP2::setDartEmbedding(VERTEX_ORBIT, e, EMBNULL) ;
		}

		if (MAP2::isOrbitEmbedded(FACE_ORBIT))
		{
			unsigned int dFEmb = MAP2::getDartEmbedding(FACE_ORBIT, MAP2::phi1(e)) ;
			if(dFEmb != EMBNULL)
				MAP2::setDartEmbedding(FACE_ORBIT, d, dFEmb) ;
			MAP2::setDartEmbedding(FACE_ORBIT, MAP2::phi1(e), EMBNULL) ;

			unsigned int eFEmb = MAP2::getDartEmbedding(FACE_ORBIT, MAP2::phi1(d)) ;
			if(eFEmb != EMBNULL)
				MAP2::setDartEmbedding(FACE_ORBIT, e, eFEmb) ;
			MAP2::setDartEmbedding(FACE_ORBIT, MAP2::phi1(d), EMBNULL) ;
		}
		return true ;
	}
	return false ;
}

template <typename MAP2>
void EmbeddedMap2<MAP2>::sewFaces(Dart d, Dart e)
{
	unsigned int vEmb1 = EMBNULL ;
	unsigned int vEmb2 = EMBNULL ;
	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		vEmb1 = MAP2::getEmbedding(d, VERTEX_ORBIT) ;
		vEmb2 = MAP2::getEmbedding(MAP2::phi1(d), VERTEX_ORBIT) ;
	}

	MAP2::sewFaces(d, e) ;

	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		MAP2::embedOrbit(VERTEX_ORBIT, d, vEmb1) ;
		MAP2::embedOrbit(VERTEX_ORBIT, e, vEmb2) ;
	}

	if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
	{
		MAP2::copyDartEmbedding(EDGE_ORBIT, e, d) ;
	}
}

template <typename MAP2>
void EmbeddedMap2<MAP2>::unsewFaces(Dart d)
{
	unsigned int vEmb1 = EMBNULL ;
	unsigned int vEmb2 = EMBNULL ;
	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		if(MAP2::isBoundaryVertex(d))
			vEmb1 = MAP2::getEmbedding(d, VERTEX_ORBIT) ;
		Dart dd = MAP2::phi1(d) ;
		if(MAP2::isBoundaryVertex(dd))
			vEmb2 = MAP2::getEmbedding(dd, VERTEX_ORBIT) ;
	}
	unsigned int eEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
	{
		eEmb = MAP2::getEmbedding(d, EDGE_ORBIT) ;
	}

	Dart e = MAP2::phi2(d) ;
	MAP2::unsewFaces(d) ;

	if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		if(vEmb1 != EMBNULL)
		{
			MAP2::setDartEmbedding(VERTEX_ORBIT, d, vEmb1) ;
			if(e != d)
			{
				MAP2::embedNewCell(VERTEX_ORBIT, MAP2::phi1(e)) ;
				MAP2::copyCell(VERTEX_ORBIT, MAP2::phi1(e), d) ;
			}
		}

		if(vEmb2 != EMBNULL)
		{
			MAP2::setDartEmbedding(VERTEX_ORBIT, MAP2::phi1(d), vEmb2) ;
			if(e != d)
			{
				MAP2::embedNewCell(VERTEX_ORBIT, e) ;
				MAP2::copyCell(VERTEX_ORBIT, e, MAP2::phi1(d)) ;
			}
		}
	}
	if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
	{
		MAP2::setDartEmbedding(EDGE_ORBIT, d, eEmb) ;
		MAP2::embedNewCell(EDGE_ORBIT, e) ;
		MAP2::copyCell(EDGE_ORBIT, e, d) ;
	}
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::collapseDegeneratedFace(Dart d)
{
	Dart e = MAP2::phi2(d) ;

	unsigned int eEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
	{
		eEmb = MAP2::getEmbedding(e, EDGE_ORBIT) ;
	}

	if(MAP2::collapseDegeneratedFace(d))
	{
		if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
		{
			MAP2::embedOrbit(EDGE_ORBIT, e, eEmb) ;
		}
		return true ;
	}
	return false ;
}

template <typename MAP2>
void EmbeddedMap2<MAP2>::splitFace(Dart d, Dart e)
{
	unsigned int fEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(FACE_ORBIT))
	{
		fEmb = MAP2::getEmbedding(d, FACE_ORBIT) ;
	}

	MAP2::splitFace(d, e) ;

	if (MAP2::isOrbitEmbedded(FACE_ORBIT))
	{
		MAP2::setDartEmbedding(FACE_ORBIT, d, fEmb) ;
		MAP2::embedNewCell(FACE_ORBIT, e) ;
		MAP2::copyCell(FACE_ORBIT, e, d) ;
	}
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::mergeFaces(Dart d)
{
	Dart dNext = MAP2::phi1(d) ;
	Dart e = MAP2::phi2(d) ;
	Dart eNext = MAP2::phi1(e) ;

	unsigned int vEmb1 = EMBNULL ;
	unsigned int vEmb2 = EMBNULL ;
	if(MAP2::isOrbitEmbedded(VERTEX_ORBIT))
	{
		vEmb1 = MAP2::getDartEmbedding(VERTEX_ORBIT, d) ;
		vEmb2 = MAP2::getDartEmbedding(VERTEX_ORBIT, e) ;
	}
	unsigned int fEmb = EMBNULL ;
	if (MAP2::isOrbitEmbedded(FACE_ORBIT))
	{
		fEmb = MAP2::getEmbedding(d, FACE_ORBIT) ;
	}

	if(MAP2::mergeFaces(d))
	{
		if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
		{
			if(vEmb1 != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, eNext, vEmb1) ;
			if(vEmb2 != EMBNULL)
				MAP2::setDartEmbedding(VERTEX_ORBIT, dNext, vEmb2) ;
		}
		if (MAP2::isOrbitEmbedded(FACE_ORBIT))
		{
			MAP2::embedOrbit(FACE_ORBIT, dNext, fEmb) ;
		}
		return true ;
	}
	return false ;
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::mergeVolumes(Dart d, Dart e)
{
	std::vector<Dart> darts ;
	std::vector<unsigned int> vEmb ;
	std::vector<unsigned int> eEmb ;
	Dart fit = d ;
	do
	{
		darts.push_back(MAP2::phi2(fit)) ;

		if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
		{
			vEmb.push_back(MAP2::getEmbedding(MAP2::phi2(fit), VERTEX_ORBIT)) ;
		}

		if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
		{
			eEmb.push_back(MAP2::getEmbedding(fit, EDGE_ORBIT)) ;
		}

		fit = MAP2::phi1(fit) ;
	} while (fit != d) ;

	if(MAP2::mergeVolumes(d, e))
	{
		for(unsigned int i = 0; i < darts.size(); ++i)
		{
			if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
			{
				MAP2::embedOrbit(VERTEX_ORBIT, darts[i], vEmb[i]) ;
			}

			if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
			{
				MAP2::embedOrbit(EDGE_ORBIT, darts[i], eEmb[i]) ;
			}
		}
		return true ;
	}
	return false ;
}

template <typename MAP2>
bool EmbeddedMap2<MAP2>::check()
{
	bool topo = MAP2::check() ;
	if (!topo)
		return false ;

	CGoGNout << "Check: embedding begin" << CGoGNendl ;
	for(Dart d = MAP2::begin(); d != MAP2::end(); MAP2::next(d))
	{
		if (MAP2::isOrbitEmbedded(VERTEX_ORBIT))
		{
			if (MAP2::getEmbedding(d, VERTEX_ORBIT) != MAP2::getEmbedding(MAP2::alpha1(d), VERTEX_ORBIT))
			{
				CGoGNout << "Check: different embeddings on vertex" << CGoGNendl ;
				return false ;
			}
		}

		if (MAP2::isOrbitEmbedded(EDGE_ORBIT))
		{
			if (MAP2::getEmbedding(d, EDGE_ORBIT) != MAP2::getEmbedding(MAP2::phi2(d), EDGE_ORBIT))
			{
				CGoGNout << "Check: different embeddings on edge" << CGoGNendl ;
				return false ;
			}
		}

		if (MAP2::isOrbitEmbedded(FACE_ORBIT))
		{
			if (MAP2::getEmbedding(d, FACE_ORBIT) != MAP2::getEmbedding(MAP2::phi1(d), FACE_ORBIT))
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
