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

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/plane_3d.h"

template <typename REAL>
class Quadric
{
public:
	static std::string CGoGNnameOfType() { return "Quadric" ; }

	typedef CGoGN::Geom::Vector<3,REAL> VEC3 ;
	typedef CGoGN::Geom::Vector<4,REAL> VEC4 ;
	typedef CGoGN::Geom::Matrix<4,4,REAL> MATRIX44 ;

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
		CGoGN::Geom::Plane3D<REAL> p(p1, p2, p3) ;
		VEC3& n = p.normal() ;
		float a = n[0] ;
		float b = n[1] ;
		float c = n[2] ;
		float d = p.d() ;
		// set the matrix associated with this plane
		A.set(1) ;
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				if(i==0) A(i,j) *= a ;
				if(j==0) A(i,j) *= a ;
				if(i==1) A(i,j) *= b ;
				if(j==1) A(i,j) *= b ;
				if(i==2) A(i,j) *= c ;
				if(j==2) A(i,j) *= c ;
				if(i==3) A(i,j) *= d ;
				if(j==3) A(i,j) *= d ;
			}
		}
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

	float evaluate(const VEC4& v) const
	{
		VEC4 Av = A * v ;
		return v * Av ;
	}

	bool optimize(VEC4& v) const
	{
		if (isnan(A(0,0)))
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

#endif
