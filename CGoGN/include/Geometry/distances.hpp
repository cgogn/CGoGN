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

namespace CGoGN
{

namespace Geom
{

template <typename VEC3>
inline typename VEC3::DATA_TYPE squaredDistancePoint2TrianglePlane(const VEC3& P, const VEC3& A, const VEC3& B, const VEC3& C)
{
    VEC3 vAB = B - A ;
    VEC3 vAC = C - A ;
	VEC3 N = vAB ^ vAC ;

	typename VEC3::DATA_TYPE x = (P * N) - (A * N) ;
	return (x * x) / (N.norm2());
}

template <typename VEC3>
inline typename VEC3::DATA_TYPE distancePoint2TrianglePlane(const VEC3& P, const VEC3& A, const VEC3& B, const VEC3& C)
{
	Geom::Plane3D<typename VEC3::DATA_TYPE> plane(A,B,C) ;
	return plane.distance(P) ;
}

// implemented following :
// Distance Between Point and Triangle in 3D
// http://www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf
template <typename VEC3>
typename VEC3::DATA_TYPE squaredDistancePoint2Triangle(const VEC3& P, const VEC3& A, const VEC3& B, const VEC3& C)
{
	VEC3 D = A - P ;
	VEC3 E0 = B - A ;
	VEC3 E1 = C - A ;

	double a = E0.norm2() ;
	double b = E0 * E1 ;
	double c = E1.norm2() ;
	double d = E0 * D ;
	double e = E1 * D ;
	double f = D.norm2() ;

	double det = fabs(a*c - b*b);
	double s = b*e - c*d;
	double t = b*d - a*e;

	double sqrDistance;

	if (s + t <= det)
	{
		if (s < 0.0f)
		{
			if (t < 0.0f)  // region 4
			{
				if (d < 0.0)
				{
					t = 0.0;
					if (-d >= a) { s = 1.0; sqrDistance = a + 2.0*d + f; }
					else { s = -d/a; sqrDistance = d*s + f; }
				}
				else
				{
					s = 0.0;
					if (e >= 0.0) { t = 0.0; sqrDistance = f; }
					else if (-e >= c) { t = 1.0; sqrDistance = c + 2.0*e + f; }
					else { t = -e/c; sqrDistance = e*t + f; }
				}
			}
			else  // region 3
			{
				s = 0.0;
				if (e >= 0.0) { t = 0.0; sqrDistance = f; }
				else if (-e >= c) { t = 1.0; sqrDistance = c + 2.0*e + f; }
				else { t = -e/c; sqrDistance = e*t + f; }
			}
		}
		else if (t < 0.0)  // region 5
		{
			t = 0.0;
			if (d >= 0.0) { s = 0.0; sqrDistance = f; }
			else if (-d >= a) { s = 1.0; sqrDistance = a + 2.0*d + f; }
			else { s = -d/a; sqrDistance = d*s + f; }
		}
		else  // region 0
		{
			// minimum at interior point
			double invDet = 1.0 / det;
			s *= invDet;
			t *= invDet;
			sqrDistance = s * (a*s + b*t + 2.0*d) + t * (b*s + c*t + 2.0*e) + f;
		}
	}
	else
	{
		double tmp0, tmp1, numer, denom;

		if (s < 0.0f)  // region 2
		{
			tmp0 = b + d;
			tmp1 = c + e;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a - 2.0*b + c;
				if (numer >= denom) { s = 1.0; t = 0.0; sqrDistance = a + 2.0*d + f; }
				else { s = numer/denom; t = 1.0 - s; sqrDistance = s * (a*s + b*t + 2.0*d) + t * (b*s + c*t + 2.0*e) + f; }
			}
			else
			{
				s = 0.0;
				if (tmp1 <= 0.0) { t = 1.0; sqrDistance = c + 2.0*e + f; }
				else if (e >= 0.0) { t = 0.0; sqrDistance = f; }
				else { t = -e/c; sqrDistance = e*t + f; }
			}
		}
		else if (t < 0.0f)  // region 6
		{
			tmp0 = b + e;
			tmp1 = a + d;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a - 2.0*b + c;
				if (numer >= denom) { t = 1.0; s = 0.0; sqrDistance = c + 2.0*e + f; }
				else { t = numer/denom; s = 1.0 - t; sqrDistance = s * (a*s + b*t + 2.0*d) + t * (b*s + c*t + 2.0*e) + f; }
			}
			else
			{
				t = 0.0;
				if (tmp1 <= 0.0) { s = 1.0; sqrDistance = a + 2.0*d + f; }
				else if (d >= 0.0) { s = 0.0; sqrDistance = f; }
				else { s = -d/a; sqrDistance = d*s + f; }
			}
		}
		else  // region 1
		{
			numer = c + e - b - d;
			if (numer <= 0.0) { s = 0.0; t = 1.0; sqrDistance = c + 2.0*e + f; }
			else
			{
				denom = a - 2.0*b + c;
				if (numer >= denom) { s = 1.0; t = 0.0; sqrDistance = a + 2.0*d + f; }
				else { s = numer/denom; t = 1.0 - s; sqrDistance = s * (a*s + b*t + 2.0*d) + t * (b*s + c*t + 2.0*e) + f; }
			}
		}
	}

//	sqrDistance = s * (a*s + b*t + 2.0*d) + t * (b*s + c*t + 2.0*e) + f;

	// account for numerical round-off error
	if (sqrDistance < 0.0)
		sqrDistance = 0.0;

	return sqrDistance;
}

template <typename VEC3>
void closestPointInTriangle(const VEC3& P, const VEC3& A, const VEC3& B, const VEC3& C, double& u, double& v, double& w)
{
	VEC3 D = A - P ;
	VEC3 E0 = B - A ;
	VEC3 E1 = C - A ;

	double a = E0.norm2() ;
	double b = E0 * E1 ;
	double c = E1.norm2() ;
	double d = E0 * D ;
	double e = E1 * D ;
	double f = D.norm2() ;

	double det = fabs(a*c - b*b);
	double s = b*e - c*d;
	double t = b*d - a*e;

	if (s + t <= det)
	{
		if (s < 0.0f)
		{
			if (t < 0.0f)  // region 4
			{
				if (d < 0.0)
				{
					t = 0.0;
					if (-d >= a) { s = 1.0; }
					else { s = -d/a; }
				}
				else
				{
					s = 0.0;
					if (e >= 0.0) { t = 0.0; }
					else if (-e >= c) { t = 1.0; }
					else { t = -e/c; }
				}
			}
			else  // region 3
			{
				s = 0.0;
				if (e >= 0.0) { t = 0.0; }
				else if (-e >= c) { t = 1.0; }
				else { t = -e/c; }
			}
		}
		else if (t < 0.0)  // region 5
		{
			t = 0.0;
			if (d >= 0.0) { s = 0.0; }
			else if (-d >= a) { s = 1.0; }
			else { s = -d/a; }
		}
		else  // region 0
		{
			// minimum at interior point
			double invDet = 1.0 / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else
	{
		double tmp0, tmp1, numer, denom;

		if (s < 0.0f)  // region 2
		{
			tmp0 = b + d;
			tmp1 = c + e;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a - 2.0*b + c;
				if (numer >= denom) { s = 1.0; t = 0.0; }
				else { s = numer/denom; t = 1.0 - s; }
			}
			else
			{
				s = 0.0;
				if (tmp1 <= 0.0) { t = 1.0; }
				else if (e >= 0.0) { t = 0.0; }
				else { t = -e/c; }
			}
		}
		else if (t < 0.0f)  // region 6
		{
			tmp0 = b + e;
			tmp1 = a + d;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a - 2.0*b + c;
				if (numer >= denom) { t = 1.0; s = 0.0; }
				else { t = numer/denom; s = 1.0 - t; }
			}
			else
			{
				t = 0.0;
				if (tmp1 <= 0.0) { s = 1.0; }
				else if (d >= 0.0) { s = 0.0; }
				else { s = -d/a; }
			}
		}
		else  // region 1
		{
			numer = c + e - b - d;
			if (numer <= 0.0) { s = 0.0; t = 1.0; }
			else
			{
				denom = a - 2.0*b + c;
				if (numer >= denom) { s = 1.0; t = 0.0; }
				else { s = numer/denom; t = 1.0 - s; }
			}
		}
	}

//	u = s;
//	v = t;
//	w = 1.0 - s - t;

	u = 1.0 - s - t;
	v = s;
	w = t;
}

template <typename VEC3>
typename VEC3::DATA_TYPE squaredDistanceLine2Point(const VEC3& A, const VEC3& AB, typename VEC3::DATA_TYPE AB2, const VEC3& P)
{
	VEC3 V = A - P ;
	VEC3 W = AB ^ V ;
	return W.norm2() / AB2 ;
}

template <typename VEC3>
typename VEC3::DATA_TYPE squaredDistanceLine2Point(const VEC3& A, const VEC3& B, const VEC3& P)
{
	VEC3 AB = B - A ;
	return squaredDistanceLine2Point(A, AB, AB.norm2(), P) ;
}

template <typename VEC3>
typename VEC3::DATA_TYPE squaredDistanceLine2Line(const VEC3& A, const VEC3& AB, typename VEC3::DATA_TYPE AB2, const VEC3& P, const VEC3& Q)
{
	typedef typename VEC3::DATA_TYPE T ;

	VEC3 PQ = Q - P ;
	VEC3 temp = AB ^ PQ ;
	T den = temp.norm2() ;

	if(den > 0)	// droites non //
	{
		T num = (P - A) * temp ;
		return (num*num) / den ;
	}
	else		// droites //
	{
		VEC3 AP = P - A ;
		VEC3 W = AB ^ AP ;
		T num = W.norm2() ;
		return num / AB2 ;
	}
}

template <typename VEC3>
typename VEC3::DATA_TYPE squaredDistanceLine2Seg(const VEC3& A, const VEC3& AB, typename VEC3::DATA_TYPE AB2, const VEC3& P, const VEC3& Q)
{
	typedef typename VEC3::DATA_TYPE T ;

	VEC3 PQ = Q - P ;
	T X = AB * PQ ;
	VEC3 AP = P - A ;

	T beta = ( AB2 * (AP*PQ) - X * (AP*AB) ) / ( X*X - AB2 * PQ.norm2() ) ;

	if(beta < T(0))
	{
		VEC3 W = AB ^ AP ;
		return W.norm2() / AB2 ;
	}

	if(beta > T(1))
	{
		VEC3 AQ = Q - A ;
		VEC3 W = AB ^ AQ ;
		return W.norm2() / AB2 ;
	}

	VEC3 temp = AB ^ PQ ;
	T num = AP * temp ;
	T den = temp.norm2() ;

	return (num*num) / den ;
}

template <typename VEC3>
typename VEC3::DATA_TYPE squaredDistanceSeg2Point(const VEC3& A, const VEC3& AB, typename VEC3::DATA_TYPE AB2, const VEC3& P)
{
	typedef typename VEC3::DATA_TYPE T ;

	VEC3 AP = P - A ;

	// position of projection of P on [A,B]
	T t = (AP * AB) / AB2 ;

	// before A, distance is PA
	if(t <= T(0))
		return AP.norm2() ;

	// after B, distantce is PB
	if(t >= T(1))
	{
		VEC3 BP = P - (AB + A) ;
		return BP.norm2() ;
	}

	// between A & B, distance is projection on (AB)
	VEC3 X = AB ^ AP ;
	return X.norm2() / AB2 ;
}


template <typename VEC3>
bool lineLineClosestPoints(const VEC3& P1, const VEC3& V1, const VEC3& P2, const VEC3& V2, VEC3& Q1, VEC3& Q2)
{
	typedef typename VEC3::DATA_TYPE SCAL;
	VEC3 P12 = P1 - P2;

	SCAL d1343 = P12 * V2;
	SCAL d4321 = V2*V1;
	SCAL d1321 = P12*V1;
	SCAL d4343 = V2*V2;
	SCAL d2121 = V1*V1;

	SCAL denom = d2121 * d4343 - d4321 * d4321;

	if (fabs(denom) < 0.0000001)
		return false;

	SCAL numer = d1343 * d4321 - d1321 * d4343;

	SCAL mua = numer / denom;
	SCAL mub = (d1343 + d4321 * mua) / d4343;

	Q1 = P1 + mua*V1;
	Q2 = P2 + mub*V2;
	return true;
}

} // namespace Geom

} // namespace CGoGN
