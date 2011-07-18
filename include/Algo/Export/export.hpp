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

	CellMarker markV(map, VERTEX) ;
	DartMarker markF(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(good(d) && !markF.isMarked(d))
		{
			markF.markOrbit(FACE, d) ;
			std::vector<unsigned int> fidx ;
			fidx.reserve(4) ;
			Dart dd = d ;
			do
			{
				unsigned int vNum = map.getEmbedding(VERTEX, dd) ;
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

	CellMarker markV(map, VERTEX) ;
	DartMarker markF(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(good(d) && !markF.isMarked(d))
		{
			markF.markOrbit(FACE, d) ;
			std::vector<unsigned int> fidx ;
			fidx.reserve(4) ;
			Dart dd = d ;
			do
			{
				unsigned int vNum = map.getEmbedding(VERTEX, dd) ;
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

	AutoAttributeHandler<unsigned int> tableVertLab(the_map, VERTEX);

	CellMarker markV(the_map,VERTEX);

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
			markF.markOrbit(FACE, d) ;
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
				}
				indicesBuffer.push_back(tableVertLab[e]);
				e = the_map.phi1(e);
			} while (e!=d);
		}
	}

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
bool exportPlyPTMgeneric(typename PFP::MAP& map, const char* filename, const typename PFP::TVEC3& position, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::TVEC3 TVEC3;
	typedef typename PFP::REAL REAL;
	typedef typename PFP::TREAL TREAL;

	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << CGoGNout ;
		return false ;
	}

	AutoAttributeHandler<unsigned int> tableVertLab(map, VERTEX);

	CellMarker markV(map,VERTEX);

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
			markF.markOrbit(FACE, d) ;
			Dart e = d;
			std::vector<unsigned int> face ;
			do
			{
				if (!markV.isMarked(e))
				{
					vertices.push_back(map.getEmbedding(VERTEX, e));
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

	TVEC3 frame[3] ;
	TVEC3 colorPTM[15] ;

	frame[0] = map.template getAttribute<VEC3>(VERTEX, "frame_T") ;
	frame[1] = map.template getAttribute<VEC3>(VERTEX, "frame_B") ;
	frame[2] = map.template getAttribute<VEC3>(VERTEX, "frame_N") ;
	for (unsigned i = 0 ; i < 15 ; ++i)
	{
		std::stringstream name ;
		name << "colorPTM_a" << i ;
		colorPTM[i] = map.template getAttribute<VEC3>(VERTEX,name.str()) ;
	}
	const unsigned int degree = colorPTM[14].isValid() ? 4 : (colorPTM[9].isValid() ? 3 : 2) ;


	out << "ply" << std::endl ;
	out << "format ascii 1.0" << std::endl ;
	out << "comment ply PTM" << std::endl ;
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
	out << "property float L1_a0" << std::endl ;
	out << "property float L1_a1" << std::endl ;
	out << "property float L1_a2" << std::endl ;
	out << "property float L1_a3" << std::endl ;
	out << "property float L1_a4" << std::endl ;
	out << "property float L1_a5" << std::endl ;
	if (degree > 2) {
		out << "property float L1_a6" << std::endl ;
		out << "property float L1_a7" << std::endl ;
		out << "property float L1_a8" << std::endl ;
		out << "property float L1_a9" << std::endl ;
		if (degree > 3) {
			out << "property float L1_a10" << std::endl ;
			out << "property float L1_a11" << std::endl ;
			out << "property float L1_a12" << std::endl ;
			out << "property float L1_a13" << std::endl ;
			out << "property float L1_a14" << std::endl ;
		}
	}
	out << "property float L2_a0" << std::endl ;
	out << "property float L2_a1" << std::endl ;
	out << "property float L2_a2" << std::endl ;
	out << "property float L2_a3" << std::endl ;
	out << "property float L2_a4" << std::endl ;
	out << "property float L2_a5" << std::endl ;
	if (degree > 2) {
		out << "property float L2_a6" << std::endl ;
		out << "property float L2_a7" << std::endl ;
		out << "property float L2_a8" << std::endl ;
		out << "property float L2_a9" << std::endl ;
		if (degree > 3) {
			out << "property float L2_a10" << std::endl ;
			out << "property float L2_a11" << std::endl ;
			out << "property float L2_a12" << std::endl ;
			out << "property float L2_a13" << std::endl ;
			out << "property float L2_a14" << std::endl ;
		}
	}
	out << "property float L3_a0" << std::endl ;
	out << "property float L3_a1" << std::endl ;
	out << "property float L3_a2" << std::endl ;
	out << "property float L3_a3" << std::endl ;
	out << "property float L3_a4" << std::endl ;
	out << "property float L3_a5" << std::endl ;
	if (degree > 2) {
		out << "property float L3_a6" << std::endl ;
		out << "property float L3_a7" << std::endl ;
		out << "property float L3_a8" << std::endl ;
		out << "property float L3_a9" << std::endl ;
		if (degree > 3) {
			out << "property float L3_a10" << std::endl ;
			out << "property float L3_a11" << std::endl ;
			out << "property float L3_a12" << std::endl ;
			out << "property float L3_a13" << std::endl ;
			out << "property float L3_a14" << std::endl ;
		}
	}

	TREAL errL2 = map.template getAttribute<REAL>(VERTEX,"errL2") ;
	TREAL errLmax = map.template getAttribute<REAL>(VERTEX,"errLmax") ;
	TREAL stdDev = map.template getAttribute<REAL>(VERTEX,"StdDev") ;
	if (errL2.isValid())
		out << "property float errL2" << std::endl ;
	if (errLmax.isValid())
		out << "property float errLmax" << std::endl ;
	if (stdDev.isValid())
		out << "property float stdDev" << std::endl ;

	out << "element face " << nbf << std::endl ;
	out << "property list uchar int vertex_indices" << std::endl ;
	out << "end_header" << std::endl ;

	switch (degree) {
	case(2) :
			for(unsigned int i = 0; i < vertices.size(); ++i)
			{
				unsigned int vi = vertices[i];
				out << position[vi][0] << " " << position[vi][1] << " " << position[vi][2] << " " ;
				out << frame[0][vi][0] << " " << frame[0][vi][1] << " " << frame[0][vi][2] << " " ;
				out << frame[1][vi][0] << " " << frame[1][vi][1] << " " << frame[1][vi][2] << " " ;
				out << frame[2][vi][0] << " " << frame[2][vi][1] << " " << frame[2][vi][2] << " " ;
				out << colorPTM[0][vi][0] << " " << colorPTM[1][vi][0] << " " << colorPTM[2][vi][0] << " " << colorPTM[3][vi][0] << " " << colorPTM[4][vi][0] << " " << colorPTM[5][vi][0] << " " ;
				out << colorPTM[0][vi][1] << " " << colorPTM[1][vi][1] << " " << colorPTM[2][vi][1] << " " << colorPTM[3][vi][1] << " " << colorPTM[4][vi][1] << " " << colorPTM[5][vi][1] << " " ;
				out << colorPTM[0][vi][2] << " " << colorPTM[1][vi][2] << " " << colorPTM[2][vi][2] << " " << colorPTM[3][vi][2] << " " << colorPTM[4][vi][2] << " " << colorPTM[5][vi][2] << " " ;
				if (errL2.isValid())
					out << errL2[vi] << " " ;
				if (errLmax.isValid())
					out << errLmax[vi] << " " ;
				if (stdDev.isValid())
					out << stdDev[vi] << " " ;
				out << std::endl ;
			}
			break ;
	case(3) :
			for(unsigned int i = 0; i < vertices.size(); ++i)
			{
				unsigned int vi = vertices[i];
				out << position[vi][0] << " " << position[vi][1] << " " << position[vi][2] << " " ;
				out << frame[0][vi][0] << " " << frame[0][vi][1] << " " << frame[0][vi][2] << " " ;
				out << frame[1][vi][0] << " " << frame[1][vi][1] << " " << frame[1][vi][2] << " " ;
				out << frame[2][vi][0] << " " << frame[2][vi][1] << " " << frame[2][vi][2] << " " ;
				out << colorPTM[0][vi][0] << " " << colorPTM[1][vi][0] << " " << colorPTM[2][vi][0] << " " << colorPTM[3][vi][0] << " " << colorPTM[4][vi][0] << " " << colorPTM[5][vi][0] << " " << colorPTM[6][vi][0] << " " << colorPTM[7][vi][0] << " " << colorPTM[8][vi][0] << " " << colorPTM[9][vi][0] << " " ;
				out << colorPTM[0][vi][1] << " " << colorPTM[1][vi][1] << " " << colorPTM[2][vi][1] << " " << colorPTM[3][vi][1] << " " << colorPTM[4][vi][1] << " " << colorPTM[5][vi][1] << " " << colorPTM[6][vi][1] << " " << colorPTM[7][vi][1] << " " << colorPTM[8][vi][1] << " " << colorPTM[9][vi][1] << " " ;
				out << colorPTM[0][vi][2] << " " << colorPTM[1][vi][2] << " " << colorPTM[2][vi][2] << " " << colorPTM[3][vi][2] << " " << colorPTM[4][vi][2] << " " << colorPTM[5][vi][2] << " " << colorPTM[6][vi][2] << " " << colorPTM[7][vi][2] << " " << colorPTM[8][vi][2] << " " << colorPTM[9][vi][2] << " " ;
				if (errL2.isValid())
					out << errL2[vi] << " " ;
				if (errLmax.isValid())
					out << errLmax[vi] << " " ;
				if (stdDev.isValid())
					out << stdDev[vi] << " " ;
				out << std::endl ;
			}

			break ;
	case (4) :
			for(unsigned int i = 0; i < vertices.size(); ++i)
			{
				unsigned int vi = vertices[i];
				out << position[vi][0] << " " << position[vi][1] << " " << position[vi][2] << " " ;
				out << frame[0][vi][0] << " " << frame[0][vi][1] << " " << frame[0][vi][2] << " " ;
				out << frame[1][vi][0] << " " << frame[1][vi][1] << " " << frame[1][vi][2] << " " ;
				out << frame[2][vi][0] << " " << frame[2][vi][1] << " " << frame[2][vi][2] << " " ;
				out << colorPTM[0][vi][0] << " " << colorPTM[1][vi][0] << " " << colorPTM[2][vi][0] << " " << colorPTM[3][vi][0] << " " << colorPTM[4][vi][0] << " " << colorPTM[5][vi][0] << " " << colorPTM[6][vi][0] << " " << colorPTM[7][vi][0] << " " << colorPTM[8][vi][0] << " " << colorPTM[9][vi][0] << " " << colorPTM[10][vi][0] << " " << colorPTM[11][vi][0] << " " << colorPTM[12][vi][0] << " " << colorPTM[13][vi][0] << " " << colorPTM[14][vi][0] << " " ;
				out << colorPTM[0][vi][1] << " " << colorPTM[1][vi][1] << " " << colorPTM[2][vi][1] << " " << colorPTM[3][vi][1] << " " << colorPTM[4][vi][1] << " " << colorPTM[5][vi][1] << " " << colorPTM[6][vi][1] << " " << colorPTM[7][vi][1] << " " << colorPTM[8][vi][1] << " " << colorPTM[9][vi][1] << " " << colorPTM[10][vi][1] << " " << colorPTM[11][vi][1] << " " << colorPTM[12][vi][1] << " " << colorPTM[13][vi][1] << " " << colorPTM[14][vi][1] << " " ;
				out << colorPTM[0][vi][2] << " " << colorPTM[1][vi][2] << " " << colorPTM[2][vi][2] << " " << colorPTM[3][vi][2] << " " << colorPTM[4][vi][2] << " " << colorPTM[5][vi][2] << " " << colorPTM[6][vi][2] << " " << colorPTM[7][vi][2] << " " << colorPTM[8][vi][2] << " " << colorPTM[9][vi][2] << " " << colorPTM[10][vi][2] << " " << colorPTM[11][vi][2] << " " << colorPTM[12][vi][2] << " " << colorPTM[13][vi][2] << " " << colorPTM[14][vi][2] << " " ;
				if (errL2.isValid())
					out << errL2[vi] << " " ;
				if (errLmax.isValid())
					out << errLmax[vi] << " " ;
				if (stdDev.isValid())
					out << stdDev[vi] << " " ;
				out << std::endl ;
			}

			break ;
	default :
		assert(false || !"exportPlyPTM : degree not in {2,3,4} unhandled") ;
		break ;
	}

	std::vector<unsigned int>::iterator it = faces.begin();
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
/*
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

	AutoAttributeHandler<unsigned int> tableVertLab(map, VERTEX);

	CellMarker markV(map,VERTEX);

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
			markF.markOrbit(FACE, d) ;
			Dart e = d;
			std::vector<unsigned int> face ;
			do
			{
				if (!markV.isMarked(e))
				{
					vertices.push_back(map.getEmbedding(VERTEX, e));
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
		out << position[vi][0] << " " << position[vi][1] << " " << position[vi][2] << " " ;
		out << frame[0][vi][0] << " " << frame[0][vi][1] << " " << frame[0][vi][2] << " " ;
		out << frame[1][vi][0] << " " << frame[1][vi][1] << " " << frame[1][vi][2] << " " ;
		out << frame[2][vi][0] << " " << frame[2][vi][1] << " " << frame[2][vi][2] << " " ;
		out << colorPTM[0][vi][0] << " " << colorPTM[1][vi][0] << " " << colorPTM[2][vi][0] << " " << colorPTM[3][vi][0] << " " << colorPTM[4][vi][0] << " " << colorPTM[5][vi][0] <<" " ;
		out << colorPTM[0][vi][1] << " " << colorPTM[1][vi][1] << " " << colorPTM[2][vi][1] << " " << colorPTM[3][vi][1] << " " << colorPTM[4][vi][1] << " " << colorPTM[5][vi][1] <<" " ;
		out << colorPTM[0][vi][2] << " " << colorPTM[1][vi][2] << " " << colorPTM[2][vi][2] << " " << colorPTM[3][vi][2] << " " << colorPTM[4][vi][2] << " " << colorPTM[5][vi][2] << std::endl ;
	}

	std::vector<unsigned int>::iterator it = faces.begin();
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
}*/


template <typename PFP>
bool exportInESS(typename PFP::MAP& map, const char *filename, const typename PFP::TVEC3& position)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << CGoGNendl ;
		return false ;
	}


}

} // namespace Export

} // namespace Algo

} // namespace CGoGN
