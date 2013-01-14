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
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Multiresolution/filter.h"

namespace CGoGN
{

namespace Algo
{

namespace Volume
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
class LerpQuadOddSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpQuadOddSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> travW(m_map) ;
		for (Dart d = travW.begin(); d != travW.end(); d = travW.next())
		{
			typename PFP::VEC3 vc = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			unsigned int count = 0;
			typename PFP::VEC3 ec(0.0);
			Traversor3WE<typename PFP::MAP> travWE(m_map, d);
			for (Dart dit = travWE.begin(); dit != travWE.end(); dit = travWE.next())
			{
				m_map.incCurrentLevel();
				ec += m_position[m_map.phi1(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ec /= count;
			ec *= 3;

			count = 0;
			typename PFP::VEC3 fc(0.0);
			Traversor3WF<typename PFP::MAP> travWF(m_map, d);
			for (Dart dit = travWF.begin(); dit != travWF.end(); dit = travWF.next())
			{
				m_map.incCurrentLevel();
				fc += m_position[m_map.phi1(m_map.phi1(dit))];
				m_map.decCurrentLevel();
				++count;
			}
			fc /= count;
			fc *= 3;

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
			if(m_position[midV] != typename PFP::VEC3(0.0f,0.0f,0.0f))
			{
				std::cout << "position[midV] = " << m_position[midV] << std::endl;
			}
			else
				m_position[midV] += vc + ec + fc;
			m_map.decCurrentLevel() ;
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 vf(0.0);
			typename PFP::VEC3 ef(0.0);

			unsigned int count = 0;
			Traversor3FE<typename PFP::MAP> travFE(m_map, d);
			for (Dart dit = travFE.begin(); dit != travFE.end(); dit = travFE.next())
			{
				vf += m_position[dit];
				m_map.incCurrentLevel();
				ef += m_position[m_map.phi1(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ef /= count;
			ef *= 2.0;

			vf /= count;

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi1(m_map.phi1(d));
			if(m_position[midF] != typename PFP::VEC3(0.0f,0.0f,0.0f))
			{
				std::cout << "position[midF] = " << m_position[midF] << std::endl;
				//m_position[midF] = vf + ef ;
			}
			else
				m_position[midF] += vf + ef ;
			m_map.decCurrentLevel() ;
		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			typename PFP::VEC3 ve = (m_position[d] + m_position[m_map.phi1(d)]) * typename PFP::REAL(0.5);

			m_map.incCurrentLevel() ;
			Dart midE = m_map.phi1(d) ;
			if(m_position[midE] != typename PFP::VEC3(0.0f,0.0f,0.0f))
			{
				std::cout << "position[midE] = " << m_position[midE] << std::endl;
				//m_position[midE] = ve;
			}
			else
				m_position[midE] += ve;
			m_map.decCurrentLevel() ;
		}
	}

};




template <typename PFP>
class LerpQuadOddAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpQuadOddAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			typename PFP::VEC3 ve = (m_position[d] + m_position[m_map.phi1(d)]) * typename PFP::REAL(0.5);

			m_map.incCurrentLevel() ;
			Dart midE = m_map.phi1(d) ;
			m_position[midE] -= ve;
			m_map.decCurrentLevel() ;
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 vf(0.0);
			typename PFP::VEC3 ef(0.0);

			unsigned int count = 0;
			Traversor3FE<typename PFP::MAP> travFE(m_map, d);
			for (Dart dit = travFE.begin(); dit != travFE.end(); dit = travFE.next())
			{
				vf += m_position[dit];
				m_map.incCurrentLevel();
				ef += m_position[m_map.phi1(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ef /= count;
			ef *= 2.0;

			vf /= count;

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi1(m_map.phi1(d));
			m_position[midF] -= vf;// + ef ;
			m_map.decCurrentLevel() ;
		}

		TraversorW<typename PFP::MAP> travW(m_map) ;
		for (Dart d = travW.begin(); d != travW.end(); d = travW.next())
		{
			typename PFP::VEC3 vc = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			unsigned int count = 0;
			typename PFP::VEC3 ec(0.0);
			Traversor3WE<typename PFP::MAP> travWE(m_map, d);
			for (Dart dit = travWE.begin(); dit != travWE.end(); dit = travWE.next())
			{
				m_map.incCurrentLevel();
				ec += m_position[m_map.phi1(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ec /= count;
			ec *= 3;//12 * m_a * m_a;

			count = 0;
			typename PFP::VEC3 fc(0.0);
			Traversor3WF<typename PFP::MAP> travWF(m_map, d);
			for (Dart dit = travWF.begin(); dit != travWF.end(); dit = travWF.next())
			{
				m_map.incCurrentLevel();
				fc += m_position[m_map.phi1(m_map.phi1(dit))];
				m_map.decCurrentLevel();
				++count;
			}
			fc /= count;
			fc *= 3;//6 * m_a;

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
			m_position[midV] -= vc;// + ec + fc;
			m_map.decCurrentLevel() ;
		}
	}

};
















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
			typename PFP::VEC3 p = Algo::Surface::Geometry::faceCentroid<PFP>(m_map, d, m_position);

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
			if(!Algo::Modelisation::Tetrahedralization::isTetrahedron<PFP>(m_map,d) && !Algo::Modelisation::isPrism<PFP>(m_map,d) && !Algo::Modelisation::isPyra<PFP>(m_map,d))
			{
				typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;

				m_map.decCurrentLevel() ;
			}
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
			typename PFP::VEC3 p = Algo::Surface::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

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

} // namespace Volume

} // namespace Algo

} // namespace CGoGN

#endif /* __3MR_FILTERS_PRIMAL__ */
