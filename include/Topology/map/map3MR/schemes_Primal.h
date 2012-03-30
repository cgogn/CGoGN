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

#ifndef __3MR_SCHEMES_PRIMAL__
#define __3MR_SCHEMES_PRIMAL__

#include <cmath>
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Multiresolution
{

class MRScheme
{
public:
	MRScheme() {}
	virtual ~MRScheme() {}
	virtual void operator() () = 0 ;
} ;


/*********************************************************************************
 *          BSXW02 BASIC FUNCTIONS : polyhedral meshes
 *********************************************************************************/





/*********************************************************************************
 *                           SUBDIVISION SCHEMES
 *********************************************************************************/

/* Linear Interpolation
 *********************************************************************************/
template <typename PFP>
class LerpVertexSubdivisionScheme : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LerpVertexSubdivisionScheme(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = m_position[d];

			m_map.incCurrentLevel() ;
			m_position[d] = p ;
			m_map.decCurrentLevel() ;
		}
	}
} ;

template <typename PFP>
class LerpEdgeSubdivisionScheme : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LerpEdgeSubdivisionScheme(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
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
class LerpFaceSubdivisionScheme : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LerpFaceSubdivisionScheme(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
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
				Dart midF = m_map.phi2(m_map.phi1(d));
				m_position[midF] = p ;
			}
			m_map.decCurrentLevel() ;

		}
	}
} ;

template <typename PFP>
class LerpVolumeSubdivisionScheme : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LerpVolumeSubdivisionScheme(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!m_map.isTetrahedron(d))
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}

			m_map.decCurrentLevel() ;

		}
	}
} ;


/* Loop on Boundary Vertices and SHW04 on Insides Vertices
 *********************************************************************************/
template <typename PFP>
class LoopEvenSubdivisionScheme : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LoopEvenSubdivisionScheme(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);

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

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class LoopOddSubdivisionScheme : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	LoopOddSubdivisionScheme(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);
				typename PFP::VEC3 p = loopOddVertex<PFP>(m_map, m_position, db) ;

				m_map.incCurrentLevel() ;

				Dart oddV = m_map.phi2(db) ;
				m_position[oddV] = p ;

				m_map.decCurrentLevel() ;
			}
			else
			{
				typename PFP::VEC3 p = (m_position[d] + m_position[m_map.phi2(d)]) * typename PFP::REAL(0.5);

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi2(d) ;
				m_position[midV] = p ;

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class SHW04SubdivisionScheme : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	SHW04SubdivisionScheme(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(!m_map.isTetrahedron(d))
			{
				typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;

				m_map.decCurrentLevel() ;
			}
		}

		m_map.incCurrentLevel() ;
		TraversorV<typename PFP::MAP> travV(m_map) ;
		for (Dart d = travV.begin(); d != travV.end(); d = travV.next())
		{
			if(!m_map.isBoundaryVertex(d))
			{
				typename PFP::VEC3 p = typename PFP::VEC3(0);
				unsigned int degree = 0;

				Traversor3VW<typename PFP::MAP> travVW(m_map, d);
				for(Dart dit = travVW.begin() ; dit != travVW.end() ; dit = travVW.next())
				{
					p += SHW04Vertex<PFP>(m_map, m_position, dit);
					++degree;
				}

				p /= degree;

				m_position[d] = p ;
			}
		}
		m_map.decCurrentLevel() ;

	}
};

/* Catmull-clark on Boundary Vertices and MJ96 on Insides Vertices
 *********************************************************************************/
template <typename PFP>
class MJ96VertexSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	MJ96VertexSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);

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

				m_map.decCurrentLevel() ;

			}
			else
			{
				typename PFP::VEC3 P = m_position[d];

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

				m_position[d] = P;//vp;

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class MJ96EdgeSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	MJ96EdgeSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);

				Dart d1 = m_map.phi2(db) ;
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

				Dart midV = m_map.phi2(d);

				m_position[midV] = p1 + p2 + p3 + p4 + p5 + p6 ;

				m_map.decCurrentLevel() ;
			}
			else
			{
				//edge points
				typename PFP::VEC3 Cavg = typename PFP::VEC3(0);
				unsigned int degree = 0;
				Traversor3EW<typename PFP::MAP> travEW(m_map, d);
				for(Dart dit = travEW.begin() ; dit != travEW.end() ; dit = travEW.next())
				{
					Cavg += Algo::Geometry::volumeCentroid<PFP>(m_map, dit, m_position);
					++degree;
				}
				Cavg /= degree;

				typename PFP::VEC3 Aavg = typename PFP::VEC3(0);
				degree = 0;
				Traversor3EF<typename PFP::MAP> travEF(m_map, d);
				for(Dart dit = travEF.begin() ; dit != travEF.end() ; dit = travEF.next())
				{
					Aavg += Algo::Geometry::faceCentroid<PFP>(m_map, dit, m_position);
					++degree;
				}
				Aavg /= degree;

				Dart d2 = m_map.phi2(d);
				typename PFP::VEC3 M = (m_position[d] + m_position[d2]) * typename PFP::REAL(0.5);

				typename PFP::VEC3 ep = Cavg + Aavg * 2 + M * (degree - 3);
				ep /= degree;

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi2(d);

				m_position[midV] = ep;

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class MJ96FaceSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	MJ96FaceSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorF<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryFace(d))
			{
				Dart db = m_map.phi3(d);

				typename PFP::VEC3 p(0) ;
				unsigned int degree = 0 ;
				Traversor2FV<typename PFP::MAP> trav(m_map, db) ;
				for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
				{
					++degree ;
					p += m_position[it] ;
				}
				p /= degree ;

				m_map.incCurrentLevel() ;

				Dart df = m_map.phi1(m_map.phi1(d)) ;

				m_position[df] = p ;

				m_map.decCurrentLevel() ;
			}
			else
			{
				//face points
				typename PFP::VEC3 C0 = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);
				typename PFP::VEC3 C1 = Algo::Geometry::volumeCentroid<PFP>(m_map, m_map.phi3(d), m_position);

				typename PFP::VEC3 A = Algo::Geometry::faceCentroid<PFP>(m_map, m_map.phi3(d), m_position);

				typename PFP::VEC3 fp = C0 + A * 2 + C1;
				fp /= 4;

				m_map.incCurrentLevel() ;

				Dart df = m_map.phi1(m_map.phi1(d)) ;
				m_position[df] = fp;

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class MJ96VolumeSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	MJ96VolumeSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			//cell points : these points are the average of the
			//vertices of the lattice that bound the cell
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!m_map.isTetrahedron(d))
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}

			m_map.decCurrentLevel() ;

		}
	}
};

/* BSXW02 on Boundary Vertices and on Insides Vertices
 *********************************************************************************/

template <typename PFP>
class BSXW02VertexSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	BSXW02VertexSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			//cell points : these points are the average of the
			//vertices of the lattice that bound the cell
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!m_map.isTetrahedron(d))
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}

			m_map.decCurrentLevel() ;
		}
	}
};

template <typename PFP>
class BSXW02EdgeSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	BSXW02EdgeSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			//cell points : these points are the average of the
			//vertices of the lattice that bound the cell
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!m_map.isTetrahedron(d))
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}

			m_map.decCurrentLevel() ;
		}
	}
};

template <typename PFP>
class BSXW02FaceSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	BSXW02FaceSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			//cell points : these points are the average of the
			//vertices of the lattice that bound the cell
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!m_map.isTetrahedron(d))
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}

			m_map.decCurrentLevel() ;
		}
	}
};

template <typename PFP>
class BSXW02VolumeSubdivision : public MRScheme
{
protected:
	typename PFP::MAP& m_map ;
	typename PFP::TVEC3& m_position ;

public:
	BSXW02VolumeSubdivision(typename PFP::MAP& m, typename PFP::TVEC3& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			//cell points : these points are the average of the
			//vertices of the lattice that bound the cell
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!m_map.isTetrahedron(d))
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}

			m_map.decCurrentLevel() ;
		}
	}
};

} // namespace Multiresolution

} // namespace CGoGN


#endif /* __3MR_SCHEMES_PRIMAL__ */
