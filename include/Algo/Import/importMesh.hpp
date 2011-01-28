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

#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/autoAttributeHandler.h"
#include "Container/fakeAttribute.h"

namespace CGoGN
{

namespace Algo
{

namespace Import
{

template <typename PFP>
bool importMesh(typename PFP::MAP& map, MeshTablesSurface<PFP>& mts)
{
	typedef typename PFP::VEC3 VEC3 ;

	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX_ORBIT, "incidents");

	unsigned nbf = mts.getNbFaces();
	int index = 0;
	// buffer for tempo faces (used to remove degenerated edges)
	std::vector<unsigned int> edgesBuffer;
	edgesBuffer.reserve(8);

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
		if (nbe > 2)
		{
			Dart d = map.newFace(nbe);
			for (unsigned int j = 0; j < nbe; ++j)
			{
				unsigned int em = edgesBuffer[j];		// get embedding
				map.setDartEmbedding(VERTEX_ORBIT, d, em);	// associate to dart
				vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
				d = map.phi1(d);
			}
			m.markOrbit(FACE_ORBIT, d);	// mark on the fly to unmark on second loop
		}
	}

	unsigned int nbnm = 0;
	// reconstruct neighbourhood

	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (m.isMarked(d))
		{
			// darts incident to end vertex of edge
			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];

			unsigned int embd = map.getDartEmbedding(VERTEX_ORBIT, d);
			Dart good_dart;
			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end() && good_dart == Dart::nil(); ++it)
			{
				if (map.getDartEmbedding(VERTEX_ORBIT, map.phi1(*it)) == embd)
					good_dart = *it;
			}

			if (good_dart != Dart::nil())
			{
				Dart aze = map.phi2(good_dart) ;
				if (good_dart == map.phi2(good_dart))
				{
					map.sewFaces(d, good_dart);
					m.unmarkOrbit(EDGE_ORBIT, d);
				}
			}
			else
			{
				++nbnm;
			}
		}
	}

	if (nbnm > 0)
		std::cout << "Warning " << nbnm << " darts with phi2 fix points" << std::endl;

	return true ;
}

template <typename PFP>
bool importMesh(typename PFP::MAP& map, MeshTablesVolume<PFP>& mtv)
{
	typedef typename PFP::VEC3 VEC3 ;

	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX_ORBIT, "incidents");

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
//					map.setDartEmbedding(VERTEX_ORBIT, d, em);	// associate to dart
//					vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
//					d = map.phi1(d);
//				}
//				m.markOrbit(FACE_ORBIT, d);	// mark on the fly to unmark on second loop
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
				map.setDartEmbedding(VERTEX_ORBIT, d, em);	// associate to dart
				vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
				d = map.phi1(d);
			}
			m.markOrbit(FACE_ORBIT, d);	// mark on the fly to unmark on second loop
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
			unsigned int emb1d = map.getDartEmbedding(VERTEX_ORBIT, map.phi_1(d));
			unsigned int emb2d = map.getDartEmbedding(VERTEX_ORBIT, map.phi1(map.phi1(d)));

			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				Dart dprim=*it;
				unsigned int emb1dprim = map.getDartEmbedding(VERTEX_ORBIT, map.phi1(map.phi1(dprim)));
				unsigned int emb2dprim = map.getDartEmbedding(VERTEX_ORBIT, map.phi_1(dprim));

				if(emb1d == emb1dprim && emb2d == emb2dprim)
				{
					map.sewVolumes(d,dprim);
				}
			}

			//Phi2 reconstruction
			emb1d = map.getDartEmbedding(VERTEX_ORBIT, d);
			emb2d = map.getDartEmbedding(VERTEX_ORBIT, map.phi1(d));

			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
			{
				Dart dprim=*it;
				unsigned int emb1dprim = map.getDartEmbedding(VERTEX_ORBIT, map.phi1(dprim));
				unsigned int emb2dprim = map.getDartEmbedding(VERTEX_ORBIT, dprim);

				if(emb1d == emb1dprim && emb2d == emb2dprim)
				{
					map.sewFaces(d,dprim);
				}
			}

			m.unmarkOrbit(DART_ORBIT,d);
		}
	}



	return true ;

//
//	//creer un polyhedre de nbf faces
//	//stocker pour un dart d tout les darts autour de l'arete
//	// reconstruire le voisine de l'arete pour phi3
//
//	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerEdge(map, VERTEX_ORBIT, "incidents");
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
//					map.setDartEmbedding(VERTEX_ORBIT, d, em);	// associate to dart
//					vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
//					d = map.phi1(d);
//
////					if(l == nbe-1)
////					{
////						unsigned int em2 = edgesBuffer[0];	// get embedding
////						map.setDartEmbedding(VERTEX_ORBIT, d3, em2);	// associate to dart
////						vecDartsPerVertex[em2].push_back(d3);	// store incident darts for fast phi2 reconstruction
////					}
////					else
////					{
////						unsigned int em2 = edgesBuffer[l+1];	// get embedding
////						map.setDartEmbedding(VERTEX_ORBIT, d3, em2);	// associate to dart
////						vecDartsPerVertex[em2].push_back(d3);	// store incident darts for fast phi2 reconstruction
////					}
////
////					d3 = map.phi_1(d3);
//				}
//				m.markOrbit(FACE_ORBIT, d);	// mark on the fly to unmark on second loop
//				//m.markOrbit(FACE_ORBIT, d3);	// mark on the fly to unmark on second loop
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
//			unsigned int embd = map.getDartEmbedding(VERTEX_ORBIT, d);
//			unsigned int nbf = 0;
//			Dart good_dart = d;
//
//			std::cout << "vec size" << vec.size() << std::endl;
//
//			for(typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
//			{
//				if (map.getDartEmbedding(VERTEX_ORBIT, map.phi1(*it)) == embd)
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
//					m.unmarkOrbit(EDGE_ORBIT, d);
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
//		std::cout << "Warning " << nbnm << " darts with phi2 fix points" << std::endl;
//	}
//
//	return true ;

}

template <typename PFP>
bool importMesh(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames, ImportSurfacique::ImportType kind)
{
	MeshTablesSurface<PFP> mts(map);

	if(!mts.importMesh(filename, attrNames, kind))
		return false;

	return importMesh<PFP>(map, mts);
}

template <typename PFP>
bool importMesh(typename PFP::MAP& map, const std::string& filename, typename PFP::TVEC3& positions, ImportVolumique::ImportType kind)
{
	AttribContainer& vertexContainer = map.getAttributeContainer(VERTEX_ORBIT);

	MeshTablesVolume<PFP> mtv(vertexContainer, positions);

	if(!mtv.importMesh(filename, kind))
		return false;

	return importMesh<PFP>(map, mtv);
}


//
//
//
//	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX_ORBIT, "incidents");
//
//
//	// Attributes container for vertex orbit
//	AttribContainer& vertexContainer = map.getAttributeContainer(VERTEX_ORBIT);
//
//	AttributeHandler<VEC3> positions(idPositions,map);
//
//	MeshTablesSurface<VEC3> mts(vertexContainer, positions);
//
//	if (!mts.importMesh(filename,kind))
//		return false;
//
//	// marker for phi2 reconstruction
//	m = map.getNewMarker();
//
//	unsigned nbf = mts.getNbFaces();
//	int index = 0;
//	// buffer for tempo faces (used to remove degenerated edges)
//	std::vector<unsigned int> edgesBuffer;
//	edgesBuffer.reserve(8);
//
//	// for each face of table
//	for(unsigned int i = 0; i < nbf; ++i)
//	{
//		// store face in buffer, removing degenerated edges
//		unsigned int nbe = mts.getNbEdgesFace(i);
//		edgesBuffer.clear();
//		unsigned int prec=EMBNULL;
//		for (unsigned int j=0; j<nbe; ++j)
//		{
//			unsigned int em = mts.getEmbIdx(index++);
//			if (em!=prec)
//			{
//				prec = em;
//				edgesBuffer.push_back(em);
//			}
//		}
//		// check first/last vertices
//		if (edgesBuffer.front() == edgesBuffer.back())
//			edgesBuffer.pop_back();
//
//		// create only non degenerated faces
//		nbe = edgesBuffer.size();
//		if (nbe >2)
//		{
//			Dart d = map.newFace(nbe);
//			for (unsigned int j=0; j<nbe; ++j)
//			{
//				unsigned int em = edgesBuffer[j];		// get embedding
//				map.setDartEmbedding(VERTEX_ORBIT, d, em);	// associate to dart
//				vecDartsPerVertex[em].push_back(d);	// store incident darts for fast phi2 reconstruction
//				d = map.phi1(d);
//			}
//			map.markOrbit(FACE_ORBIT,d,m);// mark on the fly to unmark on second loop
//		}
//	}
//
//	unsigned nbnm=0;
//	// reconstruct neighbourhood
//
//	for (Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if (map.isMarkedDart(d, m))
//		{
//			// darts incident to end vertex of edge
//			std::vector<Dart>& vec = vecDartsPerVertex[map.phi1(d)];
//
//			unsigned int embd = map.getDartEmbedding(VERTEX_ORBIT, d);
//			unsigned int nbf=0;
//			Dart good_dart;
//			for (typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
//			{
//				if ( map.getDartEmbedding(VERTEX_ORBIT,map.phi1(*it))==embd)
//				{
//					good_dart = *it;
//					nbf++;
//				}
//			}
//
//			if (nbf==1)
//			{
//				if (good_dart == map.phi2(good_dart))
//				{
//					map.sewFaces(d, good_dart);
//					map.unmarkOrbit(EDGE_ORBIT, d, m);
//				}
//			}
//			else
//			{
//				++nbnm;
//			}
//		}
//	}
//
//
////	return true;
//
//	if (nbnm > 0)
//	{
//		if (closeObject)
//		{
//			Marker mb = map.closeMap(true);
////			// for all vertices
////			for (unsigned int index = 0; index < vecDartsEmb.size(); ++index)
////			{
////				// if vertex is on boundary
////				if (vecEmbNbFp[index] > 0)
////				{
////					// find first dart that is sewed to boundary
////					std::vector<Dart>& vd = vecDartsEmb[index];
////					typename std::vector<Dart>::iterator jt = vd.begin();
////					while (!map.isMarkedDart(*jt, m))
////						jt++;
////					unsigned int P = map.getDartEmbedding(VERTEX_ORBIT, *jt);
////					// while vertex if a "non manifold point"
////					while (vecEmbNbFp[index] > 1)
////					{
////						Dart e = map.phi2(*jt);
////						// turn in boundary until we find same embedding
////						do
////						{
////							e = map.phi1(e);
////							// embedding of boundary darts on the fly
////							unsigned int emb = map.getDartEmbedding(VERTEX_ORBIT, map.phi1(map.phi2(e)));
////							map.setDartEmbedding(VERTEX_ORBIT, e, emb);
////						} while (map.getDartEmbedding(VERTEX_ORBIT, map.phi2(e)) != P);
////						// separate the face
////						map.phi1sew(map.phi2(*jt), e);
////						vecEmbNbFp[index]--;
////					}
////					// copy embedding of vertex of boundary of last face
////					Dart e = map.phi2(*jt);
////					Dart d = e;
////					if (map.getDartEmbedding(VERTEX_ORBIT, d) == EMBNULL)
////					{
////						do
////						{
////							unsigned int emb = map.getDartEmbedding(VERTEX_ORBIT, map.phi1(map.phi2(d)));
////							map.setDartEmbedding(VERTEX_ORBIT, d, emb);
////							d = map.phi1(d);
////						} while (d != e);
////					}
////				}
////			}
//			map.unmarkAll(m);
//			map.releaseMarker(m);
//			m = mb;
//		}
//		else
//		{
//			std::cout << "Warning " << nbnm << " darts with phi2 fix points" << std::endl;
//		}
//	}
//
//	return true;
//}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
