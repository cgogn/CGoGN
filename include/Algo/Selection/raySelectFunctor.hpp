/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef RAYSELECTFUNCTOR_H_
#define RAYSELECTFUNCTOR_H_

#include "Geometry/distances.h"
#include "Geometry/intersection.h"

#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace Selection
{

template <typename PFP>
class FuncFaceInter: public FunctorMap<typename PFP::MAP>
{
	typedef typename PFP::MAP MAP;
	

protected:
	std::vector<Dart>& m_faces;
	std::vector<typename PFP::VEC3>& m_Ipoints ;
	const typename PFP::VEC3& m_A;
	const typename PFP::VEC3& m_AB;
	const typename PFP::TVEC3& m_positions;
public:
	/**
 	* @param map the map
	* @param f vector of selected darts
	* @param A first point of ray
	* @param AB direction of ray
	*/
	FuncFaceInter(MAP& map, const typename PFP::TVEC3& position, std::vector<Dart>& f, std::vector<typename PFP::VEC3>& ip, const typename PFP::VEC3& A, const typename PFP::VEC3& AB):
		FunctorMap<typename PFP::MAP>(map), m_faces(f), m_Ipoints(ip), m_A(A), m_AB(AB), m_positions(position)
	{}

	bool operator()(Dart d)
	{
		const typename PFP::VEC3& Ta = m_positions[d];//this->m_map.getVertexEmb(d)->getPosition();

		Dart dd  = this->m_map.phi1(d);
		Dart ddd = this->m_map.phi1(dd);
		bool notfound = true;
		do 
		{
			// get back position of triangle Ta,Tb,Tc
			const typename PFP::VEC3& Tb = m_positions[dd]; //this->m_map.getVertexEmb(dd)->getPosition();
			const typename PFP::VEC3& Tc = m_positions[ddd]; //this->m_map.getVertexEmb(ddd)->getPosition();
			typename PFP::VEC3 I;
			if (Geom::intersectionLineTriangle<typename PFP::VEC3>(m_A, m_AB, Ta, Tb, Tc, I))
			{
				m_faces.push_back(d);
				m_Ipoints.push_back(I) ;
				notfound = false;
			}
			// next triangle if we are in polygon
			dd = ddd;
			ddd = this->m_map.phi1(dd);
		} while ((ddd != d) && notfound);
		return false;
	}
};


template <typename PFP>
class FuncEdgeInter: public FunctorMap<typename PFP::MAP>
{
	typedef typename PFP::MAP MAP;
	

protected:
	std::vector<Dart>& m_edges;
	const typename PFP::VEC3& m_A;
	const typename PFP::VEC3& m_AB;
	float m_AB2;
	float m_distMax;
	const typename PFP::TVEC3& m_positions;
public:
	/**
 	* @param map the map
	* @param e vector of selected darts
	* @param A first point of ray
	* @param AB direction of ray
	* @param AB2 squared length of direction
	* @param dm2 max distance from ray squared
	*/
	FuncEdgeInter(MAP& map, const typename PFP::TVEC3& position, std::vector<Dart>& e, const typename PFP::VEC3& A, const typename PFP::VEC3& AB, typename PFP::REAL AB2, typename PFP::REAL dm2):
		FunctorMap<typename PFP::MAP>(map), m_edges(e), m_A(A), m_AB(AB), m_AB2(AB2), m_distMax(dm2), m_positions(position)
	{}

	bool operator()(Dart d)
	{
		// get back position of segment PQ
		const typename PFP::VEC3& P = m_positions[d];//this->m_map.getVertexEmb(d)->getPosition();
		Dart dd = this->m_map.phi1(d);
		const typename PFP::VEC3& Q = m_positions[dd];//this->m_map.getVertexEmb(dd)->getPosition();
		// the three distance to P, Q and (PQ) not used here
		float dist = Geom::squaredDistanceLine2Seg(m_A, m_AB, m_AB2, P, Q);

		if (dist < m_distMax)
		{
			m_edges.push_back(d);
		}
		return false;
	}
};


template <typename PFP>
class FuncVertexInter: public FunctorMap<typename PFP::MAP>
{
	typedef typename PFP::MAP MAP;

protected:
	std::vector<Dart>& m_vertices;
	const typename PFP::VEC3& m_A;
	const typename PFP::VEC3& m_AB;
	float m_AB2;
	float m_distMax;
	const typename PFP::TVEC3& m_positions;
public:
	/**
 	* @param map the map
	* @param v vector of selected darts
	* @param A first point of ray
	* @param AB direction of ray
	* @param AB2 squared length of direction
	* @param dm2 max distance from ray squared
	*/
	FuncVertexInter(MAP& map, const typename PFP::TVEC3& position, std::vector<Dart>& v, const typename PFP::VEC3& A, const typename PFP::VEC3& AB, typename PFP::REAL AB2, typename PFP::REAL dm2):
		FunctorMap<typename PFP::MAP>(map), m_vertices(v), m_A(A), m_AB(AB), m_AB2(AB2), m_distMax(dm2), m_positions(position)
	{}

	bool operator()(Dart d)
	{
		const typename PFP::VEC3& P = m_positions[d];//this->m_map.getVertexEmb(d)->getPosition();
		float dist = Geom::squaredDistanceLine2Point(m_A, m_AB, m_AB2, P);
		if (dist < m_distMax)
		{
			m_vertices.push_back(d);
		}
		return false;
	}
};

/**
 * Fonctor which store the dart that correspond to the subpart of face
 * that is intersected
 * Must be called in foreachface
 */
template <typename PFP>
class FuncDartMapD2Inter: public FunctorMap<typename PFP::MAP>
{
	typedef typename PFP::MAP MAP ;

protected:
	std::vector<Dart>& m_darts ;
	const typename PFP::VEC3& m_A ;
	const typename PFP::VEC3& m_AB ;
	const typename PFP::TVEC3& m_positions;
public:
	/**
 	* @param map the map
	* @param f vector of selected darts
	* @param A first point of ray
	* @param AB direction of ray
	*/
	FuncDartMapD2Inter(MAP& map, const typename PFP::TVEC3& position, std::vector<Dart>& f, const typename PFP::VEC3& A, const typename PFP::VEC3& AB):
		FunctorMap<typename PFP::MAP>(map), m_darts(f), m_A(A), m_AB(AB), m_positions(position)
	{}

	bool operator()(Dart d)
	{
		typename PFP::VEC3 center = Geometry::faceCentroid<PFP>(this->m_map, d, m_positions) ;
		bool notfound = true ;
		Dart face = d ;
		do
		{
			// get back position of triangle
			const typename PFP::VEC3& Tb = m_positions[face]; //this->m_map.getVertexEmb(face)->getPosition() ;
			const typename PFP::VEC3& Tc = m_positions[this->m_map.phi1(face)]; //this->m_map.getVertexEmb(this->m_map.phi1(face))->getPosition() ;
			typename PFP::VEC3 I;
			if (Geom::intersectionLineTriangle(m_A, m_AB, center, Tb, Tc, I))
			{
				m_darts.push_back(face) ;
				notfound = false ;
			}
			face = this->m_map.phi1(face) ;
		} while((face != d) && notfound) ;
		return false;
	}
};

/**
 * Order relation between Darts with respect to the depth of
 * their 0-embedding
 *
 * Used in a std::sort algorithm
 */
//template <typename PFP>
//class DartDepthOrdering
//{
//	typedef typename PFP::MAP MAP;
////	typedef typename PFP::EMB EMB;
//	
//
//protected:
//	MAP& m_map;
//	const typename PFP::VEC3& m_A;
//	typename PFP::TVEC3 m_positions;
//public:
//	DartDepthOrdering(MAP& map, unsigned int idPos, const typename PFP::VEC3& A):
//			m_map(map), m_A(A), m_positions(idPos,map) {}
//
//	bool operator()(const Dart& d, const Dart& e)
//	{
//		typename PFP::VEC3 P = m_positions[d]; //m_map.getVertexEmb(d)->getPosition();
//		P -= m_A;
//		typename PFP::REAL ld = P*P;// l^2
//
//		P = m_positions[e]; //m_map.getVertexEmb(e)->getPosition();
//		P -= m_A;
//		typename PFP::REAL le = P*P;// l^2
//
//		if (ld < le)
//			return true;
//		return false;
//	}
//};

/**
 * Order relation between Darts with respect to the distance of
 * their 0-embedding to the line given in parameter in the constructor
 *
 * Used in a std::sort algorithm
 */
//template <typename PFP>
//class DartRayDistanceOrdering
//{
//	typedef typename PFP::MAP MAP ;
////	typedef typename PFP::EMB EMB ;
//	
//
//protected:
//	MAP& m_map ;
//	const typename PFP::VEC3& m_A ;
//	const typename PFP::VEC3& m_AB ;
//	float m_AB2 ;
//	typename PFP::TVEC3 m_positions;
//
//public:
//	DartRayDistanceOrdering(MAP& map, unsigned int idPos, const typename PFP::VEC3& A, const typename PFP::VEC3& AB) :
//	m_map(map), m_A(A), m_AB(AB), m_positions(idPos,map)
//	{
//		m_AB2 = m_AB*m_AB ; // l^2
//	}
//
//	bool operator()(const Dart& d, const Dart& e)
//	{
//		typename PFP::VEC3 P = m_positions[d]; //m_map.getVertexEmb(d)->getPosition() ;
//		float ld = Geom::squaredDistanceLine2Point(m_A, m_AB, m_AB2, P) ;
//		P =  m_positions[e]; //m_map.getVertexEmb(e)->getPosition() ;
//		float le = Geom::squaredDistanceLine2Point(m_A, m_AB, m_AB2, P) ;
//
//		if (ld < le)
//			return true;
//		return false;
//	}
//};

/**
 * Order relation between Darts with respect to the depth
 * of their faces :
 * f1 < f2 if f1 contains the vertex whose 0-embedding is the closest
 *
 * Used in a std::sort algorithm
 */
//template <typename PFP>
//class FaceDepthOrdering
//{
//	typedef typename PFP::MAP MAP ;
//	
//
//protected:
//	MAP& m_map ;
//	const typename PFP::VEC3& m_A ;
//	typename PFP::TVEC3 m_positions;
//	std::vector<Dart>& m_faces;
//
//public:
//	FaceDepthOrdering(MAP& map, unsigned int idPos, const typename PFP::VEC3& A, std::vector<Dart>& faces) : m_map(map), m_A(A), m_positions(idPos,map), m_faces(faces) {}
//
////	bool operator()(const Dart& d, const Dart& e)
//	bool operator()(Dart d, Dart e)
//	{
//		for (typename std::vector<Dart>::iterator it = m_faces.begin(); it != m_faces.end(); ++it)
//		{
//			CGoGNout << "Dart: "<< (it - m_faces.begin())  <<" = " << (*it)->getLabel()<<CGoGNendl;
//		}
//
//		CGoGNout << "beg sort op:"<<CGoGNendl;
//		CGoGNout <<"  darts "<<d->getLabel() <<" ?? "<< e->getLabel()<<CGoGNendl;
//		typename PFP::VEC3 P = m_positions[d]; //m_map.getVertexEmb(d)->getPosition() ;
//		P -= m_A ;
//		typename PFP::REAL minDist = P*P ;
//		Dart closestD = d ;
//		Dart it = d ;
//		do
//		{
//			P = m_positions[it]; //m_map.getVertexEmb(it)->getPosition() ; 	// compute the minimum
//			P -= m_A ;										// distance among the vertices
//			typename PFP::REAL dist = P*P ;			// of the face of d
//			if(dist < minDist)
//				minDist = dist ;
//			it = m_map.phi1(it) ;
//		} while(it != d) ;
//		it = e ;
//		do
//		{
//			P = m_positions[it]; //m_map.getVertexEmb(it)->getPosition() ; 	// if one vertex of the face
//			P -= m_A ;										// of e is closer than the previous
//			typename PFP::REAL dist = P*P ;			// minimum, then e < d
//			if(dist < minDist)								// else, d < e
//				return false ;
//			it = m_map.phi1(it) ;
//		} while(it != e) ;
//
//		CGoGNout << "end sort op"<<CGoGNendl;
//
////		return rand()&1;
//
//		return true ;
//	}
//} ;

template <typename PFP>
bool distndartOrdering(const std::pair<typename PFP::REAL, Dart>& e1, const std::pair<typename PFP::REAL, Dart>& e2)
{
	return (e1.first < e2.first);
}

template <typename PFP>
bool distnintOrdering(const std::pair<typename PFP::REAL, unsigned int>& e1, const std::pair<typename PFP::REAL, unsigned int>& e2)
{
	return (e1.first < e2.first);
}


} //namespace Selection

} //namespace Algo

} //namespace CGoGN

#endif
