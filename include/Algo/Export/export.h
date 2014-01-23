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

#ifndef __EXPORT_H__
#define __EXPORT_H__

#include "Topology/generic/attributeHandler.h"
#include "Algo/Import/importFileTypes.h"


#include <stdint.h>

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Export
{

/**
* export the map into a PLY file
* @param the_map map to be exported
* @param position the position container
* @param filename filename of ply file
* @param binary write in binary mode
* @return true
*/
template <typename PFP>
bool exportPLY(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename, const bool binary) ;

/**
* export the map into a PLY file
* @param the_map map to be exported
* @param vertexAttrNames the vertex attribute names
* @param filename filename of ply file
* @param binary write in binary mode
* @return true
*/
template <typename PFP>
bool exportPLYnew(typename PFP::MAP& map, const std::vector<VertexAttribute<typename PFP::VEC3>* >& attributeHandlers, const char* filename, const bool binary) ;

/**
* export the map into a OFF file
* @param the_map map to be exported
* @param filename filename of off file
* @return true
*/
template <typename PFP>
bool exportOFF(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename) ;

/**
* export the map into a OBJ file
* @param the_map map to be exported
* @param filename filename of obj file
* @return true
*/
template <typename PFP>
bool exportOBJ(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename) ;

/**
* export the map into a Trian file
* @param the_map map to be exported
* @param filename filename of trian file
* @return true
*/
template <typename PFP>
bool exportTrian(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, char* filename) ;


/**
* export the map into a VTU file (VTK unstructured grid xml format)
* @param map map to be exported
* @param position the position container
* @param filename filename of ply file
* @return true if ok
*/
template <typename PFP>
bool exportVTU(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename);

template <typename PFP>
bool exportVTUCompressed(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename);


template <typename PFP>
class VTUExporter
{
protected:
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	typename PFP::MAP& m_map;
	const VertexAttribute<typename PFP::VEC3>& m_position;

	unsigned int nbtotal;
	bool noPointData;
	bool noCellData;
	bool closed;

	std::ofstream fout ;

	std::vector<unsigned int> triangles;
	std::vector<unsigned int> quads;
	std::vector<unsigned int> others;
	std::vector<unsigned int> others_begin;

	std::vector<Dart> bufferTri;
	std::vector<Dart> bufferQuad;
	std::vector<Dart> bufferOther;


public:

	VTUExporter(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position);

	~VTUExporter();

	/**
	 * @brief start writing header of vru file
	 * @param filename
	 * @return true if ok
	 */
	bool init(const char* filename);


	/**
	 * @brief add a vertex attribute of type scalar
	 * @param attrib
	 * @param vtkType Float32/Int32
	 * @param name Data name if none then used Attribute's name
	 */
	template<typename T>
	void addVertexAttributeScal(const VertexAttribute<T>& attrib, const std::string& vtkType, const std::string& name="");

	/**
	 * @brief add a vertex attribute of type vector
	 * @param attrib
	 * @param vtkType Float32/Int32
	 * @param nbComp number components in attribute (if none comute for size of attribute)
	 * @param name Data name if none then used Attribute's name
	 */
	template<typename T>
	void addVertexAttributeVect(const VertexAttribute<T>& attrib, const std::string& vtkType, unsigned int nbComp=0, const std::string& name="");


	/**
	 * @brief finish adding vertex attributes data
	 */
	void endVertexAttributes();

	/**
	 * @brief add a face attribute of type scalar
	 * @param attrib
	 * @param vtkType Float32/Int32
	 * @param name Data name if none then used Attribute's name
	 */
	template<typename T>
	void addFaceAttributeScal(const FaceAttribute<T>& attrib, const std::string& vtkType, const std::string& name="");

	/**
	 * @brief add a face aatribute of type vector
	 * @param attrib
	 * @param vtkType Float32/Int32
	 * @param nbComp number components in attribute (if none comute for size of attribute)
	 * @param name Data name if none then used Attribute's name
	 */
	template<typename T>
	void addFaceAttributeVect(const FaceAttribute<T>& attrib, const std::string& vtkType, unsigned int nbComp=0, const std::string& name="");


	/**
	 * @brief finish adding face attributes data
	 */
	void endFaceAttributes();


	/**
	 * @brief finalize file writing & close (automatically called at destruction)
	 * @return true if ok
	 */
	bool close();

};



/**
* export the map into a PLYPTMgeneric file (K. Vanhoey generic format).
*
* exports position + any attribute named : "frame_T" (frame tangent : VEC3), "frame_B" (frame binormal : VEC3), "frame_N" (frame normal : VEC3),
* "colorPTM_a<i> : VEC3" (coefficient number i of the 3 polynomials - one per channel - ; the max i depends on the degree of the PTM polynomial),
* "errL2 : REAL" (L2 fitting error), "errLmax : REAL" (maximal fitting error), "stdDev : REAL" (standard deviation of the L2 fitting errors).
*
* @param map map to be exported
* @param filename filename of ply file
* @param position the position container
* @return true
*/
//template <typename PFP>
//bool exportPlySLFgeneric(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename) ;

/**
* export the map into a PLYPTMgeneric file (K. Vanhoey generic format).
*
* exports position + any attribute named : "frame_T" (frame tangent : VEC3), "frame_B" (frame binormal : VEC3), "frame_N" (frame normal : VEC3),
* "colorPTM_a<i> : VEC3" (coefficient number i of the 3 polynomials - one per channel - ; the max i depends on the degree of the PTM polynomial),
* "errL2 : REAL" (L2 fitting error), "errLmax : REAL" (maximal fitting error), "stdDev : REAL" (standard deviation of the L2 fitting errors).
*
* @param map map to be exported
* @param filename filename of ply file
* @param position the position container
* @return true
*/
//template <typename PFP>
//bool exportPlySLFgenericBin(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename) ;

/**
* export the map into a PLYSLF file (K. Vanhoey generic format).
*
* exports position + any attribute named : "frame_T" (frame tangent : VEC3), "frame_B" (frame binormal : VEC3), "frame_N" (frame normal : VEC3),
* "SLF_<i> : VEC3" (coefficient number i of the 3   - one per channel - ; the max i is nbCoefs),
*
* @param map map to be exported
* @param filename filename of ply file
* @param position the position container
* @param nbCoefs the number of coefficients of the representation
* @return true
*/
/*template <typename PFP>
bool exportPlyPTMgeneric(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const char* filename) ;
*/
/**
* export the map into a PLYPTMgeneric file (K. Vanhoey generic format)
* @param map map to be exported
* @param filename filename of ply file
* @param position the position container
* @param the local frame (3xVEC3 : tangent, bitangent, normal)
* @param colorPTM the 6 coefficients (x3 channels) of the PTM functions
* @return true
*/
/*
template <typename PFP>
bool exportPLYPTM(typename PFP::MAP& map, const char* filename, const VertexAttribute<typename PFP::VEC3>& position, const VertexAttribute<typename PFP::VEC3> frame[3], const VertexAttribute<typename PFP::VEC3> colorPTM[6]) ;
*/
/**
 * export meshes used at the workbench
 * export just a list of vertices and edges connectivity
 * @param map
 * @param position
 * @return
 */
template <typename PFP>
bool exportChoupi(typename PFP::MAP& map, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, const char* filename) ;


} // namespace Export

}

} // namespace Algo

} // namespace CGoGN

#include "Algo/Export/export.hpp"

#endif
