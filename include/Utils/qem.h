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

#ifndef __QEM__
#define __QEM__

#include "Utils/os_spec.h" // allow compilation under windows
#include <cmath>

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/plane_3d.h"

// GSL includes
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

namespace CGoGN {

//namespace Utils {

template <typename REAL>
class Quadric
{
public:
	static std::string CGoGNnameOfType() { return "Quadric" ; }

	typedef Geom::Vector<3,REAL> VEC3 ;
	typedef Geom::Vector<4,REAL> VEC4 ;
	typedef Geom::Matrix<4,4,double> MATRIX44 ; // double is crucial here !

	Quadric()
	{
		A.zero() ;
	}

	Quadric(int i)
	{
		A.zero() ;
	}

	Quadric(VEC3& p1, VEC3& p2, VEC3& p3)
	{
		// compute the equation of the plane of the 3 points
		Geom::Plane3D<REAL> plane(p1, p2, p3) ;
		const VEC3& n = plane.normal() ;

		Geom::Vector<4,double> p = Geom::Vector<4,double>(n[0],n[1],n[2],plane.d()) ;
		A = Geom::transposed_vectors_mult(p,p) ;
	}

	void zero()
	{
		A.zero() ;
	}

	void operator= (const Quadric<REAL>& q)
	{
		A = q.A ;
	}
	Quadric& operator+= (const Quadric<REAL>& q)
	{
		A += q.A ;
		return *this ;
	}
	Quadric& operator -= (const Quadric<REAL>& q)
	{
		A -= q.A ;
		return *this ;
	}
	Quadric& operator *= (REAL v)
	{
		A *= v ;
		return *this ;
	}
	Quadric& operator /= (REAL v)
	{
		A /= v ;
		return *this ;
	}

	REAL operator() (const VEC4& v) const
	{
		return evaluate(v) ;
	}

	REAL operator() (const VEC3& v) const
	{
		VEC4 hv(v[0], v[1], v[2], 1.0f) ;
		return evaluate(hv) ;
	}

	friend std::ostream& operator<<(std::ostream& out, const Quadric<REAL>& q)
	{
		out << q.A ;
		return out ;
	}

	friend std::istream& operator>>(std::istream& in, Quadric<REAL>& q)
	{
		in >> q.A ;
		return in ;
	}

	bool findOptimizedPos(VEC3& v)
	{
		VEC4 hv ;
		bool b = optimize(hv) ;
		if(b)
		{
			v[0] = hv[0] ;
			v[1] = hv[1] ;
			v[2] = hv[2] ;
		}
		return b ;
	}

private:
	MATRIX44 A ;

	REAL evaluate(const VEC4& v) const
	{
		// Double computation is crucial for stability
		Geom::Vector<4, double> Av = A * v ;
		return v * Av ;
	}

	bool optimize(VEC4& v) const
	{
		if (std::isnan(A(0,0)))
			return false ;

		MATRIX44 A2(A) ;
		for(int i = 0; i < 3; ++i)
			A2(3,i) = 0.0f ;
		A2(3,3) = 1.0f ;

		MATRIX44 Ainv ;
		REAL det = A2.invert(Ainv) ;

		if(det > -1e-6 && det < 1e-6)
			return false ;

		VEC4 right(0,0,0,1) ;
		v = Ainv * right ;

		return true;
	}
} ;

template <typename REAL, unsigned int N>
class QuadricNd
{
public:
	static std::string CGoGNnameOfType() { return "QuadricNd" ; }

	typedef Geom::Vector<N,REAL> VECN ;
	typedef Geom::Vector<N+1,REAL> VECNp ;

	QuadricNd()
	{
		A.zero() ;
		b.zero() ;
		c = 0 ;
	}

	QuadricNd(int i)
	{
		A.zero() ;
		b.zero() ;
		c = 0 ;
	}

	QuadricNd(const VECN& p1_r, const VECN& p2_r, const VECN& p3_r)
	{
		const Geom::Vector<N,double>& p1 = p1_r ;
		const Geom::Vector<N,double>& p2 = p2_r ;
		const Geom::Vector<N,double>& p3 = p3_r ;

		Geom::Vector<N,double> e1 = p2 - p1 ; 						e1.normalize() ;
		Geom::Vector<N,double> e2 = (p3 - p1) - (e1*(p3-p1))*e1 ; 	e2.normalize() ;

		A.identity() ;
		A -= Geom::transposed_vectors_mult(e1,e1) + Geom::transposed_vectors_mult(e2,e2) ;

		b = (p1*e1)*e1 + (p1*e2)*e2 - p1 ;

		c = p1*p1 - pow((p1*e1),2) - pow((p1*e2),2) ;
	}

	void zero()
	{
		A.zero() ;
		b.zero() ;
		c = 0 ;
	}

	void operator= (const QuadricNd<REAL,N>& q)
	{
		A = q.A ;
		b = q.b ;
		c = q.c ;
	}
	QuadricNd& operator+= (const QuadricNd<REAL,N>& q)
	{
		A += q.A ;
		b += q.b ;
		c += q.c ;
		return *this ;
	}

	QuadricNd& operator -= (const QuadricNd<REAL,N>& q)
	{
		A -= q.A ;
		b -= q.b ;
		c -= q.c ;
		return *this ;
	}

	QuadricNd& operator *= (REAL v)
	{
		A *= v ;
		b *= v ;
		c *= v ;
		return *this ;
	}
	QuadricNd& operator /= (REAL v)
	{
		A /= v ;
		b /= v ;
		c /= v ;
		return *this ;
	}

	REAL operator() (const VECNp& v) const
	{
		VECN hv ;
		for (unsigned int i = 0 ; i < N ; ++i)
			hv[i] = v[i] ;

		return evaluate(v) ;
	}

	REAL operator() (const VECN& v) const
	{
		return evaluate(v) ;
	}

	friend std::ostream& operator<<(std::ostream& out, const QuadricNd<REAL,N>& q)
	{
		out << "(" << q.A << ", " << q.b << ", " << q.c << ")" ;
		return out ;
	}

	friend std::istream& operator>>(std::istream& in, QuadricNd<REAL,N>& q)
	{
		in >> q.A ;
		in >> q.b ;
		in >> q.c ;
		return in ;
	}

	bool findOptimizedVec(VECN& v)
	{
		return optimize(v) ;
	}

private:
	// Double computation is crucial for stability
	Geom::Matrix<N,N,double> A ;
	Geom::Vector<N,double> b ;
	double c ;

	REAL evaluate(const VECN& v) const
	{
		Geom::Vector<N, double> v_d = v ;
		return v_d*A*v_d + 2.*(b*v_d) + c ;
	}

	bool optimize(VECN& v) const
	{
		if (std::isnan(A(0,0)))
			return false ;

		Geom::Matrix<N,N,double> Ainv ;
		double det = A.invert(Ainv) ;

		if(det > -1e-6 && det < 1e-6)
			return false ;

		v.zero() ;
		v -= Ainv * b ;

		return true ;
	}
} ;

template <typename REAL>
class QuadricHF
{
public:
	static std::string CGoGNnameOfType() { return "QuadricHF" ; }

	typedef Geom::Vector<3,REAL> VEC3 ;

	QuadricHF()
	{
		m_A = gsl_matrix_alloc(0,0) ;
		m_b = gsl_vector_alloc(0) ;
		m_c = 0 ;
	}

	QuadricHF(int i):
		m_A(NULL),
		m_b(NULL),
		m_c(0)
	{
		m_A = gsl_matrix_calloc(i, i) ;
		m_b = gsl_vector_calloc(i) ;
		m_c = 0 ;
	}

	QuadricHF(const std::vector<VEC3>& coefs, const REAL& gamma, const REAL& alpha)
	{
		m_A = gsl_matrix_calloc(coefs.size(), coefs.size()) ;
		m_b = gsl_vector_calloc(coefs.size()) ;
		m_c = 0 ;
		// TODO : build A, b and c
		assert(false || !"todo") ;
	}

	~QuadricHF()
	{
		gsl_matrix_free(m_A) ;
		gsl_vector_free(m_b) ;
	}

	void zero()
	{
		gsl_matrix_set_zero(m_A) ;
		gsl_vector_set_zero(m_b) ;
		m_c = 0 ;
	}

	QuadricHF& operator= (const QuadricHF<REAL>& q)
	{
		m_A = gsl_matrix_alloc(q.m_A->size1,q.m_A->size1) ;
		gsl_matrix_memcpy(m_A,q.m_A) ;

		m_b = gsl_vector_alloc(m_b->size) ;
		gsl_vector_memcpy(m_b,q.m_b) ;

		m_c = q.m_c ;

		return *this ;
	}

	QuadricHF& operator+= (const QuadricHF<REAL>& q)
	{
		assert(((m_A->size1 == q.m_A->size1) && (m_A->size2 == q.m_A->size2) && m_b->size == q.m_b->size) || !"Incompatible add of matrices") ;
		if ((m_A->size1 == q.m_A->size1) && (m_A->size2 == q.m_A->size2) && (m_b->size == q.m_b->size))
			return *this ;

		gsl_matrix_add(m_A,q.m_A) ;
		gsl_vector_add(m_b,q.m_b) ;
		m_c += q.m_c ;

		return *this ;
	}

	QuadricHF& operator -= (const QuadricHF<REAL>& q)
	{
		assert(((m_A->size1 == q.m_A->size1) && (m_A->size2 == q.m_A->size2) && m_b->size == q.m_b->size) || !"Incompatible add of matrices") ;
		if ((m_A->size1 == q.m_A->size1) && (m_A->size2 == q.m_A->size2) && (m_b->size == q.m_b->size))
			return *this ;

		gsl_matrix_sub(m_A,q.m_A) ;
		gsl_vector_sub(m_b,q.m_b) ;
		m_c -= q.m_c ;

		return *this ;
	}

	QuadricHF& operator *= (const REAL& v)
	{
		gsl_matrix_scale(m_A,v) ;
		gsl_vector_scale(m_b,v) ;
		m_c *= v ;

		return *this ;
	}
	QuadricHF& operator /= (const REAL& v)
	{
		const REAL& inv = 1. / v ;

		(*this) *= inv ;

		return *this ;
	}

//	REAL operator() (const VECNp& v) const
//	{
//		VECN hv ;
//		for (unsigned int i = 0 ; i < 3 ; ++i)
//			hv[i] = v[i] ;
//
//		return evaluate(v) ;
//	}

	REAL operator() (const std::vector<VEC3>& coefs) const
	{
		return evaluate(coefs) ;
	}

	friend std::ostream& operator<<(std::ostream& out, const QuadricHF<REAL>& q)
	{
		// TODO out << "(" << q.m_A << ", " << q.m_b << ", " << q.m_c << ")" ;
		return out ;
	}

	friend std::istream& operator>>(std::istream& in, QuadricHF<REAL>& q)
	{
		// TODO
//		in >> q.A ;
//		in >> q.b ;
//		in >> q.c ;
		return in ;
	}

	bool findOptimizedCoefs(std::vector<VEC3>& coefs)
	{
		return optimize(coefs) ;
	}

private:
	// Double computation is crucial for stability
	gsl_matrix *m_A ;
	gsl_vector *m_b ;
	double m_c ;

	REAL evaluate(const std::vector<VEC3>& coefs) const
	{
		// TODO
//		Geom::Vector<3, double> v_d = v ;
//		return v_d*A*v_d + 2.*(b*v_d) + c ;
	}

	bool optimize(std::vector<VEC3>& coefs) const
	{
		coefs.reserve(m_b->size) ;

		// TODO
/*		if (std::isnan(A(0,0)))
			return false ;

		Geom::Matrix<3,3,double> Ainv ;
		double det = A.invert(Ainv) ;

		if(det > -1e-6 && det < 1e-6)
			return false ;

		v.zero() ;
		v -= Ainv * b ;
*/
		return true ;
	}
} ;

//} // Utils

} // CGOGN

#endif
