/*
 * quadricLF.h
 *
 *  Created on: Nov 4, 2010
 *      Author: kenneth
 */

#ifndef QUADRICLF_H_
#define QUADRICLF_H_


#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

using namespace CGoGN;

enum channel {RED=0, GREEN, BLUE};
#define COLCHANNELS 3

template <typename REAL>
class QuadricRGBfunctions {
public:
	typedef Geom::Vector<3,REAL> VEC3;
	typedef Geom::Vector<6,REAL> VEC6;
	typedef Geom::Matrix<6,6,REAL> MATRIX66;

	typedef Geom::Matrix<3,3,REAL> MATRIX33;
	typedef Quadric<REAL> QUADRIC;
	typedef MATRIX33 FRAME;
	typedef Geom::Matrix<3,6,REAL> RGBFUNCTIONS;

private:
	MATRIX66 A[COLCHANNELS];
	VEC6 b[COLCHANNELS];
	REAL c[COLCHANNELS];


public:
	static std::string CGoGNnameOfType() ;

	QuadricRGBfunctions();
	QuadricRGBfunctions(int i);
	QuadricRGBfunctions(const QuadricRGBfunctions&);
	QuadricRGBfunctions(const RGBFUNCTIONS&, const REAL gamma = REAL(0), const REAL alpha = REAL(0)) ;

	virtual ~QuadricRGBfunctions() {} ;

	REAL operator() (const RGBFUNCTIONS&) const;

	bool findOptimizedRGBfunctions(RGBFUNCTIONS& lff) const;

	void operator += (const QuadricRGBfunctions&) ;
	void operator -= (const QuadricRGBfunctions&) ;
	void operator *= (const REAL v) ;
	void operator /= (const REAL v) ;

	void zero () ;

	friend std::ostream& operator<< (std::ostream &out, const QuadricRGBfunctions&) {return out;};
	friend std::istream& operator>> (std::istream &in, const QuadricRGBfunctions&) {return in;};

private :
	void buildIntegralMatrix_A(MATRIX66 &, const REAL alpha) const;
	void buildIntegralMatrix_b(MATRIX66 &, const REAL alpha) const;
	void buildIntegralMatrix_c(MATRIX66 &, const REAL alpha) const;
	void buildRotateMatrix(MATRIX66 &N, const REAL gamma) const;

};


#include "quadricRGBfunctions.hpp"

#endif /* QUADRICLF_H_ */
