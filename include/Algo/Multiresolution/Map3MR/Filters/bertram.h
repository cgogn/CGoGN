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

#ifndef __3MR_BERTRAM_FILTER__
#define __3MR_BERTRAM_FILTER__

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

//w-lift(a)
template <typename PFP>
class Ber02OddSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Ber02OddSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		float a = 0.5;

		TraversorW<typename PFP::MAP> travW(m_map) ;
		for (Dart d = travW.begin(); d != travW.end(); d = travW.next())
		{
			typename PFP::VEC3 vc = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			unsigned int count = 0;
			typename PFP::VEC3 ec(0);
			Traversor3WE<typename PFP::MAP> travWE(m_map, d);
			for (Dart dit = travWE.begin(); dit != travWE.end(); dit = travWE.next())
			{
				m_map.incCurrentLevel();
				ec += m_position[m_map.phi2(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ec /= count;

			count = 0;
			typename PFP::VEC3 fc(0);
			Traversor3WF<typename PFP::MAP> travWF(m_map, d);
			for (Dart dit = travWF.begin(); dit != travWF.end(); dit = travWF.next())
			{
				m_map.incCurrentLevel();
				fc += m_position[m_map.phi2(m_map.phi1(dit))];
				m_map.decCurrentLevel();
				++count;
			}

			fc /= count;

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
			m_position[midV] += 8 * a * a * a * vc + 12 * a * a * ec + 6 * a * fc;
			m_map.decCurrentLevel() ;
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 vf = Algo::Geometry::faceCentroid<PFP>(m_map, d, m_position);

			typename PFP::VEC3 ef(0);
			unsigned int count = 0;
			Traversor3FE<typename PFP::MAP> travFE(m_map, d);
			for (Dart dit = travFE.begin(); dit != travFE.end(); dit = travFE.next())
			{
				m_map.incCurrentLevel();
				ef += m_position[m_map.phi2(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ef /= count;

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi2(m_map.phi1(d));
			m_position[midF] += vf * 4.0 * a * a + ef * 4.0 * a;
			m_map.decCurrentLevel() ;
		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			typename PFP::VEC3 ve = (m_position[d] + m_position[m_map.phi2(d)]) * typename PFP::REAL(0.5);

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi2(d) ;
			m_position[midV] += ve * a * 2.0;
			m_map.decCurrentLevel() ;
		}

	}
} ;

// s-lift(a)
template <typename PFP>
class Ber02EvenSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Ber02EvenSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		float a = 0.5;

		TraversorV<typename PFP::MAP> travV(m_map);
		for(Dart d = travV.begin() ; d != travV.end() ; d = travV.next())
		{
			if(!m_map.isBoundaryVertex(d))
			{
				typename PFP::VEC3 cv(0);
				unsigned int count = 0;
				Traversor3VW<typename PFP::MAP> travVW(m_map,d);
				for(Dart dit = travVW.begin(); dit != travVW.end() ; dit = travVW.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(dit)));
					cv += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				cv /= count;

				typename PFP::VEC3 fv(0);
				count = 0;
				Traversor3VF<typename PFP::MAP> travVF(m_map,d);
				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(m_map.phi1(dit));
					fv += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				fv /= count;

				typename PFP::VEC3 ev(0);
				count = 0;
				Traversor3VE<typename PFP::MAP> travVE(m_map,d);
				for(Dart dit = travVE.begin(); dit != travVE.end() ; dit = travVE.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(dit);
					ev += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				ev /= count;

				m_position[d] += cv * 8 * a * a * a + fv * 12 * a * a + ev * 6 * a;
			}
			else
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);

				typename PFP::VEC3 fv(0);
				unsigned int count = 0;
				Traversor2VF<typename PFP::MAP> travVF(m_map,db);
				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(m_map.phi1(dit));
					fv += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				fv /= count;

				typename PFP::VEC3 ev(0);
				count = 0;
				Traversor2VE<typename PFP::MAP> travVE(m_map,db);
				for(Dart dit = travVE.begin(); dit != travVE.end() ; dit = travVE.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(dit);
					ev += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				ev /= count;

				m_position[db] += fv * 4 * a * a + ev * 4 * a;
			}
		}

		TraversorE<typename PFP::MAP> travE(m_map);
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);

				typename PFP::VEC3 fe(0);

				m_map.incCurrentLevel() ;
				Dart midV = m_map.phi2(m_map.phi1(db));
				fe += m_position[midV];
				m_map.decCurrentLevel() ;

				m_map.incCurrentLevel() ;
				midV = m_map.phi2(m_map.phi1(m_map.phi2(db)));
				fe += m_position[midV];
				m_map.decCurrentLevel() ;

				fe /= 2;

				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi2(db);
				m_position[midF] += fe * 2 * a;
				m_map.decCurrentLevel() ;
			}
			else
			{
				typename PFP::VEC3 ce(0);
				unsigned int count = 0;
				Traversor3EW<typename PFP::MAP> travEW(m_map, d);
				for(Dart dit = travEW.begin() ; dit != travEW.end() ; dit = travEW.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(dit)));
					ce += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}

				ce /= count;

				typename PFP::VEC3 fe(0);
				count = 0;
				Traversor3FW<typename PFP::MAP> travFW(m_map, d);
				for(Dart dit = travFW.begin() ; dit != travFW.end() ; dit = travFW.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(m_map.phi1(dit));
					fe += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}

				fe /= count;

				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi2(d);
				m_position[midF] += ce * 4 * a * a + fe * 4 * a;
				m_map.decCurrentLevel() ;
			}
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 cf(0);

			m_map.incCurrentLevel();
			Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
			cf += m_position[midV];
			m_map.decCurrentLevel();

			if(!m_map.isBoundaryFace(d))
			{
				Dart d3 = m_map.phi3(d);
				m_map.incCurrentLevel();
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d3)));
				cf += m_position[midV];
				m_map.decCurrentLevel();

				cf /= 2;
			}

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi2(m_map.phi1(d));
			m_position[midF] += cf * 2 * a;
			m_map.decCurrentLevel() ;
		}

	}
} ;

// s-scale(a)
template <typename PFP>
class Ber02ScaleSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Ber02ScaleSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		float a = 0.5;

		TraversorV<typename PFP::MAP> travV(m_map) ;
		for (Dart d = travV.begin(); d != travV.end(); d = travV.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);
				m_position[db] *= a * a;
			}
			else
			{
				m_position[d] *= a * a * a;
			}
		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);

				m_map.incCurrentLevel() ;
				Dart midE = m_map.phi2(db);
				m_position[midE] *= a ;
				m_map.decCurrentLevel() ;
			}
			else
			{
				m_map.incCurrentLevel() ;
				Dart midE = m_map.phi2(d);
				m_position[midE] *= a * a;
				m_map.decCurrentLevel() ;
			}
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			if(!m_map.isBoundaryFace(d))
			{
				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi2(m_map.phi1(d));
				m_position[midF] *= a ;
				m_map.decCurrentLevel() ;
			}
		}
	}
} ;

} // namespace Filters

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN


#endif /* __3MR_FILTERS_PRIMAL__ */
