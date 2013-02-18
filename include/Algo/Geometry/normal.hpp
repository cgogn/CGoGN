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

#include "Algo/Geometry/basic.h"
#include "Algo/Geometry/area.h"

#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2.h"

#include "Algo/Parallel/parallel_foreach.h"

#include <cmath>

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Geometry
{

template <typename PFP>
typename PFP::VEC3 triangleNormal(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	typename PFP::VEC3 N = Geom::triangleNormal(position[d], position[map.phi1(d)], position[map.phi_1(d)]) ;
	N.normalize() ;
	return N ;
}

template<typename PFP>
typename PFP::VEC3 newellNormal(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	typename PFP::VEC3 N(0);

	Traversor2FV<typename PFP::MAP> t(map, d) ;
	for(Dart it = t.begin(); it != t.end(); it = t.next())
	{
		const typename PFP::VEC3& P = position[it];
		const typename PFP::VEC3& Q = position[map.phi1(it)];
		N[0] += (P[1] - Q[1]) * (P[2] + Q[2]);
		N[1] += (P[2] - Q[2]) * (P[0] + Q[0]);
		N[2] += (P[0] - Q[0]) * (P[1] + Q[1]);
	}

	N.normalize();
	return N;
}

template <typename PFP>
typename PFP::VEC3 faceNormal(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	if(map.faceDegree(d) == 3)
		return triangleNormal<PFP>(map, d, position) ;
	else
		return newellNormal<PFP>(map, d, position) ;
//	{
//		VEC3 N(0) ;
//		Dart it = d ;
//		do
//		{
//			VEC3 n = triangleNormal<PFP>(map, it, position) ;
//			//if(!std::isnan(n[0]) && !std::isnan(n[1]) && !std::isnan(n[2]))
//			if(!n.hasNan())
//				N += n ;
//			it = map.phi1(it) ;
//		} while (it != d) ;
//		N.normalize() ;
//		return N ;
//	}
}

template <typename PFP>
typename PFP::VEC3 vertexNormal(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	typedef typename PFP::VEC3 VEC3 ;

	VEC3 N(0) ;

	Traversor2VF<typename PFP::MAP> t(map, d) ;
	for(Dart it = t.begin(); it != t.end(); it = t.next())
	{
		VEC3 n = faceNormal<PFP>(map, it, position) ;
		if(!n.hasNan())
		{
			VEC3 v1 = vectorOutOfDart<PFP>(map, it, position) ;
			VEC3 v2 = vectorOutOfDart<PFP>(map, map.phi_1(it), position) ;
			n *= convexFaceArea<PFP>(map, it, position) / (v1.norm2() * v2.norm2()) ;
			N += n ;
		}
	}

	N.normalize() ;
	return N ;
}

template <typename PFP>
typename PFP::VEC3 vertexBorderNormal(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	assert(map.dimension() == 3);

	typedef typename PFP::VEC3 VEC3 ;

	VEC3 N(0) ;
	std::vector<Dart> faces;
	CellMarker<FACE> f(map);

	FunctorStore fs(faces);
	map.foreach_dart_of_vertex(d,fs);

	for(std::vector<Dart>::iterator it = faces.begin() ; it != faces.end() ; ++it)
	{
		if(!f.isMarked(*it) && map.isBoundaryFace(*it))
		{
			f.mark(*it);
			VEC3 n = faceNormal<PFP>(map, *it, position);
			if(!n.hasNan())
			{
				VEC3 v1 = vectorOutOfDart<PFP>(map, *it, position);
				VEC3 v2 = vectorOutOfDart<PFP>(map, map.phi_1(*it), position);
				n *= convexFaceArea<PFP>(map, *it, position) / (v1.norm2() * v2.norm2());
				N += n ;
			}
		}
	}

	N.normalize() ;
	return N ;
}

template <typename PFP>
void computeNormalFaces(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& face_normal, unsigned int thread)
{
	TraversorF<typename PFP::MAP> trav(map, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		face_normal[d] = faceNormal<PFP>(map, d, position) ;
}

template <typename PFP>
void computeNormalVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& normal, unsigned int thread)
{
	TraversorV<typename PFP::MAP> trav(map, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		normal[d] = vertexNormal<PFP>(map, d, position) ;
}



namespace Parallel
{

template <typename PFP>
class FunctorComputeNormalVertices: public FunctorMapThreaded<typename PFP::MAP >
{
	 const VertexAttribute<typename PFP::VEC3>& m_position;
	 VertexAttribute<typename PFP::VEC3>& m_normal;
public:
	 FunctorComputeNormalVertices<PFP>(	typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& normal):
	 	 FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_normal(normal)
	 { }

	void run(Dart d, unsigned int threadID)
	{
		m_normal[d] = vertexNormal<PFP>(this->m_map, d, m_position) ;
	}
};

template <typename PFP>
void computeNormalVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& normal, unsigned int nbth)
{
	FunctorComputeNormalVertices<PFP> funct(map,position,normal);
	Algo::Parallel::foreach_cell<typename PFP::MAP,VERTEX>(map, funct, nbth, false);
}


template <typename PFP>
class FunctorComputeNormalFaces: public FunctorMapThreaded<typename PFP::MAP >
{
	 const VertexAttribute<typename PFP::VEC3>& m_position;
	 FaceAttribute<typename PFP::VEC3>& m_normal;
public:
	 FunctorComputeNormalFaces<PFP>( typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& normal):
	 	 FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_normal(normal)
	 { }

	void run(Dart d, unsigned int threadID)
	{
		m_normal[d] = faceNormal<PFP>(this->m_map, d, m_position) ;
	}
};

template <typename PFP>
void computeNormalFaces(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& normal, unsigned int nbth)
{
	FunctorComputeNormalFaces<PFP> funct(map,position,normal);
	Algo::Parallel::foreach_cell<typename PFP::MAP,FACE>(map, funct, nbth, false);
}


template <typename PFP>
class FunctorComputeAngleBetweenNormalsOnEdge: public FunctorMapThreaded<typename PFP::MAP >
{
	 const VertexAttribute<typename PFP::VEC3>& m_position;
	 FaceAttribute<typename PFP::VEC3>& m_angles;
public:
	 FunctorComputeAngleBetweenNormalsOnEdge<PFP>( typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, EdgeAttribute<typename PFP::VEC3>& angles):
	 	 FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_angles(angles)
	 { }

	void run(Dart d, unsigned int threadID)
	{
		m_angles[d] = computeAngleBetweenNormalsOnEdge<PFP>(this->m_map, d, m_position) ;
	}
};


template <typename PFP>
void computeAnglesBetweenNormalsOnEdges(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, EdgeAttribute<typename PFP::REAL>& angles, unsigned int nbth)
{
	FunctorComputeAngleBetweenNormalsOnEdge<PFP> funct(map,position,angles);
	Algo::Parallel::foreach_cell<typename PFP::MAP,EDGE>(map, funct, nbth, false);
}

} // endnamespace Parallel



template <typename PFP>
typename PFP::REAL computeAngleBetweenNormalsOnEdge(typename PFP::MAP& map, Dart d, const VertexAttribute<typename PFP::VEC3>& position)
{
	typedef typename PFP::VEC3 VEC3 ;

	if(map.isBoundaryEdge(d))
		return 0 ;

	Dart dd = map.phi2(d) ;
	const VEC3 n1 = faceNormal<PFP>(map, d, position) ;
	const VEC3 n2 = faceNormal<PFP>(map, dd, position) ;
	VEC3 e = position[dd] - position[d] ;
	e.normalize() ;
	typename PFP::REAL s = e * (n1 ^ n2) ;
	typename PFP::REAL c = n1 * n2 ;
	typename PFP::REAL a(0) ;

	// the following trick is useful for avoiding NaNs (due to floating point errors)
	if (c > 0.5) a = asin(s) ;
	else
	{
		if(c < -1) c = -1 ;
		if (s >= 0) a = acos(c) ;
		else a = -acos(c) ;
	}
//	if (isnan(a))
	if(a != a)
		std::cerr<< "Warning : computeAngleBetweenNormalsOnEdge returns NaN on edge " << d << "-" << dd << std::endl ;
	return a ;
}

template <typename PFP>
void computeAnglesBetweenNormalsOnEdges(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, EdgeAttribute<typename PFP::REAL>& angles, unsigned int thread)
{
	TraversorE<typename PFP::MAP> trav(map, thread);
	for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		angles[d] = computeAngleBetweenNormalsOnEdge<PFP>(map, d, position) ;
}

} // namespace Geometry

}

} // namespace Algo

} // namespace CGoGN
