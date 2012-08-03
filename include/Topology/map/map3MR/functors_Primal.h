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

#ifndef __3MR_FUNCTORS_PRIMAL__
#define __3MR_FUNCTORS_PRIMAL__

#include <cmath>
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Multiresolution
{

/*********************************************************************************
 *                           LOOP BASIC FUNCTIONS
 *********************************************************************************/
template <typename PFP>
typename PFP::VEC3 loopOddVertex(typename PFP::MAP& map, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, Dart d1)
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
typename PFP::VEC3 loopEvenVertex(typename PFP::MAP& map, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, Dart d)
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
 *          SHW04 BASIC FUNCTIONS : tetrahedral/octahedral meshes
 *********************************************************************************/
template <typename PFP>
typename PFP::VEC3 SHW04Vertex(typename PFP::MAP& map, const AttributeHandler<typename PFP::VEC3, VERTEX>& position, Dart d)
{
	typename PFP::VEC3 res(0);

	if(map.isTetrahedron(d))
	{
		Dart d1 = map.phi1(d) ;
		Dart d2 = map.phi_1(d);
		Dart d3 = map.phi_1(map.phi2(d));

		typename PFP::VEC3 p = position[d];
		typename PFP::VEC3 p1 = position[d1] ;
		typename PFP::VEC3 p2 = position[d2] ;
		typename PFP::VEC3 p3 = position[d3] ;

		p *= -1;
		p1 *= 17.0 / 3.0;
		p2 *= 17.0 / 3.0;
		p3 *= 17.0 / 3.0;

		res += p + p1 + p2 + p3;
		res *= 1.0 / 16.0;
	}
	else
	{
		Dart d1 = map.phi1(d);
		Dart d2 = map.phi_1(d);
		Dart d3 = map.phi_1(map.phi2(d));
		Dart d4 = map.phi_1(map.phi2(d3));
		Dart d5 = map.phi_1(map.phi2(map.phi_1(d)));

		typename PFP::VEC3 p = position[d];
		typename PFP::VEC3 p1 = position[d1] ;
		typename PFP::VEC3 p2 = position[d2] ;
		typename PFP::VEC3 p3 = position[d3] ;
		typename PFP::VEC3 p4 = position[d4] ;
		typename PFP::VEC3 p5 = position[d5] ;

		p *= 3.0 / 4.0;
		p1 *= 1.0 / 6.0;
		p2 *= 1.0 / 6.0;
		p3 *= 1.0 / 6.0;
		p4 *= 7.0 / 12.0;
		p5 *= 1.0 / 6.0;

		res += p + p1 + p2 + p3 + p4 + p5;
		res *= 1.0 / 2.0;
	}

	return res;
}

/*********************************************************************************
 *          BSXW02 BASIC FUNCTIONS : polyhedral meshes
 *********************************************************************************/




/*******************************MJ96EdgeVertexFunctor**************************************************
 *                           FUNCTORS
 *********************************************************************************/

/* Linear Interpolation
 *********************************************************************************/
template <typename PFP>
class LerpVertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	LerpVertexVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		m_map.decCurrentLevel() ;
		typename PFP::VEC3 p = m_position[d] ;
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
};

template <typename PFP>
class LerpEdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	LerpEdgeVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart d1 = m_map.phi2(d);

		m_map.decCurrentLevel();
		Dart d2 = m_map.phi2(d1);
		typename PFP::VEC3 p = (m_position[d1] + m_position[d2]) * typename PFP::REAL(0.5);
		m_map.incCurrentLevel();

		m_position[d] = p;

		return false;
	}
} ;

template <typename PFP>
class LerpFaceVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	LerpFaceVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi1(m_map.phi1(d)) ;

		m_map.decCurrentLevel() ;
		typename PFP::VEC3 p =  Algo::Geometry::faceCentroid<PFP>(m_map, df, m_position);
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;

template <typename PFP>
class LerpVolumeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	LerpVolumeVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi_1(m_map.phi2(m_map.phi1(d))) ;

		m_map.decCurrentLevel() ;
		typename PFP::VEC3 p =  Algo::Geometry::volumeCentroid<PFP>(m_map, df, m_position);
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false ;
	}
} ;


/* SHW04 basic functions : tetrahedral/octahedral meshes
 *********************************************************************************/
template <typename PFP>
class SHW04VertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	SHW04VertexVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{

		if(m_map.isBoundaryVertex(d))
		{
			Dart db = m_map.findBoundaryFaceOfVertex(d);

			m_map.decCurrentLevel() ;

			typename PFP::VEC3 np(0) ;
			unsigned int degree = 0 ;
			Traversor2VVaE<typename PFP::MAP> trav(m_map, db) ;
			for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
			{
				++degree ;
				np += m_position[it] ;
			}
			float tmp = 3.0 + 2.0 * cos(2.0 * M_PI / degree) ;
			float beta = (5.0 / 8.0) - ( tmp * tmp / 64.0 ) ;
			np *= beta / degree ;

			typename PFP::VEC3 vp = m_position[db] ;
			vp *= 1.0 - beta ;

			m_map.incCurrentLevel() ;

			m_position[d] = np + vp ;
		}
		else
		{
			typename PFP::VEC3 p = typename PFP::VEC3(0);
			unsigned int degree = 0;

			m_map.decCurrentLevel() ;

			Traversor3VW<typename PFP::MAP> travVW(m_map, d);
			for(Dart dit = travVW.begin() ; dit != travVW.end() ; dit = travVW.next())
			{
				p += SHW04Vertex<PFP>(m_map, m_position, dit);
				++degree;
			}

			p /= degree;

			m_map.incCurrentLevel() ;

			m_position[d] = p ;
		}
		return false ;
	}
} ;

template <typename PFP>
class SHW04EdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	SHW04EdgeVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		if(m_map.isBoundaryEdge(d))
		{
			Dart dd = m_map.phi2(d) ;
			m_map.decCurrentLevel() ;

			Dart d1 = m_map.findBoundaryFaceOfEdge(dd);

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
		}
		else
		{
			Dart d1 = m_map.phi2(d);

			m_map.decCurrentLevel();
			Dart d2 = m_map.phi2(d1);
			typename PFP::VEC3 mid = (m_position[d1] + m_position[d2]) * typename PFP::REAL(0.5);


			typename PFP::VEC3 p = typename PFP::VEC3(0);
			unsigned int degree = 0;

			Traversor3VW<typename PFP::MAP> travVW(m_map, d);
			for(Dart dit = travVW.begin() ; dit != travVW.end() ; dit = travVW.next())
			{
				p += SHW04Vertex<PFP>(m_map, m_position, dit);
				++degree;
			}

			p /= degree;

			m_map.incCurrentLevel();

			m_position[d] = mid + p ;
		}

		return false ;
	}
} ;

template <typename PFP>
class SHW04VolumeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position;

public:
	SHW04VolumeVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi_1(m_map.phi2(m_map.phi1(d))) ;

		m_map.decCurrentLevel() ;
		typename PFP::VEC3 p =  Algo::Geometry::volumeCentroid<PFP>(m_map, df, m_position);
		m_map.incCurrentLevel() ;

		m_position[d] = p ;

		return false;
	}
};


/* MJ96 basic functions : polyhedral meshes
 *********************************************************************************/
template <typename PFP>
class MJ96VertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	MJ96VertexVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		if(m_map.isBoundaryVertex(d))
		{
			Dart db = m_map.findBoundaryFaceOfVertex(d);

			m_map.decCurrentLevel() ;

			typename PFP::VEC3 np1(0) ;
			typename PFP::VEC3 np2(0) ;
			unsigned int degree1 = 0 ;
			unsigned int degree2 = 0 ;
			Dart it = db ;
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
				it = m_map.phi2(m_map.phi_1(it)) ;
			} while(it != db) ;

			float beta = 3.0 / (2.0 * degree1) ;
			float gamma = 1.0 / (4.0 * degree2) ;
			np1 *= beta / degree1 ;
			np2 *= gamma / degree2 ;

			typename PFP::VEC3 vp = m_position[db] ;
			vp *= 1.0 - beta - gamma ;

			m_map.incCurrentLevel() ;

			m_position[d] = np1 + np2 + vp ;

		}
		else
		{
			typename PFP::VEC3 P = m_position[d];

			m_map.decCurrentLevel() ;
			//vertex points
			typename PFP::VEC3 Cavg = typename PFP::VEC3(0);
			unsigned int degree = 0;
			Traversor3VW<typename PFP::MAP> travVW(m_map, d);
			for(Dart dit = travVW.begin() ; dit != travVW.end() ; dit = travVW.next())
			{
				Cavg += Algo::Geometry::volumeCentroid<PFP>(m_map, dit, m_position);
				++degree;
			}
			Cavg /= degree;

			typename PFP::VEC3 Aavg = typename PFP::VEC3(0);
			degree = 0;
			Traversor3VF<typename PFP::MAP> travVF(m_map, d);
			for(Dart dit = travVF.begin() ; dit != travVF.end() ; dit = travVF.next())
			{
				Aavg += Algo::Geometry::faceCentroid<PFP>(m_map, dit, m_position);
				++degree;
			}
			Aavg /= degree;

			typename PFP::VEC3 Mavg = typename PFP::VEC3(0);
			degree = 0;
			Traversor3VE<typename PFP::MAP> travVE(m_map, d);
			for(Dart dit = travVE.begin() ; dit != travVE.end() ; dit = travVE.next())
			{
				Dart d2 = m_map.phi2(dit);
				Aavg += (m_position[dit] + m_position[d2]) * typename PFP::REAL(0.5);
				++degree;
			}
			Aavg /= degree;

			typename PFP::VEC3 vp = Cavg + Aavg * 3 + Mavg * 3 + P;
			vp /= 8;

			m_map.incCurrentLevel() ;

			m_position[d] = vp;
		}

		return false;
	}
};

template <typename PFP>
class MJ96EdgeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	MJ96EdgeVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		if(m_map.isBoundaryEdge(d))
		{
			Dart db = m_map.findBoundaryFaceOfEdge(d);

			Dart d1 = m_map.phi2(db) ;

			//m_map.decCurrentLevel() ;

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

			//m_map.incCurrentLevel() ;

			m_position[d] = p1 + p2 + p3 + p4 + p5 + p6 ;
		}
		else
		{
			Dart d2 = m_map.phi2(d);

			//m_map.decCurrentLevel() ;
			//edge points
			typename PFP::VEC3 Cavg = typename PFP::VEC3(0);
			unsigned int degree = 0;
			Traversor3EW<typename PFP::MAP> travEW(m_map, d2);
			for(Dart dit = travEW.begin() ; dit != travEW.end() ; dit = travEW.next())
			{
				Cavg += Algo::Geometry::volumeCentroid<PFP>(m_map, dit, m_position);
				++degree;
			}
			Cavg /= degree;

			typename PFP::VEC3 Aavg = typename PFP::VEC3(0);
			degree = 0;
			Traversor3EF<typename PFP::MAP> travEF(m_map, d2);
			for(Dart dit = travEF.begin() ; dit != travEF.end() ; dit = travEF.next())
			{
				Aavg += Algo::Geometry::faceCentroid<PFP>(m_map, dit, m_position);
				++degree;
			}
			Aavg /= degree;

			Dart d22 = m_map.phi2(d2);
			typename PFP::VEC3 M = (m_position[d2] + m_position[d22]) * typename PFP::REAL(0.5);

			typename PFP::VEC3 ep = Cavg + Aavg * 2 + M * (degree - 3);
			ep /= degree;

			//m_map.incCurrentLevel() ;

			m_position[d] = ep;
		}

		return false;
	}
};

template <typename PFP>
class MJ96FaceVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	MJ96FaceVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		if(m_map.isBoundaryVertex(d))
		{
			Dart db = m_map.findBoundaryFaceOfVertex(d);

			Dart df = m_map.phi1(m_map.phi1(db)) ;

			//m_map.decCurrentLevel() ;

			typename PFP::VEC3 p(0) ;
			unsigned int degree = 0 ;
			Traversor2FV<typename PFP::MAP> trav(m_map, df) ;
			for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
			{
				++degree ;
				p += m_position[it] ;
			}
			p /= degree ;

			//m_map.incCurrentLevel() ;

			m_position[d] = p ;
		}
		else
		{
			Dart df = m_map.phi1(m_map.phi1(d)) ;
			//m_map.decCurrentLevel() ;
			//face points
			typename PFP::VEC3 C0 = Algo::Geometry::volumeCentroid<PFP>(m_map, df, m_position);
			typename PFP::VEC3 C1 = Algo::Geometry::volumeCentroid<PFP>(m_map, m_map.phi3(df), m_position);

			typename PFP::VEC3 A = Algo::Geometry::volumeCentroid<PFP>(m_map, m_map.phi3(df), m_position);

			typename PFP::VEC3 fp = C0 + A * 2 + C1;
			fp /= 4;

			//m_map.incCurrentLevel() ;

			m_position[d] = fp;
		}

		return false;
	}

};

template <typename PFP>
class MJ96VolumeVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	AttributeHandler<typename PFP::VEC3, VERTEX>& m_position ;

public:
	MJ96VolumeVertexFunctor(typename PFP::MAP& m, AttributeHandler<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi_1(m_map.phi2(m_map.phi1(d))) ;

		m_map.decCurrentLevel() ;
		//cell points : these points are the average of the
		//vertices of the lattice that bound the cell
		typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map,df,m_position);
		m_map.incCurrentLevel() ;

		m_position[d] = p;

		return false;
	}
};


} // namespace Multiresolution

} // namespace CGoGN


#endif /* __3MR_FUNCTORS_PRIMAL__ */
