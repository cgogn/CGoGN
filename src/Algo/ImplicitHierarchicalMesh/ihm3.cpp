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

#include "Algo/ImplicitHierarchicalMesh/ihm3.h"
#include <math.h>
#include <limits>

namespace CGoGN
{

namespace Algo
{

namespace IHM
{

ImplicitHierarchicalMap3::ImplicitHierarchicalMap3() : m_curLevel(0), m_maxLevel(0), m_edgeIdCount(0), m_faceIdCount(0)
{
	m_dartLevel = Map3::addAttribute<unsigned int>(DART, "dartLevel") ;
	m_faceId = Map3::addAttribute<unsigned int>(DART, "faceId") ;
	m_edgeId = Map3::addAttribute<unsigned int>(DART, "edgeId") ;

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		m_nextLevelCell[i] = NULL ;
}

ImplicitHierarchicalMap3::~ImplicitHierarchicalMap3()
{
	Map3::removeAttribute(m_edgeId) ;
	Map3::removeAttribute(m_faceId) ;
	Map3::removeAttribute(m_dartLevel) ;
}

void ImplicitHierarchicalMap3::init()
{
	initEdgeId() ;
	initFaceId();

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if(m_nextLevelCell[orbit] != NULL)
		{
			AttributeContainer& cellCont = m_attribs[orbit] ;
			for(unsigned int i = cellCont.begin(); i < cellCont.end(); cellCont.next(i))
				m_nextLevelCell[orbit]->operator[](i) = EMBNULL ;
		}
	}
}


Dart ImplicitHierarchicalMap3::beginSplittingPath(Dart d, DartMarker& m)
{
	Dart dres = NIL;
	Dart dit = d;
	bool found = false;

	// Recherche d'un brin de depart du chemin d'arete
	do
	{
		Dart eit = phi1(dit);

		if(!m.isMarked(eit) && getDartLevel(eit) == getCurrentLevel())
		{
			found = true;
			dres = eit;
		}

		dit = phi2(phi_1(dit));
	}
	while(!found && dit != d);

	return dres;
}

void ImplicitHierarchicalMap3::constructSplittingPath(Dart d, std::vector<Dart>& v, DartMarker& m)
{

	//Construction du chemin d'arete
	Dart cit = d;

	v.push_back(cit);
	m.markOrbit(EDGE, cit);

	do
	{

		if(std::min(getDartLevel(phi1(cit)),getDartLevel(phi2(phi1(cit))))  == getDartLevel(d))
		{
			if(m.isMarked(phi1(cit)))
			{
				cit = phi1(phi2(phi1(cit)));
				std::cout << "1_1" << std::endl;
			}
		}
		else if(std::min(getDartLevel(phi1(cit)),getDartLevel(phi2(phi1(cit)))) < getDartLevel(d))
		{
			cit = phi1(phi2(phi1(cit)));
			std::cout << "2" << std::endl;
		}
		else
			cit = phi1(cit);

		v.push_back(cit);
		m.markOrbit(EDGE, cit);


	}
	while(cit != d);

//	do
//	{
//		v.push_back(cit);
//		m.markOrbit(EDGE, cit);
//
//		cit = phi1(cit);
//
//		//std::cout << "cit = " << cit << std::endl;
//
//		if(std::min(getDartLevel(cit), getDartLevel(phi2(cit))) == getDartLevel(d))
//		{
//			if(m.isMarked(cit))
//			{
//				cit = phi1(phi2(cit));
//				//std::cout << "1_1" << std::endl;
//			}
//		}
//		else if(std::min(getDartLevel(cit),getDartLevel(phi2(cit))) < getDartLevel(d))
//		{
//			cit = phi1(phi2(cit));
//			//std::cout << "2" << std::endl;
//		}
//
//	}while(cit != d);

}


void ImplicitHierarchicalMap3::swapEdges(Dart d, Dart e)
{
	if(!Map2::isBoundaryEdge(d) && !Map2::isBoundaryEdge(e))
	{
	Dart d2 = phi2(d);
	Dart e2 = phi2(e);



	Map2::unsewFaces(d);
	Map2::unsewFaces(e);

	Map2::sewFaces(d, e);
	Map2::sewFaces(d2, e2);

	if(isOrbitEmbedded(VERTEX))
	{
		copyDartEmbedding(VERTEX, d, phi2(phi_1(d)));
		copyDartEmbedding(VERTEX, e, phi2(phi_1(e)));
		copyDartEmbedding(VERTEX, d2, phi2(phi_1(d2)));
		copyDartEmbedding(VERTEX, e2, phi2(phi_1(e2)));
	}

	if(isOrbitEmbedded(EDGE))
	{

	}

	if(isOrbitEmbedded(VOLUME))
		embedNewCell(VOLUME, d);
	}
}

void ImplicitHierarchicalMap3::saveRelationsAroundVertex(Dart d, std::vector<std::pair<Dart, Dart> >& vd)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	//le brin est forcement du niveau cur
	Dart dit = d;

	do
	{
		vd.push_back(std::pair<Dart,Dart>(dit,phi2(dit)));

		dit = phi2(phi_1(dit));

	}while(dit != d);
}

void ImplicitHierarchicalMap3::unsewAroundVertex(std::vector<std::pair<Dart, Dart> >& vd)
{
	//unsew the edge path
	for(std::vector<std::pair<Dart, Dart> >::iterator it = vd.begin() ; it != vd.end() ; ++it)
	{
		Dart dit = (*it).first;
		Dart dit2 = (*it).second;

		Map2::unsewFaces(dit);

		if(isOrbitEmbedded(VERTEX))
		{
			copyDartEmbedding(VERTEX, phi2(dit2), dit);
			copyDartEmbedding(VERTEX, phi2(dit), dit2);
		}

		if(isOrbitEmbedded(EDGE))
		{

		}
	}
}

Dart ImplicitHierarchicalMap3::quadranguleFace(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	Dart centralDart = NIL;
	Map2::fillHole(phi1(d));

	Dart old = phi2(phi1(d));
	Dart bc = newBoundaryCycle(faceDegree(old));
	sewVolumes(old, bc, false);

	if (isOrbitEmbedded(VERTEX))
	{
		Dart it = bc;
		do
		{
			//copyDartEmbedding(VERTEX, it, phi1(phi3(it)));
			embedOrbit(VERTEX, it, getEmbedding(VERTEX, phi1(phi3(it))));
			it = phi1(it) ;
		} while(it != bc) ;
	}


	Dart dd = phi1(phi1(old)) ;
	splitFace(old,dd) ;

	unsigned int idface = getNewFaceId();
	setFaceId(dd,idface, FACE);

	Dart ne = phi1(phi1(old)) ;

	cutEdge(ne);
	centralDart = phi1(ne);

	//newEdges.push_back(ne);
	//newEdges.push_back(map.phi1(ne));

	unsigned int id = getNewEdgeId() ;
	setEdgeId(ne, id, EDGE) ;

	Dart stop = phi2(phi1(ne));
	ne = phi2(ne);
	do
	{
		dd = phi1(phi1(phi1(ne)));

		splitFace(ne, dd) ;

		unsigned int idface = getNewFaceId();
		setFaceId(dd,idface, FACE);

		//newEdges.push_back(map.phi1(dd));

		ne = phi2(phi_1(ne));
		dd = phi1(phi1(dd));
	}
	while(dd != stop);

	return centralDart;
}

//Dart ImplicitHierarchicalMap3::cutEdge(Dart d)
//{
//        Dart resV = EmbeddedMap3::cutEdge(d);
//
//        unsigned int eId = getEdgeId(d);
//        Dart dit = d;
//        do
//        {
//        	//EdgeId
//        	m_edgeId[phi1(dit)] = eId;
//        	m_edgeId[phi3(dit)] = eId;
//
//        	//FaceId
//        	unsigned int fId = getFaceId(dit);
//        	m_faceId[phi1(dit)] = fId;
//        	m_edgeId[phi3(dit)] = fId;
//
//            dit = alpha2(dit);
//        }
//        while(dit != d);
//
//        return resV;
//}
//
//bool ImplicitHierarchicalMap3::uncutEdge(Dart d)
//{
//       return EmbeddedMap3::uncutEdge(d);
//}
//
//void ImplicitHierarchicalMap3::splitFace(Dart d, Dart e)
//{
//        EmbeddedMap3::splitFace(d,e);
//
//        unsigned int eId = getNewEdgeId();
//        unsigned int fId = getFaceId(d);
//
//        Dart ne = phi_1(d);
//        Dart ne3 = phi3(ne);
//
//        m_edgeId[ne] = eId;
//        m_edgeId[phi2(ne)] = eId;
//        m_edgeId[ne3] = eId;
//        m_edgeId[phi2(ne3)] = eId;
//
//        m_faceId[ne] = fId;
//        m_faceId[phi2(ne)] = fId;
//        m_faceId[ne3] = fId;
//        m_faceId[phi2(ne3)] = fId;
//}
//
//void ImplicitHierarchicalMap3::sewVolumes(Dart d, Dart e, bool withBoundary)
//{
//        EmbeddedMap3::sewVolumes(d,e);
//
//        unsigned int fId;
//
//        if(m_faceId[d] < m_faceId[phi3(d)])
//        	fId = m_faceId[d] ;
//        else
//        	fId = m_edgeId[phi3(d)];
//
//        Dart dit = d;
//        do
//        {
//                //EdgeId
////                if(m_edgeId[dit] < m_edgeId[phi3(dit)])
////                	m_edgeId[phi3(dit)] = m_edgeId[dit] ;
////                else
////                	m_edgeId[dit] = m_edgeId[phi3(dit)];
//
//                //FaceId
//                m_faceId[dit] = fId;
//                m_faceId[phi3(dit)] = fId;
//
//                dit = phi1(dit);
//        }
//        while(dit != d);
//}
//
//void ImplicitHierarchicalMap3::splitVolume(std::vector<Dart>& vd)
//{
//        EmbeddedMap3::splitVolume(vd);
//
//        unsigned int fId = getNewFaceId();
//
//        for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
//        {
//                Dart dit = *it;
//
//                //Edge Id
//                m_edgeId[phi2(dit)] = m_edgeId[dit];
//
//                //Face Id
//                m_faceId[phi2(dit)] = fId;
//        }
//}


void ImplicitHierarchicalMap3::initEdgeId()
{
	DartMarker edgeMark(*this) ;
	for(Dart d = Map3::begin(); d != Map3::end(); Map3::next(d))
	{
		if(!edgeMark.isMarked(d))
		{
			Dart e = d;
			do
			{
				m_edgeId[e] = m_edgeIdCount;
				edgeMark.mark(e);

				m_edgeId[Map3::phi2(e)] = m_edgeIdCount ;
				edgeMark.mark(Map3::phi2(e));

				e = Map3::alpha2(e);
			} while(e != d);

			m_edgeIdCount++;
		}
	}
}

void ImplicitHierarchicalMap3::initFaceId()
{
	DartMarker faceMark(*this) ;
	for(Dart d = Map3::begin(); d != Map3::end(); Map3::next(d))
	{
		if(!faceMark.isMarked(d))
		{
			Dart e = d;
			do
			{
				m_faceId[e] = m_faceIdCount ;
				faceMark.mark(e);

				Dart e3 = Map3::phi3(e);
				m_faceId[e3] = m_faceIdCount ;
				faceMark.mark(e3);

				e = Map3::phi1(e);
			} while(e != d);

			m_faceIdCount++;
		}
	}
}

unsigned int ImplicitHierarchicalMap3::faceLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	if(m_curLevel == 0)
		return 0 ;

	Dart it = d ;
	Dart old = it ;
	unsigned int l_old = m_dartLevel[old] ;
	unsigned int fLevel = edgeLevel(it) ;
	do
	{
		it = phi1(it) ;
		unsigned int dl = m_dartLevel[it] ;
		if(dl < l_old)							// compute the oldest dart of the face
		{										// in the same time
			old = it ;
			l_old = dl ;
		}										// in a first time, the level of a face
		unsigned int l = edgeLevel(it) ;		// is the minimum of the levels
		fLevel = l < fLevel ? l : fLevel ;		// of its edges
	} while(it != d) ;


	unsigned int cur = m_curLevel ;
	m_curLevel = fLevel ;

	unsigned int nbSubd = 0 ;
	it = old ;
	unsigned int eId = m_edgeId[old] ;			// the particular case of a face
	do											// with all neighboring faces regularly subdivided
	{											// but not the face itself
		++nbSubd ;								// is treated here
		it = phi1(it) ;
	} while(m_edgeId[it] == eId) ;

	while(nbSubd > 1)
	{
		nbSubd /= 2 ;
		--fLevel ;
	}

	m_curLevel = cur ;

	return fLevel ;
}

unsigned int ImplicitHierarchicalMap3::volumeLevel(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	if(m_curLevel == 0)
		return 0 ;

	//First : the level of a volume is the
	//minimum of the levels of its faces

	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(512);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;

	Dart oldest = d ;
	unsigned int vLevel = std::numeric_limits<unsigned int>::max() ; //hook de ouf

	//parcours les faces du volume au niveau courant
	//on cherche le brin de niveau le plus bas de la hierarchie
	//on note le niveau le plus bas de la hierarchie
	mark.markOrbit(FACE, d) ;
	for(unsigned int i = 0; i < visitedFaces.size(); ++i)
	{
		Dart e = visitedFaces[i] ;

		// in a first time, the level of a face
		//the level of the volume is the minimum of the
		//levels of its faces

		//
		// Compute the level of this face
		// and the oldest dart
		//
		Dart it = e ;
		Dart old = it ;
		unsigned int l_old = m_dartLevel[old] ;
		unsigned int fLevel = edgeLevel(it) ;
		do
		{
			it = phi1(it) ;
			unsigned int dl = m_dartLevel[it] ;
			if(dl < l_old)							// compute the oldest dart of the face
			{										// in the same time
				old = it ;
				l_old = dl ;
			}										// in a first time, the level of a face
			unsigned int l = edgeLevel(it) ;		// is the minimum of the levels
			fLevel = l < fLevel ? l : fLevel ;		// of its edges
		} while(it != e) ;

		unsigned int cur = m_curLevel ;
		m_curLevel = fLevel ;

		unsigned int nbSubd = 0 ;
		it = old ;
		unsigned int eId = m_edgeId[old] ;			// the particular case of a face
		do											// with all neighboring faces regularly subdivided
		{											// but not the face itself
			++nbSubd ;								// is treated here
			it = phi1(it) ;
		} while(m_edgeId[it] == eId) ;

		while(nbSubd > 1)
		{
			nbSubd /= 2 ;
			--fLevel ;
		}

		m_curLevel = cur ;

		//
		// compute the minimum level of the volume
		// if the level of this face is lower than the saved volume level
		//
		vLevel = fLevel < vLevel ? fLevel : vLevel ;

		//
		// compute the oldest dart from the volume
		// if the oldest dart from this face is oldest than the oldest saved dart
		//
		if(m_dartLevel[old] < m_dartLevel[oldest])
				oldest = old ;

		//
		// add all face neighbours to the table
		//
		do
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != visitedFaces[i]) ;
	}


	//Second : the case of all faces regularly subdivided but not the volume itself
	unsigned int cur = m_curLevel ;
	m_curLevel = vLevel ;

	unsigned int nbSubd = 0 ;
	Dart it = oldest ;
	unsigned int eId = m_edgeId[oldest] ;

	do
	{
		++nbSubd ;
		it = phi1(it) ;
	} while(m_edgeId[it] == eId) ;


	while(nbSubd > 1)
	{
		nbSubd /= 2 ;
	    --vLevel ;
	}

	m_curLevel = cur ;

	return vLevel;
}


Dart ImplicitHierarchicalMap3::faceOldestDart(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	Dart it = d ;
	Dart oldest = it ;
	unsigned int l_old = m_dartLevel[oldest] ;
	do
	{
		unsigned int l = m_dartLevel[it] ;
		if(l < l_old || (l == l_old && it < oldest))
		{
			oldest = it ;
			l_old = l ;
		}
		it = phi1(it) ;
	} while(it != d) ;
	return oldest ;
}

Dart ImplicitHierarchicalMap3::volumeOldestDart(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	Dart oldest = d;
	DartMarkerStore mark(*this);	// Lock a marker

	std::vector<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.reserve(512);
	visitedFaces.push_back(d);		// Start with the face of d

	// For every face added to the list
	//the oldest dart from a volume is the oldest dart from all faces of this volume
	mark.markOrbit(FACE, d) ;

	for(unsigned int i = 0; i < visitedFaces.size(); ++i)
	{
		Dart e = visitedFaces[i] ;

		//for every dart in this face
		Dart old = faceOldestDart(e);
		if(m_dartLevel[old] < m_dartLevel[oldest])
			oldest = old;

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != visitedFaces[i]) ;
	}

	return oldest;
}

bool ImplicitHierarchicalMap3::edgeIsSubdivided(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	Dart d2 = phi2(d) ;
	++m_curLevel ;
	Dart d2_l = phi2(d) ;
	--m_curLevel ;
	if(d2 != d2_l)
		return true ;
	else
		return false ;
}

bool ImplicitHierarchicalMap3::faceIsSubdivided(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < m_curLevel)
		return false ;

	bool subd = false ;
	++m_curLevel ;
	if(m_dartLevel[phi1(d)] == m_curLevel && m_edgeId[phi1(d)] != m_edgeId[d])
		subd = true ;
	--m_curLevel ;

	return subd ;
}

bool ImplicitHierarchicalMap3::volumeIsSubdivided(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int vLevel = volumeLevel(d);
	if(vLevel < m_curLevel)
		return false;

	//test si toutes les faces sont subdivisee
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(512);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;


//	Dart old = d ;
	bool facesAreSubdivided = faceIsSubdivided(d) ;

	//parcours les faces du volume au niveau courant
	//on cherche le brin de niveau le plus bas de la hierarchie
	//on note le niveau le plus bas de la hierarchie
	mark.markOrbit(FACE, d) ;
	for(unsigned int i = 0; i < visitedFaces.size(); ++i)
	{
		Dart e = visitedFaces[i] ;

		// in a first time, the level of a face
		//the level of the volume is the minimum of the
		//levels of its faces

		facesAreSubdivided &= faceIsSubdivided(e) ;

		do	// add all face neighbours to the table
		{
			Dart ee = phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(FACE, ee) ;
			}
			e = phi1(e) ;
		} while(e != visitedFaces[i]) ;
	}

	//mais pas le volume lui-meme
	bool subd = false;
	++m_curLevel;
	if(facesAreSubdivided && m_dartLevel[phi2(phi1(phi1(d)))] == m_curLevel && m_faceId[phi2(phi1(phi1(d)))] != m_faceId[d])
		subd = true;
	--m_curLevel;
	return subd;
}


bool ImplicitHierarchicalMap3::edgeCanBeCoarsened(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	bool subd = false ;
	bool subdOnce = true ;
	bool degree2 = false ;

	if(edgeIsSubdivided(d))
	{
		subd = true ;
		Dart d2 = phi2(d) ;
		++m_curLevel ;

		std::cout << "vertex degree = " << vertexDegree(phi1(d)) << std::endl;

		if(vertexDegree(phi1(d)) == 2)
		{
			degree2 = true ;
			if(edgeIsSubdivided(d) || edgeIsSubdivided(d2))
				subdOnce = false ;
		}
		--m_curLevel ;
	}
	return subd && degree2 && subdOnce ;
}

bool ImplicitHierarchicalMap3::faceCanBeCoarsened(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	bool subd = false;
	bool subdOnce = true;
	bool subdNeighborhood = false; //deux volumes voisins de la face ne sont pas subdivise

	if(faceIsSubdivided(d))
	{
		subd = true;
		Dart d3 = phi3(d);

//		std::cout << "d3 = " << d3 << std::endl;
//		std::cout << "d = " << d << std::endl;
//		std::cout << "curLevel = " << m_curLevel << std::endl;
//		std::cout << "volSubd(d3) = " << volumeIsSubdivided(d3) << std::endl;

		//tester si le volume voisin est subdivise
		if(d3 != d && volumeIsSubdivided(d3))
			subdNeighborhood = true;

		++m_curLevel;
		//tester si la face subdivise a des faces subdivise
		Dart cf = phi1(d);

		do
		{
			if(faceIsSubdivided(cf))
				subdOnce = false;

			cf = phi2(phi1(cf));
		}
		while(subdOnce && cf != phi1(d));

		--m_curLevel;
	}

//	std::cout << "subdNeighborhood = " << subdNeighborhood << std::endl;
// 	std::cout << "faceCanBeCoarsened ? " << (subd && !subdNeighborhood && subdOnce) << std::endl;

	return subd && !subdNeighborhood && subdOnce;
}

bool ImplicitHierarchicalMap3::faceIsSubdividedOnce(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;
	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < m_curLevel)		// a face whose level in the current level map is lower than
		return false ;			// the current level can not be subdivided to higher levels

	unsigned int degree = 0 ;
	bool subd = false ;
	bool subdOnce = true ;
	Dart fit = d ;
	do
	{
		++m_curLevel ;
		if(m_dartLevel[phi1(fit)] == m_curLevel && m_edgeId[phi1(fit)] != m_edgeId[fit])
		{
			subd = true ;
			++m_curLevel ;
			if(m_dartLevel[phi1(fit)] == m_curLevel && m_edgeId[phi1(fit)] != m_edgeId[fit])
				subdOnce = false ;
			--m_curLevel ;
		}
		--m_curLevel ;
		++degree ;
		fit = phi1(fit) ;

	} while(subd && subdOnce && fit != d) ;

	if(degree == 3 && subd)
	{
		++m_curLevel ;
		Dart cf = phi2(phi1(d)) ;
		++m_curLevel ;
		if(m_dartLevel[phi1(cf)] == m_curLevel && m_edgeId[phi1(cf)] != m_edgeId[cf])
			subdOnce = false ;
		--m_curLevel ;
		--m_curLevel ;
	}

	return subd && subdOnce ;
}

bool ImplicitHierarchicalMap3:: volumeIsSubdividedOnce(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	return true;
}

bool ImplicitHierarchicalMap3::neighborhoodLevelDiffersByOne(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	bool found = false;

	unsigned int vLevel = volumeLevel(d);// + 1;

	Dart old = volumeOldestDart(d);

	DartMarkerStore mf(*this);		// Lock a face marker to save one dart per face

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(512);
	visitedFaces.push_back(old);

	mf.markOrbit(FACE, old) ;

	for(unsigned int i = 0; !found && i < visitedFaces.size(); ++i)
	{
		Dart e = visitedFaces[i] ;
		do
		{
			// add all face neighbours to the table

			if(phi3(e) != e)
			{
				Dart old = volumeOldestDart(phi3(e));
				//if((abs(volumeLevel(old) - vLevel) > 1))
				if(volumeLevel(old) < vLevel)
					found = true;
			}

			Dart ee = phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
			}

			e = phi1(e) ;
		} while(e != visitedFaces[i]) ;
	}

	return found;
}

bool ImplicitHierarchicalMap3::coarsenNeighborhoodLevelDiffersByOne(Dart d)
{
	assert(m_dartLevel[d] <= m_curLevel || !"Access to a dart introduced after current level") ;

	bool found = false;

	//unsigned int vLevel = volumeLevel(d);

	Dart old = volumeOldestDart(d);

	DartMarkerStore mf(*this);		// Lock a face marker to save one dart per face

	//Store faces that are traversed and start with the face of d
	std::vector<Dart> visitedFaces;
	visitedFaces.reserve(512);
	visitedFaces.push_back(old);

	mf.markOrbit(FACE, old) ;

	for(unsigned int i = 0; !found && i < visitedFaces.size(); ++i)
	{
		Dart e = visitedFaces[i] ;
		do
		{
			// add all face neighbours to the table

			if(faceIsSubdivided(e))
			{
				++m_curLevel;

				if(faceIsSubdividedOnce(e))
					found = true;

				--m_curLevel;
			}
			Dart ee = phi2(e) ;
			if(!mf.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mf.markOrbit(FACE, ee) ;
			}

			e = phi1(e) ;
		} while(e != visitedFaces[i]) ;
	}

	return found;
}

} //namespace IHM

} //namespace Algo

} //namespace CGoGN

