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

#ifndef _IMPORT2TABLE_H
#define _IMPORT2TABLE_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

//#include "Topology/generic/attributeHandler.h"
#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

#include "gzstream.h"
#include "openctm.h"
#include "assimp.h"
#include "aiScene.h"

namespace CGoGN
{

namespace Algo
{

namespace Import
{

	namespace ImportSurfacique
	{
		enum ImportType { UNKNOWNSURFACE, TRIAN, TRIANBGZ, PLY, PLYPTM, OFF, OBJ, CTM, VRML };
	}

	namespace ImportVolumique
	{
		enum ImportType { UNKNOWNVOLUME ,TET ,TRIANBGZ ,PLY };
	}


template <typename PFP>
class MeshTablesSurface
{
protected:
	typename PFP::MAP& m_map;

	unsigned m_nbVertices;

	unsigned m_nbFaces;

	unsigned int m_lab;

	/**
	* number of edges per face
	*/
	std::vector<short> m_nbEdges;

	/**
	* table of emb indice (for each edge, first vertex)
	*/
	std::vector<unsigned int> m_emb;

	static ImportSurfacique::ImportType getFileType(const std::string& filename);

	void extractMeshRec(AttribContainer& container, AttributeHandler<typename PFP::VEC3>& positions, const struct aiScene* scene, const struct aiNode* nd, struct aiMatrix4x4* trafo);

public:
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename VEC3::DATA_TYPE DATA_TYPE ;

	inline unsigned getNbFaces() const { return m_nbFaces; }

	inline unsigned getNbVertices() const { return m_nbVertices; }

	inline short getNbEdgesFace(int i) const  { return m_nbEdges[i]; }

	inline unsigned int getEmbIdx(int i) { return  m_emb[i]; }

	bool importMesh(const std::string& filename, std::vector<std::string>& attrNames, ImportSurfacique::ImportType kind);

	bool importTrian(const std::string& filename, std::vector<std::string>& attrNames);

	bool importTrianBinGz(const std::string& filename, std::vector<std::string>& attrNames);

	bool importOff(const std::string& filename, std::vector<std::string>& attrNames);

	bool importObj(const std::string& filename, std::vector<std::string>& attrNames);

	bool importPly(const std::string& filename, std::vector<std::string>& attrNames);

	bool importPlyPTM(const std::string& filename, std::vector<std::string>& attrNames);

	bool importCTM(const std::string& filename, std::vector<std::string>& attrNames);

	bool importASSIMP(const std::string& filename, std::vector<std::string>& attrNames);

	/**
	 * @param container container of vertex orbite
	 * @param idPositions id of position attribute in the container
	 * @param idLabels id of label attribute in the container
	 */
	MeshTablesSurface(typename PFP::MAP& map):
		m_map(map)
	{
	}
};


template <typename PFP>
class MeshTablesVolume
{
protected:
	typename PFP::MAP& m_map;

	unsigned m_nbVertices;

	unsigned m_nbFaces;

	unsigned m_nbVolumes;

	/**
	* number of edges per face
	*/
	std::vector<short> m_nbEdges;

	/**
	* table of emb ptr (for each face, first vertex)
	*/
	std::vector<unsigned int> m_emb;

	static ImportVolumique::ImportType getFileType(const std::string& filename);

public:
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename VEC3::DATA_TYPE DATA_TYPE ;

	typedef Geom::Vector<6,DATA_TYPE> VEC6;
	typedef Geom::Matrix<3,3,DATA_TYPE> MAT33;
	typedef Geom::Matrix<3,6,DATA_TYPE> MAT36;

	inline short getNbEdgesFace(int i) const  { return m_nbEdges[i]; }

	inline unsigned getNbVolumes() const { return m_nbVolumes; }

	inline unsigned getNbFaces() const { return m_nbFaces; }

	inline unsigned getNbVertices() const { return m_nbVertices; }

	//inline short getNbVerticesPerFace(int i) const  { return m_nbVerticesPerFace[i]; }

	//inline short getNbFacesPerVolume(int i) const { return m_nbFacesPerVolume[i]; }

	inline unsigned int getEmbIdx(int i) { return  m_emb[i]; }

	bool importMesh(const std::string& filename, std::vector<std::string>& attrNames, ImportVolumique::ImportType kind, float scaleFactor);

	bool importTet(const std::string& filename, std::vector<std::string>& attrNames, float scaleFactor);

	bool importPly(const std::string& filename, std::vector<std::string>& attrNames);

	bool importTrianBinGz(const std::string& filename, std::vector<std::string>& attrNames);

	MeshTablesVolume(typename PFP::MAP& map):
		m_map(map)
	{
	}
};

} // namespace Import

} // namespace Algo

} // namespace CGoGN

#include "Algo/Import/import2tablesSurface.hpp"
#include "Algo/Import/import2tablesVolume.hpp"

#endif 
