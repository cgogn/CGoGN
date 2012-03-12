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
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2.h"
#include "Topology/generic/cellmarker.h"

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
	TraversorF<MAP> t(map, good) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		std::vector<unsigned int> fidx ;
		fidx.reserve(8) ;
		unsigned int degree = 0 ;
		Traversor2FV<typename PFP::MAP> tfv(map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			++degree ;
			unsigned int vNum = map.getEmbedding(VERTEX, it) ;
			if(!markV.isMarked(it))
			{
				markV.mark(it) ;
				vIndex[vNum] = vCpt++ ;
				vertices.push_back(vNum) ;
			}
			fidx.push_back(vIndex[vNum]) ;
		}
		facesSize.push_back(degree) ;
		facesIdx.push_back(fidx) ;
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
	TraversorF<MAP> t(map, good) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		std::vector<unsigned int> fidx ;
		fidx.reserve(8) ;
		unsigned int degree = 0 ;
		Traversor2FV<typename PFP::MAP> tfv(map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			++degree ;
			unsigned int vNum = map.getEmbedding(VERTEX, it) ;
			if(!markV.isMarked(it))
			{
				markV.mark(it) ;
				vIndex[vNum] = vCpt++ ;
				vertices.push_back(vNum) ;
			}
			fidx.push_back(vIndex[vNum]) ;
		}
		facesSize.push_back(degree) ;
		facesIdx.push_back(fidx) ;
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
bool exportPlyPTMgeneric(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::TVEC3 TVEC3;
	typedef typename PFP::REAL REAL;
	typedef typename PFP::TREAL TREAL;

	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	AutoAttributeHandler<unsigned int> tableVertLab(map, VERTEX);


	unsigned int nbDarts = map.getNbDarts() ;

	std::vector<unsigned int> vertices;
	std::vector<unsigned int> faces;

	vertices.reserve(nbDarts/5);	// TODO non optimal reservation
	faces.reserve(nbDarts/3);

	CellMarker markV(map, VERTEX);
	TraversorF<MAP> t(map, good) ;
	unsigned int lab = 0;
	unsigned int nbf = 0;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		std::vector<unsigned int> face ;
		Traversor2FV<typename PFP::MAP> tfv(map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			if (!markV.isMarked(it))
			{
				markV.mark(it);
				tableVertLab[it] = lab++;
				vertices.push_back(map.getEmbedding(VERTEX, it));
			}
			face.push_back(tableVertLab[it]);
		}

		faces.push_back(face.size()) ;
		for (unsigned int i = 0 ; i < face.size() ; ++i)
			faces.push_back(face.at(i)) ;

		++nbf;
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
	const unsigned int nbCoefs = colorPTM[14].isValid() ? 15 : (colorPTM[9].isValid() ? 10 : 6) ;

	out << "ply" << std::endl ;
	out << "format ascii 1.0" << std::endl ;
	out << "comment ply PTM (K. Vanhoey generic format)" << std::endl ;
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
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		out << "property float C0_a" << coefI << std::endl ;
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		out << "property float C1_a" << coefI << std::endl ;
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		out << "property float C2_a" << coefI << std::endl ;

	TREAL errL2 = map.template getAttribute<REAL>(VERTEX,"errL2") ;
	TREAL errLmax = map.template getAttribute<REAL>(VERTEX,"errLmax") ;
	TREAL stdDev = map.template getAttribute<REAL>(VERTEX,"stdDev") ;
	if (errL2.isValid())
		out << "property float errL2" << std::endl ;
	if (errLmax.isValid())
		out << "property float errLmax" << std::endl ;
	if (stdDev.isValid())
		out << "property float stdDev" << std::endl ;

	out << "element face " << nbf << std::endl ;
	out << "property list uchar int vertex_indices" << std::endl ;
	out << "end_header" << std::endl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		unsigned int vi = vertices[i];
		 // position
		for(unsigned int coord = 0 ; coord < 3 ; ++coord)
			out << position[vi][coord] << " " ;
		 // frame
		for(unsigned int axis = 0 ; axis < 3 ; ++axis)
			for (unsigned int coord = 0 ; coord < 3 ; ++coord)
				out << frame[axis][vi][coord] << " " ;
		 // coefficients
		for (unsigned int channel = 0 ; channel < 3 ; ++channel)
			for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
				out << colorPTM[coefI][vi][channel] << " "  ;
		 // fitting errors (if any)
		if (errL2.isValid())
			out << errL2[vi] << " " ;
		if (errLmax.isValid())
			out << errLmax[vi] << " " ;
		if (stdDev.isValid())
			out << stdDev[vi] << " " ;
		out << std::endl ;
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

template <typename PFP>
bool exportPlySLFgeneric(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::TVEC3 TVEC3;
	typedef typename PFP::REAL REAL;
	typedef typename PFP::TREAL TREAL;

	std::ofstream out(filename, std::ios::out) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	AutoAttributeHandler<unsigned int> tableVertLab(map, VERTEX);


	unsigned int nbDarts = map.getNbDarts() ;

	std::vector<unsigned int> vertices;
	std::vector<unsigned int> faces;

	vertices.reserve(nbDarts/5);	// TODO non optimal reservation
	faces.reserve(nbDarts/3);

	CellMarker markV(map, VERTEX);
	TraversorF<MAP> t(map, good) ;
	unsigned int lab = 0;
	unsigned int nbf = 0;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		std::vector<unsigned int> face ;
		Traversor2FV<typename PFP::MAP> tfv(map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			if (!markV.isMarked(it))
			{
				markV.mark(it);
				tableVertLab[it] = lab++;
				vertices.push_back(map.getEmbedding(VERTEX, it));
			}
			face.push_back(tableVertLab[it]);
		}

		faces.push_back(face.size()) ;
		for (unsigned int i = 0 ; i < face.size() ; ++i)
			faces.push_back(face.at(i)) ;

		++nbf;
	}

	TVEC3 frame[3] ;
	std::vector<TVEC3> coefs ;

	frame[0] = map.template getAttribute<VEC3>(VERTEX, "frame_T") ;
	frame[1] = map.template getAttribute<VEC3>(VERTEX, "frame_B") ;
	frame[2] = map.template getAttribute<VEC3>(VERTEX, "frame_N") ;

	unsigned int i = 0 ;
	do {
		std::stringstream name ;
		name << "SLFcoefs_" << i++ ;
		coefs.push_back(map.template getAttribute<VEC3>(VERTEX, name.str())) ;
	} while (coefs[i-1].isValid()) ;
	const unsigned int nbCoefs = i - 1 ; // last valid one is i-2

	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		assert(coefs[coefI].isValid()) ;

	std::string file(filename) ;
	size_t pos = file.rfind(".") ; // position of "." in filename
	std::string extension = file.substr(pos) ;

	out << "ply" << std::endl ;
	out << "format ascii 1.0" << std::endl ;
	out << "comment ply SLF (K. Vanhoey generic format): SLF_" << ((extension == ".plyPTMext") ? "PTMext" : "SHreal") << std::endl ;
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
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		out << "property float C0_" << coefI << std::endl ;
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		out << "property float C1_" << coefI << std::endl ;
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		out << "property float C2_" << coefI << std::endl ;

	out << "element face " << nbf << std::endl ;
	out << "property list uchar int vertex_indices" << std::endl ;
	out << "end_header" << std::endl ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		unsigned int vi = vertices[i];
		 // position
		for(unsigned int coord = 0 ; coord < 3 ; ++coord)
			out << position[vi][coord] << " " ;
		 // frame
		for(unsigned int axis = 0 ; axis < 3 ; ++axis)
			for (unsigned int coord = 0 ; coord < 3 ; ++coord)
				out << frame[axis][vi][coord] << " " ;
		 // coefficients
		for (unsigned int channel = 0 ; channel < 3 ; ++channel)
			for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
				out << coefs[coefI][vi][channel] << " "  ;

		out << std::endl ;
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

template <typename PFP>
bool exportPlySLFgenericBin(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;
	typedef typename PFP::TVEC3 TVEC3;
	typedef typename PFP::REAL REAL;
	typedef typename PFP::TREAL TREAL;

	std::ofstream out(filename, std::ios::out | std::ios::binary) ;
	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
		return false ;
	}

	AutoAttributeHandler<unsigned int> tableVertLab(map, VERTEX);


	unsigned int nbDarts = map.getNbDarts() ;

	std::vector<unsigned int> vertices;
	std::vector<unsigned int> faces;

	vertices.reserve(nbDarts/5);	// TODO non optimal reservation
	faces.reserve(nbDarts/3);

	CellMarker markV(map, VERTEX);
	TraversorF<MAP> t(map, good) ;
	unsigned int lab = 0;
	unsigned int nbf = 0;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		std::vector<unsigned int> face ;
		Traversor2FV<typename PFP::MAP> tfv(map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			if (!markV.isMarked(it))
			{
				markV.mark(it);
				tableVertLab[it] = lab++;
				vertices.push_back(map.getEmbedding(VERTEX, it));
			}
			face.push_back(tableVertLab[it]);
		}

		faces.push_back(face.size()) ;
		for (unsigned int i = 0 ; i < face.size() ; ++i)
			faces.push_back(face.at(i)) ;

		++nbf;
	}

	TVEC3 frame[3] ;
	std::vector<TVEC3> coefs ;

	frame[0] = map.template getAttribute<VEC3>(VERTEX, "frame_T") ;
	frame[1] = map.template getAttribute<VEC3>(VERTEX, "frame_B") ;
	frame[2] = map.template getAttribute<VEC3>(VERTEX, "frame_N") ;

	unsigned int i = 0 ;
	do {
		std::stringstream name ;
		name << "SLFcoefs_" << i++ ;
		coefs.push_back(map.template getAttribute<VEC3>(VERTEX, name.str())) ;
	} while (coefs[i-1].isValid()) ;
	const unsigned int nbCoefs = i - 1 ; // last valid one is i-2

	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		assert(coefs[coefI].isValid()) ;

	std::string file(filename) ;
	size_t pos = file.rfind(".") ; // position of "." in filename
	std::string extension = file.substr(pos) ;

	std::stringstream header ;
	header << "ply" << std::endl ;
	header << "format binary_little_endian 1.0" << std::endl ;
	header << "comment ply SLF (K. Vanhoey generic format): SLF_" << ((extension == ".plyPTMext") ? "PTMext" : "SHreal") << std::endl ;
	header << "element vertex " << vertices.size() << std::endl ;
	header << "property float x" << std::endl ;
	header << "property float y" << std::endl ;
	header << "property float z" << std::endl ;
	header << "property float tx" << std::endl ;
	header << "property float ty" << std::endl ;
	header << "property float tz" << std::endl ;
	header << "property float bx" << std::endl ;
	header << "property float by" << std::endl ;
	header << "property float bz" << std::endl ;
	header << "property float nx" << std::endl ;
	header << "property float ny" << std::endl ;
	header << "property float nz" << std::endl ;
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		header << "property float C0_" << coefI << std::endl ;
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		header << "property float C1_" << coefI << std::endl ;
	for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
		header << "property float C2_" << coefI << std::endl ;

	header << "element face " << nbf << std::endl ;
	header << "property list uchar int vertex_indices" << std::endl ;
	header << "end_header" << std::endl ;

	size_t nbCharsOfHeader = header.str().size() ;
	out.write((char*)nbCharsOfHeader, sizeof(size_t)) ;
	out.write((char*)(header.str().c_str()), nbCharsOfHeader*sizeof(char)) ;

	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		unsigned int vi = vertices[i];
		 // position
		for(unsigned int coord = 0 ; coord < 3 ; ++coord)
			out.write((char*)(&(position[vi][coord])), sizeof(float)) ;
		 // frame
		for(unsigned int axis = 0 ; axis < 3 ; ++axis)
			for (unsigned int coord = 0 ; coord < 3 ; ++coord)
				out.write((char*)(&(frame[axis][vi][coord])), sizeof(float)) ;
		 // coefficients
		for (unsigned int channel = 0 ; channel < 3 ; ++channel)
			for(unsigned int coefI = 0 ; coefI < nbCoefs ; ++coefI)
				out.write((char*)(&(coefs[vi][coefI])), sizeof(float)) ;
	}

	std::vector<unsigned int>::iterator it = faces.begin();
	while (it != faces.end())
	{
		unsigned int nbe = *it++;
		out.write((char*)nbe, sizeof(unsigned int)) ;
		for(unsigned int j = 0; j < nbe; ++j)
			out.write((char*)(*it++), sizeof(unsigned int)) ;
	}

	out.close() ;
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

	AutoAttributeHandler<unsigned int> tableVertLab(map, VERTEX);

	unsigned int nbDarts = map.getNbDarts() ;

	std::vector<unsigned int> vertices;
	std::vector<unsigned int> faces;

	vertices.reserve(nbDarts/5);	// TODO non optimal reservation
	faces.reserve(nbDarts/3);

	CellMarker markV(map, VERTEX);
	TraversorF<MAP> t(map, good) ;
	unsigned int lab = 0;
	unsigned int nbf = 0;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		std::vector<unsigned int> face ;
		Traversor2FV<typename PFP::MAP> tfv(map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			if (!markV.isMarked(it))
			{
				markV.mark(it);
				tableVertLab[it] = lab++;
				vertices.push_back(map.getEmbedding(VERTEX, it));
			}
			face.push_back(tableVertLab[it]);
		}

		faces.push_back(face.size()) ;
		for (unsigned int i = 0 ; i < face.size() ; ++i)
			faces.push_back(face.at(i)) ;

		++nbf;
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
}

} // namespace Export

} // namespace Algo

} // namespace CGoGN
