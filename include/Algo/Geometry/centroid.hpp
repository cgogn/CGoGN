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

#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP, typename EMBV, typename EMB>
EMB volumeCentroidGen(typename PFP::MAP& map, Dart d, const EMBV& attributs)
{
	EMB center = AttribOps::zero<EMB,PFP>() ;
	unsigned count = 0 ;

//	CellMarkerStore marker(map, VERTEX) ;
//
//	std::vector<Dart> visitedVertices ;
//	visitedVertices.reserve(100) ;
//	visitedVertices.push_back(d) ;
//	marker.mark(d) ;
//
//	for(typename std::vector<Dart>::iterator vert = visitedVertices.begin(); vert != visitedVertices.end(); ++vert)
//	{
//		Dart e = *vert;
//		center += attributs[e];	// add to center and mark
//		++count;
//		do	// add all vertex neighbors to the table
//		{
//			Dart ee = map.phi2(e) ;
//			if(!marker.isMarked(ee))
//			{
//				visitedVertices.push_back(ee) ;
//				marker.mark(e) ;
//			}
//			e = map.phi1(ee) ;
//		} while(e != *vert) ;
//	}
	DartMarkerStore mark(map);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(100);
	visitedFaces.push_back(d);			// Start with the face of d

	mark.markOrbit(VERTEX, d) ;

	for(std::vector<Dart>::iterator face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		Dart e = *face ;

		center += attributs[e];	// add to center and mark
		++count;

		do	// add all face neighbours to the table
		{
			Dart ee = map.phi2(e) ;
			if(!mark.isMarked(ee)) // not already marked
			{
				visitedFaces.push_back(ee) ;
				mark.markOrbit(VERTEX, ee) ;
			}
			e = map.phi1(e) ;
		} while(e != *face) ;
	}

	center /= double(count) ;
	return center ;
}

template <typename PFP, typename EMBV, typename EMB>
EMB faceCentroidGen(typename PFP::MAP& map, Dart d, const EMBV& attributs)
{
	EMB center = AttribOps::zero<EMB,PFP>();
	unsigned int count = 0 ;
	Traversor2FV<typename PFP::MAP> t(map, d) ;
	for(Dart it = t.begin(); it != t.end(); it = t.next())
	{
		center += attributs[it];
		++count ;
	}
	center /= double(count);
	return center ;
}

template <typename PFP, typename EMBV, typename EMB>
EMB vertexNeighborhoodCentroidGen(typename PFP::MAP& map, Dart d, const EMBV& attributs)
{
	EMB center = AttribOps::zero<EMB,PFP>();
	unsigned int count = 0 ;
	Traversor2VVaE<typename PFP::MAP> t(map, d) ;
	for(Dart it = t.begin(); it != t.end(); it = t.next())
	{
		center += attributs[it];
		++count ;
	}
	center /= count ;
	return center ;
}

template <typename PFP>
void computeCentroidVolumes(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& vol_centroid, const FunctorSelect& select)
{
	TraversorW<typename PFP::MAP> t(map, select) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
		vol_centroid[d] = volumeCentroid<PFP>(map, d, position) ;
}

template <typename PFP>
void computeCentroidFaces(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& face_centroid, const FunctorSelect& select)
{
	TraversorF<typename PFP::MAP> t(map, select) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
		face_centroid[d] = faceCentroid<PFP>(map, d, position) ;
}

template <typename PFP>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& vertex_centroid, const FunctorSelect& select)
{
	TraversorV<typename PFP::MAP> t(map, select) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
		vertex_centroid[d] = vertexNeighborhoodCentroid<PFP>(map, d, position) ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
