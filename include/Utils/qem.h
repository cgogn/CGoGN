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

/*! \file qem.h
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

/*! \namespace CGoGN
 * \brief namespace for all elements composing the CGoGN library
 */
namespace CGoGN {

/*! \namespace Utils
 * \brief namespace for tool classes used by CGoGN and its applications
 */
namespace Utils {

/*! \class Quadric
 *
 * \brief Quadric for computing the quadric error metric (QEM)
 * introduced by Garland and Heckbert in 1997.
 */
template <typename REAL>
class Quadric
{
public:
	/**
	 * \return name of the CGoGN type
	 */
	static std::string CGoGNnameOfType()
	{
		return "Quadric" ;
	}

	typedef Geom::Vector<3,REAL> VEC3 ;
	typedef Geom::Vector<4,REAL> VEC4 ;
	typedef Geom::Matrix<4,4,double> MATRIX44 ; // double is crucial here !

	/*!
	 * \brief Default constructor
	 *
	 * Initializes empty members
	 */
	Quadric() ;

	//Quadric(int i) ;

	/*!
	 * \brief Constructor building a quadric given three points (defining a plane);
	 *
	 * \param p1 first point
	 * \param p2 second point
	 * \param p3 third point
	 */
	Quadric(VEC3& p1, VEC3& p2, VEC3& p3) ;

	/*!
	 * \brief set members to zero
	 */
	void zero() ;

	/*!
	 * \brief affectation operator (by copy)
	 *
	 * \param q the Quadric to copy
	 */
	void operator= (const Quadric<REAL>& q) ;

	/*!
	 * \brief sum of Quadric operator
	 *
	 * \param q the Quadric to sum
	 */
	Quadric& operator+= (const Quadric<REAL>& q) ;

	/*!
	 * \brief substract of Quadric operator
	 *
	 * \param q the Quadric to substract
	 */
	Quadric& operator -= (const Quadric<REAL>& q) ;

	/*!
	 * \brief scalar product operator
	 *
	 * \param v the scalar to multiply the Quadric with
	 */
	Quadric& operator *= (const REAL& v) ;

	/*!
	 * \brief scalar division operator
	 *
	 * \param v the scalar to divide the Quadric with
	 */
	Quadric& operator /= (const REAL& v) ;

	/*!
	 * `brief error evaluation operator
	 *
	 * \param v a point expressed in homogeneous coordinates in space
	 *
	 * \return the error
	 */
	REAL operator() (const VEC4& v) const ;

	/*!
	 * `brief error evaluation operator
	 *
	 * \param v a point in space
	 *
	 * \param the error
	 */
	REAL operator() (const VEC3& v) const ;

	/*!
	 * \brief Write to stream operator
	 *
	 * \param out the stream to write to
	 * \param q the Quadric to write in the stream
	 *
	 * \return the stream reference
	 */
	friend std::ostream& operator<<(std::ostream& out, const Quadric<REAL>& q)
	{
		out << q.A ;
		return out ;
	} ;

	/*!
	 * \brief Read from stream operator
	 *
	 * \param int the stream to read from
	 * \param q the Quadric to write the data that has been read in
	 *
	 * \return the stream reference
	 */
	friend std::istream& operator>>(std::istream& in, Quadric<REAL>& q)
	{
		in >> q.A ;
		return in ;
	} ;

	/*!
	 * \brief Method to deduce a position in space that minimizes the error
	 *
	 * \param v the ideal position (if it can be computed)
	 *
	 * \return true if the ideal position has been computed correctly
	 */
	bool findOptimizedPos(VEC3& v) ;

private:
	MATRIX44 A ; /*!< The Quadric matrix */

	/*!
	 * \brief method to evaluate the error at a given point in space (homogeneous coordinates)
	 *
	 * \param v the given point
	 *
	 * \return the error
	 */
	REAL evaluate(const VEC4& v) const ;

	/*!
	 * \brief method to deduce an optimal position in space (homogeneous coordinates)
	 * w.r.t. the current Quadric.
	 *
	 * \param v will contain the optimal position (if it can be computed)
	 *
	 * \return true if an optimal position was correctly computed
	 */
	bool optimize(VEC4& v) const ;
} ;

/*! \class QuadricNd
 * \brief extension of Quadric (which is 3D) to nD points.
 * This was published by Garland and Heckbert in 1998 and is meant to define a quadric
 * for a nD-space which contains geometry (3D) + other attributes like color (3D),
 * normals (3D) or texture coordinates (2D) for instance.
 */
template <typename REAL, unsigned int N>
class QuadricNd
{
public:
	static std::string CGoGNnameOfType()
	{
		return "QuadricNd" ;
	}

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
