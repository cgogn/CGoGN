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

#include "Algo/Multiresolution/map3MR/map3MR_PrimalAdapt.h"

namespace CGoGN
{

namespace Algo
{

namespace MR
{

namespace Primal
{

namespace Adaptive
{

template <typename PFP>
Map3MR<PFP>::Map3MR(typename PFP::MAP& map) :
	m_map(map),
	shareVertexEmbeddings(false),
	vertexVertexFunctor(NULL),
	edgeVertexFunctor(NULL),
	faceVertexFunctor(NULL),
	volumeVertexFunctor(NULL)
{

}

/*! @name Topological helping functions
 *************************************************************************/
template <typename PFP>
void Map3MR<PFP>::swapEdges(Dart d, Dart e)
{
	if(!m_map.PFP::MAP::ParentMap::isBoundaryEdge(d) && !m_map.PFP::MAP::ParentMap::isBoundaryEdge(e))
	{
		Dart d2 = m_map.phi2(d);
		Dart e2 = m_map.phi2(e);

//		Map2::swapEdges(d,e);

		m_map.PFP::MAP::ParentMap::unsewFaces(d);
		m_map.PFP::MAP::ParentMap::unsewFaces(e);

		m_map.PFP::MAP::ParentMap::sewFaces(d, e);
		m_map.PFP::MAP::ParentMap::sewFaces(d2, e2);

		if(m_map.template isOrbitEmbedded<VERTEX>())
		{
			m_map.template copyDartEmbedding<VERTEX>(d, m_map.phi2(m_map.phi_1(d)));
			m_map.template copyDartEmbedding<VERTEX>(e, m_map.phi2(m_map.phi_1(e)));
			m_map.template copyDartEmbedding<VERTEX>(d2, m_map.phi2(m_map.phi_1(d2)));
			m_map.template copyDartEmbedding<VERTEX>(e2, m_map.phi2(m_map.phi_1(e2)));
		}

		if(m_map.template isOrbitEmbedded<EDGE>())
		{

		}

		if(m_map.template isOrbitEmbedded<VOLUME>())
			m_map.template setOrbitEmbeddingNewCell<VOLUME>(d);


//		propagateDartRelation(d, m_phi2) ;
//		propagateDartRelation(d, m_phi3) ;
//		//propagateDartRelation(d2, m_phi2) ;
//		//propagateDartRelation(d2, m_phi3) ;
//		propagateDartRelation(e, m_phi2) ;
//		propagateDartRelation(e, m_phi3) ;
//		//propagateDartRelation(e2, m_phi2) ;
//		//propagateDartRelation(e2, m_phi3) ;
	}
}

template <typename PFP>
void Map3MR<PFP>::splitSurfaceInVolume(std::vector<Dart>& vd, bool firstSideClosed, bool secondSideClosed)
{
	std::vector<Dart> vd2 ;
	vd2.reserve(vd.size());

	// save the edge neighbors darts
	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
	{
		vd2.push_back(phi2(*it));
	}

	assert(vd2.size() == vd.size());

	Map2::splitSurface(vd, firstSideClosed, secondSideClosed);

	// follow the edge path a second time to embed the vertex, edge and volume orbits
	for(unsigned int i = 0; i < vd.size(); ++i)
	{
		Dart dit = vd[i];
		Dart dit2 = vd2[i];

//		propagateDartRelation(dit, m_phi1) ;
//		propagateDartRelation(dit, m_phi_1) ;
//		propagateDartRelation(dit, m_phi2) ;
//
//		propagateDartRelation(dit2, m_phi1) ;
//		propagateDartRelation(dit2, m_phi_1) ;
//		propagateDartRelation(dit2, m_phi2) ;

		// embed the vertex embedded from the origin volume to the new darts
		if(isOrbitEmbedded<VERTEX>())
		{
			copyDartEmbedding<VERTEX>(phi2(dit), phi1(dit));
			copyDartEmbedding<VERTEX>(phi2(dit2), phi1(dit2));
		}
	}

}

template <typename PFP>
void Map3MR<PFP>::splitFaceInVolume(Dart d, Dart e)
{
	Dart dprev = phi_1(d) ;
	Dart eprev = phi_1(e) ;
	Map2::splitFace(d, e) ;
	Dart dd = phi1(dprev) ;
	Dart ee = phi1(eprev) ;

	propagateDartRelation(d, m_phi_1) ;
	propagateDartRelation(e, m_phi_1) ;
	propagateDartRelation(dd, m_phi1) ;
	propagateDartRelation(dd, m_phi_1) ;
	propagateDartRelation(dd, m_phi2) ;
	propagateDartRelation(ee, m_phi1) ;
	propagateDartRelation(ee, m_phi_1) ;
	propagateDartRelation(ee, m_phi2) ;
	propagateDartRelation(dprev, m_phi1) ;
	propagateDartRelation(eprev, m_phi1) ;

	propagateDartEmbedding<VERTEX>(dd) ;
	propagateDartEmbedding<VERTEX>(ee) ;
}

template <typename PFP>
Dart Map3MR<PFP>::cutEdgeInVolume(Dart d)
{
	Dart dd = phi2(d) ;
	Dart d1 = Map2::cutEdge(d) ;
	Dart dd1 = phi1(dd) ;
	Dart d11 = phi1(d1) ;
	Dart dd11 = phi1(dd1) ;

	propagateDartRelation(d, m_phi1) ;
	propagateDartRelation(d, m_phi2) ;
	propagateDartRelation(dd, m_phi1) ;
	propagateDartRelation(dd, m_phi2) ;
	propagateDartRelation(d1, m_phi1) ;
	propagateDartRelation(d1, m_phi_1) ;
	propagateDartRelation(d1, m_phi2) ;
	propagateDartRelation(dd1, m_phi1) ;
	propagateDartRelation(dd1, m_phi_1) ;
	propagateDartRelation(dd1, m_phi2) ;
	propagateDartRelation(d11, m_phi_1) ;
	propagateDartRelation(dd11, m_phi_1) ;

	return d1 ;
}


template <typename PFP>
Dart Map3MR<PFP>::cutEdge(Dart d)
{
	Dart dit = d;
	do
	{
		Dart dd = phi2(dit) ;
		Dart d1 = phi1(dit) ;
		Dart dd1 = phi1(dd) ;

		Dart dd3 = phi3(dit);
		Dart d13 = phi2(dd3) ;
		Dart d13 = phi1(d13) ;
		Dart dd13 = phi1(dd) ;


		m_map.duplicateDart(d);
		m_map.duplicateDart(dd);
		m_map.duplicateDart(d1);
		m_map.duplicateDart(dd1);

		m_map.duplicateDart(d);
		m_map.duplicateDart(dd);
		m_map.duplicateDart(d1);
		m_map.duplicateDart(dd1);


		dit = m_map.alpha2(dit);
	}while(dit != d);

	Dart nd = EmbeddedMap3::cutEdge(d);

	return nd;
}

template <typename PFP>
void Map3MR<PFP>::splitFace(Dart d, Dart e)
{
	Dart dprev = phi_1(d) ;
	Dart eprev = phi_1(e) ;
	EmbeddedMap3::splitFace(d,e);
	Dart dd = phi1(dprev) ;
	Dart ee = phi1(eprev) ;

	propagateDartRelation(d, m_phi_1) ;
	propagateDartRelation(e, m_phi_1) ;
	propagateDartRelation(dd, m_phi1) ;
	propagateDartRelation(dd, m_phi_1) ;
	propagateDartRelation(dd, m_phi2) ;
	propagateDartRelation(ee, m_phi1) ;
	propagateDartRelation(ee, m_phi_1) ;
	propagateDartRelation(ee, m_phi2) ;
	propagateDartRelation(dprev, m_phi1) ;
	propagateDartRelation(eprev, m_phi1) ;

	propagateDartRelation(phi3(dprev), m_phi_1) ;
	propagateDartRelation(phi3(eprev), m_phi_1) ;
	propagateDartRelation(phi3(dd), m_phi1) ;
	propagateDartRelation(phi3(dd), m_phi_1) ;
	propagateDartRelation(phi3(dd), m_phi2) ;
	propagateDartRelation(phi3(ee), m_phi1) ;
	propagateDartRelation(phi3(ee), m_phi_1) ;
	propagateDartRelation(phi3(ee), m_phi2) ;
	propagateDartRelation(phi3(d), m_phi1) ;
	propagateDartRelation(phi3(e), m_phi1) ;

	propagateDartEmbedding<VERTEX>(dd) ;
	propagateDartEmbedding<VERTEX>(ee) ;
	propagateDartEmbedding<VERTEX>(phi3(dd)) ;
	propagateDartEmbedding<VERTEX>(phi3(ee)) ;
}

template <typename PFP>
void Map3MR<PFP>::splitVolume(std::vector<Dart>& vd)
{
	EmbeddedMap3::splitVolume(vd);

	for(std::vector<Dart>::iterator it = vd.begin() ; it != vd.end() ; ++it)
	{
		Dart dit = *it;
		propagateDartRelation(dit, m_phi2) ;

		Dart dit2 = phi2(dit);
		propagateDartRelation(dit2, m_phi1) ;
		propagateDartRelation(dit2, m_phi_1) ;
		propagateDartRelation(dit2, m_phi2) ;
		propagateDartRelation(dit2, m_phi3) ;

		Dart dit23 = phi3(dit2);
		propagateDartRelation(dit23, m_phi1) ;
		propagateDartRelation(dit23, m_phi_1) ;
		propagateDartRelation(dit23, m_phi2) ;
		propagateDartRelation(dit23, m_phi3) ;

		Dart dit232 = phi2(dit23);
		propagateDartRelation(dit232, m_phi2) ;

		propagateDartEmbedding<VERTEX>(dit2) ;
		propagateDartEmbedding<VERTEX>(dit23) ;
	}

}

/*! @name Cells informations
 *************************************************************************/
template <typename PFP>
unsigned int Map3MR<PFP>::edgeLevel(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"edgeLevel : called with a dart inserted after current level") ;

	unsigned int r = 0;
	Dart dit = d;
	do
	{
		unsigned int ld = m_map.getDartLevel(dit) ;
		unsigned int ldd = m_map.getDartLevel(m_map.phi2(dit)) ;
		unsigned int min = ld < ldd ? ldd : ld;

		r =  r < min ? min : r ;

		dit = m_map.alpha2(dit);
	} while(dit != d);

	return r;
}

template <typename PFP>
unsigned int Map3MR<PFP>::faceLevel(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"faceLevel : called with a dart inserted after current level") ;

	if(m_map.getCurrentLevel() == 0)
		return 0 ;

	Dart it = d ;
	unsigned int min1 = m_map.getDartLevel(it) ;		// the level of a face is the second minimum of the
	it = m_map.phi1(it) ;
	unsigned int min2 = m_map.getDartLevel(it) ;		// insertion levels of its darts

	if(min2 < min1)
	{
		unsigned int tmp = min1 ;
		min1 = min2 ;
		min2 = tmp ;
	}

	it = m_map.phi1(it) ;
	while(it != d)
	{
		unsigned int dl = m_map.getDartLevel(it) ;
		if(dl < min2)
		{
			if(dl < min1)
			{
				min2 = min1 ;
				min1 = dl ;
			}
			else
				min2 = dl ;
		}
		it = m_map.phi1(it) ;
	}

	return min2 ;
}

template <typename PFP>
unsigned int Map3MR<PFP>::volumeLevel(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"volumeLevel : called with a dart inserted after current level") ;

	if(m_map.getCurrentLevel() == 0)
		return 0 ;

	Dart oldest = d ;
	unsigned int vLevel = std::numeric_limits<unsigned int>::max(); //hook sioux

	//First : the level of a volume is the minimum of the levels of its faces
	Traversor3WF<typename PFP::MAP> travF(m_map, d);
	for (Dart dit = travF.begin(); dit != travF.end(); dit = travF.next())
	{
		// in a first time, the level of a face
		//the level of the volume is the minimum of the
		//levels of its faces
		unsigned int fLevel = faceLevel(dit);
		vLevel = fLevel < vLevel ? fLevel : vLevel ;
	}

	//Second : the case of all faces regularly subdivided but not the volume itself

	return vLevel;
}

template <typename PFP>
Dart Map3MR<PFP>::faceOldestDart(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"faceOldestDart : called with a dart inserted after current level") ;

	Dart it = d ;
	Dart oldest = it ;
	unsigned int l_old = m_map.getDartLevel(oldest) ;
	do
	{
		unsigned int l = m_map.getDartLevel(it) ;
		if(l == 0)
			return it ;
		if(l < l_old)
		{
			oldest = it ;
			l_old = l ;
		}
		it = m_map.phi1(it) ;
	} while(it != d) ;
	return oldest ;
}

template <typename PFP>
Dart Map3MR<PFP>::volumeOldestDart(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"volumeOldestDart : called with a dart inserted after current level") ;

	Dart oldest = d;
	Traversor3WF<typename PFP::MAP> travF(m_map, d);
	for (Dart dit = travF.begin(); dit != travF.end(); dit = travF.next())
	{
		//for every dart in this face
		Dart old = faceOldestDart(dit);
		if(m_map.getDartLevel(old) < m_map.getDartLevel(oldest))
			oldest = old;
	}

	return oldest;
}

template <typename PFP>
bool Map3MR<PFP>::edgeIsSubdivided(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"edgeIsSubdivided : called with a dart inserted after current level") ;

	if(m_map.getCurrentLevel() == m_map.getMaxLevel())
		return false ;

	Dart d2 = m_map.phi2(d) ;
	m_map.incCurrentLevel() ;
	Dart d2_l = m_map.phi2(d) ;
	m_map.decCurrentLevel() ; ;
	if(d2 != d2_l)
		return true ;
	else
		return false ;
}

template <typename PFP>
bool Map3MR<PFP>::faceIsSubdivided(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"faceIsSubdivided : called with a dart inserted after current level") ;

	if(m_map.getCurrentLevel() == m_map.getMaxLevel())
		return false ;

	// a face whose level in the current level map is lower than
	// the current level can be subdivided to higher levels
	unsigned int fLevel = faceLevel(d) ;
	if(fLevel < m_map.getCurrentLevel())
		return false ;

	bool subd = false ;

	//Une face dont toute les aretes sont subdivise mais pas la face elle meme renvoie false .. sinon c'est true
	Dart dit = d;
	bool edgesAreSubdivided = true;
	do
	{
		edgesAreSubdivided &= edgeIsSubdivided(dit);
		dit = m_map.phi1(dit);
	}while(dit != d);

	if(edgesAreSubdivided)
	{
		m_map.incCurrentLevel() ;
		if(m_map.getDartLevel(m_map.phi1(m_map.phi1(d))) == m_map.getCurrentLevel())
			subd = true ;
		m_map.decCurrentLevel() ;
	}
	else
		return false;

	return subd ;
}

template <typename PFP>
bool Map3MR<PFP>::volumeIsSubdivided(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"volumeIsSubdivided : called with a dart inserted after current level") ;

	unsigned int vLevel = volumeLevel(d);
	if(vLevel <= m_map.getCurrentLevel())
		return false;

	//Test if all faces are subdivided
	bool facesAreSubdivided = faceIsSubdivided(d) ;
	Traversor3WF<typename PFP::MAP> travF(m_map, d);
	for (Dart dit = travF.begin(); dit != travF.end(); dit = travF.next())
	{
		facesAreSubdivided &= faceIsSubdivided(dit) ;
	}

	//But not the volume itself
	bool subd = false;
	m_map.incCurrentLevel();
	if(facesAreSubdivided && m_map.getDartLevel(m_map.phi2(m_map.phi1(m_map.phi1(d)))) == m_map.getCurrentLevel())
		subd = true;
	m_map.decCurrentLevel() ;
	return subd;
}

/*! @name Subdivision
 *************************************************************************/
template <typename PFP>
void Map3MR<PFP>::subdivideEdge(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"subdivideEdge : called with a dart inserted after current level") ;
	assert(!edgeIsSubdivided(d) || !"Trying to subdivide an already subdivided edge") ;

	m_map.incCurrentLevel();

	Dart nd = cutEdge(d);

	(*edgeVertexFunctor)(nd) ;

	m_map.decCurrentLevel() ;
}

template <typename PFP>
void Map3MR<PFP>::subdivideFace(Dart d, SubdivideType sType)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"subdivideFace : called with a dart inserted after current level") ;
	assert(!faceIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int fLevel = faceLevel(d) ;
	Dart old = faceOldestDart(d) ;

	m_map.pushLevel() ;
	m_map.setCurrentLevel(fLevel) ;		// go to the level of the face to subdivide its edges

	unsigned int degree = 0 ;
	Dart it = old ;
	do
	{
		++degree ;						// compute the degree of the face

		if(!edgeIsSubdivided(it))
			subdivideEdge(it) ;			// and cut the edges (if they are not already)
		it = m_map.phi1(it) ;
	} while(it != old) ;

	m_map.setCurrentLevel(fLevel + 1) ;	// go to the next level to perform face subdivision

	if(degree == 3 && sType == S_TRI)	// if subdividing a triangle
	{
		Dart dd = m_map.phi1(old) ;
		Dart e = m_map.phi1(dd) ;
		(*vertexVertexFunctor)(e) ;
		e = m_map.phi1(e) ;
		splitFace(dd, e) ;

		dd = e ;
		e = m_map.phi1(dd) ;
		(*vertexVertexFunctor)(e) ;
		e = m_map.phi1(e) ;
		splitFace(dd, e) ;

		dd = e ;
		e = m_map.phi1(dd) ;
		(*vertexVertexFunctor)(e) ;
		e = m_map.phi1(e) ;
		splitFace(dd, e) ;
	}
	else							// if subdividing a polygonal face
	{
		Dart dd = m_map.phi1(old) ;
		Dart next = m_map.phi1(dd) ;
		(*vertexVertexFunctor)(next) ;
		next = m_map.phi1(next) ;
		splitFace(dd, next) ;			// insert a first edge
		Dart ne = m_map.phi2(m_map.phi_1(dd));

		cutEdge(ne) ;					// cut the new edge to insert the central vertex

		dd = m_map.phi1(next) ;
		(*vertexVertexFunctor)(dd) ;
		dd = m_map.phi1(dd) ;
		while(dd != ne)					// turn around the face and insert new edges
		{								// linked to the central vertex
			splitFace(m_map.phi1(ne), dd) ;
			dd = m_map.phi1(dd) ;
			(*vertexVertexFunctor)(dd) ;
			dd = m_map.phi1(dd) ;
		}

		(*faceVertexFunctor)(m_map.phi1(ne)) ;
	}

	m_map.popLevel() ;
}

template <typename PFP>
void Map3MR<PFP>::subdivideVolume(Dart d)
{
	assert(m_map.getDartLevel(d) <= m_map.getCurrentLevel() || !"subdivideVolume : called with a dart inserted after current level") ;
	assert(!volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int vLevel = volumeLevel(d);
	Dart old = volumeOldestDart(d);

	m_map.pushLevel() ;
	m_map.setCurrentLevel(vLevel) ;		// go to the level of the volume to subdivide its faces

	if(m_map.getCurrentLevel() == m_map.getMaxLevel())
		m_map.addLevelBack() ;

	//
	// Subdivide Faces and Edges
	//
	Traversor3WF<typename PFP::MAP> traF(m_map, old);
	for(Dart dit = traF.begin(); dit != traF.end(); dit = traF.next())
	{
		//if needed subdivide face
		if(!faceIsSubdivided(dit))
			subdivideFace(dit);
	}

	//
	// Create inside volumes
	//
	Dart centralDart = NIL;
	std::vector<std::pair<Dart, Dart> > subdividedFaces;
	subdividedFaces.reserve(128);

	Traversor3WV<typename PFP::MAp> traWV(m_map, old);
	for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
	{
		m_map.setCurrentLevel(m_map.getMaxLevel()) ;
		m_map.template setOrbitEmbedding<VERTEX>(ditWV, EMBNULL);
		(*vertexVertexFunctor)(ditWV) ;

		Dart e = ditWV;
		std::vector<Dart> v ;

		do
		{
			v.push_back(m_map.phi1(e));
			v.push_back(m_map.phi1(m_map.phi1(e)));

			if(!Map2::isBoundaryEdge(m_map.phi1(e)))
				subdividedFaces.push_back(std::pair<Dart,Dart>(m_map.phi1(e),m_map.phi2(m_map.phi1(e))));

			if(!Map2::isBoundaryEdge(m_map.phi1(m_map.phi1(e))))
				subdividedFaces.push_back(std::pair<Dart,Dart>(m_map.phi1(m_map.phi1(e)),m_map.phi2(m_map.phi1(m_map.phi1(e)))));

			e = m_map.phi2(m_map.phi_1(e));
		}
		while(e != ditWV);

		splitSurfaceInVolume(v);

		Dart dd = m_map.phi2(m_map.phi1(ditWV));
		Dart next = m_map.phi1(m_map.phi1(dd)) ;
		//Map2::splitFace(dd, next) ;
		splitFaceInVolume(dd, next);

		Dart ne = m_map.phi2(m_map.phi_1(dd));
		//Map2::cutEdge(ne) ;
		cutEdgeInVolume(ne);
		centralDart = m_map.phi1(ne);

		dd = m_map.phi1(m_map.phi1(next)) ;
		while(dd != ne)
		{
			Dart tmp = m_map.phi1(ne) ;
			//Map2::splitFace(tmp, dd) ;
			splitFaceInVolume(tmp, dd);
			dd = m_map.phi1(m_map.phi1(dd)) ;
		}

		m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ;
	}

	std::cout << "nb Vertices incident to volume : " << i << std::endl;

	m_map.setCurrentLevel(m_map.getMaxLevel()) ;
	DartMarkerNoUnmark mf(*this);

	std::cout << "subdFaces size = " << subdividedFaces.size() << std::endl;
	for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
	{
		Dart f1 = m_map.phi2((*it).first);
		Dart f2 = m_map.phi2((*it).second);

		//if(isBoundaryFace(f1) && isBoundaryFace(f2))
		if(m_map.phi3(f1) == f1 && m_map.phi3(f2) == f2)
		//if(!mf.isMarked(phi3(f1)) && !mf.isMarked(phi3(f2)))
		{
			m_map.sewVolumes(f1, f2, false);
			mf.markOrbit<FACE>(f1);

			Dart temp = f1;
			do
			{
				propagateDartRelation(temp, m_phi3) ;
				propagateDartRelation(phi3(temp), m_phi3) ;
				temp = m_map.phi1(temp);
			}while(temp != f1);
		}
	}

	m_map.template setOrbitEmbedding<VERTEX>(centralDart, EMBNULL);
	(*volumeVertexFunctor)(centralDart) ;
	m_map.template setOrbitEmbedding<VERTEX>(centralDart, m_map.template getEmbedding<VERTEX>(centralDart));
	propagateOrbitEmbedding<VERTEX>(centralDart) ;


	//A optimiser
	m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
	TraversorE<typename PFP::MAP> travE2(m_map);
	for (Dart d = travE2.begin(); d != travE2.end(); d = travE2.next())
	{
		m_map.setCurrentLevel(m_map.getMaxLevel()) ;
		m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
		m_map.template setOrbitEmbedding<VERTEX>(m_map.phi1(d), m_map.template getEmbedding<VERTEX>(m_map.phi1(d)));
	}
	m_map.setCurrentLevel(m_map.getMaxLevel()) ;

	m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
	TraversorF<typename PFP::MAP> travF2(m_map) ;
	for (Dart d = travF2.begin(); d != travF2.end(); d = travF2.next())
	{
		m_map.setCurrentLevel(m_map.getMaxLevel()) ;
		m_map.template setOrbitEmbedding<VERTEX>(m_map.phi2(m_map.phi1(d)), map.template getEmbedding<VERTEX>(m_map.phi2(m_map.phi1(d))));
		m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
	}
	m_map.setCurrentLevel(m_map.getMaxLevel()) ;

	std::cout << std::endl;

	m_map.popLevel();
}

template <typename PFP>
void Map3MR<PFP>::subdivideVolumeTetOcta(Dart d)
{
	assert(getDartLevel(d) <= getCurrentLevel() || !"subdivideVolumeTetOcta : called with a dart inserted after current level") ;
	assert(!volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;

	unsigned int vLevel = volumeLevel(d);
	Dart old = volumeOldestDart(d);

	pushLevel() ;
	setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges

	if(getCurrentLevel() == getMaxLevel())
		addNewLevel() ;

	unsigned int j = 0;

	//
	// Subdivide Faces and Edges
	//
	Traversor3WF<Map3MR_PrimalAdapt> traF(*this, old);
	for(Dart dit = traF.begin(); dit != traF.end(); dit = traF.next())
	{
		std::cout << "CurrentLevel = " << getCurrentLevel() << std::endl;
		std::cout << "face level = " << faceLevel(dit) << std::endl;
		//std::cout << "d = " << dit << " is Subdivided ? " << faceIsSubdivided(dit) << std::endl;

		//if needed subdivide face
		if(!faceIsSubdivided(dit))
		{
			std::cout << "subdivide face = " << dit << std::endl;
			subdivideFace(dit, S_TRI);
			++j;
		}
	}

	//
	// Create inside volumes
	//
	Dart centralDart = NIL;
	bool isNotTet = false;
	Traversor3WV<Map3MR_PrimalAdapt> traV(*this, old);
	setCurrentLevel(vLevel + 1) ;
	for(Dart dit = traV.begin(); dit != traV.end(); dit = traV.next())
	{
		setOrbitEmbedding<VERTEX>(dit, EMBNULL);
		(*vertexVertexFunctor)(dit) ;

		Dart f1 = phi1(dit);
		Dart e = dit;
		std::vector<Dart> v ;

		do
		{
			v.push_back(phi1(e));
			e = phi2(phi_1(e));
		}
		while(e != dit);

		std::cout << "v size = " << v.size() << std::endl;

		splitVolume(v) ;

		//if is not a tetrahedron
		unsigned int fdeg = faceDegree(phi2(f1));
		if(fdeg > 3)
		{
			isNotTet = true;
			Dart old = phi2(phi1(dit));
			Dart dd = phi1(old) ;
			splitFace(old,dd) ;

			Dart ne = phi1(old);

			cutEdge(ne);
			centralDart = phi1(ne);
			//(*volumeVertexFunctor)(centralDart) ;
			//propagateOrbitEmbedding<VERTEX>(centralDart) ;

			Dart stop = phi2(phi1(ne));
			ne = phi2(ne);
			do
			{
				dd = phi1(phi1(ne));

				splitFace(ne, dd) ;

				ne = phi2(phi_1(ne));
				dd = phi1(dd);
			}
			while(dd != stop);
		}
	}

	//switch inner faces
	if(isNotTet)
	{
		DartMarkerStore me(*this);

		for(Dart dit = traV.begin(); dit != traV.end(); dit = traV.next())
		{
			Dart x = phi_1(phi2(phi1(dit)));
			Dart f = x;

			do
			{
				Dart f3 = phi3(f);

				if(!me.isMarked(f3))
				{
					Dart tmp =  phi_1(phi2(phi_1(phi2(phi_1(f3))))); //future voisin par phi2
					//centralDart = tmp;

					Dart f32 = phi2(f3);
					swapEdges(f3, tmp);

					me.markOrbit<EDGE>(f3);
					me.markOrbit<EDGE>(f32);
				}

				f = phi2(phi_1(f));
			}while(f != x);
		}

		setOrbitEmbedding<VERTEX>(centralDart, EMBNULL);
		setOrbitEmbedding<VERTEX>(centralDart, getEmbedding<VERTEX>(centralDart));
		(*volumeVertexFunctor)(centralDart) ;
		propagateOrbitEmbedding<VERTEX>(centralDart) ;
	}

	popLevel();
}

//void Map3MR_PrimalAdapt::subdivideVolumeTetOctaTemp(Dart d)
//{
//	assert(getDartLevel(d) <= getCurrentLevel() || !"subdivideVolumeTetOcta : called with a dart inserted after current level") ;
//	assert(!volumeIsSubdivided(d) || !"Trying to subdivide an already subdivided face") ;
//
//	unsigned int vLevel = volumeLevel(d);
//	Dart old = volumeOldestDart(d);
//
//	pushLevel() ;
//	setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges
//
//	if(getCurrentLevel() == getMaxLevel())
//		addNewLevel() ;
//
//	unsigned int j = 0;
//
//	//
//	// Subdivide Faces and Edges
//	//
//	Traversor3WF<Map3MR_PrimalAdapt> traF(*this, old);
//	for(Dart dit = traF.begin(); dit != traF.end(); dit = traF.next())
//	{
//		std::cout << "CurrentLevel = " << getCurrentLevel() << std::endl;
//		std::cout << "face level = " << faceLevel(dit) << std::endl;
//
//		//if needed subdivide face
//		if(!faceIsSubdivided(dit))
//		{
//			std::cout << "subdivide face = " << dit << std::endl;
//			subdivideFace(dit, S_TRI);
//			++j;
//		}
//	}
//
//	//
//	// Create inside volumes
//	//
//	Dart centralDart = NIL;
//	bool isNotTet = false;
//	Traversor3WV<Map3MR_PrimalAdapt> traV(*this, old);
//	setCurrentLevel(vLevel + 1) ;
//	for(Dart dit = traV.begin(); dit != traV.end(); dit = traV.next())
//	{
//		Dart f1 = phi1(dit);
//		Dart e = dit;
//		std::vector<Dart> v ;
//
//		do
//		{
//			v.push_back(phi1(e));
//			e = phi2(phi_1(e));
//		}
//		while(e != dit);
//
//		std::cout << "v size = " << v.size() << std::endl;
//
//	}
//
//
//	popLevel();
//}

} // namespace Adaptive

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN


















//void Map3MR_PrimalAdapt::saveRelationsAroundVertex(Dart d, std::vector<std::pair<Dart, Dart> >& vd)
//{
//	assert(getDartLevel(d) <= getCurrentLevel() || !"saveRelationsAroundVertex : called with a dart inserted after current level") ;
//
//	//le brin est forcement du niveau cur
//	Dart dit = d;
//
//	do
//	{
//		vd.push_back(std::pair<Dart,Dart>(dit,phi2(dit)));
//
//		dit = phi2(phi_1(dit));
//
//	}while(dit != d);
//}
//
//void Map3MR_PrimalAdapt::unsewAroundVertex(std::vector<std::pair<Dart, Dart> >& vd)
//{
//	//unsew the edge path
//	for(std::vector<std::pair<Dart, Dart> >::iterator it = vd.begin() ; it != vd.end() ; ++it)
//	{
//		Dart dit = (*it).first;
//		Dart dit2 = (*it).second;
//
//		Map2::unsewFaces(dit);
//
//		if(isOrbitEmbedded<VERTEX>())
//		{
//			copyDartEmbedding<VERTEX>(phi2(dit2), dit);
//			copyDartEmbedding<VERTEX>(phi2(dit), dit2);
//		}
//
//		if(isOrbitEmbedded<EDGE>())
//		{
//
//		}
//	}
//}
//	unsigned int vLevel = volumeLevel(d);
//	Dart old = volumeOldestDart(d);
//
//	Traversor3WV<Map3MR_PrimalAdapt> traV(*this, old);
//
//	pushLevel() ;
//	setCurrentLevel(vLevel) ;		// go to the level of the face to subdivide its edges
//
//	if(getCurrentLevel() == getMaxLevel())
//		addNewLevel() ;
//
//	//
//	// Subdivide Faces
//	//
//	std::vector<std::pair<Dart,Dart> > subdividedFaces;
//	subdividedFaces.reserve(128);
//
//	Traversor3WF<Map3MR_PrimalAdapt> traF(*this, old);
//	for(Dart dit = traF.begin(); dit != traF.end(); dit = traF.next())
//	{
//		//if needed subdivide face
//		if(!faceIsSubdivided(dit))
//			subdivideFace(dit);
//
//		//save darts from the central vertex of each subdivided face
//		incCurrentLevel() ;
//		saveRelationsAroundVertex(phi2(phi1(dit)), subdividedFaces);
//		decCurrentLevel() ;
//	}
//
//	incCurrentLevel() ;
//	unsewAroundVertex(subdividedFaces);
//	decCurrentLevel() ;
//
//	//
//	// Create inside volumes
//	//
//	Dart centralDart = NIL;
//	setCurrentLevel(vLevel + 1) ; // go to the next level to perform volume subdivision
//	for(Dart dit = traV.begin(); dit != traV.end(); dit = traV.next())
//	{
//		Map2::fillHole(phi1(dit));
//
//		Dart old = phi2(phi1(dit));
//		Dart bc = newBoundaryCycle(faceDegree(old));
//		sewVolumes(old, bc, false);
//
//		if (isOrbitEmbedded<VERTEX>())
//		{
//			Dart it = bc;
//			do
//			{
//				copyDartEmbedding<VERTEX>(it, phi1(phi3(it)));
//				it = phi1(it) ;
//			} while(it != bc) ;
//		}
//
//		Dart dd = phi1(phi1(old)) ;
//		splitFace(old,dd) ;
//
//		Dart ne = phi1(phi1(old)) ;
//
//		cutEdge(ne);
//		centralDart = phi1(ne);
//
//		Dart stop = phi2(phi1(ne));
//		ne = phi2(ne);
//		do
//		{
//			dd = phi1(phi1(phi1(ne)));
//
//			splitFace(ne, dd) ;
//
//			ne = phi2(phi_1(ne));
//			dd = phi1(phi1(dd));
//		}
//		while(dd != stop);
//	}
//
//	//
//	// Sew inside volumes
//	//
//	for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
//	{
//		Dart f1 = phi2((*it).first);
//		Dart f2 = phi2((*it).second);
//
//		if(isBoundaryFace(f1) && isBoundaryFace(f2))
//		{
//			std::cout << "plop" << std::endl;
//			sewVolumes(f1, f2, false);
//		}
//	}
//
//	(*volumeVertexFunctor)(centralDart) ;
//	popLevel() ;


