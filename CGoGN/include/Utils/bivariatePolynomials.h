/*
 * bivariatePolynomials.h
 *
 *  Created on: July, 2015
 *      Author: sauvage
 */

#ifndef __BIVARIATEPOLYNOMIALS_H__
#define __BIVARIATEPOLYNOMIALS_H__

#include <cassert>
#include <cmath>
#include <iostream>

#include <Geometry/vector_gen.h>

//#include <Eigen/Core>
//#include <Eigen/Dense>
//#include <Eigen/Cholesky>

namespace CGoGN
{

namespace Utils
{

template <typename Tscalar, typename Tcoef>
class BivariatePolynomials
{
private :
	int degree ;	// (bi-)degree of the polynomial (=4)
	int nb_coefs ;	// number of coefs (=15)
	Tcoef* coefs;	// table of coefficients

public :
	// construction, destruction and initialization
	BivariatePolynomials();
	BivariatePolynomials(BivariatePolynomials const &);
	~BivariatePolynomials();

	int get_degree () const { return degree; }
	int get_nb_coefs () const { return nb_coefs; }

	// evaluation

	Tcoef evaluate_at (Tscalar u, Tscalar v) const;
	Tcoef evaluate_at (const Geom::Vector<3,Tscalar>& tu, const Geom::Vector<3,Tscalar>& tv, const Geom::Vector<3,Tscalar>& n, const Geom::Vector<3,Tscalar>& eval_dir) const;

	// I/O
	const Tcoef& get_coef (int du, int dv) const {assert ((du>=0 && dv <=degree && dv>=0 && dv <=degree) || !" bi-degree is incorrect"); return get_coef(index(du,dv));}
	Tcoef& get_coef (int du, int dv) {assert ((du>=0 && dv <=degree && dv>=0 && dv <=degree) || !" bi-degree is incorrect"); return get_coef(index(du,dv));}

	Tcoef* get_coef_tab () const {return coefs;}

	template <typename TS,typename TC> friend std::ostream & operator<< (std::ostream & os, const BivariatePolynomials<TS,TC> & sh);

	// operators
//	void operator= (const BivariatePolynomials<Tscalar,Tcoef>&);
//	void operator+= (const BivariatePolynomials<Tscalar,Tcoef>&);
//	BivariatePolynomials<Tscalar,Tcoef> operator+ (const BivariatePolynomials<Tscalar,Tcoef>&) const;
//	void operator-= (const BivariatePolynomials<Tscalar,Tcoef> &);
//	BivariatePolynomials<Tscalar,Tcoef> operator- (const BivariatePolynomials<Tscalar,Tcoef>&) const;
//	void operator*= (Tscalar);
//	BivariatePolynomials<Tscalar,Tcoef> operator* (Tscalar) const;
//	void operator/= (Tscalar);
//	BivariatePolynomials<Tscalar,Tcoef> operator/ (Tscalar) const;

	std::string CGoGNnameOfType() const { return "BivariatePolynomials"; }

	// fitting
//	template <typename Tdirection, typename Tchannel>
//	void fit_to_data(int n, Tdirection* t_theta, Tdirection* t_phi, Tchannel* t_R, Tchannel* t_G, Tchannel* t_B, double lambda);
//	template <typename Tdirection, typename Tchannel>
//	void fit_to_data(int n, Tdirection* t_x, Tdirection* t_y, Tdirection* t_z, Tchannel* t_R, Tchannel* t_G, Tchannel* t_B, double lambda);

private :
	static int index (int du, int dv) {int n=du+dv; return n*(n+1)/2 + dv;}
	// order of coefs : 1,u,v,u2, uv, v2, u3, u2v, uv2, v3, u4, u3v, u2v2, uv3, v4
	// Thus i = n*(n+1)/2 + dv where n =

	const Tcoef& get_coef (int i) const {assert ((i>=0 && i<nb_coefs ) || !" incorrect index"); return coefs[i];}
	Tcoef& get_coef (int i) {assert ((i>=0 && i<nb_coefs ) || !" incorrect index"); return coefs[i];}

	// fitting
//	template <typename Tchannel>
//	void fit_to_data(int n, Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic>& mM, Tchannel* t_R, Tchannel* t_G, Tchannel* t_B, double lambda);
};

} // namespace Utils

} // namespace CGoGN

#include "Utils/bivariatePolynomials.hpp"

#endif /* __BIVARIATEPOLYNOMIALS_H__ */
