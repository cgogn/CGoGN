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

//
// Synthesis
//

//w-lift(a)
template <typename PFP>
class Ber02OddSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;
	typename PFP::VEC3::DATA_TYPE m_a;


public:
	Ber02OddSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p, typename PFP::VEC3::DATA_TYPE a) : m_map(m), m_position(p), m_a(a)
	{}

	void operator() ()
	{
		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 vf(0.0);
			typename PFP::VEC3 ef(0.0);

			unsigned int count = 0;
			Traversor2FE<typename PFP::MAP> travFE(m_map, d);
			for (Dart dit = travFE.begin(); dit != travFE.end(); dit = travFE.next())
			{
				vf += m_position[dit];
				m_map.incCurrentLevel();
				ef += m_position[m_map.phi1(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ef /= count;
			ef *= 4.0 * m_a;

			vf /= count;
			vf *= 4.0 * m_a * m_a;

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi1(m_map.phi1(d));
			m_position[midF] += vf + ef ;
			m_map.decCurrentLevel() ;

		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			typename PFP::VEC3 ve = (m_position[d] + m_position[m_map.phi1(d)]) * typename PFP::REAL(0.5);
			ve *= 2.0 * m_a;

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi1(d) ;
			m_position[midV] += ve;
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
	typename PFP::VEC3::DATA_TYPE m_a;

public:
	Ber02EvenSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p, typename PFP::VEC3::DATA_TYPE a) : m_map(m), m_position(p), m_a(a)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> travV(m_map);
		for(Dart d = travV.begin() ; d != travV.end() ; d = travV.next())
		{
			typename PFP::VEC3 ev(0.0);
			typename PFP::VEC3 fv(0.0);
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryEdgeOfVertex(d);
				m_map.incCurrentLevel() ;
				ev += (m_position[m_map.phi1(db)] + m_position[m_map.phi_1(db)]) * typename PFP::REAL(0.5);
				m_map.decCurrentLevel() ;
				ev *= 2 * m_a;

				m_position[db] += ev;
			}
			else
			{
				unsigned int count = 0;
				Traversor2VF<typename PFP::MAP> travVF(m_map,d);
				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
				{
					m_map.incCurrentLevel() ;

					Dart midEdgeV = m_map.phi1(dit);
					ev += m_position[midEdgeV];
					fv += m_position[m_map.phi1(midEdgeV)];

					m_map.decCurrentLevel() ;
					++count;
				}
				fv /= count;
				fv *= 4 * m_a * m_a;

				ev /= count;
				ev *= 4 * m_a;

				m_position[d] += fv + ev;
			}
		}

		TraversorE<typename PFP::MAP> travE(m_map);
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			if(!m_map.isBoundaryEdge(d))
			{
				unsigned int count = 0;

				typename PFP::VEC3 fe(0.0);
				Traversor2EF<typename PFP::MAP> travEF(m_map, d);
				for(Dart dit = travEF.begin() ; dit != travEF.end() ; dit = travEF.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi1(m_map.phi1(dit));
					fe += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}

				fe /= count;
				fe *= 2 * m_a;

				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi1(d);
				m_position[midF] += fe;
				m_map.decCurrentLevel() ;
			}
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
	typename PFP::VEC3::DATA_TYPE m_a;

public:
	Ber02ScaleSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p, typename PFP::VEC3::DATA_TYPE a) : m_map(m), m_position(p), m_a(a)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> travV(m_map) ;
		for (Dart d = travV.begin(); d != travV.end(); d = travV.next())
		{
			if(m_map.isBoundaryVertex(d))
				m_position[d] *= m_a;
			else
				m_position[d] *= m_a * m_a;
		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			m_map.incCurrentLevel() ;
			Dart midE = m_map.phi1(d);
			if(!m_map.isBoundaryVertex(midE))
				m_position[midE] *= m_a ;
			m_map.decCurrentLevel() ;
		}
	}
} ;

//
// Analysis
//

//w-lift(a)
template <typename PFP>
class Ber02OddAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;
	typename PFP::VEC3::DATA_TYPE m_a;

public:
	Ber02OddAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p, typename PFP::VEC3::DATA_TYPE a) : m_map(m), m_position(p), m_a(a)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			typename PFP::VEC3 ve = (m_position[d] + m_position[m_map.phi1(d)]) * typename PFP::REAL(0.5);
			ve *= 2.0 * m_a;

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi1(d) ;
			m_position[midV] -= ve;
			m_map.decCurrentLevel() ;
		}


		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 vf(0.0);
			typename PFP::VEC3 ef(0.0);

			unsigned int count = 0;
			Traversor2FE<typename PFP::MAP> travFE(m_map, d);
			for (Dart dit = travFE.begin(); dit != travFE.end(); dit = travFE.next())
			{
				vf += m_position[dit];
				m_map.incCurrentLevel();
				ef += m_position[m_map.phi1(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ef /= count;
			ef *= 4.0 * m_a;

			vf /= count;
			vf *= 4.0 * m_a * m_a;

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi1(m_map.phi1(d));
			m_position[midF] -= vf + ef ;
			m_map.decCurrentLevel() ;
		}

	}
};

// s-lift(a)
template <typename PFP>
class Ber02EvenAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;
	typename PFP::VEC3::DATA_TYPE m_a;

public:
	Ber02EvenAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p, typename PFP::VEC3::DATA_TYPE a) : m_map(m), m_position(p), m_a(a)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> travE(m_map);
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			if(!m_map.isBoundaryEdge(d))
			{
				unsigned int count = 0;

				typename PFP::VEC3 fe(0);
				Traversor2EF<typename PFP::MAP> travEF(m_map, d);
				for(Dart dit = travEF.begin() ; dit != travEF.end() ; dit = travEF.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi1(m_map.phi1(dit));
					fe += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}

				fe /= count;
				fe *= 2 * m_a;

				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi1(d);
				m_position[midF] -= fe;
				m_map.decCurrentLevel() ;
			}
		}

		TraversorV<typename PFP::MAP> travV(m_map);
		for(Dart d = travV.begin() ; d != travV.end() ; d = travV.next())
		{
			typename PFP::VEC3 ev(0.0);
			typename PFP::VEC3 fv(0.0);
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryEdgeOfVertex(d);
				m_map.incCurrentLevel() ;
				ev = (m_position[m_map.phi1(db)] + m_position[m_map.phi_1(db)]);
				m_map.decCurrentLevel() ;
				ev *= m_a;

				m_position[d] -= ev;
			}
			else
			{
				unsigned int count = 0;

				Traversor2VF<typename PFP::MAP> travVF(m_map,d);
				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
				{
					m_map.incCurrentLevel() ;

					Dart midEdgeV = m_map.phi1(dit);
					ev += m_position[midEdgeV];
					fv += m_position[m_map.phi1(midEdgeV)];

					m_map.decCurrentLevel() ;
					++count;
				}
				fv /= count;
				fv *= 4 * m_a * m_a;

				ev /= count;
				ev *= 4 * m_a;

				m_position[d] -= fv + ev;
			}
		}
	}
};

// s-scale(a)
template <typename PFP>
class Ber02ScaleAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;
	typename PFP::VEC3::DATA_TYPE m_a;

public:
	Ber02ScaleAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p, typename PFP::VEC3::DATA_TYPE a) : m_map(m), m_position(p), m_a(a)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> travV(m_map) ;
		for (Dart d = travV.begin(); d != travV.end(); d = travV.next())
		{
			if(m_map.isBoundaryVertex(d))
				m_position[d] /= m_a;
			else
				m_position[d] /= m_a * m_a;
		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			m_map.incCurrentLevel() ;
			Dart midE = m_map.phi1(d);
			if(!m_map.isBoundaryVertex(midE))
				m_position[midE] /= m_a ;
			m_map.decCurrentLevel() ;
		}
	}
};



///*************************************************************************************
// * 							With features preserving
// *************************************************************************************/
//
////
//// Synthesis
////
//
////w-lift(a)
//template <typename PFP>
//class Ber02OddSynthesisFilterFeatures : public Ber02OddSynthesisFilter<PFP>
//{
//protected:
//	const VertexAttribute<unsigned int>& m_Vfeature;
//	const EdgeAttribute<unsigned int>& m_Efeature;
//
//public:
//	Ber02OddSynthesisFilterFeatures(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p,
//			const VertexAttribute<unsigned int> v, const EdgeAttribute<unsigned int> e, typename PFP::VEC3::DATA_TYPE a) :
//		Ber02OddSynthesisFilter<PFP>(m, p, a), m_Vfeature(v), m_Efeature(e)
//	{}
//} ;
//
//// s-lift(a)
//template <typename PFP>
//class Ber02EvenSynthesisFilterFeatures  : public Filter
//{
//protected:
//	typename PFP::MAP& m_map ;
//	VertexAttribute<typename PFP::VEC3>& m_position ;
//	const VertexAttribute<unsigned int>& m_Vfeature;
//	const EdgeAttribute<unsigned int>& m_Efeature;
//	typename PFP::VEC3::DATA_TYPE m_a;
//
//public:
//	Ber02EvenSynthesisFilterFeatures(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p,
//			const VertexAttribute<unsigned int> v, const EdgeAttribute<unsigned int> e, typename PFP::VEC3::DATA_TYPE a) :
//		m_map(m), m_position(p), m_Vfeature(v), m_Efeature(e), m_a(a)
//	{}
//
//	void operator() ()
//	{
//		TraversorV<typename PFP::MAP> travV(m_map);
//		for(Dart d = travV.begin() ; d != travV.end() ; d = travV.next())
//		{
//			if(m_Vfeature[d] == 2) //is a part of a face-feature
//			{
//				unsigned int count = 0;
//				typename PFP::VEC3 ev(0.0);
//				typename PFP::VEC3 fv(0.0);
//				Traversor2VF<typename PFP::MAP> travVF(m_map,d);
//				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
//				{
//					m_map.incCurrentLevel() ;
//
//					Dart midEdgeV = m_map.phi1(dit);
//					ev += m_position[midEdgeV];
//					fv += m_position[m_map.phi1(midEdgeV)];
//
//					m_map.decCurrentLevel() ;
//					++count;
//				}
//				fv /= count;
//				fv *= 4 * m_a * m_a;
//
//				ev /= count;
//				ev *= 4 * m_a;
//
//				m_position[d] += fv + ev;
//			}
//			else if(m_Vfeature[d] == 1) //is a part of an edge-feature
//			{
//				unsigned int count = 0;
//				typename PFP::VEC3 ev(0.0);
//
//				Traversor2VF<typename PFP::MAP> travVF(m_map,d);
//				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
//				{
//					m_map.incCurrentLevel() ;
//
//					Dart midEdgeV = m_map.phi1(dit);
//
//					if(m_Vfeature[midEdgeV] == 1)
//					{
//						ev += m_position[midEdgeV];
//						++count;
//					}
//
//					m_map.decCurrentLevel() ;
//				}
//
//				ev /= count;
//				ev *= 2 * m_a;
//
//				m_position[d] += ev;
//			}
//			else
//			{
//				m_position[d] += 0.0;
//			}
//		}
//
//		TraversorE<typename PFP::MAP> travE(m_map);
//		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
//		{
//			if(m_Efeature[d] == 2)
//			{
//				unsigned int count = 0;
//
//				typename PFP::VEC3 fe(0);
//				Traversor2EF<typename PFP::MAP> travEF(m_map, d);
//				for(Dart dit = travEF.begin() ; dit != travEF.end() ; dit = travEF.next())
//				{
//					m_map.incCurrentLevel() ;
//					Dart midV = m_map.phi1(m_map.phi1(dit));
//					fe += m_position[midV];
//					m_map.decCurrentLevel() ;
//					++count;
//				}
//
//				fe /= count;
//				fe *= 2 * m_a;
//
//				m_map.incCurrentLevel() ;
//				Dart midF = m_map.phi1(d);
//				m_position[midF] += fe;
//				m_map.decCurrentLevel() ;
//			}
//			else
//			{
//				m_map.incCurrentLevel() ;
//				Dart midF = m_map.phi1(d);
//				m_position[midF] += 0.0;
//				m_map.decCurrentLevel() ;
//			}
//		}
//
//	}
//} ;
//
//// s-scale(a)
//template <typename PFP>
//class Ber02ScaleFilterFeatures : public Filter
//{
//protected:
//	typename PFP::MAP& m_map ;
//	VertexAttribute<typename PFP::VEC3>& m_position ;
//	const VertexAttribute<unsigned int>& m_Vfeature;
//	const EdgeAttribute<unsigned int>& m_Efeature;
//	typename PFP::VEC3::DATA_TYPE m_a;
//
//public:
//	Ber02ScaleFilterFeatures(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p,
//			const VertexAttribute<unsigned int> v, const EdgeAttribute<unsigned int> e, typename PFP::VEC3::DATA_TYPE a) :
//		m_map(m), m_position(p), m_Vfeature(v), m_Efeature(e), m_a(a)
//	{}
//
//	void operator() ()
//	{
//		TraversorV<typename PFP::MAP> travV(m_map) ;
//		for (Dart d = travV.begin(); d != travV.end(); d = travV.next())
//		{
//			m_position[d] *= pow(m_a, m_Vfeature[d]);
//		}
//
//		TraversorE<typename PFP::MAP> travE(m_map) ;
//		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
//		{
//			if(m_Efeature[d] == 2)
//			{
//				m_map.incCurrentLevel() ;
//				Dart midE = m_map.phi1(d);
//				m_position[midE] *= m_a ;
//				m_map.decCurrentLevel() ;
//			}
//		}
//	}
//} ;
//
////
//// Analysis
////
//
////w-lift(a)
//template <typename PFP>
//class Ber02OddAnalysisFilterFeatures : public Ber02OddAnalysisFilter<PFP>
//{
//protected:
//	const VertexAttribute<unsigned int>& m_Vfeature;
//	const EdgeAttribute<unsigned int>& m_Efeature;
//
//public:
//	Ber02OddAnalysisFilterFeatures(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p,
//			const VertexAttribute<unsigned int> v, const EdgeAttribute<unsigned int> e, typename PFP::VEC3::DATA_TYPE a) :
//		Ber02OddAnalysisFilter<PFP>(m, p, a), m_Vfeature(v), m_Efeature(e)
//	{}
//} ;


} // namespace Filters

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN


#endif /* __3MR_FILTERS_PRIMAL__ */
