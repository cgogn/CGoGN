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

#ifndef __3MR_MCCRACKENJOY_MASK__
#define __3MR_MCCRACKENJOY_MASK__

#include <cmath>
#include "Algo/Geometry/centroid.h"
#include "Algo/ImplicitHierarchicalMesh/ihm3.h"

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

namespace Masks
{



/* MJ96 basic functions : polyhedral meshes
 *********************************************************************************/
template <typename PFP>
class MJ96VertexVertexFunctor : public FunctorType
{
protected:
	typename PFP::MAP& m_map ;
	//Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;
	Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;

public:
	//MJ96VertexVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	MJ96VertexVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
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
			m_map.decCurrentLevel() ;

			typename PFP::VEC3 P = m_position[d];

			//vertex points
			typename PFP::VEC3 Cavg = typename PFP::VEC3(0);
			unsigned int degree = 0;
			Traversor3VW<typename PFP::MAP> travVW(m_map, d);
			for(Dart dit = travVW.begin() ; dit != travVW.end() ; dit = travVW.next())
			{
				Cavg += Algo::Surface::Geometry::volumeCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map, dit, m_position);
				++degree;
			}
			Cavg /= degree;

			typename PFP::VEC3 Aavg = typename PFP::VEC3(0);
			degree = 0;
			Traversor3VF<typename PFP::MAP> travVF(m_map, d);
			for(Dart dit = travVF.begin() ; dit != travVF.end() ; dit = travVF.next())
			{
				Aavg += Algo::Surface::Geometry::faceCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map, dit, m_position);
				++degree;
			}
			Aavg /= degree;

			typename PFP::VEC3 Mavg = typename PFP::VEC3(0);
			degree = 0;
			Traversor3VE<typename PFP::MAP> travVE(m_map, d);
			for(Dart dit = travVE.begin() ; dit != travVE.end() ; dit = travVE.next())
			{
				Dart d2 = m_map.phi2(dit);
				Mavg += (m_position[dit] + m_position[d2]) * typename PFP::REAL(0.5);
				++degree;
			}
			Mavg /= degree;

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
	//Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;
	Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;

public:
	//MJ96EdgeVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	MJ96EdgeVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		if(m_map.isBoundaryEdge(d))
		{
			Dart db = m_map.findBoundaryFaceOfEdge(d);

			Dart d1 = m_map.phi2(db) ;

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
		}
		else
		{
			Dart d2 = m_map.phi2(d);

			m_map.decCurrentLevel() ;

			//edge points
			typename PFP::VEC3 Cavg = typename PFP::VEC3(0);
			unsigned int degree = 0;
			Traversor3EW<typename PFP::MAP> travEW(m_map, d2);
			for(Dart dit = travEW.begin() ; dit != travEW.end() ; dit = travEW.next())
			{
				Cavg += Algo::Surface::Geometry::volumeCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map, dit, m_position);
				++degree;
			}
			Cavg /= degree;

			typename PFP::VEC3 Aavg = typename PFP::VEC3(0);
			degree = 0;
			Traversor3EF<typename PFP::MAP> travEF(m_map, d2);
			for(Dart dit = travEF.begin() ; dit != travEF.end() ; dit = travEF.next())
			{
				Aavg += Algo::Surface::Geometry::faceCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map, dit, m_position);
				++degree;
			}
			Aavg /= degree;

			Dart d22 = m_map.phi2(d2);
			typename PFP::VEC3 M = (m_position[d2] + m_position[d22]) * typename PFP::REAL(0.5);

			typename PFP::VEC3 ep = Cavg + Aavg * 2 + M * (degree - 3);
			ep /= degree;

			m_map.incCurrentLevel() ;

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
	//Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;
	Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;

public:
	//MJ96FaceVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	MJ96FaceVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		if(m_map.isBoundaryFace(d))
		{
			Dart db = m_map.phi_1(m_map.phi3(d));

			m_map.decCurrentLevel() ;

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

			m_position[d] = p ;
		}
		else
		{
			Dart df = m_map.phi1(m_map.phi1(d)) ;

			m_map.decCurrentLevel() ;

			//face points
			typename PFP::VEC3 C0 = Algo::Surface::Geometry::volumeCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map, df, m_position);
			typename PFP::VEC3 C1 = Algo::Surface::Geometry::volumeCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map, m_map.phi3(df), m_position);

			typename PFP::VEC3 A = Algo::Surface::Geometry::faceCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map, df, m_position);

			typename PFP::VEC3 fp = C0 + A * 2 + C1;
			fp /= 4;

			m_map.incCurrentLevel() ;

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
	//Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;
	Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& m_position ;

public:
	//MJ96VolumeVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	MJ96VolumeVertexFunctor(typename PFP::MAP& m, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>& p) : m_map(m), m_position(p)
	{}

	bool operator() (Dart d)
	{
		Dart df = m_map.phi_1(m_map.phi2(m_map.phi1(d))) ;

		m_map.decCurrentLevel() ;

		//cell points : these points are the average of the
		//vertices of the lattice that bound the cell
		typename PFP::VEC3 p = Algo::Surface::Geometry::volumeCentroidGen<PFP, Algo::Volume::IHM::AttributeHandler_IHM<typename PFP::VEC3, VERTEX>, typename PFP::VEC3>(m_map,df,m_position);

		m_map.incCurrentLevel() ;

		m_position[d] = p;

		return false;
	}
};




} // namespace Masks

} // namespace Primal

} // namespace MR

} // namespace Volume

} // namespace Algo

} // namespace CGoGN


#endif /* __3MR_FUNCTORS_PRIMAL__ */
