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

namespace CGoGN
{

namespace Algo
{

namespace MR
{

namespace Primal
{

namespace Regular
{

template <typename PFP>
Map3MR<PFP>::Map3MR(typename PFP::MAP& map) :
	m_map(map),
	shareVertexEmbeddings(true)
{

}

template <typename PFP>
Map3MR<PFP>::~Map3MR()
{
	unsigned int level = m_map.getCurrentLevel();
	unsigned int maxL = m_map.getMaxLevel();

	for(unsigned int i = maxL ; i > level ; --i)
		m_map.removeLevelBack();

	for(unsigned int i = 0 ; i < level ; ++i)
		m_map.removeLevelFront();
}


/************************************************************************
 *					Topological helping functions						*
 ************************************************************************/
template <typename PFP>
void Map3MR<PFP>::swapEdges(Dart d, Dart e)
{
	if(!m_map.PFP::MAP::ParentMap::isBoundaryEdge(d) && !m_map.PFP::MAP::ParentMap::isBoundaryEdge(e))
	{
		Dart d2 = m_map.phi2(d);
		Dart e2 = m_map.phi2(e);

		m_map.PFP::MAP::ParentMap::swapEdges(d,e);

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
			m_map.template embedNewCell<VOLUME>(d);
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
		vd2.push_back(m_map.phi2(*it));
	}

	assert(vd2.size() == vd.size());

	m_map.PFP::MAP::ParentMap::splitSurface(vd, firstSideClosed, secondSideClosed);

	// follow the edge path a second time to embed the vertex, edge and volume orbits
	for(unsigned int i = 0; i < vd.size(); ++i)
	{
		Dart dit = vd[i];
		Dart dit2 = vd2[i];

		// embed the vertex embedded from the origin volume to the new darts
		if(m_map.template isOrbitEmbedded<VERTEX>())
		{
			m_map.template copyDartEmbedding<VERTEX>(m_map.phi2(dit), m_map.phi1(dit));
			m_map.template copyDartEmbedding<VERTEX>(m_map.phi2(dit2), m_map.phi1(dit2));
		}
	}
}

/************************************************************************
 * 							Level creation								*
 ************************************************************************/
template <typename PFP>
void Map3MR<PFP>::addNewLevelSqrt3(bool embedNewVertices)
{
	m_map.pushLevel();

	m_map.addLevelBack();
	m_map.duplicateDarts(m_map.getMaxLevel());
	m_map.setCurrentLevel(m_map.getMaxLevel());

	DartMarkerStore m(m_map);

	//
	// 1-4 flip of all tetrahedra
	//
	TraversorW<typename PFP::MAP> tW(m_map);
	for(Dart dit = tW.begin() ; dit != tW.end() ; dit = tW.next())
	{
		Traversor3WF<typename PFP::MAP> tWF(m_map, dit);
		for(Dart ditWF = tWF.begin() ; ditWF != tWF.end() ; ditWF = tWF.next())
		{
			if(!m_map.isBoundaryFace(ditWF))
				m.markOrbit<FACE>(ditWF);
		}

		Algo::Modelisation::Tetrahedralization::flip1To4<PFP>(m_map, dit);
	}

/*
	//
	// 2-3 swap of all old interior faces
	//
	TraversorF<typename PFP::MAP> tF(m_map);
	for(Dart dit = tF.begin() ; dit != tF.end() ; dit = tF.next())
	{
		if(m.isMarked(dit))
		{
			m.unmarkOrbit<FACE>(dit);
			Algo::Modelisation::Tetrahedralization::swap2To3<PFP>(m_map, dit);
		}
	}

	//
	// 1-3 flip of all boundary tetrahedra
	//
	TraversorW<typename PFP::MAP> tWb(m_map);
	for(Dart dit = tWb.begin() ; dit != tWb.end() ; dit = tWb.next())
	{
		if(m_map.isBoundaryVolume(dit))
		{
			Traversor3WE<typename PFP::MAP> tWE(m_map, dit);
			for(Dart ditWE = tWE.begin() ; ditWE != tWE.end() ; ditWE = tWE.next())
			{
				if(m_map.isBoundaryEdge(ditWE))
					m.markOrbit<EDGE>(ditWE);
			}

			Algo::Modelisation::Tetrahedralization::flip1To3<PFP>(m_map, dit);
		}
	}

	//
	// edge-removal on all old boundary edges
	//
	TraversorE<typename PFP::MAP> tE(m_map);
	for(Dart dit = tE.begin() ; dit != tE.end() ; dit = tE.next())
	{
		if(m.isMarked(dit))
		{
			m.unmarkOrbit<EDGE>(dit);
			Dart d = m_map.phi2(m_map.phi3(m_map.findBoundaryFaceOfEdge(dit)));
			Algo::Modelisation::Tetrahedralization::swapGen3To2<PFP>(m_map, d);

		}
	}
*/

	m_map.setCurrentLevel(m_map.getMaxLevel());
	m_map.popLevel() ;
}

template <typename PFP>
void Map3MR<PFP>::addNewLevelTetraOcta(bool embedNewVertices)
{
	m_map.pushLevel();

	m_map.addLevelBack();
	m_map.duplicateDarts(m_map.getMaxLevel());
	m_map.setCurrentLevel(m_map.getMaxLevel());

//	if(!shareVertexEmbeddings)
//	{
//		//create the new level with the old one
//		for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
//		{
//			unsigned int index = (*m_mrDarts[m_mrCurrentLevel])[i] ;
//			(*m_embeddings[VERTEX])[index] = EMBNULL ;		// set vertex embedding to EMBNULL if no sharing
//		}
//	}

	//subdivision
	//1. cut edges
	TraversorE<typename PFP::MAP> travE(m_map);
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
//		if(!shareVertexEmbeddings)
//		{
//			if(getEmbedding<VERTEX>(d) == EMBNULL)
//				embedNewCell<VERTEX>(d) ;
//			if(getEmbedding<VERTEX>(phi1(d)) == EMBNULL)
//				embedNewCell<VERTEX>(d) ;
//		}

		m_map.cutEdge(d) ;
		travE.skip(d) ;
		travE.skip(m_map.phi1(d)) ;

// When importing MR files
//		if(embedNewVertices)
//			m_map.template embedNewCell<VERTEX>(m_map.phi1(d)) ;
	}

	//2. split faces - triangular faces
	TraversorF<typename PFP::MAP> travF(m_map) ;
	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
	{
		Dart old = d ;
		if(m_map.getDartLevel(old) == m_map.getMaxLevel())
			old = m_map.phi1(old) ;

		Dart dd = m_map.phi1(old) ;
		Dart e = m_map.phi1(m_map.phi1(dd)) ;
		m_map.splitFace(dd, e) ;
		travF.skip(dd) ;

		dd = e ;
		e = m_map.phi1(m_map.phi1(dd)) ;
		m_map.splitFace(dd, e) ;
		travF.skip(dd) ;

		dd = e ;
		e = m_map.phi1(m_map.phi1(dd)) ;
		m_map.splitFace(dd, e) ;
		travF.skip(dd) ;

		travF.skip(e) ;
	}

	//3. create inside volumes
	m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ;
	TraversorW<typename PFP::MAP> traW(m_map);
	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
	{
		Traversor3WV<typename PFP::MAP> traWV(m_map, dit);

		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
		{
			m_map.setCurrentLevel(m_map.getMaxLevel()) ;
			Dart f1 = m_map.phi1(ditWV);
			Dart e = ditWV;
			std::vector<Dart> v ;

			do
			{
				v.push_back(m_map.phi1(e));
				e = m_map.phi2(m_map.phi_1(e));
			}
			while(e != ditWV);

			m_map.splitVolume(v) ;

			//if is not a tetrahedron
			unsigned int fdeg = m_map.faceDegree(m_map.phi2(f1));
			if(fdeg > 3)
			{
				Dart old = m_map.phi2(m_map.phi1(ditWV));
				Dart dd = m_map.phi1(old) ;
				m_map.splitFace(old,dd) ;

				Dart ne = m_map.phi1(old);

				m_map.cutEdge(ne);

				Dart stop = m_map.phi2(m_map.phi1(ne));
				ne = m_map.phi2(ne);
				do
				{
					dd = m_map.phi1(m_map.phi1(ne));

					m_map.splitFace(ne, dd) ;

					ne = m_map.phi2(m_map.phi_1(ne));
					dd = m_map.phi1(dd);
				}
				while(dd != stop);
			}

			m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ;
		}
	}

	m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ;
	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
	{
		Traversor3WV<typename PFP::MAP> traWV(m_map, dit);

		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
		{
			m_map.setCurrentLevel(m_map.getMaxLevel()) ;
			Dart x = m_map.phi_1(m_map.phi2(m_map.phi1(ditWV)));

			if(!Algo::Modelisation::Tetrahedralization::isTetrahedron<PFP>(m_map,x))
			{
				DartMarkerStore me(m_map);

				Dart f = x;

				do
				{
					Dart f3 = m_map.phi3(f);

					if(!me.isMarked(f3))
					{
						Dart tmp =  m_map.phi_1(m_map.phi2(m_map.phi_1(m_map.phi2(m_map.phi_1(f3))))); //future voisin par phi2

						Dart f32 = m_map.phi2(f3);
						swapEdges(f3, tmp);

						me.markOrbit<EDGE>(f3);
						me.markOrbit<EDGE>(f32);
					}

					f = m_map.phi2(m_map.phi_1(f));
				}while(f != x);

				// When importing MR files
				//if(embedNewVertices)
				//	m_map.template embedNewCell<VERTEX>(x) ;
			}
			m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ;
		}
	}

	m_map.popLevel() ;
}

template <typename PFP>
void Map3MR<PFP>::addNewLevelHexa(bool embedNewVertices)
{
	m_map.pushLevel();

	m_map.addLevelBack();
	m_map.duplicateDarts(m_map.getMaxLevel());
	m_map.setCurrentLevel(m_map.getMaxLevel());

	//1. cut edges
	TraversorE<typename PFP::MAP> travE(m_map);
	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
	{
		if(!shareVertexEmbeddings && embedNewVertices)
		{
			if(m_map.template getEmbedding<VERTEX>(d) == EMBNULL)
				m_map.template embedNewCell<VERTEX>(d) ;
			if(m_map.template getEmbedding<VERTEX>(m_map.phi1(d)) == EMBNULL)
				m_map.template embedNewCell<VERTEX>(d) ;
		}

		m_map.cutEdge(d) ;
		travE.skip(d) ;
		travE.skip(m_map.phi1(d)) ;

		if(embedNewVertices)
			m_map.template embedNewCell<VERTEX>(m_map.phi1(d)) ;
	}
	std::cout << "current Level = " << m_map.getCurrentLevel() << std::endl;

	//2. split faces - quadrangule faces
	TraversorF<typename PFP::MAP> travF(m_map) ;
	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
	{
		Dart old = d;
		if(m_map.getDartLevel(old) == m_map.getMaxLevel())
			old = m_map.phi1(old) ;

		Dart dd = m_map.phi1(old) ;
		Dart next = m_map.phi1(m_map.phi1(dd)) ;
		m_map.splitFace(dd, next) ;		// insert a first edge

		Dart ne = m_map.phi2(m_map.phi_1(dd)) ;
		m_map.cutEdge(ne) ;				// cut the new edge to insert the central vertex
		travF.skip(dd) ;

		if(embedNewVertices)
			m_map.template embedNewCell<VERTEX>(m_map.phi1(ne)) ;

		dd = m_map.phi1(m_map.phi1(next)) ;
		while(dd != ne)				// turn around the face and insert new edges
		{							// linked to the central vertex
			Dart tmp = m_map.phi1(ne) ;
			m_map.splitFace(tmp, dd) ;
			travF.skip(tmp) ;
			dd = m_map.phi1(m_map.phi1(dd)) ;
		}
		travF.skip(ne) ;
	}

	//3. create inside volumes
	m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ;
	TraversorW<typename PFP::MAP> traW(m_map);
	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
	{
		std::vector<std::pair<Dart, Dart> > subdividedFaces;
		subdividedFaces.reserve(64);
		Dart centralDart = NIL;

		Traversor3WV<typename PFP::MAP> traWV(m_map, dit);
		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
		{
			m_map.setCurrentLevel(m_map.getMaxLevel()) ;	//Utile ?

			Dart e = ditWV;
			std::vector<Dart> v ;

			do
			{
				v.push_back(m_map.phi1(e));
				v.push_back(m_map.phi1(m_map.phi1(e)));

				if(!m_map.PFP::MAP::ParentMap::isBoundaryEdge(m_map.phi1(e)))
					subdividedFaces.push_back(std::pair<Dart,Dart>(m_map.phi1(e),m_map.phi2(m_map.phi1(e))));

				if(!m_map.PFP::MAP::ParentMap::isBoundaryEdge(m_map.phi1(m_map.phi1(e))))
					subdividedFaces.push_back(std::pair<Dart,Dart>(m_map.phi1(m_map.phi1(e)),m_map.phi2(m_map.phi1(m_map.phi1(e)))));

				e = m_map.phi2(m_map.phi_1(e));
			}
			while(e != ditWV);

			splitSurfaceInVolume(v);

			Dart dd = m_map.phi2(m_map.phi1(ditWV));
			Dart next = m_map.phi1(m_map.phi1(dd)) ;
			m_map.PFP::MAP::ParentMap::splitFace(dd, next) ;

			Dart ne = m_map.phi2(m_map.phi_1(dd));
			m_map.PFP::MAP::ParentMap::cutEdge(ne) ;
			centralDart = m_map.phi1(ne);

			dd = m_map.phi1(m_map.phi1(next)) ;
			while(dd != ne)
			{
				Dart tmp = m_map.phi1(ne) ;
				m_map.PFP::MAP::ParentMap::splitFace(tmp, dd) ;
				dd = m_map.phi1(m_map.phi1(dd)) ;
			}

			m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ; //Utile ?
		}

		m_map.setCurrentLevel(m_map.getMaxLevel()) ;
		//4 couture des relations precedemment sauvegarde
		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
		{
			Dart f1 = m_map.phi2((*it).first);
			Dart f2 = m_map.phi2((*it).second);

			//if(isBoundaryFace(f1) && isBoundaryFace(f2))
			if(m_map.phi3(f1) == f1 && m_map.phi3(f2) == f2)
				m_map.sewVolumes(f1, f2, false);
		}
		m_map.template embedOrbit<VERTEX>(centralDart, m_map.template getEmbedding<VERTEX>(centralDart));

		m_map.setCurrentLevel(m_map.getMaxLevel() - 1) ;
	}

	//A optimiser
	m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
	TraversorE<typename PFP::MAP> travE2(m_map);
	for (Dart d = travE2.begin(); d != travE2.end(); d = travE2.next())
	{
		m_map.setCurrentLevel(m_map.getMaxLevel()) ;
		m_map.template embedOrbit<VERTEX>(m_map.phi1(d), m_map.template getEmbedding<VERTEX>(m_map.phi1(d)));
		m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
	}
	m_map.setCurrentLevel(m_map.getMaxLevel()) ;

	m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
	TraversorF<typename PFP::MAP> travF2(m_map) ;
	for (Dart d = travF2.begin(); d != travF2.end(); d = travF2.next())
	{
		m_map.setCurrentLevel(m_map.getMaxLevel()) ;
		m_map.template embedOrbit<VERTEX>(m_map.phi2(m_map.phi1(d)), m_map.template getEmbedding<VERTEX>(m_map.phi2(m_map.phi1(d))));
		m_map.setCurrentLevel(m_map.getMaxLevel()-1) ;
	}
	m_map.setCurrentLevel(m_map.getMaxLevel()) ;

	m_map.popLevel() ;
}

//void Map3MR_PrimalRegular::addNewLevel(bool embedNewVertices)
//{
//	pushLevel();
//
//	addLevel();
//	setCurrentLevel(getMaxLevel());
//
//	//create the new level with the old one
//	for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
//	{
//		unsigned int newindex = copyDartLine((*m_mrDarts[m_mrCurrentLevel])[i]) ;	// duplicate all darts
//		(*m_mrDarts[m_mrCurrentLevel])[i] = newindex ;								// on the new max level
//		if(!shareVertexEmbeddings)
//			(*m_embeddings[VERTEX])[newindex] = EMBNULL ;	// set vertex embedding to EMBNULL if no sharing
//	}
//
//	//subdivision
//	//1. cut edges
//	TraversorE<Map3MR_PrimalRegular> travE(*this);
//	for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
//	{
//		if(!shareVertexEmbeddings)
//		{
//			if(getEmbedding<VERTEX>(d) == EMBNULL)
//				embedNewCell<VERTEX>(d) ;
//			if(getEmbedding<VERTEX>(phi1(d)) == EMBNULL)
//				embedNewCell<VERTEX>(d) ;
//		}
//
//		cutEdge(d) ;
//		travE.skip(d) ;
//		travE.skip(phi1(d)) ;
//
//// When importing MR files  : activated for DEBUG
//		if(embedNewVertices)
//			embedNewCell<VERTEX>(phi1(d)) ;
//	}
//
//	//2. split faces - quadrangule faces
//	TraversorF<Map3MR_PrimalRegular> travF(*this) ;
//	for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
//	{
//		Dart old = d;
//		if(getDartLevel(old) == getMaxLevel())
//			old = phi1(old) ;
//
//		decCurrentLevel() ;
//		unsigned int degree = faceDegree(old) ;
//		incCurrentLevel() ;
//
//		if(degree == 3)					// if subdividing a triangle
//		{
//			Dart dd = phi1(old) ;
//			Dart e = phi1(phi1(dd)) ;
//			splitFace(dd, e) ;
//			travF.skip(dd) ;
//
//			dd = e ;
//			e = phi1(phi1(dd)) ;
//			splitFace(dd, e) ;
//			travF.skip(dd) ;
//
//			dd = e ;
//			e = phi1(phi1(dd)) ;
//			splitFace(dd, e) ;
//			travF.skip(dd) ;
//
//			travF.skip(e) ;
//		}
//		else							// if subdividing a polygonal face
//		{
//			Dart dd = phi1(old) ;
//			Dart next = phi1(phi1(dd)) ;
//			splitFace(dd, next) ;		// insert a first edge
//
//			Dart ne = phi2(phi_1(dd)) ;
//			cutEdge(ne) ;				// cut the new edge to insert the central vertex
//			travF.skip(dd) ;
//
//			// When importing MR files : activated for DEBUG
//			if(embedNewVertices)
//				embedNewCell<VERTEX>(phi1(ne)) ;
//
//			dd = phi1(phi1(next)) ;
//			while(dd != ne)				// turn around the face and insert new edges
//			{							// linked to the central vertex
//				Dart tmp = phi1(ne) ;
//				splitFace(tmp, dd) ;
//				travF.skip(tmp) ;
//				dd = phi1(phi1(dd)) ;
//			}
//			travF.skip(ne) ;
//		}
//	}
//
//	//3. create inside volumes
//	setCurrentLevel(getMaxLevel() - 1) ;
//	TraversorW<Map3MR_PrimalRegular> traW(*this);
//	for(Dart dit = traW.begin(); dit != traW.end(); dit = traW.next())
//	{
//		std::vector<std::pair<Dart, Dart> > subdividedFaces;
//		subdividedFaces.reserve(64);
//		Dart centralDart = NIL;
//
//		Traversor3WV<Map3MR_PrimalRegular> traWV(*this, dit);
//		for(Dart ditWV = traWV.begin(); ditWV != traWV.end(); ditWV = traWV.next())
//		{
//			setCurrentLevel(getMaxLevel()) ;	//Utile ?
//
//			Dart e = ditWV;
//			std::vector<Dart> v ;
//
//			do
//			{
//				if(phi1(phi1(phi1(e))) != e)
//				{
//					v.push_back(phi1(phi1(e)));
//
//					if(!Map2::isBoundaryEdge(phi1(phi1(e))))
//						subdividedFaces.push_back(std::pair<Dart,Dart>(phi1(phi1(e)),phi2(phi1(phi1(e)))));
//				}
//
//				v.push_back(phi1(e));
//
//				if(!Map2::isBoundaryEdge(phi1(e)))
//					subdividedFaces.push_back(std::pair<Dart,Dart>(phi1(e),phi2(phi1(e))));
//
//
//				e = phi2(phi_1(e));
//			}
//			while(e != ditWV);
//
//			splitSurfaceInVolume(v,true,true);
//
////			Dart dd = phi2(phi1(ditWV));
////			Dart next = phi1(phi1(dd)) ;
////			Map2::splitFace(dd, next) ;
////
////			Dart ne = phi2(phi_1(dd));
////			Map2::cutEdge(ne) ;
////			centralDart = phi1(ne);
////
////			dd = phi1(phi1(next)) ;
////			while(dd != ne)
////			{
////				Dart tmp = phi1(ne) ;
////				Map2::splitFace(tmp, dd) ;
////				dd = phi1(phi1(dd)) ;
////			}
//
//			setCurrentLevel(getMaxLevel() - 1) ; //Utile ?
//		}
//
//		setCurrentLevel(getMaxLevel()) ;
//		//4 couture des relations precedemment sauvegarde
//		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
//		{
//			Dart f1 = phi2((*it).first);
//			Dart f2 = phi2((*it).second);
//
//			//closeHole(f1);
//		}
//		setCurrentLevel(getMaxLevel() - 1) ;
//
////		setCurrentLevel(getMaxLevel()) ;
////		//4 couture des relations precedemment sauvegarde
////		for (std::vector<std::pair<Dart,Dart> >::iterator it = subdividedFaces.begin(); it != subdividedFaces.end(); ++it)
////		{
////			Dart f1 = phi2((*it).first);
////			Dart f2 = phi2((*it).second);
////
////			//if(isBoundaryFace(f1) && isBoundaryFace(f2))
////			if(phi3(f1) == f1 && phi3(f2) == f2)
////				sewVolumes(f1, f2, false);
////		}
////		embedOrbit<VERTEX>(centralDart, getEmbedding<VERTEX>(centralDart));
////
////		setCurrentLevel(getMaxLevel() - 1) ;
//	}
//
//	//A optimiser
//	setCurrentLevel(getMaxLevel()-1) ;
//	TraversorE<Map3MR_PrimalRegular> travE2(*this);
//	for (Dart d = travE2.begin(); d != travE2.end(); d = travE2.next())
//	{
//		setCurrentLevel(getMaxLevel()) ;
//		embedOrbit<VERTEX>(phi1(d), getEmbedding<VERTEX>(phi1(d)));
//		setCurrentLevel(getMaxLevel()-1) ;
//	}
//	setCurrentLevel(getMaxLevel()) ;
//
////	setCurrentLevel(getMaxLevel()-1) ;
////	TraversorF<Map3MR_PrimalRegular> travF2(*this) ;
////	for (Dart d = travF2.begin(); d != travF2.end(); d = travF2.next())
////	{
////		if(!faceDegree(d) != 3)
////		{
////			setCurrentLevel(getMaxLevel()) ;
////			embedOrbit<VERTEX>(phi2(phi1(d)), getEmbedding<VERTEX>(phi2(phi1(d))));
////			setCurrentLevel(getMaxLevel()-1) ;
////		}
////	}
////	setCurrentLevel(getMaxLevel()) ;
//
//	popLevel() ;
//}

/************************************************************************
 *						Geometry modification							*
 ************************************************************************/

template <typename PFP>
void Map3MR<PFP>::analysis()
{
	assert(m_map.getCurrentLevel() > 0 || !"analysis : called on level 0") ;

	m_map.decCurrentLevel() ;

	for(unsigned int i = 0; i < analysisFilters.size(); ++i)
		(*analysisFilters[i])() ;
}

template <typename PFP>
void Map3MR<PFP>::synthesis()
{
	assert(m_map.getCurrentLevel() < m_map.getMaxLevel() || !"synthesis : called on max level") ;

	for(unsigned int i = 0; i < synthesisFilters.size(); ++i)
		(*synthesisFilters[i])() ;

	m_map.incCurrentLevel() ;
}

} // namespace Regular

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN
