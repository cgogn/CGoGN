/*******************************************************************************
 * CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
 * version 0.1                                                                  *
 * Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
 * Web site: http://cgogn.u-strasbg.fr/                                         *
 * Contact information: cgogn@unistra.fr                                        *
 *                                                                              *
 *******************************************************************************/

#ifndef __VECTOR_GEN__
#define __VECTOR_GEN__

#include <cassert>
#include <iostream>
#include <math.h>
#include <string.h>

#include "Utils/static_assert.h"
#include "Utils/nameTypes.h"

namespace CGoGN
{

namespace Geom
{

/*
 * Class for the representation of vectors
 */
template <unsigned int DIM, typename T>
class Vector
{
public:
	typedef T DATA_TYPE ;
	enum { DIMENSION = DIM } ;

	static std::string CGoGNnameOfType() ;

	/**********************************************/
	/*                CONSTRUCTORS                */
	/**********************************************/

	Vector() ;

	Vector(const Vector<DIM,T>& v) ;

	Vector(T x, T y) ;

	Vector(T x, T y, T z) ;

	Vector(T x, T y, T z, T w) ;

	/**
	 * constructor that initialize all component to a given value
	 * @param x the value to assign to all component
	 */
	Vector(T x) ;

	void set(T a) ;

	void zero() ;

	/**********************************************/
	/*                 ACCESSORS                  */
	/**********************************************/

	T& operator[](unsigned int index) ;

	const T& operator[](unsigned int index) const ;

	unsigned int dimension() const ;

	T* data() ;

	const T* data() const ;

	/**********************************************/
	/*         ARITHMETIC SELF-OPERATORS          */
	/**********************************************/

	Vector<DIM,T>& operator+=(const Vector<DIM,T>& v) ;

	Vector<DIM,T>& operator-=(const Vector<DIM,T>& v) ;

	Vector<DIM,T> operator*=(T a) ;

	Vector<DIM,T> operator/=(T a) ;

	/**********************************************/
	/*            ARITHMETIC OPERATORS            */
	/**********************************************/

	Vector<DIM,T> operator+(const Vector<DIM,T>& v) const ;

	Vector<DIM,T> operator-(const Vector<DIM,T>& v) const ;

	Vector<DIM,T> operator*(T a) const ;

	Vector<DIM,T> operator/(T a) const ;

	/**********************************************/
	/*             UTILITY FUNCTIONS              */
	/**********************************************/

	T norm2() const ;

	double norm() const ;

	double normalize() ;

	// dot product
	T operator*(const Vector<DIM,T> v) const ;

	// cross product
	Vector<DIM,T> operator^(const Vector<DIM,T> v) const ;

	// Equal
	bool operator==(const Vector<DIM,T>& v) const ;

	// Different
	bool operator!=(const Vector<DIM,T>& v) const ;

	bool hasNan() const ;

	/**********************************************/
	/*             STREAM OPERATORS               */
	/**********************************************/

	template <unsigned int DD, typename TT>
	friend std::ostream& operator<< (std::ostream& out, const Vector<DD,TT>& v) ;

	template <unsigned int DD, typename TT>
	friend std::istream& operator>> (std::istream& in, Vector<DD,TT>& v) ;

private:
	T m_data[DIM] ;
};


template <unsigned int DIM, typename T>
Vector<DIM,T> operator*(T a, const Vector<DIM,T>& v) ;

template <unsigned int DIM, typename T>
Vector<DIM,T> operator/(T a, const Vector<DIM,T>& v) ;

// returns the signed volume of the parallelepiped spanned by vectors v1, v2 and v3
template <unsigned int DIM, typename T>
T tripleProduct(const Vector<DIM,T>& v1, const Vector<DIM,T>& v2, const Vector<DIM,T>& v3) ;

// returns a spherical interpolation of two vectors considering parameter t ((0 <= t <= 1) => result between v1 and v2)
template <unsigned int DIM, typename T>
Vector<DIM,T> slerp(const Vector<DIM,T> &v1, const Vector<DIM,T> &v2, const T &t) ;

/**********************************************/
/*           SOME USEFUL TYPEDEFS             */
/**********************************************/

typedef Vector<2,float> Vec2f ;
typedef Vector<2,double> Vec2d ;
typedef Vector<2,unsigned int> Vec2ui ;
typedef Vector<2,int> Vec2i ;
typedef Vector<2,unsigned char> Vec2uc ;

typedef Vector<3,float> Vec3f ;
typedef Vector<3,double> Vec3d ;
typedef Vector<3,unsigned int> Vec3ui ;
typedef Vector<3,int> Vec3i ;
typedef Vector<3,unsigned char> Vec3uc ;

typedef Vector<4,float> Vec4f ;
typedef Vector<4,double> Vec4d ;
typedef Vector<4,unsigned int> Vec4ui ;
typedef Vector<4,int> Vec4i ;
typedef Vector<4,unsigned char> Vec4uc ;

}

}

#include "vector_gen.hpp"

#endif
