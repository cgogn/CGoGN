/*
 * quadricLF.hpp
 *
 *  Created on: Nov 4, 2010
 *      Author: kenneth
 */

#ifndef QUADRICLF_HPP_
#define QUADRICLF_HPP_

template <typename REAL>
std::string QuadricRGBfunctions<REAL>::CGoGNnameOfType() {
	return std::string("QuadricColFuncs");
}

template <typename REAL>
QuadricRGBfunctions<REAL>::QuadricRGBfunctions() {
	for (unsigned col = RED; col < BLUE+1 ; ++col) {
		for (unsigned i = 0; i < 6; ++i) {
			b[col][i] = (REAL)0;
			for (unsigned j = 0; j < 6; ++j)
				A[col](i,j) = REAL(0);
		}

		c[col] = REAL(0);
	}
}

template <typename REAL>
QuadricRGBfunctions<REAL>::QuadricRGBfunctions(int i) {
	QuadricRGBfunctions();
}

template <typename REAL>
QuadricRGBfunctions<REAL>::QuadricRGBfunctions(const QuadricRGBfunctions& q) {
	for (unsigned col = RED; col < BLUE+1 ; ++col) {
		for (unsigned i = 0; i < 6; ++i) {
			b[col][i] = q.b[col][i];
			for (unsigned j = 0; j < 6; ++j)
				A[col](i,j) = q.A[col](i,j);
		}

		c[col] = q.c[col];
	}
}

template <typename REAL>
QuadricRGBfunctions<REAL>::QuadricRGBfunctions(const RGBFUNCTIONS& cf, REAL alpha, REAL gamma) {
	MATRIX66 R1,R2_A,R2_b,R2_c;
	// Matrice de rotation 1
	buildRotateMatrix(R1,gamma);
//	std::cout <<"R1 : " <<  R1 << std::endl ;
//	std::cout <<"alpha : " << alpha << std::endl ;
	// Matrice de rotation 2 + intégrale
	buildIntegralMatrix_A(R2_A,alpha);
	buildIntegralMatrix_b(R2_b,alpha);
	buildIntegralMatrix_b(R2_c,alpha);

//	std::cout << "A : " << R2_A << std::endl ;
//	std::cout << "b : " << R2_b << std::endl ;
//	std::cout << "c : " << R2_c << std::endl ;


	// Quadrique (A,b,c) tel que L*A*Lt - 2*b*Lt + c = ERROR
	for (unsigned col = RED; col < BLUE+1; ++col) {
		// Rotation 1

		Geom::Vector<6,REAL> function;
		assert(cf.getSubVectorH(col,0,function) || !"QuadricRGBfunctions::constructor") ;

		VEC6 coefs = R1 * function ; // Multiply coefs

		// Calcul de A (intégrale)
		A[col] = R2_A ;
		// Calcul de b (intégrale+rotation sur 1 vec)
		b[col] = R2_b * function ;

		// Calcul de c (intégrale+rotation sur 2 vec)
		c[col] = function * (R2_c * function) ;
	}
}

template <typename REAL>
QuadricRGBfunctions<REAL>::~QuadricRGBfunctions() {

}

template <typename REAL>
REAL QuadricRGBfunctions<REAL>::operator() (const RGBFUNCTIONS& cf) const {
	REAL res = REAL(0);

	for (unsigned col = RED; col < BLUE+1; ++col) {
		Geom::Vector<6,REAL> function ;

		assert (cf.getSubVectorH(col,0,function) || !"QuadricRGBfunctions::operator()") ;

		VEC6 Al = A[col] * function;

		REAL res_local = REAL(0) ;
		res_local += (function * Al); // l*A*lt
		res_local -= 2 * (function * b[col]);
		res_local += c[col];

		res += res_local;
	}

	return res;
}

template <typename REAL>
bool QuadricRGBfunctions<REAL>::findOptimizedRGBfunctions(RGBFUNCTIONS& cf) const {

	for (unsigned  col = RED; col < BLUE+1 ; ++col) {
		MATRIX66 Ainv,M;
		M = A[col];

		REAL det = M.invert(Ainv);

		if(det > -1e-8 && det < 1e-8)
			return false ;

		VEC6 coefs;

		coefs = Ainv * b[col];

		assert (cf.setSubVectorH(col,0,coefs) || "QuadricRGBfunctions::findOptimizedRGBfunctions(cf) setSubVector failed") ;
	}

	return true;
}

template <typename REAL>
void QuadricRGBfunctions<REAL>::buildIntegralMatrix_A(MATRIX66 &M, REAL alpha) const {
	REAL alphaAbs = alpha > 0 ? alpha : -alpha;

	const REAL pi = 3.141592 ;

	const REAL cosAbs = cos(alphaAbs); // = - cos(alpha) si alpha < 0
	const REAL cosinus = cos(alpha);
	const REAL sinus = sin(alpha);

	// Line 1
	M(0,0) = 2.0*(pi-alphaAbs)/5.0f;

	M(0,1) = 2.0 * (pi - alphaAbs - cosAbs*sinus) / 15.0 ;

	M(0,2) = 0;

	M(0,3) = 0;

	M(0,4) = - sinus*pi / 8.0;

	M(0,5) = 2.0*(pi-alphaAbs)/3.0;

	// Line 2
	M(1,0) = M(0,1);

	M(1,1) = -4.0 * sinus*cosAbs*cosAbs*cosAbs / 15.0 + 2.0 * (pi - cosAbs*sinus - alphaAbs) / 5.0 ;

	M(1,2) = 0;

	M(1,3) = 0;

	M(1,4) = -(sinus*cosinus*cosinus*pi + 2*sinus*pi) / 8.0;

	M(1,5) = 2.0*(pi-cosAbs*sinus-alphaAbs)/3.0;

	// LINE 3
	M(2,0) = REAL(0);

	M(2,1) = REAL(0);

	M(2,2) = 2.0*(pi - alphaAbs - cosAbs*sinus)/15.0;

	M(2,3) = -pi*sinus/8.0f;

	M(2,4) = REAL(0) ;

	M(2,5) = REAL(0) ;

	// Line 4
	M(3,0) = REAL(0) ;

	M(3,1) = REAL(0) ;

	M(3,2) = M(2,3);

	M(3,3) = 2.0*(pi-alphaAbs)/3.0 ;

	M(3,4) = REAL(0) ;

	M(3,5) = REAL(0) ;

	// Line 5
	M(4,0) = - M(0,4) ;

	M(4,1) = - (2*pi*sinus + sinus*cosinus*cosinus*pi ) / 8.0 ;

	M(4,2) = REAL(0) ;

	M(4,3) = REAL(0) ;

	M(4,4) = 2.0 * (pi - cosAbs*sinus - alphaAbs ) / 3.0 ;

	M(4,5) = - pi*sinus / 2.0 ;

	// Line 6
	M(5,0) = M(0,5) ;

	M(5,1) = M(1,5) ;

	M(5,2) = REAL(0) ;

	M(5,3) = REAL(0) ;

	M(5,4) = M(4,5) ;

	M(5,5) = 2.0*(pi-alphaAbs);
}


template <typename REAL>
void QuadricRGBfunctions<REAL>::buildIntegralMatrix_b(MATRIX66 &M, REAL alpha) const {
	REAL alphaAbs = alpha > 0 ? alpha : -alpha;

	const REAL pi = 3.141592 ;

	const REAL cosAbs = cos(alphaAbs); // = - cos(alpha) si alpha < 0
	const REAL cosinus = cos(alpha);
	const REAL sinus = sin(alpha);

	// Line 1
	M(0,0) = 2.0*(pi-alphaAbs)/5.0f;

	M(0,1) = 2.0 * (pi - alphaAbs - cosAbs*sinus) / 15.0 ;

	M(0,2) = 0;

	M(0,3) = 0;

	M(0,4) = sinus*pi / 8.0;

	M(0,5) = 2.0*(pi-alphaAbs)/3.0;

	// Line 2
	M(1,0) = M(0,1);

	M(1,1) = 2 * (pi - alphaAbs + 2*(cosAbs*cosAbs*pi-alphaAbs*cosAbs*cosAbs))/15.0 - 2.0*cosAbs*sinus/3.0 ;

	M(1,2) = 0;

	M(1,3) = 0;

	M(1,4) = (sinus*pi - 2*sinus*cosinus*pi) / 8.0;

	M(1,5) = 2.0*(pi-cosAbs*sinus-alphaAbs)/3.0;

	// LINE 3
	M(2,0) = REAL(0);

	M(2,1) = REAL(0);

	M(2,2) = 2.0*(cosinus*pi - cosinus*alphaAbs + (alpha < 0 ? sinus : -sinus))/15.0;

	M(2,3) = -pi*sinus/8.0f;

	M(2,4) = REAL(0) ;

	M(2,5) = REAL(0) ;

	// Line 4
	M(3,0) = REAL(0) ;

	M(3,1) = REAL(0) ;

	M(3,2) = - M(2,3);

	M(3,3) = 2.0*(pi-alphaAbs)/3.0 ;

	M(3,4) = REAL(0) ;

	M(3,5) = REAL(0) ;

	// Line 5
	M(4,0) = - M(0,4) ;

	M(4,1) = (2*pi*cosinus*sinus - sinus*pi ) / 8.0 ;

	M(4,2) = REAL(0) ;

	M(4,3) = REAL(0) ;

	M(4,4) = 2.0 * (cosinus*pi - cosinus*alphaAbs + (alpha < 0 ? sinus : -sinus) ) / 3.0 ;

	M(4,5) = pi*sinus / 2.0 ;

	// Line 6
	M(5,0) = M(0,5) ;

	M(5,1) = M(1,5) ;

	M(5,2) = REAL(0) ;

	M(5,3) = REAL(0) ;

	M(5,4) = M(4,5) ;

	M(5,5) = 2.0*(pi-alphaAbs);
}

template <typename REAL>
void QuadricRGBfunctions<REAL>::buildIntegralMatrix_c(MATRIX66 &M, REAL alpha) const {
	REAL alphaAbs = alpha > 0 ? alpha : -alpha;

	const REAL pi = 3.141592 ;

	const REAL cosAbs = cos(alphaAbs); // = - cos(alpha) si alpha < 0
	const REAL cos = cos(alpha);
	const REAL sin = sin(alpha);

	// Line 1
	M(0,0) = 2.0*(pi-alphaAbs)/5.0f;

	M(0,1) = 2.0 * (pi - alphaAbs - cosAbs*sin) / 15.0 ;

	M(0,2) = 0;

	M(0,3) = 0;

	M(0,4) = sin*pi / 8.0;

	M(0,5) = 2.0*(pi-alphaAbs)/3.0;

	// Line 2
	M(1.0) = M(0,1);

	M(1,1) = -4*cosAbs*cosAbs*cosAbs*sin/15.0f + 2*(pi-cosAbs*sin-alphaAbs)/5.0f;

	M(1,2) = 0;

	M(1,3) = 0;

	M(1,4) = (sin*cos*cos*pi + 2*sin*pi)/8.0f ;

	M(1,5) = 2.0*(pi-cosAbs*sin-alphaAbs)/3.0;

	// LINE 3
	M(2,0) = REAL(0);

	M(2,1) = REAL(0);

	M(2,2) = 2.0*(pi - alphaAbs -cosAbs*sin)/15.0;

	M(2,3) = pi*sin/8.0f;

	M(2,4) = REAL(0) ;

	M(2,5) = REAL(0) ;

	// Line 4
	M(3,0) = REAL(0) ;

	M(3,1) = REAL(0) ;

	M(3,2) = M(2,3);

	M(3,3) = 2.0*(pi-alphaAbs)/3.0 ;

	M(3,4) = REAL(0) ;

	M(3,5) = REAL(0) ;

	// Line 5
	M(4,0) = M(0,4) ;

	M(4,1) = (2*sin*pi + sin*cos*cos*pi ) / 8.0 ;

	M(4,2) = REAL(0) ;

	M(4,3) = REAL(0) ;

	M(4,4) = 2.0 * (pi - alphaAbs - cosAbs*sin ) / 3.0 ;

	M(4,5) = pi*sin / 2.0 ;

	// Line 6
	M(5,0) = M(0,5) ;

	M(5,1) = M(1,5) ;

	M(5,2) = REAL(0) ;

	M(5,3) = REAL(0) ;

	M(5,4) = M(4,5) ;

	M(5,5) = 2.0*(pi-alphaAbs);
}


template <typename REAL>
void QuadricRGBfunctions<REAL>::buildRotateMatrix(MATRIX66 &N, REAL gamma) const {
	REAL cosinus = cos(gamma), cos2 = cosinus*cosinus;
	REAL sinus = sin(gamma), sin2 = sinus*sinus;
	REAL sincos = sinus*cosinus;

	N(0,0) = cos2;
	N(0,1) = sin2;
	N(0,2) = -sincos;
	N(0,3) = REAL(0);
	N(0,4) = REAL(0);
	N(0,5) = REAL(0);

	N(1,0) = sin2;
	N(1,1) = cos2;
	N(1,2) = sincos;
	N(1,3) = REAL(0);
	N(1,4) = REAL(0);
	N(1,5) = REAL(0);

	N(2,0) = 2*sincos;
	N(2,1) = -2*sincos;
	N(2,2) = cos2-sin2;
	N(2,3) = REAL(0);
	N(2,4) = REAL(0);
	N(2,5) = REAL(0);

	N(3,0) = REAL(0);
	N(3,1) = REAL(0);
	N(3,2) = REAL(0);
	N(3,3) = cosinus;
	N(3,4) = -sinus;
	N(3,5) = REAL(0);

	N(4,0) = REAL(0);
	N(4,1) = REAL(0);
	N(4,2) = REAL(0);
	N(4,3) = sinus;
	N(4,4) = cosinus;
	N(4,5) = REAL(0);

	N(5,0) = REAL(0);
	N(5,1) = REAL(0);
	N(5,2) = REAL(0);
	N(5,3) = REAL(0);
	N(5,4) = REAL(0);
	N(5,5) = 1.0;
}

template <typename REAL>
void QuadricRGBfunctions<REAL>::operator += (const QuadricRGBfunctions& q) {
	for (unsigned col = RED; col < BLUE+1 ; ++col) {
		for (unsigned i = 0; i < 6; ++i) {
			b[col][i] += q.b[col][i];
			for (unsigned j = 0; j < 6; ++j)
				A[col](i,j) += q.A[col](i,j);
		}

		c[col] += q.c[col];
	}
}

template <typename REAL>
void QuadricRGBfunctions<REAL>::operator -= (const QuadricRGBfunctions& q) {
	for (unsigned col = RED; col < BLUE+1 ; ++col) {
		for (unsigned i = 0; i < 6; ++i) {
			b[col][i] -= q.b[col][i];
			for (unsigned j = 0; j < 6; ++j)
				A[col](i,j) -= q.A[col](i,j);
		}

		c[col] -= q.c[col];
	}
}

template <typename REAL>
void QuadricRGBfunctions<REAL>::operator *= (REAL v) {
	for (unsigned col = RED; col < BLUE+1 ; ++col) {
		for (unsigned i = 0; i < 6; ++i) {
			b[col][i] *= v;
			for (unsigned j = 0; j < 6; ++j)
				A[col](i,j) *= v;
		}

		c[col] *= v;
	}
}

template <typename REAL>
void QuadricRGBfunctions<REAL>::operator /= (REAL v) {
	for (unsigned col = RED; col < BLUE+1 ; ++col) {
		for (unsigned i = 0; i < 6; ++i) {
			b[col][i] /= v;
			for (unsigned j = 0; j < 6; ++j)
				A[col](i,j) /= v;
		}

		c[col] /= v;
	}
}

template <typename REAL>
void QuadricRGBfunctions<REAL>::zero () {
	for (unsigned int i = 0 ; i < COLCHANNELS ; ++i) {
		A[i].zero();
		b[i].zero();
		c[i] = REAL(0) ;
	}
}

#endif /* QUADRICLF_HPP_ */
