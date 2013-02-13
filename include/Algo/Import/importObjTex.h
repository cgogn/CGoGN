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
#ifndef IMPORTOBJTEX_H
#define IMPORTOBJTEX_H

#include "Topology/generic/mapBrowser.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{
namespace Algo
{
namespace Surface
{
namespace Import
{


struct MaterialOBJ
{
	Geom::Vec3f ambiantColor;
	Geom::Vec3f diffuseColor;
	Geom::Vec3f specularColor;
	float shininess;

	// TODO add textures info
};


template <typename PFP>
class OBJModel
{
	typedef  typename PFP::VEC3 VEC3;
	typedef Geom::Vec2f VEC2;

protected:
	typename PFP::MAP& m_map;

	/// marker for special vertices (with several normals & tex coords)
	CellMarker<VERTEX> m_specialVertices;

	/// marker for darts with phi2 reconstruction face
	DartMarker m_dirtyEdges;

	/// Face Attribute for group ID storage
	FaceAttribute<unsigned int> m_groups;

	/// Vertex Attribute Handlers
	VertexAttribute<VEC3> m_positions;
	VertexAttribute<VEC3> m_normals;
	VertexAttribute<Geom::Vec2f> m_texCoords;

	/// Vertex of face Attribute Handlers
	AttributeHandler<VEC3,VERTEX1> m_normalsF;
	AttributeHandler<Geom::Vec2f,VERTEX1> m_texCoordsF;

	/// vector of group name
	std::vector<std::string> m_groupNames;

	/// vector of material names
	std::vector<std::string> m_materialNames;

	/// vector of material struct
	std::vector<MaterialOBJ> m_materials;

	/// read face line with different indices v  v/t v//n v/t/n
	short readObjLine(std::stringstream& oss, std::vector<unsigned int>& indices);

public:

	/**
	 * @brief Constructeur
	 * @param map
	 */
	OBJModel(typename PFP::MAP& map);

	/**
	 * @brief import
	 * @param filename
	 * @param attrNames
	 * @return
	 */
	bool import(const std::string& filename, std::vector<std::string>& attrNames);

	// Faire un handler ?
	/**
	 * @brief getNormal
	 * @param d
	 * @return
	 */
	typename PFP::VEC3 getNormal(Dart d);

	/**
	 * @brief getTexCoord
	 * @param d
	 * @return
	 */
	Geom::Vec2f getTexCoord(Dart d);

	/**
	 * @brief getPosition
	 * @param d
	 * @return
	 */
	typename PFP::VEC3 getPosition(Dart d);


	/**
	 * @brief Generate one browser per group
	 * @param browsers vector of MapBrowers representing the groups
	 * @return ok or not
	 */
	bool generateBrowsers(std::vector<MapBrowser*>& browsers);

	/**
	 * @brief getMaterialNames
	 * @return
	 */
	std::vector<std::string>& getMaterialNames();

	/**
	 * @brief read materials from files
	 * @param filename name of file
	 */
	void readMaterials(const std::string& filename);

};


}
}
} // end namespaces
}

#include "importObjTex.hpp"

#endif // IMPORTOBJTEX_H
