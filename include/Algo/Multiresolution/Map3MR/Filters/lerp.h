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

#ifndef __3MR_LERP_FILTER__
#define __3MR_LERP_FILTER__

#include <cmath>
#include "Algo/Geometry/centroid.h"
#include "Algo/Modelisation/tetrahedralization.h"
#include "Algo/Multiresolution/filter.h"

namespace CGoGN
{

namespace Algo
{

namespace MR
{

namespace Primal
{

namespace Filters
{

/*********************************************************************************
 *                           SYNTHESIS FILTERS
 *********************************************************************************/

/* Linear Interpolation
 *********************************************************************************/
template <typename PFP>
class LerpEdgeSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpEdgeSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = (m_position[d] + m_position[m_map.phi2(d)]) * typename PFP::REAL(0.5);

			m_map.incCurrentLevel() ;

			Dart midV = m_map.phi2(d) ;
			m_position[midV] = p ;

			m_map.decCurrentLevel() ;
		}
	}
} ;

template <typename PFP>
class LerpFaceSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpFaceSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorF<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = Algo::Geometry::faceCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			Dart midF = m_map.phi2(m_map.phi1(d));
			m_position[midF] = p ;

			m_map.decCurrentLevel() ;

		}
	}
} ;

template <typename PFP>
class LerpTriQuadFaceSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpTriQuadFaceSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorF<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.faceDegree(d) > 3)
			{
				typename PFP::VEC3 p = Algo::Geometry::faceCentroid<PFP>(m_map, d, m_position);

				m_map.incCurrentLevel() ;

				Dart midF = m_map.phi2(m_map.phi1(d));
				m_position[midF] = p ;

				m_map.decCurrentLevel() ;
			}
		}
	}
} ;


template <typename PFP>
class LerpVolumeSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpVolumeSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!Algo::Modelisation::Tetrahedralization::isTetrahedron<PFP>(m_map,d)) // &&  is not a pyramide && is not a prisme
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;

				std::cout << "midV  = " << midV << std::endl;
			}

			m_map.decCurrentLevel() ;

		}
	}
} ;

template <typename PFP>
class LerpSqrt3VolumeSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpSqrt3VolumeSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			Dart midV = m_map.phi_1(m_map.phi2(d));
			m_position[midV] = p;

			m_map.decCurrentLevel() ;

		}
	}
} ;

} // namespace Filters

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN

#endif /* __3MR_FILTERS_PRIMAL__ */
