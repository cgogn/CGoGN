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

#ifndef __MR_FILTERS_PRIMAL__
#define __MR_FILTERS_PRIMAL__

#include <cmath>
//#include "Algo/Decimation/decimation.h"

namespace CGoGN
{

namespace Algo
{

namespace MR
{

class Filter
{
public:
	Filter() {}
	virtual ~Filter() {}
	virtual void operator() () = 0 ;
} ;

/*********************************************************************************
 *                           LOOP BASIC FUNCTIONS
 *********************************************************************************/

template <typename PFP>
typename PFP::VEC3 loopOddVertex(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, Dart d1)
{
	Dart d2 = map.phi2(d1) ;
	Dart d3 = map.phi_1(d1) ;
	Dart d4 = map.phi_1(d2) ;

	typename PFP::VEC3 p1 = position[d1] ;
	typename PFP::VEC3 p2 = position[d2] ;
	typename PFP::VEC3 p3 = position[d3] ;
	typename PFP::VEC3 p4 = position[d4] ;

	p1 *= 3.0 / 8.0 ;
	p2 *= 3.0 / 8.0 ;
	p3 *= 1.0 / 8.0 ;
	p4 *= 1.0 / 8.0 ;

	return p1 + p2 + p3 + p4 ;
}

template <typename PFP>
typename PFP::VEC3 loopEvenVertex(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, Dart d)
{
	map.incCurrentLevel() ;

	typename PFP::VEC3 np(0) ;
	unsigned int degree = 0 ;
	Traversor2VVaE<typename PFP::MAP> trav(map, d) ;
	for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
	{
		++degree ;
		np += position[it] ;
	}

	map.decCurrentLevel() ;

	float mu = 3.0/8.0 + 1.0/4.0 * cos(2.0 * M_PI / degree) ;
	mu = (5.0/8.0 - (mu * mu)) / degree ;
	np *= 8.0/5.0 * mu ;

	return np ;
}

/*********************************************************************************
 *                           ANALYSIS FILTERS
 *********************************************************************************/

template <typename PFP>
class LoopOddAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopOddAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = loopOddVertex<PFP>(m_map, m_position, d) ;

			m_map.incCurrentLevel() ;

			Dart oddV = m_map.phi2(d) ;
			m_position[oddV] -= p ;

			m_map.decCurrentLevel() ;
		}
	}
} ;

template <typename PFP>
class LoopEvenAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position;

public:
	LoopEvenAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = loopEvenVertex<PFP>(m_map, m_position, d) ;
			m_position[d] -= p ;
		}
	}
} ;

template <typename PFP>
class LoopNormalisationAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position;

public:
	LoopNormalisationAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			unsigned int degree = m_map.vertexDegree(d) ;
			float n = 3.0/8.0 + 1.0/4.0 * cos(2.0 * M_PI / degree) ;
			n = 8.0/5.0 * (n * n) ;

			m_position[d] /= n ;
		}
	}
} ;

/*********************************************************************************
 *                           SYNTHESIS FILTERS
 *********************************************************************************/

template <typename PFP>
class LoopOddSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position;

public:
	LoopOddSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = loopOddVertex<PFP>(m_map, m_position, d) ;

			m_map.incCurrentLevel() ;

			Dart oddV = m_map.phi2(d) ;
			m_position[oddV] += p ;

			m_map.decCurrentLevel() ;
		}
	}
} ;

template <typename PFP>
class LoopEvenSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopEvenSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = loopEvenVertex<PFP>(m_map, m_position, d) ;
			m_position[d] += p ;
		}
	}
} ;

template <typename PFP>
class LoopNormalisationSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopNormalisationSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			unsigned int degree = m_map.vertexDegree(d) ;
			float n = 3.0/8.0 + 1.0/4.0 * cos(2.0 * M_PI / degree) ;
			n = 8.0/5.0 * (n * n) ;

			m_position[d] *= n ;
		}
	}
} ;

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
			typename PFP::VEC3 p = (m_position[d] + m_position[m_map.phi1(d)]) * typename PFP::REAL(0.5);

			m_map.incCurrentLevel() ;

			Dart midV = m_map.phi1(d) ;
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
			if(m_map.faceDegree(d) != 3)
			{
				Dart midF = m_map.phi1(m_map.phi1(d));
				m_position[midF] = p ;
			}
			m_map.decCurrentLevel() ;

		}
	}
} ;

/* SQRT(3)
 *********************************************************************************/

template <typename PFP>
class Sqrt3OddSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map;
	VertexAttribute<typename PFP::VEC3>& m_position;

public:
	Sqrt3OddSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorF<typename PFP::MAP> trav(m_map);
		for(Dart d = trav.begin() ; d != trav.end() ; d = trav.next())
		{
			typename PFP::VEC3 p(0.0);

			p += m_position[d];
			p += m_position[m_map.phi1(d)];
			p += m_position[m_map.phi_1(d)];

			p /= 3.0;

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi1(d);
			m_position[midF] = p ;
			m_map.decCurrentLevel() ;
		}
	}
};


/*********************************************************************************
 *                           SUBDIVISION FUNCTORS
 *********************************************************************************/

template <typename PFP>
class PipoVertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	PipoVertexVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;
		typename PFP::VEC3 p = m_position[d] ;
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

template <typename PFP>
class PipoEdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	PipoEdgeVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d1 = m_map.phi2(d) ;

		m_map.decCurrentLevel() ;
		Dart d2 = m_map.phi2(d1) ;
		typename PFP::VEC3 p = (m_position[d1] + m_position[d2]) / 2.0 ;
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

template <typename PFP>
class PipoFaceVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	PipoFaceVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi1(m_map.phi1(d)) ;

		m_map.decCurrentLevel() ;

		typename PFP::VEC3 p(0) ;
		unsigned int degree = 0 ;
		Traversor2FV<typename PFP::MAP> trav(m_map, df) ;
		for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
		{
			++degree ;
			p += m_position[it] ;
		}
		p /= degree ;

		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

template <typename PFP>
class LoopVertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopVertexVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;

		typename PFP::VEC3 np(0) ;
		unsigned int degree = 0 ;
		Traversor2VVaE<typename PFP::MAP> trav(m_map, d) ;
		for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
		{
			++degree ;
			np += m_position[it] ;
		}
		float tmp = 3.0 + 2.0 * cos(2.0 * M_PI / degree) ;
		float beta = (5.0 / 8.0) - ( tmp * tmp / 64.0 ) ;
		np *= beta / degree ;

		typename PFP::VEC3 vp = m_position[d] ;
		vp *= 1.0 - beta ;

		m_map.incCurrentLevel() ;

		m_position[d] = np + vp ;

		return false ;
	}
} ;

template <typename PFP>
class LoopEdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopEdgeVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d1 = m_map.phi2(d) ;

		m_map.decCurrentLevel() ;

		Dart d2 = m_map.phi2(d1) ;
		Dart d3 = m_map.phi_1(d1) ;
		Dart d4 = m_map.phi_1(d2) ;

		typename PFP::VEC3 p1 = m_position[d1] ;
		typename PFP::VEC3 p2 = m_position[d2] ;
		typename PFP::VEC3 p3 = m_position[d3] ;
		typename PFP::VEC3 p4 = m_position[d4] ;

		p1 *= 3.0 / 8.0 ;
		p2 *= 3.0 / 8.0 ;
		p3 *= 1.0 / 8.0 ;
		p4 *= 1.0 / 8.0 ;

		m_map.incCurrentLevel() ;

		m_position[d] = p1 + p2 + p3 + p4 ;

		return false ;
	}
} ;

template <typename PFP>
class CCVertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	CCVertexVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;

		typename PFP::VEC3 np1(0) ;
		typename PFP::VEC3 np2(0) ;
		unsigned int degree1 = 0 ;
		unsigned int degree2 = 0 ;
		Dart it = d ;
		do
		{
			++degree1 ;
			Dart dd = m_map.phi1(it) ;
			np1 += m_position[dd] ;
			Dart end = m_map.phi_1(it) ;
			dd = m_map.phi1(dd) ;
			do
			{
				++degree2 ;
				np2 += m_position[dd] ;
				dd = m_map.phi1(dd) ;
			} while(dd != end) ;
			it = m_map.alpha1(it) ;
		} while(it != d) ;

		float beta = 3.0 / (2.0 * degree1) ;
		float gamma = 1.0 / (4.0 * degree2) ;
		np1 *= beta / degree1 ;
		np2 *= gamma / degree2 ;

		typename PFP::VEC3 vp = m_position[d] ;
		vp *= 1.0 - beta - gamma ;

		m_map.incCurrentLevel() ;

		m_position[d] = np1 + np2 + vp ;

		return false ;
	}
} ;

template <typename PFP>
class CCEdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	CCEdgeVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d1 = m_map.phi2(d) ;

		m_map.decCurrentLevel() ;

		Dart d2 = m_map.phi2(d1) ;
		Dart d3 = m_map.phi_1(d1) ;
		Dart d4 = m_map.phi_1(d2) ;
		Dart d5 = m_map.phi1(m_map.phi1(d1)) ;
		Dart d6 = m_map.phi1(m_map.phi1(d2)) ;

		typename PFP::VEC3 p1 = m_position[d1] ;
		typename PFP::VEC3 p2 = m_position[d2] ;
		typename PFP::VEC3 p3 = m_position[d3] ;
		typename PFP::VEC3 p4 = m_position[d4] ;
		typename PFP::VEC3 p5 = m_position[d5] ;
		typename PFP::VEC3 p6 = m_position[d6] ;

		p1 *= 3.0 / 8.0 ;
		p2 *= 3.0 / 8.0 ;
		p3 *= 1.0 / 16.0 ;
		p4 *= 1.0 / 16.0 ;
		p5 *= 1.0 / 16.0 ;
		p6 *= 1.0 / 16.0 ;

		m_map.incCurrentLevel() ;

		m_position[d] = p1 + p2 + p3 + p4 + p5 + p6 ;

		return false ;
	}
} ;

template <typename PFP>
class CCFaceVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	CCFaceVertexFunctor(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi1(m_map.phi1(d)) ;

		m_map.decCurrentLevel() ;

		typename PFP::VEC3 p(0) ;
		unsigned int degree = 0 ;
		Traversor2FV<typename PFP::MAP> trav(m_map, df) ;
		for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
		{
			++degree ;
			p += m_position[it] ;
		}
		p /= degree ;

		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;


} // namespace MR

} // namespace Algo

} // namespace CGoGN

#endif

