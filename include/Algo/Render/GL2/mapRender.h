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

#ifndef _GL2_MAP_RENDER_
#define _GL2_MAP_RENDER_

#include <GL/glew.h>
#include <vector>
#include <list>

#include "Topology/generic/dart.h"
#include "Topology/generic/functor.h"
#include "Topology/generic/attributeHandler.h"
#include "Container/convert.h"
#include "Geometry/vector_gen.h"

#include "Utils/vbo.h"


namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

enum drawingType {
	TRIANGLES = 1,
	LINES = 2,
	POINTS = 4,
	EXPLODED = 8,
	FLAT_TRIANGLES = 16,
	ERR = 32
} ;

enum bufferIndex {
	TRIANGLE_INDICES = 0,
	LINE_INDICES = 1,
	POINT_INDICES = 2,
	FLAT_BUFFER = 3,
} ;

//const unsigned int NB_BUFFERS = 16 ;

// forward def
class Utils::GLSLShader;

class MapRender
{

protected:
	/**
	 * vbo buffers
	 */
	GLuint m_VBOBuffers[4] ;

	/**
	 * number of indices of triangles
	 */
	GLuint m_nbIndicesTri ;

	/**
	 * number of indices of lines
	 */
	GLuint m_nbIndicesLines ;

	/**
	 * number of indices of points
	 */
	GLuint m_nbIndicesPoints ;

	/**
	 * number of elts for flat vbo
	 */
	GLuint m_nbFlatElts;

public:
	/**
	 * Constructor
	 */
	MapRender() ;

	/**
	 * Constructor that share vertices attributes vbo (position/normals/colors...)
	 */
	MapRender(const MapRender& mrvbo);

	/**
	 * Destructor
	 */
	~MapRender() ;

protected:
	/**
	 * addition of indices table of one triangle
	 * @param d a dart of the triangle
	 * @param tableIndices the indices table
	 */
	template <typename PFP>
	void addTri(typename PFP::MAP& map, Dart d, std::vector<GLuint>& tableIndices) ;

public:
	/**
	 * creation of indices table of triangles (optimized order)
	 * @param tableIndices the table where indices are stored
	 */
	template <typename PFP>
	void initTriangles(typename PFP::MAP& map, const FunctorSelect& good,std::vector<GLuint>& tableIndices, unsigned int thread=0) ;
	template <typename PFP>
	void initTrianglesOptimized(typename PFP::MAP& map, const FunctorSelect& good,std::vector<GLuint>& tableIndices, unsigned int thread=0) ;

	/**
	 * creation of indices table of lines (optimized order)
	 * @param tableIndices the table where indices are stored
	 */
	template <typename PFP>
	void initLines(typename PFP::MAP& map, const FunctorSelect& good,std::vector<GLuint>& tableIndices, unsigned int thread=0) ;
	template <typename PFP>
	void initLinesOptimized(typename PFP::MAP& map, const FunctorSelect& good,std::vector<GLuint>& tableIndices, unsigned int thread=0) ;

	/**
	 * creation of indices table of points
	 * @param tableIndices the table where indices are stored
	 */
	template <typename PFP>
	void initPoints(typename PFP::MAP& map, const FunctorSelect& good,std::vector<GLuint>& tableIndices, unsigned int thread=0) ;

	/**
	 * creation of VBO for flat faces rendering
	 */
//	template <typename PFP>
//	void initFlatTriangles(typename PFP::MAP& map, const Utils::GL2& vboposition, const FunctorSelect& good=SelectorTrue() , unsigned int thread=0);

	/**
	 * initialization of the VBO indices primitives
	 * computed by a traversal of the map
	 * @param prim primitive to draw: VBO_TRIANGLES, VBO_LINES
	 */
	template <typename PFP>
	void initPrimitives(typename PFP::MAP& map, const FunctorSelect& good, int prim, bool optimized = true, unsigned int thread=0) ;

	/**
	 * initialization of the VBO indices primitives
	 * using the given table
	 * @param prim primitive to draw: VBO_TRIANGLES, VBO_LINES
	 */
	void initPrimitives(int prim, std::vector<GLuint>& tableIndices) ;

public:
	/**
	 * draw the VBO (function to call in the drawing callback)
	 */
	void draw(Utils::GLSLShader* sh, int prim) ;
} ;

} // namespace GL2

} // namespace Render

} // namespace Algo

} // namespace CGoGN

#include "Algo/Render/GL2/mapRender.hpp"

#endif
