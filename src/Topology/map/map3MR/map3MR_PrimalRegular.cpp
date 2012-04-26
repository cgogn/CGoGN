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

#include "Topology/map/map3MR/map3MR_PrimalRegular.h"

namespace CGoGN
{

Map3MR_PrimalRegular::Map3MR_PrimalRegular() :
		shareVertexEmbeddings(true)
{
	initMR();
}

/*! @name Topological helping functions
 *************************************************************************/
void Map3MR_PrimalRegular::swapEdges(Dart d, Dart e)
{
	if(!Map2::isBoundaryEdge(d) && !Map2::isBoundaryEdge(e))
	{
		Dart d2 = phi2(d);
		Dart e2 = phi2(e);

		Map2::swapEdges(d,e);

		if(isOrbitEmbedded<VERTEX>())
		{
			copyDartEmbedding<VERTEX>(d, phi2(phi_1(d)));
			copyDartEmbedding<VERTEX>(e, phi2(phi_1(e)));
			copyDartEmbedding<VERTEX>(d2, phi2(phi_1(d2)));
			copyDartEmbedding<VERTEX>(e2, phi2(phi_1(e2)));
		}

		if(isOrbitEmbedded<EDGE>())
		{

		}

		if(isOrbitEmbedded<VOLUME>())
			embedNewCell<VOLUME>(d);
	}
}

bool Map3MR_PrimalRegular::isTetrahedron(Dart d)
{
	unsigned int nbFaces = 0;

	//Test the number of faces end its valency
	Traversor3WF<Map3MR_PrimalRegular> travWF(*this, d);
	for(Dart dit = travWF.begin() ; dit != travWF.end(); dit = travWF.next())
	{
		//increase the number of faces
		nbFaces++;
		if(nbFaces > 4)	//too much faces
			return false;

		//test the valency of this face
		if(faceDegree(dit) != 3)
			return false;
	}

	return true;
}



void Map3MR_PrimalRegular::splitSurfaceInVolume(std::vector<Dart>& vd, bool firstSideClosed, bool secondSideClosed)
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

		// embed the vertex embedded from the origin volume to the new darts
		if(isOrbitEmbedded<VERTEX>())
		{
			copyDartEmbedding<VERTEX>(phi2(dit), phi1(dit));
			copyDartEmbedding<VERTEX>(phi2(dit2), phi1(dit2));
		}
	}

}

void Map3MR_PrimalRegular::addNewLevelTetraOcta(bool embedNewVertices)
{
	pushLevel();

	addLevel();
	setCurrentLevel(getMaxLevel());

	if(!shareVertexEmbeddings)
	{
		//create the new level with the old one
		for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
		{
			unsigned int index = (*m_mrDarts[m_mrCurrentLevel])[i] ;
			(*m_embeddings[VERTEX])[index] = EMBNULL ;		// set vertex embedding to EMBNULL if no sharing
		}
	}

	//subdivision
	//1. cut edges
	TraversorE<Map3MR_PrimalRegular> travE(*this);
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
//		if(!shareVertexEmbeddings)
//		{
//			if(getEmbedding<VERTEX>(d) == EMBNULL)
//				embedNewCell<VERTEX>(d) ;
//			if(getEmbedding<VERTEX>(phi1(d)) == EMBNULL)
//				embedNewCell<VERTEX>(d) ;
//		}

		cutEdge(d) ;
		travE.skip(d) ;
		travE.skip(phi1(d)) ;

// When importing MR files
//		if(embedNewVertices)
//			embedNewCell<VERTEX>(phi1(d)) ;
	}

	//2. split faces - triangular faces
	TraversorF<Map3MR_PrimalRegular> travF(*this) ;
	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
	{
		Dart old = d ;
		if(getDartLevel(old) == getMaxLevel())
			old = phi1(old) ;

		Dart dd = phi1(old) ;
		Dart e = phi1(phi1(dd)) ;
		splitFace(dd, e) ;
		travF.skip(dd) ;

		dd = e ;
		e = phi1(phi1(dd)) ;
		splitFace(dd, e) ;
		travF.skip(dd) ;

		dd = e ;
		e = phi1(phi1(dd)) ;
		splitFace(dd, e) ;
		travF.skip(dd) ;

		travF.skip(e) ;
	}

	//3. create inside volumes
	setCurrentLevel(getMaxLevel() - 1) ;
	TraversorW<Map3MR_PrimalRegular> traW(*this);
	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
	{
		Traversor3WV<Map3MR_PrimalRegular> traWV(*this, dit);

		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
		{
			setCurrentLevel(getMaxLevel()) ;
			Dart f1 = phi1(ditWV);
			Dart e = ditWV;
			std::vector<Dart> v ;

			do
			{
				v.push_back(phi1(e));
				e = phi2(phi_1(e));
			}
			while(e != ditWV);

			splitVolume(v) ;

			//if is not a tetrahedron
			unsigned int fdeg = faceDegree(phi2(f1));
			if(fdeg > 3)
			{
				Dart old = phi2(phi1(ditWV));
				Dart dd = phi1(old) ;
				splitFace(old,dd) ;

				Dart ne = phi1(old);

				cutEdge(ne);

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

			setCurrentLevel(getMaxLevel() - 1) ;
		}
	}

	setCurrentLevel(getMaxLevel() - 1) ;
	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
	{
		Traversor3WV<Map3MR_PrimalRegular> traWV(*this, dit);

		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
		{
			setCurrentLevel(getMaxLevel()) ;
			Dart x = phi_1(phi2(phi1(ditWV)));

			if(!isTetrahedron(x))
			{
				DartMarkerStore me(*this);

				Dart f = x;

				do
				{
					Dart f3 = phi3(f);

					if(!me.isMarked(f3))
					{
						Dart tmp =  phi_1(phi2(phi_1(phi2(phi_1(f3))))); //future voisin par phi2

						Dart f32 = phi2(f3);
						swapEdges(f3, tmp);

						me.markOrbit<EDGE>(f3);
						me.markOrbit<EDGE>(f32);
					}

					f = phi2(phi_1(f));
				}while(f != x);

				// When importing MR files
				//if(embedNewVertices)
				//	embedNewCell<VERTEX>(x) ;
			}
			setCurrentLevel(getMaxLevel() - 1) ;
		}
	}

	popLevel() ;
}

void Map3MR_PrimalRegular::addNewLevelHexa(bool embedNewVertices)
{
	pushLevel();

	addLevel();
	setCurrentLevel(getMaxLevel());

	if(!shareVertexEmbeddings)
	{
		//create the new level with the old one
		for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
		{
			unsigned int index = (*m_mrDarts[m_mrCurrentLevel])[i] ;
			(*m_embeddings[VERTEX])[index] = EMBNULL ;		// set vertex embedding to EMBNULL if no sharing
		}
	}

	//subdivision
	//1. cut edges
	TraversorE<Map3MR_PrimalRegular> travE(*this);
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
		if(!shareVertexEmbeddings)
		{
			if(getEmbedding<VERTEX>(d) == EMBNULL)
				embedNewCell<VERTEX>(d) ;
			if(getEmbedding<VERTEX>(phi1(d)) == EMBNULL)
				embedNewCell<VERTEX>(phi1(d)) ;
		}

		cutEdge(d) ;
		travE.skip(d) ;
		travE.skip(phi1(d)) ;

// When importing MR files  : activated for DEBUG
		if(embedNewVertices)
			embedNewCell<VERTEX>(phi1(d)) ;
	}

	//2. split faces - quadrangule faces
	TraversorF<Map3MR_PrimalRegular> travF(*this) ;
	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
	{
		Dart old = d;
		if(getDartLevel(old) == getMaxLevel())
			old = phi1(old) ;

		Dart dd = phi1(old) ;
		Dart next = phi1(phi1(dd)) ;
		splitFace(dd, next) ;		// insert a first edge

		Dart ne = phi2(phi_1(dd)) ;
		cutEdge(ne) ;				// cut the new edge to insert the central vertex
		travF.skip(dd) ;

// When importing MR files : activated for DEBUG
		if(embedNewVertices)
			embedNewCell<VERTEX>(phi1(ne)) ;

		dd = phi1(phi1(next)) ;
		while(dd != ne)				// turn around the face and insert new edges
		{							// linked to the central vertex
			Dart tmp = phi1(ne) ;
			splitFace(tmp, dd) ;
			travF.skip(tmp) ;
			dd = phi1(phi1(dd)) ;
		}
		travF.skip(ne) ;
	}

	//3. create inside volumes
	setCurrentLevel(getMaxLevel() - 1) ;
	TraversorW<Map3MR_PrimalRegular> traW(*this);
	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
	{
		std::vector<std::pair<Dart, Dart> > subdividedFaces;
		subdividedFaces.reserve(64);
		Dart centralDart = NIL;

		Traversor3WV<Map3MR_PrimalRegular> traWV(*this, dit);
		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
		{
			setCurrentLevel(getMaxLevel()) ;	//Utile ?

			Dart e = ditWV;
			std::vector<Dart> v ;

			do
			{
				v.push_back(phi1(e));
				v.push_back(phi1(phi1(e)));

				if(!Map2::isBoundaryEdge(phi1(e)))
					subdividedFaces.push_back(std::pair<Dart,Dart>(phi1(e),phi2(phi1(e))));

				if(!Map2::isBoundaryEdge(phi1(phi1(e))))
					subdividedFaces.push_back(std::pair<Dart,Dart>(phi1(phi1(e)),phi2(phi1(phi1(e)))));

				e = phi2(phi_1(e));
			}
			while(e != ditWV);

			splitSurfaceInVolume(v);

			Dart dd = phi2(phi1(ditWV));
			Dart next = phi1(phi1(dd)) ;
			Map2::splitFace(dd, next) ;

			Dart ne = phi2(phi_1(dd));
			Map2::cutEdge(ne) ;
			centralDart = phi1(ne);

			dd = phi1(phi1(next)) ;
			while(dd != ne)
			{
				Dart tmp = phi1(ne) ;
				Map2::splitFace(tmp, dd) ;
				dd = phi1(phi1(dd)) ;
			}

			setCurrentLevel(getMaxLevel() - 1) ; //Utile ?
		}

		setCurrentLevel(getMaxLevel()) ;
		//4 couture des relations precedemment sauvegarde
		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
		{
			Dart f1 = phi2((*it).first);
			Dart f2 = phi2((*it).second);

			//if(isBoundaryFace(f1) && isBoundaryFace(f2))
			if(phi3(f1) == f1 && phi3(f2) == f2)
				sewVolumes(f1, f2, false);
		}
		embedOrbit<VERTEX>(centralDart, getEmbedding<VERTEX>(centralDart));

		setCurrentLevel(getMaxLevel() - 1) ;
	}

	//A optimiser
	//setCurrentLevel(getMaxLevel()-1) ;
	TraversorE<Map3MR_PrimalRegular> travE2(*this);
	for (Dart d = travE2.begin(); d != travE2.end(); d = travE2.next())
	{
		setCurrentLevel(getMaxLevel()) ;
		embedOrbit<VERTEX>(phi1(d), getEmbedding<VERTEX>(phi1(d)));
		setCurrentLevel(getMaxLevel()-1) ;
	}
	//setCurrentLevel(getMaxLevel()) ;

	//setCurrentLevel(getMaxLevel()-1) ;
	TraversorF<Map3MR_PrimalRegular> travF2(*this) ;
	for (Dart d = travF2.begin(); d != travF2.end(); d = travF2.next())
	{
		setCurrentLevel(getMaxLevel()) ;
		embedOrbit<VERTEX>(phi2(phi1(d)), getEmbedding<VERTEX>(phi2(phi1(d))));
		setCurrentLevel(getMaxLevel()-1) ;
	}
	//setCurrentLevel(getMaxLevel()) ;

	popLevel() ;
}

void Map3MR_PrimalRegular::addNewLevel(bool embedNewVertices)
{
	pushLevel();

	addLevel();
	setCurrentLevel(getMaxLevel());

	//create the new level with the old one
	for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
	{
		unsigned int newindex = copyDartLine((*m_mrDarts[m_mrCurrentLevel])[i]) ;	// duplicate all darts
		(*m_mrDarts[m_mrCurrentLevel])[i] = newindex ;								// on the new max level
		if(!shareVertexEmbeddings)
			(*m_embeddings[VERTEX])[newindex] = EMBNULL ;	// set vertex embedding to EMBNULL if no sharing
	}

	//subdivision
	//1. cut edges
	TraversorE<Map3MR_PrimalRegular> travE(*this);
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
		if(!shareVertexEmbeddings)
		{
			if(getEmbedding<VERTEX>(d) == EMBNULL)
				embedNewCell<VERTEX>(d) ;
			if(getEmbedding<VERTEX>(phi1(d)) == EMBNULL)
				embedNewCell<VERTEX>(d) ;
		}

		cutEdge(d) ;
		travE.skip(d) ;
		travE.skip(phi1(d)) ;

// When importing MR files  : activated for DEBUG
		if(embedNewVertices)
			embedNewCell<VERTEX>(phi1(d)) ;
	}

	//2. split faces - quadrangule faces
	TraversorF<Map3MR_PrimalRegular> travF(*this) ;
	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
	{
		Dart old = d;
		if(getDartLevel(old) == getMaxLevel())
			old = phi1(old) ;

		decCurrentLevel() ;
		unsigned int degree = faceDegree(old) ;
		incCurrentLevel() ;

		if(degree == 3)					// if subdividing a triangle
		{
			Dart dd = phi1(old) ;
			Dart e = phi1(phi1(dd)) ;
			splitFace(dd, e) ;
			travF.skip(dd) ;

			dd = e ;
			e = phi1(phi1(dd)) ;
			splitFace(dd, e) ;
			travF.skip(dd) ;

			dd = e ;
			e = phi1(phi1(dd)) ;
			splitFace(dd, e) ;
			travF.skip(dd) ;

			travF.skip(e) ;
		}
		else							// if subdividing a polygonal face
		{
			Dart dd = phi1(old) ;
			Dart next = phi1(phi1(dd)) ;
			splitFace(dd, next) ;		// insert a first edge

			Dart ne = phi2(phi_1(dd)) ;
			cutEdge(ne) ;				// cut the new edge to insert the central vertex
			travF.skip(dd) ;

			// When importing MR files : activated for DEBUG
			if(embedNewVertices)
				embedNewCell<VERTEX>(phi1(ne)) ;

			dd = phi1(phi1(next)) ;
			while(dd != ne)				// turn around the face and insert new edges
			{							// linked to the central vertex
				Dart tmp = phi1(ne) ;
				splitFace(tmp, dd) ;
				travF.skip(tmp) ;
				dd = phi1(phi1(dd)) ;
			}
			travF.skip(ne) ;
		}
	}

	//3. create inside volumes
	setCurrentLevel(getMaxLevel() - 1) ;
	TraversorW<Map3MR_PrimalRegular> traW(*this);
	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
	{
		std::vector<std::pair<Dart, Dart> > subdividedFaces;
		subdividedFaces.reserve(64);
		Dart centralDart = NIL;

		Traversor3WV<Map3MR_PrimalRegular> traWV(*this, dit);
		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
		{
			setCurrentLevel(getMaxLevel()) ;	//Utile ?

			Dart e = ditWV;
			std::vector<Dart> v ;

			do
			{
				if(phi1(phi1(phi1(e))) != e)
				{
					v.push_back(phi1(phi1(e)));

					if(!Map2::isBoundaryEdge(phi1(phi1(e))))
						subdividedFaces.push_back(std::pair<Dart,Dart>(phi1(phi1(e)),phi2(phi1(phi1(e)))));
				}

				v.push_back(phi1(e));

				if(!Map2::isBoundaryEdge(phi1(e)))
					subdividedFaces.push_back(std::pair<Dart,Dart>(phi1(e),phi2(phi1(e))));


				e = phi2(phi_1(e));
			}
			while(e != ditWV);

			splitSurfaceInVolume(v,true,true);

//			Dart dd = phi2(phi1(ditWV));
//			Dart next = phi1(phi1(dd)) ;
//			Map2::splitFace(dd, next) ;
//
//			Dart ne = phi2(phi_1(dd));
//			Map2::cutEdge(ne) ;
//			centralDart = phi1(ne);
//
//			dd = phi1(phi1(next)) ;
//			while(dd != ne)
//			{
//				Dart tmp = phi1(ne) ;
//				Map2::splitFace(tmp, dd) ;
//				dd = phi1(phi1(dd)) ;
//			}

			setCurrentLevel(getMaxLevel() - 1) ; //Utile ?
		}

		setCurrentLevel(getMaxLevel()) ;
		//4 couture des relations precedemment sauvegarde
		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
		{
			Dart f1 = phi2((*it).first);
			Dart f2 = phi2((*it).second);

			//closeHole(f1);
		}
		setCurrentLevel(getMaxLevel() - 1) ;

//		setCurrentLevel(getMaxLevel()) ;
//		//4 couture des relations precedemment sauvegarde
//		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
//		{
//			Dart f1 = phi2((*it).first);
//			Dart f2 = phi2((*it).second);
//
//			//if(isBoundaryFace(f1) && isBoundaryFace(f2))
//			if(phi3(f1) == f1 && phi3(f2) == f2)
//				sewVolumes(f1, f2, false);
//		}
//		embedOrbit<VERTEX>(centralDart, getEmbedding<VERTEX>(centralDart));
//
//		setCurrentLevel(getMaxLevel() - 1) ;
	}

	//A optimiser
	setCurrentLevel(getMaxLevel()-1) ;
	TraversorE<Map3MR_PrimalRegular> travE2(*this);
	for (Dart d = travE2.begin(); d != travE2.end(); d = travE2.next())
	{
		setCurrentLevel(getMaxLevel()) ;
		embedOrbit<VERTEX>(phi1(d), getEmbedding<VERTEX>(phi1(d)));
		setCurrentLevel(getMaxLevel()-1) ;
	}
	setCurrentLevel(getMaxLevel()) ;

//	setCurrentLevel(getMaxLevel()-1) ;
//	TraversorF<Map3MR_PrimalRegular> travF2(*this) ;
//	for (Dart d = travF2.begin(); d != travF2.end(); d = travF2.next())
//	{
//		if(!faceDegree(d) != 3)
//		{
//			setCurrentLevel(getMaxLevel()) ;
//			embedOrbit<VERTEX>(phi2(phi1(d)), getEmbedding<VERTEX>(phi2(phi1(d))));
//			setCurrentLevel(getMaxLevel()-1) ;
//		}
//	}
//	setCurrentLevel(getMaxLevel()) ;

	popLevel() ;
}

void Map3MR_PrimalRegular::analysis()
{
	assert(getCurrentLevel() > 0 || !"analysis : called on level 0") ;

	decCurrentLevel() ;

	for(unsigned int i = 0; i < analysisFilters.size(); ++i)
		(*analysisFilters[i])() ;
}

void Map3MR_PrimalRegular::synthesis()
{
	assert(getCurrentLevel() < getMaxLevel() || !"synthesis : called on max level") ;

	for(unsigned int i = 0; i < synthesisFilters.size(); ++i)
		(*synthesisFilters[i])() ;

	incCurrentLevel() ;
}


void Map3MR_PrimalRegular::subdivision()
{
	assert(getCurrentLevel() < getMaxLevel() || !"subdivision: called on max level") ;

	for(unsigned int i = 0; i < subdivisionSchemes.size(); ++i)
		(*subdivisionSchemes[i])() ;

	incCurrentLevel() ;
}

} //end namespace CGoGN

//	setCurrentLevel(getMaxLevel()-1) ;
//	//3.1 sauvegarde des relations
//	std::vector<std::pair<Dart,Dart> > subdividedFaces;
//	subdividedFaces.reserve(1024);
//	TraversorF<Map3MR_PrimalRegular> travF2(*this) ;
//	for(Dart dit = travF2.begin(); dit != travF2.end(); dit = travF2.next())
//	{
//		setCurrentLevel(getMaxLevel()) ;
//		saveRelationsAroundVertex(phi2(phi1(dit)), subdividedFaces);
//		setCurrentLevel(getMaxLevel()-1) ;
//	}
//
//	//3.2 decoupe des faces
//	setCurrentLevel(getMaxLevel()) ;
//	unsewAroundVertex(subdividedFaces);
//	setCurrentLevel(getMaxLevel()-1) ;

//	//3.3 quadrangulation des faces interieurs
//	TraversorV<Map3MR_PrimalRegular> travV(*this) ;
//	for (Dart dit = travV.begin(); dit != travV.end(); dit = travV.next())
//	{
//		setCurrentLevel(getMaxLevel()) ;
//
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
//
//		if(embedNewVertices)
//			embedNewCell<VERTEX>(phi1(ne)) ;
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


//		quadranguleFace(dit);
//		setCurrentLevel(getMaxLevel() - 1) ;
//	}

//	//4 couture des relations precedemment sauvegarde
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


//void Map3MR_PrimalRegular::splitFaceInSurface(Dart d, Dart e)
//{
//	Map2::splitFace(d, e) ;
//
//	if (isOrbitEmbedded<VERTEX>())
//	{
//		copyDartEmbedding<VERTEX>(phi_1(e), d) ;
//		copyDartEmbedding<VERTEX>(phi_1(d), e) ;
//	}
//	if (isOrbitEmbedded<FACE>())
//	{
//		copyDartEmbedding<FACE>(phi_1(d), d) ;
//		embedNewCell<FACE>(e) ;
//		copyCell<FACE>(e, d) ;
//	}
//}
//
//Dart Map3MR_PrimalRegular::cutEdgeInSurface(Dart d)
//{
//	Dart nd = Map2::cutEdge(d) ;
//
//	if (isOrbitEmbedded<EDGE>())
//	{
//		copyDartEmbedding<EDGE>(phi2(d), d) ;
//		embedNewCell<EDGE>(nd) ;
//		copyCell<EDGE>(nd, d) ;
//	}
//
//	if(isOrbitEmbedded<FACE>())
//	{
//		copyDartEmbedding<FACE>(nd, d) ;
//		Dart e = phi2(nd) ;
//		copyDartEmbedding<FACE>(phi1(e), e) ;
//	}
//
//	return nd;
//}

//void Map3MR_PrimalRegular::saveRelationsAroundVertex(Dart d, std::vector<std::pair<Dart, Dart> >& vd)
//{
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
//void Map3MR_PrimalRegular::unsewAroundVertex(std::vector<std::pair<Dart, Dart> >& vd)
//{
//	//unsew the edge path
//	for(std::vector<std::pair<Dart, Dart> >::iterator it = vd.begin() ; it != vd.end() ; ++it)
//	{
//		Dart dit = (*it).first;
//		Dart dit2 = (*it).second;
//
//		//if(!Map2::isBoundaryFace(dit))
//		//{
//			Map2::unsewFaces(dit);
//
//			if(isOrbitEmbedded<VERTEX>())
//			{
//				copyDartEmbedding<VERTEX>(phi2(dit2), dit);
//				copyDartEmbedding<VERTEX>(phi2(dit), dit2);
//			}
//
//			if(isOrbitEmbedded<EDGE>())
//			{
//
//			}
//		//}
//	}
//}
//
//Dart Map3MR_PrimalRegular::quadranguleFace(Dart d)
//{
//	Dart centralDart = NIL;
//
//	Map2::fillHole(phi1(d));
//
//	Dart old = phi2(phi1(d));
//	Dart bc = newBoundaryCycle(faceDegree(old));
//	sewVolumes(old, bc, false);
//
//	if (isOrbitEmbedded<VERTEX>())
//	{
//		Dart it = bc;
//		do
//		{
//			//copyDartEmbedding<VERTEX>(it, phi1(phi3(it)));
//			embedOrbit<VERTEX>(it, getEmbedding<VERTEX>(phi1(phi3(it))));
//			it = phi1(it) ;
//		} while(it != bc) ;
//	}
//
//	Dart dd = phi1(phi1(old)) ;
//	splitFace(old,dd) ;
//
//	Dart ne = phi1(phi1(old)) ;
//
//	cutEdge(ne);
//	centralDart = phi1(ne);
//
//	Dart stop = phi2(phi1(ne));
//	ne = phi2(ne);
//	do
//	{
//		dd = phi1(phi1(phi1(ne)));
//
//		splitFace(ne, dd) ;
//
//		ne = phi2(phi_1(ne));
//		dd = phi1(phi1(dd));
//	}
//	while(dd != stop);
//
//	return centralDart;
//}



