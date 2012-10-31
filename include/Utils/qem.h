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

/**
* @file qem.h
* Header file for Quadric Error Metric classes.
*/



#ifndef __QEM__
#define __QEM__

#include "Utils/os_spec.h" // allow compilation under windows
#include <cmath>

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/tensor.h"
#include "Geometry/plane_3d.h"

// Eigen includes
#include <Eigen/Dense>

#define CONST_VAL -5212368.54127 // random value

namespace CGoGN {

namespace Utils {

template <typename REAL>
class Quadric
{
public:
	static std::string CGoGNnameOfType() { return "Quadric" ; }

	typedef Geom::Vector<3,REAL> VEC3 ;
	typedef Geom::Vector<4,REAL> VEC4 ;
	typedef Geom::Matrix<4,4,double> MATRIX44 ; // double is crucial here !

	Quadric() ;
	Quadric(int i) ;

	Quadric(VEC3& p1, VEC3& p2, VEC3& p3) ;

	void zero() ;

	void operator= (const Quadric<REAL>& q) ;
	Quadric& operator+= (const Quadric<REAL>& q) ;
	Quadric& operator -= (const Quadric<REAL>& q) ;
	Quadric& operator *= (REAL v) ;
	Quadric& operator /= (REAL v) ;

	REAL operator() (const VEC4& v) const ;
	REAL operator() (const VEC3& v) const ;

	friend std::ostream& operator<<(std::ostream& out, const Quadric<REAL>& q)
	{
		out << q.A ;
		return out ;
	} ;

	friend std::istream& operator>>(std::istream& in, Quadric<REAL>& q)
	{
		in >> q.A ;
		return in ;
	} ;

	bool findOptimizedPos(VEC3& v) ;

private:
	MATRIX44 A ;

	REAL evaluate(const VEC4& v) const ;

	bool optimize(VEC4& v) const ;
} ;

template <typename REAL, unsigned int N>
class QuadricNd
{
public:
	static std::string CGoGNnameOfType() { return "QuadricNd" ; }

	typedef Geom::Vector<N,REAL> VECN ;
	typedef Geom::Vector<N+1,REAL> VECNp ;

	QuadricNd() ;

	QuadricNd(int i) ;

	QuadricNd(const VECN& p1_r, const VECN& p2_r, const VECN& p3_r) ;

	void zero() ;

	void operator= (const QuadricNd<REAL,N>& q) ;

	QuadricNd& operator+= (const QuadricNd<REAL,N>& q) ;

	QuadricNd& operator -= (const QuadricNd<REAL,N>& q) ;

	QuadricNd& operator *= (REAL v) ;

	QuadricNd& operator /= (REAL v) ;

	REAL operator() (const VECNp& v) const ;

	REAL operator() (const VECN& v) const ;

	friend std::ostream& operator<<(std::ostream& out, const QuadricNd<REAL,N>& q)
	{
		out << "(" << q.A << ", " << q.b << ", " << q.c << ")" ;
		return out ;
	} ;

	friend std::istream& operator>>(std::istream& in, QuadricNd<REAL,N>& q)
	{
		in >> q.A ;
		in >> q.b ;
		in >> q.c ;
		return in ;
	} ;

	bool findOptimizedVec(VECN& v) ;

private:
	// Double computation is crucial for stability
	Geom::Matrix<N,N,double> A ;
	Geom::Vector<N,double> b ;
	double c ;

	REAL evaluate(const VECN& v) const ;

	bool optimize(VECN& v) const ;
} ;

template <typename REAL>
class QuadricHF
{
public:
	static std::string CGoGNnameOfType() { return "QuadricHF" ; }

	typedef Geom::Vector<3,REAL> VEC3 ;

	QuadricHF() ;

	QuadricHF(int i) ;

	QuadricHF(const std::vector<VEC3>& v, const REAL& gamma, const REAL& alpha) ;

	QuadricHF(const Geom::Tensor3d* T, const REAL& gamma, const REAL& alpha) ;

	~QuadricHF() ;

	void zero() ;

	QuadricHF& operator= (const QuadricHF<REAL>& q) ;

	QuadricHF& operator+= (const QuadricHF<REAL>& q) ;
	QuadricHF& operator -= (const QuadricHF<REAL>& q) ;

	QuadricHF& operator *= (const REAL& v) ;

	QuadricHF& operator /= (const REAL& v) ;

	//	REAL operator() (const VECNp& v) const ;

	REAL operator() (const std::vector<VEC3>& coefs) const ;

	friend std::ostream& operator<<(std::ostream& out, const QuadricHF<REAL>& q)
	{
		// TODO out << "(" << q.m_A << ", " << q.m_b << ", " << q.m_c << ")" ;
		return out ;
	} ;

	friend std::istream& operator>>(std::istream& in, QuadricHF<REAL>& q)
	{
		// TODO
		//		in >> q.A ;
		//		in >> q.b ;
		//		in >> q.c ;
		return in ;
	} ;

	bool findOptimizedCoefs(std::vector<VEC3>& coefs) ;

private:
	// Double computation is crucial for stability
	Eigen::MatrixXd m_A ;
	Eigen::VectorXd m_b[3] ;
	double m_c[3] ;

	REAL evaluate(const std::vector<VEC3>& coefs) const ;

	bool optimize(std::vector<VEC3>& coefs) const ;

	Geom::Matrix33d rotateMatrix(const REAL& gamma) ;

	Geom::Tensor3d rotate(const Geom::Tensor3d& T, const Geom::Matrix33d& R) ;

	Eigen::MatrixXd buildIntegralMatrix_A(const REAL& alpha, unsigned int size) ;
	Eigen::MatrixXd buildIntegralMatrix_B(const REAL& alpha, unsigned int size) ;

	Eigen::MatrixXd buildIntegralMatrix_C(const REAL& alpha, unsigned int size) ;

	void fillTensor(Geom::Tensor3d& T) ;

	Geom::Tensor3d* tensorsFromCoefs(const std::vector<VEC3>& coefs) ;

	std::vector<VEC3> coefsFromTensors(Geom::Tensor3d* A) ;
} ;

} // Utils

} // CGOGN

#include "qem.hpp"

#endif
