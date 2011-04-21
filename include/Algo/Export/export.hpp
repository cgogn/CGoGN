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
#include "Topology/generic/cellmarker.h"
#include "openctm.h"

namespace CGoGN
{

namespace Algo
{

namespace Export
{

template <typename PFP>
bool exportPLY(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	
	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << CGoGNendl ;
		return false ;
	}

	unsigned int nbDarts = map.getNbDarts() ;
	std::vector<unsigned int> facesSize ;
	std::vector<std::vector<unsigned int> > facesIdx ;
	facesSize.reserve(nbDarts/3) ;
	facesIdx.reserve(nbDarts/3) ;
	std::map<unsigned int, unsigned int> vIndex ;
	unsigned int vCpt = 0 ;
	std::vector<unsigned int> vertices ;
	vertices.reserve(nbDarts/6) ;

	CellMarker markV(map, VERTEX_CELL) ;
	DartMarker markF(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(good(d) && !markF.isMarked(d))
		{
			markF.markOrbit(FACE_ORBIT, d) ;
			std::vector<unsigned int> fidx ;
			fidx.reserve(4) ;
			Dart dd = d ;
			do
			{
				unsigned int vNum = map.getEmbedding(dd, VERTEX_ORBIT) ;
				if(!markV.isMarked(dd))
				{
					markV.mark(dd) ;
					vIndex[vNum] = vCpt++ ;
					vertices.push_back(vNum) ;
				}
				fidx.push_back(vIndex[vNum]) ;
				dd = map.phi1(dd) ;
			} while(dd != d) ;
			facesSize.push_back(map.faceDegree(d)) ;
			facesIdx.push_back(fidx) ;
		}
	}

	out << "ply" << CGoGNendl ;
	out << "format ascii 1.0" << CGoGNendl ;
	out << "comment no comment" << CGoGNendl ;
	out << "element vertex " << vertices.size() << CGoGNendl ;
	out << "property float x" << CGoGNendl ;
	out << "property float y" << CGoGNendl ;
	out << "property float z" << CGoGNendl ;
	out << "element face " << facesSize.size() << CGoGNendl ;
	out << "property list uchar int vertex_indices" << CGoGNendl ;
	out << "end_header" << CGoGNendl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		const VEC3& v = position[vertices[i]] ;
		out << v[0] << " " << v[1] << " " << v[2] << CGoGNendl ;
	}
	for(unsigned int i = 0; i < facesSize.size(); ++i)
	{
		out << facesSize[i] ;
		for(unsigned int j = 0; j < facesIdx[i].size(); ++j)
			out << " " << facesIdx[i][j] ;
		out << CGoGNendl ;
	}

	out.close() ;
	return true ;
}

template <typename PFP>
bool exportOFF(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	
	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << CGoGNendl ;
		return false ;
	}

	unsigned int nbDarts = map.getNbDarts() ;
	std::vector<unsigned int> facesSize ;
	std::vector<std::vector<unsigned int> > facesIdx ;
	facesSize.reserve(nbDarts/3) ;
	facesIdx.reserve(nbDarts/3) ;
	std::map<unsigned int, unsigned int> vIndex ;
	unsigned int vCpt = 0 ;
	std::vector<unsigned int> vertices ;
	vertices.reserve(nbDarts/6) ;

	CellMarker markV(map, VERTEX_CELL) ;
	DartMarker markF(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(good(d) && !markF.isMarked(d))
		{
			markF.markOrbit(FACE_ORBIT, d) ;
			std::vector<unsigned int> fidx ;
			fidx.reserve(4) ;
			Dart dd = d ;
			do
			{
				unsigned int vNum = map.getEmbedding(dd, VERTEX_ORBIT) ;
				if(!markV.isMarked(dd))
				{
					markV.mark(dd) ;
					vIndex[vNum] = vCpt++ ;
					vertices.push_back(vNum) ;
				}
				fidx.push_back(vIndex[vNum]) ;
				dd = map.phi1(dd) ;
			} while(dd != d) ;
			facesSize.push_back(map.faceDegree(d)) ;
			facesIdx.push_back(fidx) ;
		}
	}

	out << "OFF" << CGoGNendl ;
	out << vertices.size() << " " << facesSize.size() << " " << 0 << CGoGNendl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		const VEC3& v = position[vertices[i]] ;
		out << v[0] << " " << v[1] << " " << v[2] << CGoGNendl ;
	}
	for(unsigned int i = 0; i < facesSize.size(); ++i)
	{
		out << facesSize[i] ;
		for(unsigned int j = 0; j < facesIdx[i].size(); ++j)
			out << " " << facesIdx[i][j] ;
		out << CGoGNendl ;
	}

	out.close() ;
	return true ;
}

template <typename PFP>
bool exportCTM(typename PFP::MAP& the_map, const typename PFP::TVEC3& position, const std::string& filename, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	AutoAttributeHandler<unsigned int> tableVertLab(the_map, VERTEX_ORBIT);

	CellMarker markV(the_map,VERTEX_CELL);

	unsigned int nbDarts = the_map.getNbDarts() ;

	std::vector<CTMfloat> verticesBuffer;
	std::vector<CTMuint> indicesBuffer;
	verticesBuffer.reserve(nbDarts/5);	// TODO non optimal reservation
	indicesBuffer.reserve(nbDarts/3);

	DartMarker markF(the_map);
	unsigned int lab=0;
	for(Dart d = the_map.begin(); d != the_map.end(); the_map.next(d))
	{
		if(good(d) && !markF.isMarked(d))
		{
			markF.markOrbit(FACE_ORBIT, d) ;
			Dart e = d;
			do
			{
				if (!markV.isMarked(e))
				{
					tableVertLab[e] = lab++;
					markV.mark(e);
					const VEC3& vert =  position[e];
					verticesBuffer.push_back(vert[0]);
					verticesBuffer.push_back(vert[1]);
					verticesBuffer.push_back(vert[2]);
//					CGoGNout << vert<< CGoGNendl;
				}
				indicesBuffer.push_back(tableVertLab[e]);
				e = the_map.phi1(e);
			} while (e!=d);
		}
	}

//	for (int i=0; i< indicesBuffer.size(); ++i)
//	{
//		CGoGNout << indicesBuffer[i]<<", "<< CGoGNendl;
//		if (i%3==0)
//			CGoGNout << CGoGNendl;
//	}

	// Save the file using the OpenCTM API
	CTMexporter ctm;

	// Define mesh

	ctm.DefineMesh(&(verticesBuffer[0]) , verticesBuffer.size()/3,
			&(indicesBuffer[0]) , indicesBuffer.size()/3, NULL);


	ctm.VertexPrecisionRel(0.0001f);

	// Set compression method and level
	ctm.CompressionMethod(CTM_METHOD_MG2);
	ctm.CompressionLevel(9);

	// Export file
	ctm.Save(filename.c_str());

	return true ;
}

template <typename PFP>
bool exportPLYPTM(typename PFP::MAP& map, const char* filename, const typename PFP::TVEC3& position, const typename PFP::TVEC3 frame[3], const typename PFP::TVEC3 colorPTM[6], const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << CGoGNendl ;
		return false ;
	}

	AutoAttributeHandler<unsigned int> tableVertLab(map, VERTEX_ORBIT);

	CellMarker markV(map,VERTEX_CELL);

	unsigned int nbDarts = map.getNbDarts() ;

	std::vector<unsigned int> vertices;
	std::vector<unsigned int> faces;

	vertices.reserve(nbDarts/5);	// TODO non optimal reservation
	faces.reserve(nbDarts/3);

	DartMarker markF(map);
	unsigned int lab = 0;
	unsigned int nbf = 0;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(good(d) && !markF.isMarked(d))
		{
			markF.markOrbit(FACE_ORBIT, d) ;
			Dart e = d;
			std::vector<unsigned int> face ;
			do
			{
				if (!markV.isMarked(e))
				{
					vertices.push_back(map.getEmbedding(e, VERTEX_ORBIT));
					tableVertLab[e] = lab++;

					markV.mark(e);
				}
				face.push_back(tableVertLab[e]);
				e = map.phi1(e);
			} while (e!=d) ;

			faces.push_back(face.size()) ;
			for (unsigned int i = 0 ; i < face.size() ; ++i)
				faces.push_back(face.at(i)) ;

			++nbf;
		}
	}

	out << "ply" << CGoGNendl ;
	out << "format ascii 1.0" << CGoGNendl ;
	out << "comment ply PTM (F. Larue format)" << CGoGNendl ;
	out << "element vertex " << vertices.size() << CGoGNendl ;
	out << "property float x" << CGoGNendl ;
	out << "property float y" << CGoGNendl ;
	out << "property float z" << CGoGNendl ;
	out << "property float tx" << CGoGNendl ;
	out << "property float ty" << CGoGNendl ;
	out << "property float tz" << CGoGNendl ;
	out << "property float bx" << CGoGNendl ;
	out << "property float by" << CGoGNendl ;
	out << "property float bz" << CGoGNendl ;
	out << "property float nx" << CGoGNendl ;
	out << "property float ny" << CGoGNendl ;
	out << "property float nz" << CGoGNendl ;
	out << "property float L1_a" << CGoGNendl ;
	out << "property float L1_b" << CGoGNendl ;
	out << "property float L1_c" << CGoGNendl ;
	out << "property float L1_d" << CGoGNendl ;
	out << "property float L1_e" << CGoGNendl ;
	out << "property float L1_f" << CGoGNendl ;
	out << "property float L2_a" << CGoGNendl ;
	out << "property float L2_b" << CGoGNendl ;
	out << "property float L2_c" << CGoGNendl ;
	out << "property float L2_d" << CGoGNendl ;
	out << "property float L2_e" << CGoGNendl ;
	out << "property float L2_f" << CGoGNendl ;
	out << "property float L3_a" << CGoGNendl ;
	out << "property float L3_b" << CGoGNendl ;
	out << "property float L3_c" << CGoGNendl ;
	out << "property float L3_d" << CGoGNendl ;
	out << "property float L3_e" << CGoGNendl ;
	out << "property float L3_f" << CGoGNendl ;
	out << "element face " << nbf << CGoGNendl ;
	out << "property list uchar int vertex_indices" << CGoGNendl ;
	out << "end_header" << CGoGNendl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		unsigned int vi = vertices[i];
		out << position[vi][0] << " " << position[vi][1] << " " << position[vi][2] << " " ;
		out << frame[0][vi][0] << " " << frame[0][vi][1] << " " << frame[0][vi][2] << " " ;
		out << frame[1][vi][0] << " " << frame[1][vi][1] << " " << frame[1][vi][2] << " " ;
		out << frame[2][vi][0] << " " << frame[2][vi][1] << " " << frame[2][vi][2] << " " ;
		out << colorPTM[0][vi][0] << " " << colorPTM[1][vi][0] << " " << colorPTM[2][vi][0] << " " << colorPTM[3][vi][0] << " " << colorPTM[4][vi][0] << " " << colorPTM[5][vi][0] <<" " ;
		out << colorPTM[0][vi][1] << " " << colorPTM[1][vi][1] << " " << colorPTM[2][vi][1] << " " << colorPTM[3][vi][1] << " " << colorPTM[4][vi][1] << " " << colorPTM[5][vi][1] <<" " ;
		out << colorPTM[0][vi][2] << " " << colorPTM[1][vi][2] << " " << colorPTM[2][vi][2] << " " << colorPTM[3][vi][2] << " " << colorPTM[4][vi][2] << " " << colorPTM[5][vi][2] << CGoGNendl ;
	}

	std::vector<unsigned int>::iterator it = faces.begin();
	while (it != faces.end())
	{
		unsigned int nbe = *it++;
		out << nbe ;
		for(unsigned int j = 0; j < nbe; ++j)
			out << " " << *it++;
		out << CGoGNendl ;
	}

	out.close() ;
	return true ;
}

} // namespace Export

} // namespace Algo

} // namespace CGoGN
