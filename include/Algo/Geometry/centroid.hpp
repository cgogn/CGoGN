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

#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2.h"
#include "Topology/generic/cellmarker.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor3.h"
#include "Algo/Parallel/parallel_foreach.h"


namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Geometry
{

template <typename PFP, typename EMBV, typename EMB>
EMB volumeCentroidGen(typename PFP::MAP& map, Dart d, const EMBV& attributs, unsigned int thread)
{
	EMB center = AttribOps::zero<EMB,PFP>() ;
	unsigned int count = 0 ;

	Traversor3WV<typename PFP::MAP> tra(map,d,false,thread);
	for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
	{
		center += attributs[d];
		++count;
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
void computeCentroidVolumes(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, VolumeAttribute<typename PFP::VEC3>& vol_centroid, const FunctorSelect& select, unsigned int thread)
{
	TraversorW<typename PFP::MAP> t(map, select,thread) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
		vol_centroid[d] = volumeCentroid<PFP>(map, d, position,thread) ;
}

template <typename PFP>
void computeCentroidFaces(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& face_centroid, const FunctorSelect& select, unsigned int thread)
{
	TraversorF<typename PFP::MAP> t(map, select,thread) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
		face_centroid[d] = faceCentroid<PFP>(map, d, position) ;
}

template <typename PFP>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& vertex_centroid, const FunctorSelect& select, unsigned int thread)
{
	TraversorV<typename PFP::MAP> t(map, select, thread) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
		vertex_centroid[d] = vertexNeighborhoodCentroid<PFP>(map, d, position) ;
}



namespace Parallel
{
template <typename PFP>
class FunctorComputeCentroidVolumes: public FunctorMapThreaded<typename PFP::MAP >
{
	 const VertexAttribute<typename PFP::VEC3>& m_position;
	 VolumeAttribute<typename PFP::VEC3>& m_vol_centroid;
public:
	 FunctorComputeCentroidVolumes<PFP>( typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, VolumeAttribute<typename PFP::VEC3>& vol_centroid):
	 	 FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_vol_centroid(vol_centroid)
	 { }

	void run(Dart d, unsigned int threadID)
	{
		m_vol_centroid[d] = volumeCentroid<PFP>(this->m_map, d, m_position,threadID) ;
	}
};


template <typename PFP>
void computeCentroidVolumes(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, VolumeAttribute<typename PFP::VEC3>& vol_centroid,
		const FunctorSelect& select, unsigned int nbth)
{
	FunctorComputeCentroidVolumes<PFP> funct(map,position,vol_centroid);
	Algo::Parallel::foreach_cell<typename PFP::MAP,VOLUME>(map, funct, nbth, true, select);
}


template <typename PFP>
class FunctorComputeCentroidFaces: public FunctorMapThreaded<typename PFP::MAP >
{
	 const VertexAttribute<typename PFP::VEC3>& m_position;
	 FaceAttribute<typename PFP::VEC3>& m_fcentroid;
public:
	 FunctorComputeCentroidFaces<PFP>( typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& fcentroid):
	 	 FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_fcentroid(fcentroid)
	 { }

	void run(Dart d, unsigned int threadID)
	{
		m_fcentroid[d] = faceCentroid<PFP>(this->m_map, d, m_position) ;
	}
};


template <typename PFP>
void computeCentroidFaces(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, FaceAttribute<typename PFP::VEC3>& face_centroid,
		const FunctorSelect& select, unsigned int nbth, unsigned int current_thread)
{
	FunctorComputeCentroidFaces<PFP> funct(map,position,face_centroid);
	Algo::Parallel::foreach_cell<typename PFP::MAP,FACE>(map, funct, nbth, false, select, current_thread);
}


template <typename PFP>
class FunctorComputeNeighborhoodCentroidVertices: public FunctorMapThreaded<typename PFP::MAP >
{
	 const VertexAttribute<typename PFP::VEC3>& m_position;
	 VertexAttribute<typename PFP::VEC3>& m_vcentroid;
public:
	 FunctorComputeNeighborhoodCentroidVertices<PFP>( typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& vcentroid):
	 	 FunctorMapThreaded<typename PFP::MAP>(map), m_position(position), m_vcentroid(vcentroid)
	 { }

	void run(Dart d, unsigned int threadID)
	{
		m_vcentroid[d] = vertexNeighborhoodCentroid<PFP>(this->m_map, d, m_position) ;
	}
};

template <typename PFP>
void computeNeighborhoodCentroidVertices(typename PFP::MAP& map,
		const VertexAttribute<typename PFP::VEC3>& position, VertexAttribute<typename PFP::VEC3>& vertex_centroid,
		const FunctorSelect& select, unsigned int nbth, unsigned int current_thread)
{
	FunctorComputeNeighborhoodCentroidVertices<PFP> funct(map,position,vertex_centroid);
	Algo::Parallel::foreach_cell<typename PFP::MAP,VERTEX>(map, funct, nbth, false, select, current_thread);
}

}













} // namespace Geometry

}

} // namespace Algo

} // namespace CGoGN
