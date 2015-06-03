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

#ifndef __SUBDIVISION_H__
#define __SUBDIVISION_H__

#include <cmath>

#include <vector>
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Modelisation
{

/**
* Triangule a face with central vertex
* @param d dart of face
* @return a dart of the central vertex
*/
template <typename PFP>
Dart trianguleFace(typename PFP::MAP& map, Dart d);

/**
 * Triangule all the faces of the mesh
 */
template <typename PFP, typename EMBV>
void trianguleFaces(typename PFP::MAP& map, EMBV& attributs) ;

//template <typename PFP>
//void trianguleFaces(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& position) ;

/**
 * Triangule all the faces of the mesh
 * positions for new vertices are given as face attribute
 */
template <typename PFP>
void trianguleFaces(
	typename PFP::MAP& map,
	VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position,
	const FaceAttribute<typename PFP::VEC3, typename PFP::MAP>& positionF) ;

/**
* Quadrangule a face with central vertex
* warning: edges are considered here as already cut !!
* A face with 2n edges give n quads
* @param d dart of face with belong to a vertex introduce when cutting the edge
* @return a dart of central point
*/
template <typename PFP>
Dart quadranguleFace(typename PFP::MAP& map, Dart d);

/**
 * Quadrangule all the faces of the mesh
 */
template <typename PFP, typename EMBV>
void quadranguleFaces(typename PFP::MAP& map, EMBV& attributs) ;

//template <typename PFP>
//void quadranguleFaces(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position) ;

/**
 * Catmull-Clark subdivision scheme
 */
template <typename PFP, typename EMBV>
void CatmullClarkSubdivision(typename PFP::MAP& map, EMBV& attributs) ;

/**
* Interpolant Catmull-Clark subdivision scheme
*/
template <typename PFP, typename EMBV>
void CatmullClarkInterpolSubdivision(typename PFP::MAP& map, EMBV& attributs);

/**
 * Loop subdivision scheme
 */
template <typename PFP, typename EMBV>
void LoopSubdivision(typename PFP::MAP& map, EMBV& attributs) ;

template <typename PFP>
void LoopSubdivisionGen(typename PFP::MAP& map, VertexAttributeGen& attrib)
{
	auto va3 = dynamic_cast<VertexAttribute<typename PFP::VEC3, typename PFP::MAP>*>(&attrib);
	if (va3 != NULL)
		return LoopSubdivision<PFP>(map,*va3);

	auto va4 = dynamic_cast<VertexAttribute<typename PFP::VEC4, typename PFP::MAP>*>(&attrib);
	if (va4 != NULL)
		return LoopSubdivision<PFP>(map,*va4);

	auto var = dynamic_cast<VertexAttribute<typename PFP::REAL, typename PFP::MAP>*>(&attrib);
	if (var != NULL)
		return LoopSubdivision<PFP>(map,*var);

	CGoGNerr << "LoopSubdivision not supported on attribute of type "<< attrib.typeName() << CGoGNendl;
}

/**
 * Loop typed version with attribute name parameter
 */
template <typename PFP, typename T>
inline void LoopSubdivisionAttribNameTyped(typename PFP::MAP& map, const std::string& nameAttrib)
{
	VertexAttribute<T,typename PFP::MAP> va = map.template getAttribute<T,VERTEX,typename PFP::MAP>(nameAttrib) ;
}

/**
 * Loop genereo version with attribute name parameter
 */
template <typename PFP>
void LoopSubdivisionAttribName(typename PFP::MAP& map, const std::string& nameAttrib)
{
	typedef typename PFP::MAP MAP;

	switch(map.template getAttributeTypeCode<VERTEX>(nameAttrib))
	{
	case CGoGNFLOAT:
		return LoopSubdivisionAttribNameTyped<PFP,float>(map,nameAttrib);
		break;
	case CGoGNDOUBLE:
		return LoopSubdivisionAttribNameTyped<PFP,double>(map,nameAttrib);
		break;
	case CGoGNVEC3F:
		return LoopSubdivisionAttribNameTyped<PFP,Geom::Vec3f>(map,nameAttrib);
		break;
	case CGoGNVEC3D:
		return LoopSubdivisionAttribNameTyped<PFP,Geom::Vec3d>(map,nameAttrib);
		break;
	case CGoGNVEC4F:
		return LoopSubdivisionAttribNameTyped<PFP,Geom::Vec4f>(map,nameAttrib);
		break;
	case CGoGNVEC4D:
		return LoopSubdivisionAttribNameTyped<PFP,Geom::Vec4d>(map,nameAttrib);
		break;
	default:
		break;
	}
	CGoGNerr << "LoopSubdivision not supported on attribute "<< nameAttrib << CGoGNendl;
}



/**
 * Cut 1 face of degree n in 2n+1 faces (cut 2 times each edge and "insert" a face in the middle)
 * @param size indicates where to cut edges, its value must stay between 0 and 0.5 excluded
 */
template <typename PFP, typename EMBV>
void TwoNPlusOneSubdivision(typename PFP::MAP& map, EMBV& attributs, float size=1.0f/3.0f) ;

/**
 * Doo-Sabin subdivision scheme
 */
template <typename PFP, typename EMBV>
void DooSabin(typename PFP::MAP& map, EMBV& position);

///**
// * Reverse the orientation of the map
// * NOW  IN THE MAP
// */
//template <typename PFP>
//void Sqrt3Subdivision(typename PFP::MAP& map, typename PFP::TVEC3& position) ;


///**
// * Sqrt(3) subdivision scheme
// */
//template <typename PFP>
//void Sqrt3Subdivision(typename PFP::MAP& map, typename PFP::TVEC3& position) ;

template <typename PFP>
void computeDual(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position);

template <typename PFP>
void computeBoundaryConstraintDual(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position);

template <typename PFP>
void computeBoundaryConstraintKeepingOldVerticesDual(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position);

} // namespace Modelisation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Modelisation/subdivision.hpp"

#endif
