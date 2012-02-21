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

#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/autoAttributeHandler.h"
#include "Container/fakeAttribute.h"
#include "Algo/Modelisation/polyhedron.h"

namespace CGoGN
{

namespace Algo
{

namespace Import
{

template <typename PFP>
bool importMesh(typename PFP::MAP& map, MeshTablesSurface<PFP>& mts)
{
	AutoAttributeHandler< NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX, "incidents");

	unsigned nbf = mts.getNbFaces();
	int index = 0;
	// buffer for tempo faces (used to remove degenerated edges)
	std::vector<unsigned int> edgesBuffer;
	edgesBuffer.reserve(16);

	DartMarkerNoUnmark m(map) ;

	// for each face of table
	for(unsigned int i = 0; i < nbf; ++i)
	{
		// store face in buffer, removing degenerated edges
		unsigned int nbe = mts.getNbEdgesFace(i);
		edgesBuffer.clear();
		unsigned int prec = EMBNULL;
		for (unsigned int j = 0; j < nbe; ++j)
		{
			unsigned int em = mts.getEmbIdx(index++);
			if (em != prec)
			{
				prec = em;
				edgesBuffer.push_back(em);
			}
		}
		// check first/last vertices
		if (edgesBuffer.front() == edgesBuffer.back())
			edgesBuffer.pop_back();

		// create only non degenerated faces
		nbe = edgesBuffer.size();
		if (nbe > 2)
		{
			Dart d = map.newFace(nbe, false);
			for (unsigned int j = 0; j < nbe; ++j)
			{
				unsigned int em = edgesBuffer[j];		// get embedding

				FunctorSetEmb<typename PFP::MAP> fsetemb(map, VERTEX, em);
//				foreach_dart_of_orbit_in_parent<typename PFP::MAP>(&map, VERTEX, d, fsetemb) ;
				map.foreach_dart_of_orbit( PFP::MAP::ORBIT_IN_PARENT(VERTEX), d, fsetemb);

				m.mark(d) ;								// mark on the fly to unmark on second loop
				vecDartsPerVertex[em].push_back(d);		// store incident darts for fast adjacency reconstruction
				d = map.phi1(d);
			}
		}
	}

	// reconstruct neighbourhood
	unsigned int nbBoundaryEdges = 0;
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (m.isMarked(d))
		{
			// darts incident to end vertex of edge
			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];

			unsigned int embd = map.getEmbedding(VERTEX, d);
			Dart good_dart = NIL;
			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end() && good_dart == NIL; ++it)
			{
				if (map.getEmbedding(VERTEX, map.phi1(*it)) == embd)
					good_dart = *it;
			}

			if (good_dart != NIL)
			{
				map.sewFaces(d, good_dart, false);
				m.unmarkOrbit(EDGE, d);
			}
			else
			{
				m.unmark(d);
				++nbBoundaryEdges;
			}
		}
	}

	if (nbBoundaryEdges > 0)
	{
		map.closeMap();
		CGoGNout << "Map closed (" << nbBoundaryEdges << " boundary edges)" << CGoGNendl;
		// ensure bijection between topo and embedding
		map.bijectiveOrbitEmbedding(VERTEX);
	}

	return true ;
}


template <typename PFP>
bool importMeshSToV(typename PFP::MAP& map, MeshTablesSurface<PFP>& mts, float dist)
{
	AutoAttributeHandler< NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX, "incidents");
	unsigned nbf = mts.getNbFaces();
	int index = 0;
	// buffer for tempo faces (used to remove degenerated edges)
	std::vector<unsigned int> edgesBuffer;
	edgesBuffer.reserve(16);

	DartMarkerNoUnmark m(map) ;

	AttributeHandler<typename PFP::VEC3> position = map.template getAttribute<typename PFP::VEC3>(VERTEX, "position");
	std::vector<unsigned int > backEdgesBuffer(mts.getNbVertices(), EMBNULL);

	// for each face of table -> create a prism
	for(unsigned int i = 0; i < nbf; ++i)
	{
		// store face in buffer, removing degenerated edges
		unsigned int nbe = mts.getNbEdgesFace(i);
		edgesBuffer.clear();
		unsigned int prec = EMBNULL;
		for (unsigned int j = 0; j < nbe; ++j)
		{
			unsigned int em = mts.getEmbIdx(index++);
			if (em != prec)
			{
				prec = em;
				edgesBuffer.push_back(em);
			}
		}
		// check first/last vertices
		if (edgesBuffer.front() == edgesBuffer.back())
			edgesBuffer.pop_back();

		// create only non degenerated faces
		nbe = edgesBuffer.size();
		if (nbe > 2)
		{
			Dart d = Algo::Modelisation::createPrism<PFP>(map, nbe);

			//Embed the base faces
			for (unsigned int j = 0; j < nbe; ++j)
			{
				unsigned int em = edgesBuffer[j];		// get embedding

				if(backEdgesBuffer[em] == EMBNULL)
				{
					unsigned int emn = map.newCell(VERTEX);
					map.copyCell(VERTEX, emn, em);
					backEdgesBuffer[em] = emn;
					position[emn] += typename PFP::VEC3(0,0,dist);
				}

				FunctorSetEmb<typename PFP::MAP> fsetemb(map, VERTEX, em);
				//foreach_dart_of_orbit_in_parent<typename PFP::MAP>(&map, VERTEX, d, fsetemb) ;
				map.foreach_dart_of_orbit(PFP::MAP::ORBIT_IN_PARENT(VERTEX), d, fsetemb);

				//Embed the other base face
				Dart d2 = map.phi1(map.phi1(map.phi2(d)));
				unsigned int em2 = backEdgesBuffer[em];
				FunctorSetEmb<typename PFP::MAP> fsetemb2(map, VERTEX, em2);
				//foreach_dart_of_orbit_in_parent<typename PFP::MAP>(&map, VERTEX, d2, fsetemb2) ;
				map.foreach_dart_of_orbit(PFP::MAP::ORBIT_IN_PARENT(VERTEX), d2, fsetemb2);

				m.mark(d) ;								// mark on the fly to unmark on second loop
				vecDartsPerVertex[em].push_back(d);		// store incident darts for fast adjacency reconstruction
				d = map.phi_1(d);
			}

		}
	}

	// reconstruct neighbourhood
	unsigned int nbBoundaryEdges = 0;
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (m.isMarked(d))
		{
			// darts incident to end vertex of edge
			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];

			unsigned int embd = map.getEmbedding(VERTEX, d);
			Dart good_dart = NIL;
			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end() && good_dart == NIL; ++it)
			{
				if (map.getEmbedding(VERTEX, map.phi1(*it)) == embd)
					good_dart = *it;
			}

			if (good_dart != NIL)
			{
				map.sewVolumes(map.phi2(d), map.phi2(good_dart), false);
				m.unmarkOrbit(EDGE, d);
			}
			else
			{
				m.unmark(d);
				++nbBoundaryEdges;
			}
		}
	}

	return true ;
}

template <typename PFP>
bool importMeshSurfToVol(typename PFP::MAP& map, MeshTablesSurface<PFP>& mts, float dist)
{
	AutoAttributeHandler< NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX, "incidents");
	unsigned nbf = mts.getNbFaces();
	int index = 0;
	// buffer for tempo faces (used to remove degenerated edges)
	std::vector<unsigned int> edgesBuffer;
	edgesBuffer.reserve(16);

	DartMarkerNoUnmark m(map) ;

	AttributeHandler<typename PFP::VEC3> position = map.template getAttribute<typename PFP::VEC3>(VERTEX, "position");
	std::vector<unsigned int > backEdgesBuffer(mts.getNbVertices(), EMBNULL);

	// for each face of table -> create a prism
	for(unsigned int i = 0; i < nbf; ++i)
	{
		// store face in buffer, removing degenerated edges
		unsigned int nbe = mts.getNbEdgesFace(i);
		edgesBuffer.clear();
		unsigned int prec = EMBNULL;
		for (unsigned int j = 0; j < nbe; ++j)
		{
			unsigned int em = mts.getEmbIdx(index++);
			if (em != prec)
			{
				prec = em;
				edgesBuffer.push_back(em);
			}
		}
		// check first/last vertices
		if (edgesBuffer.front() == edgesBuffer.back())
			edgesBuffer.pop_back();

		// create only non degenerated faces
		nbe = edgesBuffer.size();
		if (nbe > 2)
		{
			Dart d = Algo::Modelisation::createPrism<PFP>(map, nbe);

			//Embed the base faces
			for (unsigned int j = 0; j < nbe; ++j)
			{
				unsigned int em = edgesBuffer[j];		// get embedding

				if(backEdgesBuffer[em] == EMBNULL)
				{
					unsigned int emn = map.newCell(VERTEX);
					map.copyCell(VERTEX, emn, em);
					backEdgesBuffer[em] = emn;
					position[emn] += typename PFP::VEC3(0,0,dist);
				}

				FunctorSetEmb<typename PFP::MAP> fsetemb(map, VERTEX, em);
				//foreach_dart_of_orbit_in_parent<typename PFP::MAP>(&map, VERTEX, d, fsetemb) ;
				map.foreach_dart_of_orbit(PFP::MAP::ORBIT_IN_PARENT(VERTEX), d, fsetemb);

				//Embed the other base face
				Dart d2 = map.phi1(map.phi1(map.phi2(d)));
				unsigned int em2 = backEdgesBuffer[em];
				FunctorSetEmb<typename PFP::MAP> fsetemb2(map, VERTEX, em2);
				//foreach_dart_of_orbit_in_parent<typename PFP::MAP>(&map, VERTEX, d2, fsetemb2) ;
				map.foreach_dart_of_orbit(PFP::MAP::ORBIT_IN_PARENT(VERTEX), d2, fsetemb2);

				m.mark(d) ;								// mark on the fly to unmark on second loop
				vecDartsPerVertex[em].push_back(d);		// store incident darts for fast adjacency reconstruction
				d = map.phi_1(d);
			}

		}
	}

	// reconstruct neighbourhood
	unsigned int nbBoundaryEdges = 0;
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (m.isMarked(d))
		{
			// darts incident to end vertex of edge
			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];

			unsigned int embd = map.getEmbedding(VERTEX, d);
			Dart good_dart = NIL;
			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end() && good_dart == NIL; ++it)
			{
				if (map.getEmbedding(VERTEX, map.phi1(*it)) == embd)
					good_dart = *it;
			}

			if (good_dart != NIL)
			{
				map.sewVolumes(map.phi2(d), map.phi2(good_dart), false);
				m.unmarkOrbit(EDGE, d);
			}
			else
			{
				m.unmark(d);
				++nbBoundaryEdges;
			}
		}
	}

	map.closeMap();

	return true ;
}

template <typename PFP>
bool importMesh(typename PFP::MAP& map, MeshTablesVolume<PFP>& mtv)
{
	typedef typename PFP::VEC3 VEC3 ;

	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX, "incidents");

	//ALGO
	// précondition
	// indexer le tableau du nombre d'arrete par faces par le nombre de faces par volume
	//
	//creation d'une sous-carte (2-carte) pour les faces d'un volume en fonction du nombre d'arete grace a un meshtableSurface
	//avoir un tableau_de_sommets[nbsommets] indexe par les sommets dans lequel on comptera le nombre de fois qu'on a croisé un sommet
	//
	//
	//fusion de toutes les composantes connexes avec couture par phi3


	unsigned int nbf = mtv.getNbFaces(); //getNbVolume()
	unsigned int indexNbFaces = 0;

	//for each volume
	for(unsigned int i = 0 ; i < nbf ; ++i)
	{
		unsigned int indexNbFacesMin = indexNbFaces;

		indexNbFaces += mtv.getNbFacesVolume(i) - 1;

		//for each face of this volume
		for(unsigned int j = indexNbFacesMin ; j<= indexNbFaces ; ++j)
		{
			//reconstructing the faces for this volume

			unsigned int nbEdges = mtv.getNbEdgesFace(j);



//			std::vector<unsigned int> edgesBuffer;
//			edgesBuffer.reserve(8);
//
//			edgesBuffer.clear();
//			unsigned int prec = EMBNULL;
//			for (unsigned int j = 0; j < nbe; ++j)
//			{
//				unsigned int em = mtv.getEmbIdx(index++);
//				if (em!=prec)
//				{
//					prec = em;
//					edgesBuffer.push_back(em);
//				}
//			}
//			// check first/last vertices
//			if (edgesBuffer.front() == edgesBuffer.back())
//				edgesBuffer.pop_back();
//
//			// create only non degenerated faces
//			nbe = edgesBuffer.size();
//			if (nbe >2)
//			{
//				Dart d = map.newFace(nbe);
//				for (unsigned int j = 0; j < nbe; ++j)
//				{
//					unsigned int em = edgesBuffer[j];		// get embedding
//					map.setDartEmbedding(VERTEX, d, em);	// associate to dart
//					vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
//					d = map.phi1(d);
//				}
//				m.markOrbit(FACE, d);	// mark on the fly to unmark on second loop
//			}

		}

		indexNbFaces++;
	}









	int index = 0;
	// buffer for tempo faces (used to remove degenerated edges)
	std::vector<unsigned int> edgesBuffer;
	edgesBuffer.reserve(8);

	DartMarkerNoUnmark m(map) ;

	// for each face of table
	for(unsigned int i = 0; i < nbf; ++i)
	{
		// store face in buffer, removing degenerated edges
		unsigned int nbe = mtv.getNbEdgesFace(i);
		edgesBuffer.clear();
		unsigned int prec = EMBNULL;
		for (unsigned int j = 0; j < nbe; ++j)
		{
			unsigned int em = mtv.getEmbIdx(index++);
			if (em!=prec)
			{
				prec = em;
				edgesBuffer.push_back(em);
			}
		}
		// check first/last vertices
		if (edgesBuffer.front() == edgesBuffer.back())
			edgesBuffer.pop_back();

		// create only non degenerated faces
		nbe = edgesBuffer.size();
		if (nbe >2)
		{
			Dart d = map.newFace(nbe);
			for (unsigned int j = 0; j < nbe; ++j)
			{
				unsigned int em = edgesBuffer[j];		// get embedding
				map.setDartEmbedding(VERTEX, d, em);	// associate to dart
				vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
				d = map.phi1(d);
			}
			m.markOrbit(FACE, d);	// mark on the fly to unmark on second loop
		}
	}
	//unsigned int nbnm = 0;
	// reconstruct neighbourhood

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (m.isMarked(d))
		{
			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];

			//Phi3 reconstruction
			unsigned int emb1d = map.getEmbedding(VERTEX, map.phi_1(d));
			unsigned int emb2d = map.getEmbedding(VERTEX, map.phi1(map.phi1(d)));

			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				Dart dprim=*it;
				unsigned int emb1dprim = map.getEmbedding(VERTEX, map.phi1(map.phi1(dprim)));
				unsigned int emb2dprim = map.getEmbedding(VERTEX, map.phi_1(dprim));

				if(emb1d == emb1dprim && emb2d == emb2dprim)
				{
					map.sewVolumes(d,dprim);
				}
			}

			//Phi2 reconstruction
			emb1d = map.getEmbedding(VERTEX, d);
			emb2d = map.getEmbedding(VERTEX, map.phi1(d));

			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				Dart dprim=*it;
				unsigned int emb1dprim = map.getEmbedding(VERTEX, map.phi1(dprim));
				unsigned int emb2dprim = map.getEmbedding(VERTEX, dprim);

				if(emb1d == emb1dprim && emb2d == emb2dprim)
				{
					map.sewFaces(d,dprim);
				}
			}

			m.unmarkOrbit(DART,d);
		}
	}

	return true ;

//
//	//creer un polyhedre de nbf faces
//	//stocker pour un dart d tout les darts autour de l'arete
//	// reconstruire le voisine de l'arete pour phi3
//
//	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerEdge(map, VERTEX, "incidents");
//
//	unsigned nbv = mtv.getNbVolumes();
//	int index = 0;
//	// buffer for tempo faces (used to remove degenerated edges)
//	std::vector<unsigned int> edgesBuffer;
//	edgesBuffer.reserve(8);
//
//	DartMarkerNoUnmark m(map) ;
//
//	//for each volume of table
//	for(unsigned int i = 0; i < nbv ; ++i)
//	{
//		unsigned int nbf = mtv.getNbFacesPerVolume(i);
//
//		//for each face of a volume
//		for(unsigned int j = 0; j < nbf; ++j)
//		{
//			// store face in buffer, removing degenerated edges
//			unsigned int nbe = mtv.getNbVerticesPerFace(j);
//			edgesBuffer.clear();
//			unsigned int prec = EMBNULL;
//			for (unsigned int k = 0; k < nbe; ++k)
//			{
//				unsigned int em = mtv.getEmbIdx(index++);
//				if (em!=prec)
//				{
//					prec = em;
//					edgesBuffer.push_back(em);
//				}
//			}
//
//			// check first/last vertices
//			if (edgesBuffer.front() == edgesBuffer.back())
//				edgesBuffer.pop_back();
//
//			// create only non degenerated faces
//			nbe = edgesBuffer.size();
//
//			if (nbe >2)
//			{
//				Dart d = map.newFace(nbe);
//				Dart d3 = map.newFace(nbe);
//				map.sewVolumes(d,d3);
//
//				for (unsigned int l = 0; l < nbe; ++l)
//				{
//					unsigned int em = edgesBuffer[l];		// get embedding
//					map.setDartEmbedding(VERTEX, d, em);	// associate to dart
//					vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
//					d = map.phi1(d);
//
////					if(l == nbe-1)
////					{
////						unsigned int em2 = edgesBuffer[0];	// get embedding
////						map.setDartEmbedding(VERTEX, d3, em2);	// associate to dart
////						vecDartsPerVertex[em2].push_back(d3);	// store incident darts for fast phi2 reconstruction
////					}
////					else
////					{
////						unsigned int em2 = edgesBuffer[l+1];	// get embedding
////						map.setDartEmbedding(VERTEX, d3, em2);	// associate to dart
////						vecDartsPerVertex[em2].push_back(d3);	// store incident darts for fast phi2 reconstruction
////					}
////
////					d3 = map.phi_1(d3);
//				}
//				m.markOrbit(FACE, d);	// mark on the fly to unmark on second loop
//				//m.markOrbit(FACE, d3);	// mark on the fly to unmark on second loop
//			}
//		}
//	}
//
//
//	unsigned int nbnm = 0;
//	// reconstruct neighbourhood
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if (m.isMarked(d))
//		{
//			// darts incident to end vertex of edge
//			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];
//
//			unsigned int embd = map.getEmbedding(VERTEX, d);
//			unsigned int nbf = 0;
//			Dart good_dart = d;
//
//			CGoGNout << "vec size" << vec.size() << CGoGNendl;
//
//			for(typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
//			{
//				if (map.getEmbedding(VERTEX, map.phi1(*it)) == embd)
//				{
//					good_dart = *it;
//					nbf++;
//				}
//			}
//
//			if (nbf == 2)
//			{
//				if (good_dart == map.phi2(good_dart))
//				{
//					map.sewFaces(d, good_dart);
//					m.unmarkOrbit(EDGE, d);
//				}
//			}
//			else
//			{
//				++nbnm;
//			}
//		}
//	}
//
//	if (nbnm > 0)
//	{
//		CGoGNout << "Warning " << nbnm << " darts with phi2 fix points" << CGoGNendl;
//	}
//
//	return true ;

}

template <typename PFP>
bool importMesh(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, ImportSurfacique::ImportType kind, bool mergeCloseVertices)
{
	MeshTablesSurface<PFP> mts(map);

	if(!mts.importMesh(filename, attrNames, kind))
		return false;

	if (mergeCloseVertices)
		mts.mergeCloseVertices();

	return importMesh<PFP>(map, mts);
}

template <typename PFP>
bool importMeshToExtrude(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, ImportSurfacique::ImportType kind)
{
	float dist = 1.0f;
	MeshTablesSurface<PFP> mts(map);

	if(!mts.importMesh(filename, attrNames, kind))
		return false;

	return importMeshSurfToVol<PFP>(map, mts, dist);
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
