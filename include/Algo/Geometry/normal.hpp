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

#include "Topology/generic/traversor/traversorCell.h"
#include "Topology/generic/traversor/traversor2.h"

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

template<typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE triangleNormal(typename PFP::MAP& map, Face f, const V_ATT& position)
{
	typename V_ATT::DATA_TYPE N = Geom::triangleNormal(
		position[f.dart],
		position[map.phi1(f)],
		position[map.phi_1(f)]
	) ;
	N.normalize() ;
	return N ;
}

template<typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE newellNormal(typename PFP::MAP& map, Face f, const V_ATT& position)
{
	typedef typename V_ATT::DATA_TYPE VEC3;
	VEC3 N(0);

	foreach_incident2<VERTEX>(map, f, [&] (Vertex v)
	{
		const VEC3& P = position[v];
		const VEC3& Q = position[map.phi1(v)];
		N[0] += (P[1] - Q[1]) * (P[2] + Q[2]);
		N[1] += (P[2] - Q[2]) * (P[0] + Q[0]);
		N[2] += (P[0] - Q[0]) * (P[1] + Q[1]);
	});

	N.normalize();
	return N;
}

template<typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE faceNormal(typename PFP::MAP& map, Face f, const V_ATT& position)
{
	if(map.faceDegree(f) == 3)
		return triangleNormal<PFP>(map, f, position) ;
	else
		return newellNormal<PFP>(map, f, position) ;
}

template<typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE vertexNormal(typename PFP::MAP& map, Vertex v, const V_ATT& position)
{
	typedef typename V_ATT::DATA_TYPE VEC3 ;

	VEC3 N(0) ;

	foreach_incident2<FACE>(map, v, [&] (Face f)
	{
		VEC3 n = faceNormal<PFP>(map, f, position) ;
		if(!n.hasNan())
		{
			VEC3 v1 = vectorOutOfDart<PFP>(map, f.dart, position) ;
			VEC3 v2 = vectorOutOfDart<PFP>(map, map.phi_1(f), position) ;
			n *= convexFaceArea<PFP>(map, f, position) / (v1.norm2() * v2.norm2()) ;
			N += n ;
		}
	});

	N.normalize() ;
	return N ;
}

template<typename PFP, typename V_ATT>
typename V_ATT::DATA_TYPE vertexBorderNormal(typename PFP::MAP& map, Vertex v, const V_ATT& position)
{
	assert(map.dimension() == 3);

	typedef typename V_ATT::DATA_TYPE VEC3 ;

	VEC3 N(0) ;

	std::vector<Dart> faces;
	faces.reserve(16);
	map.foreach_dart_of_vertex(v, [&] (Dart d) { faces.push_back(d); });

	CellMarker<typename PFP::MAP, FACE> f(map);

	for(std::vector<Dart>::iterator it = faces.begin() ; it != faces.end() ; ++it)
	{
		if(!f.isMarked(*it) && map.isBoundaryIncidentFace(*it))
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

template <typename PFP, typename V_ATT, typename F_ATT>
void computeNormalFaces(typename PFP::MAP& map, const V_ATT& position, F_ATT& face_normal, unsigned int thread)
{
	foreach_cell<FACE>(map, [&] (Face f)
	{
		face_normal[f] = faceNormal<PFP>(map, f, position) ;
	},
	false, thread);
}

template <typename PFP, typename V_ATT>
void computeNormalVertices(typename PFP::MAP& map, const V_ATT& position, V_ATT& normal, unsigned int thread)
{
	foreach_cell<VERTEX>(map, [&] (Vertex v)
	{
		normal[v] = vertexNormal<PFP>(map, v, position) ;
	},
	FORCE_CELL_MARKING, thread);
}



namespace Parallel
{

template <typename PFP, typename V_ATT>
class FunctorComputeNormalVertices : public FunctorMapThreaded<typename PFP::MAP>
{
	const V_ATT& m_position;
	V_ATT& m_normal;
public:
	FunctorComputeNormalVertices<PFP,V_ATT>(typename PFP::MAP& map, const V_ATT& position, V_ATT& normal):
		FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_normal(normal)
	{ }

	void run(Dart d, unsigned int /*threadID*/)
	{
		m_normal[d] = vertexNormal<PFP>(this->m_map, d, m_position) ;
	}
};

template <typename PFP, typename V_ATT>
void computeNormalVertices(typename PFP::MAP& map, const V_ATT& position, V_ATT& normal, unsigned int nbth)
{
	FunctorComputeNormalVertices<PFP,V_ATT> funct(map, position, normal);
	Algo::Parallel::foreach_cell<typename PFP::MAP, VERTEX>(map, funct, nbth, false);
}

template <typename PFP, typename V_ATT, typename F_ATT>
class FunctorComputeNormalFaces : public FunctorMapThreaded<typename PFP::MAP>
{
	const V_ATT& m_position;
	F_ATT& m_normal;
public:
	FunctorComputeNormalFaces<PFP,V_ATT,F_ATT>(typename PFP::MAP& map, const V_ATT& position, F_ATT& normal):
		FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_normal(normal)
	{ }

	void run(Dart d, unsigned int /*threadID*/)
	{
		m_normal[d] = faceNormal<PFP>(this->m_map, d, m_position) ;
	}
};

template <typename PFP, typename V_ATT, typename F_ATT>
void computeNormalFaces(typename PFP::MAP& map, const V_ATT& position, F_ATT& normal, unsigned int nbth)
{
	FunctorComputeNormalFaces<PFP,V_ATT,F_ATT> funct(map, position, normal);
	Algo::Parallel::foreach_cell<typename PFP::MAP, FACE>(map, funct, nbth, false);
}

template <typename PFP, typename V_ATT, typename E_ATT>
class FunctorComputeAngleBetweenNormalsOnEdge: public FunctorMapThreaded<typename PFP::MAP>
{
	const V_ATT& m_position;
	E_ATT& m_angles;

public:
	FunctorComputeAngleBetweenNormalsOnEdge<PFP,V_ATT,E_ATT>( typename PFP::MAP& map, const V_ATT& position, E_ATT& angles):
		FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_angles(angles)
	{ }

	void run(Dart d, unsigned int threadID)
	{
		m_angles[d] = computeAngleBetweenNormalsOnEdge<PFP>(this->m_map, d, m_position) ;
	}
};

template <typename PFP, typename V_ATT, typename E_ATT>
void computeAnglesBetweenNormalsOnEdges(typename PFP::MAP& map, const V_ATT& position, E_ATT& angles, unsigned int nbth)
{
	FunctorComputeAngleBetweenNormalsOnEdge<PFP,V_ATT,E_ATT> funct(map,position,angles);
	Algo::Parallel::foreach_cell<typename PFP::MAP,EDGE>(map, funct, nbth, false);
}

} // namespace Parallel



template <typename PFP, typename V_ATT>
typename PFP::REAL computeAngleBetweenNormalsOnEdge(typename PFP::MAP& map, Edge e, const V_ATT& position)
{
	typedef typename V_ATT::DATA_TYPE VEC3 ;

	if(map.isBoundaryEdge(e))
		return 0 ;

	Vertex v1(e.dart);
	Vertex v2 = map.phi2(e) ;
	const VEC3 n1 = faceNormal<PFP>(map, Face(v1), position) ;
	const VEC3 n2 = faceNormal<PFP>(map, Face(v2), position) ;
	VEC3 edge = position[v2] - position[v1] ;
	edge.normalize() ;
	typename PFP::REAL s = edge * (n1 ^ n2) ;
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
		std::cerr<< "Warning : computeAngleBetweenNormalsOnEdge returns NaN on edge " << v1 << "-" << v2 << std::endl ;

	return a ;
}

template <typename PFP, typename V_ATT, typename E_ATT>
void computeAnglesBetweenNormalsOnEdges(typename PFP::MAP& map, const V_ATT& position, E_ATT& angles, unsigned int thread)
{
	foreach_cell<EDGE>(map, [&] (Edge e)
	{
		angles[e] = computeAngleBetweenNormalsOnEdge<PFP>(map, e, position) ;
	},
	false, thread);
}

} // namespace Geometry

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
