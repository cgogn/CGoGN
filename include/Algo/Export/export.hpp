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
		std::cerr << "Unable to open file " << std::endl ;
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

	out << "ply" << std::endl ;
	out << "format ascii 1.0" << std::endl ;
	out << "comment no comment" << std::endl ;
	out << "element vertex " << vertices.size() << std::endl ;
	out << "property float x" << std::endl ;
	out << "property float y" << std::endl ;
	out << "property float z" << std::endl ;
	out << "element face " << facesSize.size() << std::endl ;
	out << "property list uchar int vertex_indices" << std::endl ;
	out << "end_header" << std::endl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		const VEC3& v = position[vertices[i]] ;
		out << v[0] << " " << v[1] << " " << v[2] << std::endl ;
	}
	for(unsigned int i = 0; i < facesSize.size(); ++i)
	{
		out << facesSize[i] ;
		for(unsigned int j = 0; j < facesIdx[i].size(); ++j)
			out << " " << facesIdx[i][j] ;
		out << std::endl ;
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
		std::cerr << "Unable to open file " << std::endl ;
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

	out << "OFF" << std::endl ;
	out << vertices.size() << " " << facesSize.size() << " " << 0 << std::endl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		const VEC3& v = position[vertices[i]] ;
		out << v[0] << " " << v[1] << " " << v[2] << std::endl ;
	}
	for(unsigned int i = 0; i < facesSize.size(); ++i)
	{
		out << facesSize[i] ;
		for(unsigned int j = 0; j < facesIdx[i].size(); ++j)
			out << " " << facesIdx[i][j] ;
		out << std::endl ;
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
//					std::cout << vert<< std::endl;
				}
				indicesBuffer.push_back(tableVertLab[e]);
				e = the_map.phi1(e);
			} while (e!=d);
		}
	}

//	for (int i=0; i< indicesBuffer.size(); ++i)
//	{
//		std::cout << indicesBuffer[i]<<", "<< std::endl;
//		if (i%3==0)
//			std::cout << std::endl;
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
bool exportPLYPTM(typename PFP::MAP& map, const char* filename, const typename PFP::TVEC3& Pos,	const typename PFP::TFRAME& Frame, const typename PFP::TRGBFUNCS& RGBfuncs, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		std::cerr << "Unable to open file " << std::endl ;
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
			faces.push_back(0);
			unsigned int& nbe = faces.back();
			Dart e = d;
			do
			{
				if (!markV.isMarked(e))
				{
					vertices.push_back(map.getEmbedding(e, VERTEX_ORBIT));
					tableVertLab[e] = lab++;

					markV.mark(e);
				}
				faces.push_back(tableVertLab[e]);
				nbe++;
				e = map.phi1(e);
			} while (e!=d);
			nbf++;
		}
	}

	out << "ply" << std::endl ;
	out << "format ascii 1.0" << std::endl ;
	out << "comment ply PTM (F. Larue format)" << std::endl ;
	out << "element vertex " << vertices.size() << std::endl ;
	out << "property float x" << std::endl ;
	out << "property float y" << std::endl ;
	out << "property float z" << std::endl ;
	out << "property float tx" << std::endl ;
	out << "property float ty" << std::endl ;
	out << "property float tz" << std::endl ;
	out << "property float bx" << std::endl ;
	out << "property float by" << std::endl ;
	out << "property float bz" << std::endl ;
	out << "property float nx" << std::endl ;
	out << "property float ny" << std::endl ;
	out << "property float nz" << std::endl ;
	out << "property float L1_a" << std::endl ;
	out << "property float L1_b" << std::endl ;
	out << "property float L1_c" << std::endl ;
	out << "property float L1_d" << std::endl ;
	out << "property float L1_e" << std::endl ;
	out << "property float L1_f" << std::endl ;
	out << "property float L2_a" << std::endl ;
	out << "property float L2_b" << std::endl ;
	out << "property float L2_c" << std::endl ;
	out << "property float L2_d" << std::endl ;
	out << "property float L2_e" << std::endl ;
	out << "property float L2_f" << std::endl ;
	out << "property float L3_a" << std::endl ;
	out << "property float L3_b" << std::endl ;
	out << "property float L3_c" << std::endl ;
	out << "property float L3_d" << std::endl ;
	out << "property float L3_e" << std::endl ;
	out << "property float L3_f" << std::endl ;
	out << "element face " << nbf << std::endl ;
	out << "property list uchar int vertex_indices" << std::endl ;
	out << "end_header" << std::endl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		unsigned int vi = vertices[i];
		out << Pos[vi][0] << " " << Pos[vi][1] << " " << Pos[vi][2]<<" ";
		out << Frame[vi](0,0) << " " << Frame[vi](0,1) << " " << Frame[vi](0,2) << " " ;
		out << Frame[vi](1,0) << " " << Frame[vi](1,1) << " " << Frame[vi](1,2) << " " ;
		out << Frame[vi](2,0) << " " << Frame[vi](2,1) << " " << Frame[vi](2,2) << " " ;
		out << RGBfuncs[vi](0,0) << " " << RGBfuncs[vi](0,1) << " " << RGBfuncs[vi](0,2) << " " << RGBfuncs[vi](0,3) << " " << RGBfuncs[vi](0,4) << " " << RGBfuncs[vi](0,5) <<" ";
		out << RGBfuncs[vi](1,0) << " " << RGBfuncs[vi](1,1) << " " << RGBfuncs[vi](1,2) << " " << RGBfuncs[vi](1,3) << " " << RGBfuncs[vi](1,4) << " " << RGBfuncs[vi](1,5) <<" ";
		out << RGBfuncs[vi](2,0) << " " << RGBfuncs[vi](2,1) << " " << RGBfuncs[vi](2,2) << " " << RGBfuncs[vi](2,3) << " " << RGBfuncs[vi](2,4) << " " << RGBfuncs[vi](2,5) << std::endl ;
	}

	std::vector<unsigned int>::iterator it = faces.begin();;
	while (it != faces.end())
	{
		unsigned int nbe = *it++;
		out << nbe ;
		for(unsigned int j = 0; j < nbe; ++j)
			out << " " << *it++;
		out << std::endl ;
	}

	out.close() ;
	return true ;
}


template <typename PFP>
bool exportPLYPTM(typename PFP::MAP& map, const char* filename, const typename PFP::TVEC3& Pos,
		const typename PFP::TVEC3& Tan, const typename PFP::TVEC3& Btan, const typename PFP::TVEC3& Norm,
		const AttributeHandler<typename PFP::VEC6>& L1, AttributeHandler<typename PFP::VEC6>& L2, const AttributeHandler<typename PFP::VEC6>& L3,
		const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		std::cerr << "Unable to open file " << std::endl ;
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
			faces.push_back(0);
			unsigned int& nbe = faces.back();
			Dart e = d;
			do
			{
				if (!markV.isMarked(e))
				{
					vertices.push_back(map.getEmbedding(e, VERTEX_ORBIT));
					tableVertLab[e] = lab++;

					markV.mark(e);
				}
				faces.push_back(tableVertLab[e]);
				nbe++;
				e = map.phi1(e);
			} while (e!=d);
			nbf++;
		}
	}

	out << "ply" << std::endl ;
	out << "format ascii 1.0" << std::endl ;
	out << "comment ply PTM (F. Larue format)" << std::endl ;
	out << "element vertex " << vertices.size() << std::endl ;
	out << "property float x" << std::endl ;
	out << "property float y" << std::endl ;
	out << "property float z" << std::endl ;
	out << "property float tx" << std::endl ;
	out << "property float ty" << std::endl ;
	out << "property float tz" << std::endl ;
	out << "property float bx" << std::endl ;
	out << "property float by" << std::endl ;
	out << "property float bz" << std::endl ;
	out << "property float nx" << std::endl ;
	out << "property float ny" << std::endl ;
	out << "property float nz" << std::endl ;
	out << "property float L1_a" << std::endl ;
	out << "property float L1_b" << std::endl ;
	out << "property float L1_c" << std::endl ;
	out << "property float L1_d" << std::endl ;
	out << "property float L1_e" << std::endl ;
	out << "property float L1_f" << std::endl ;
	out << "property float L2_a" << std::endl ;
	out << "property float L2_b" << std::endl ;
	out << "property float L2_c" << std::endl ;
	out << "property float L2_d" << std::endl ;
	out << "property float L2_e" << std::endl ;
	out << "property float L2_f" << std::endl ;
	out << "property float L3_a" << std::endl ;
	out << "property float L3_b" << std::endl ;
	out << "property float L3_c" << std::endl ;
	out << "property float L3_d" << std::endl ;
	out << "property float L3_e" << std::endl ;
	out << "property float L3_f" << std::endl ;
	out << "element face " << nbf << std::endl ;
	out << "property list uchar int vertex_indices" << std::endl ;
	out << "end_header" << std::endl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		unsigned int vi = vertices[i];
		out << Pos[vi][0] << " " << Pos[vi][1] << " " << Pos[vi][2]<<" ";
		out << Tan[vi][0] << " " << Tan[vi][1] << " " << Tan[vi][2]<<" ";
		out << Btan[vi][0] << " " << Btan[vi][1] << " " << Btan[vi][2]<<" ";
		out << Norm[vi][0] << " " << Norm[vi][1] << " " << Norm[vi][2]<<" ";
		out << L1[vi][0] << " " << L1[vi][1] << " " << L1[vi][2]<< " " << L1[vi][3] << " " << L1[vi][4] << " " << L1[vi][5] <<" ";
		out << L2[vi][0] << " " << L2[vi][1] << " " << L2[vi][2]<< " " << L2[vi][3] << " " << L2[vi][4] << " " << L2[vi][5] <<" ";
		out << L3[vi][0] << " " << L3[vi][1] << " " << L3[vi][2]<< " " << L3[vi][3] << " " << L3[vi][4] << " " << L3[vi][5] << std::endl ;
	}

	std::vector<unsigned int>::iterator it = faces.begin();;
	while (it != faces.end())
	{
		unsigned int nbe = *it++;
		out << nbe ;
		for(unsigned int j = 0; j < nbe; ++j)
			out << " " << *it++;
		out << std::endl ;
	}

	out.close() ;
	return true ;
}


} // namespace Export

} // namespace Algo

} // namespace CGoGN
