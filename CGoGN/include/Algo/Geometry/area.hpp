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

#include "Geometry/basic.h"
#include "Algo/Geometry/centroid.h"

#include "Topology/generic/autoAttributeHandler.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Geometry
{

template <typename PFP>
typename PFP::REAL triangleArea(typename PFP::MAP& map, Face d, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	typename PFP::VEC3 p1 = position[d.dart] ;
	typename PFP::VEC3 p2 = position[map.phi1(d)] ;
	typename PFP::VEC3 p3 = position[map.phi_1(d)] ;

	return Geom::triangleArea(p1, p2, p3) ;
}

template <typename PFP>
typename PFP::REAL convexFaceArea(typename PFP::MAP& map, Face d, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	typedef typename PFP::VEC3 VEC3 ;

	if(map.faceDegree(d) == 3)
		return triangleArea<PFP>(map, d, position) ;
	else
	{
		typename PFP::REAL area = 0;
		VEC3 centroid = faceCentroid<PFP>(map, d, position) ;
		Traversor2FE<typename PFP::MAP> t(map, d) ;
		for(Dart it = t.begin(); it != t.end(); it = t.next())
		{
			VEC3 p1 = position[it] ;
			VEC3 p2 = position[map.phi1(it)] ;
			area += Geom::triangleArea(p1, p2, centroid) ;
		}
		return area ;
	}
}

template <typename PFP>
typename PFP::REAL totalArea(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	if (CGoGN::Parallel::NumberOfThreads > 1)
	{
		return Parallel::totalArea<PFP>(map, position);
	}

	typename PFP::REAL area(0);

	foreach_cell<FACE>(map, [&] (Face f)
	{
		area += convexFaceArea<PFP>(map, f, position);
	}
	,AUTO);
	return area;
}

template <typename PFP>
typename PFP::REAL vertexOneRingArea(typename PFP::MAP& map, Vertex v, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	typename PFP::REAL area(0) ;

	foreach_incident2<FACE>(map, v, [&] (Face f)
	{
		area += convexFaceArea<PFP>(map,f, position) ;
	});
	return area ;
}

template <typename PFP>
typename PFP::REAL vertexBarycentricArea(typename PFP::MAP& map, Vertex v, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	typename PFP::REAL area(0) ;

	foreach_incident2<FACE>(map, v, [&] (Face f)
	{
		area += convexFaceArea<PFP>(map, f, position) / 3 ;
	});
	return area ;
}

template <typename PFP>
typename PFP::REAL vertexVoronoiArea(typename PFP::MAP& map, Vertex v, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	typename PFP::REAL area(0) ;
	foreach_incident2<FACE>(map, v, [&] (Face it)
	{
		const typename PFP::VEC3& p1 = position[it.dart] ;
		const typename PFP::VEC3& p2 = position[map.phi1(it)] ;
		const typename PFP::VEC3& p3 = position[map.phi_1(it)] ;
		if(!Geom::isTriangleObtuse(p1, p2, p3))
		{
			typename PFP::REAL a = Geom::angle(p3 - p2, p1 - p2) ;
			typename PFP::REAL b = Geom::angle(p1 - p3, p2 - p3) ;
			area += ( (p2 - p1).norm2() / tan(b) + (p3 - p1).norm2() / tan(a) ) / 8 ;
		}
		else
		{
			typename PFP::REAL tArea = Geom::triangleArea(p1, p2, p3) ;
			if(Geom::angle(p2 - p1, p3 - p1) > M_PI / 2)
				area += tArea / 2 ;
			else
				area += tArea / 4 ;
		}
	});
	return area ;
}

template <typename PFP>
typename PFP::REAL edgeArea(typename PFP::MAP& map, Edge e, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	typename PFP::REAL area(0) ;

	foreach_incident2<FACE>(map, e, [&] (Face f)
	{
		area += convexFaceArea<PFP>(map, f, position) / map.faceDegree(f) ;
	});
	return area ;
}

template <typename PFP>
void computeAreaFaces(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, FaceAttribute<typename PFP::REAL, typename PFP::MAP>& face_area)
{
	if (CGoGN::Parallel::NumberOfThreads > 1)
	{
		Parallel::computeAreaFaces<PFP>(map, position, face_area);
		return;
	}

	foreach_cell<FACE>(map, [&] (Face f)
	{
		face_area[f] = convexFaceArea<PFP>(map, f, position) ;
	}
	,AUTO);
}

template <typename PFP>
void computeAreaEdges(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, EdgeAttribute<typename PFP::REAL, typename PFP::MAP>& edge_area)
{
	if (CGoGN::Parallel::NumberOfThreads > 1)
	{
		Parallel::computeAreaEdges<PFP>(map, position, edge_area);
		return;
	}

	foreach_cell<EDGE>(map, [&] (Edge e)
	{
		edge_area[e] = edgeArea<PFP>(map, e, position) ;
	}
	,AUTO);
}

template <typename PFP>
void computeOneRingAreaVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, VertexAttribute<typename PFP::REAL, typename PFP::MAP>& vertex_area)
{
	if (CGoGN::Parallel::NumberOfThreads > 1)
	{
		Parallel::computeOneRingAreaVertices<PFP>(map,position,vertex_area);
		return;
	}

	FaceAutoAttribute<typename PFP::REAL,typename PFP::MAP> areas(map);
	computeAreaFaces<PFP>(map,position,areas);

	foreach_cell<VERTEX>(map, [&] (Vertex v)
	{
		vertex_area[v] = typename PFP::REAL(0);
		foreach_incident2<FACE>(map, v, [&] (Face f)
		{
			vertex_area[v] += areas[f];
		});
	}
	,FORCE_CELL_MARKING);
}


template <typename PFP>
void computeBarycentricAreaVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, VertexAttribute<typename PFP::REAL, typename PFP::MAP>& vertex_area)
{
	if (CGoGN::Parallel::NumberOfThreads > 1)
	{
		Parallel::computeBarycentricAreaVertices<PFP>(map,position,vertex_area);
		return;
	}

	foreach_cell<VERTEX>(map, [&] (Vertex v)
	{
		vertex_area[v] = vertexBarycentricArea<PFP>(map, v, position) ;
	}
	,FORCE_CELL_MARKING);
}

template <typename PFP>
void computeVoronoiAreaVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, VertexAttribute<typename PFP::REAL, typename PFP::MAP>& vertex_area)
{
	if (CGoGN::Parallel::NumberOfThreads > 1)
	{
		Parallel::computeVoronoiAreaVertices<PFP>(map,position,vertex_area);
		return;
	}

	foreach_cell<VERTEX>(map, [&] (Vertex v)
	{
		vertex_area[v] = vertexVoronoiArea<PFP>(map, v, position) ;
	}
	,FORCE_CELL_MARKING);
}



namespace Parallel
{

template <typename PFP>
typename PFP::REAL totalArea(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position)
{
	std::vector<typename PFP::REAL> areas;
	areas.resize(CGoGN::Parallel::NumberOfThreads);

	CGoGN::Parallel::foreach_cell<FACE>(map, [&] (Face f, unsigned int thr)
	{
		areas[thr-1] += convexFaceArea<PFP>(map, f, position);
	}
	,AUTO);

	typename PFP::REAL area(0);
	for (typename PFP::REAL a : areas)
		area += a;
	return area;
}

template <typename PFP>
void computeAreaFaces(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, FaceAttribute<typename PFP::REAL, typename PFP::MAP>& area)
{
//	if (map.isOrbitEmbedded<FACE>())
//	{
//		Parallel::foreach_cell<FACE>(map,[&](Face f, unsigned int thr)
//		{
//			area[f] = convexFaceArea<PFP>(map, f, position) ;
//		},nbth,false,FORCE_CELL_MARKING);
//	}
//	else
//	{
//		Parallel::foreach_cell<FACE>(map,[&](Face f, unsigned int thr)
//		{
//			area[f] = convexFaceArea<PFP>(map, f, position) ;
//		},nbth,false,AUTO);
//	}

	CGoGN::Parallel::foreach_cell<FACE>(map, [&] (Face f, unsigned int /*thr*/)
	{
		area[f] = convexFaceArea<PFP>(map, f, position) ;
	});
}

template <typename PFP>
void computeAreaEdges(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, EdgeAttribute<typename PFP::REAL, typename PFP::MAP>& area)
{
	CGoGN::Parallel::foreach_cell<EDGE>(map, [&] (Edge e, unsigned int /*thr*/)
	{
		area[e] = edgeArea<PFP>(map, e, position) ;
	});
}

template <typename PFP>
void computeOneRingAreaVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, VertexAttribute<typename PFP::REAL, typename PFP::MAP>& area)
{
	CGoGN::Parallel::foreach_cell<VERTEX>(map, [&] (Vertex v, unsigned int /*thr*/)
	{
		area[v] = vertexOneRingArea<PFP>(map, v, position) ;
	}, FORCE_CELL_MARKING);
}

template <typename PFP>
void computeBarycentricAreaVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, VertexAttribute<typename PFP::REAL, typename PFP::MAP>& vertex_area)
{
	CGoGN::Parallel::foreach_cell<VERTEX>(map, [&] (Vertex v, unsigned int /*thr*/)
	{
		vertex_area[v] = vertexBarycentricArea<PFP>(map, v, position) ;
	}, FORCE_CELL_MARKING);
}

template <typename PFP>
void computeVoronoiAreaVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, VertexAttribute<typename PFP::REAL, typename PFP::MAP>& area)
{
	CGoGN::Parallel::foreach_cell<VERTEX>(map, [&] (Vertex v, unsigned int /*thr*/)
	{
		area[v] = vertexVoronoiArea<PFP>(map, v, position) ;
	}, FORCE_CELL_MARKING);
}

} // namespace Parallel



} // namespace Geometry

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
