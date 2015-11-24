/*
 * bivariatePolynomials.hpp
 *
 *  Created on: July, 2015
 *      Author: sauvage
 */

namespace CGoGN
{

namespace Utils
{



/*************************************************************************
construction, destruction and initialization
**************************************************************************/

template <typename Tscalar,typename Tcoef>
BivariatePolynomials<Tscalar,Tcoef>::BivariatePolynomials() : degree(4), nb_coefs(15)
{
	assert ( (nb_coefs > 0) || !" negative # of coefs");
	coefs = new Tcoef[nb_coefs];
	for (int i = 0; i < nb_coefs; i++)
		coefs[i] = Tcoef (0);
}

template <typename Tscalar,typename Tcoef>
BivariatePolynomials<Tscalar,Tcoef>::BivariatePolynomials(BivariatePolynomials const & r) : degree(r.degree), nb_coefs (r.nb_coefs)
{
	assert ( (nb_coefs > 0) || !" negative # of coefs");
	coefs = new Tcoef[nb_coefs];
	for (int i = 0; i < nb_coefs; i++)
		coefs[i] = r.coefs[i];
}

template <typename Tscalar,typename Tcoef>
BivariatePolynomials<Tscalar,Tcoef>::~BivariatePolynomials()
{
	delete[] coefs;
}

/*************************************************************************
evaluation
**************************************************************************/

template <typename Tscalar,typename Tcoef>
Tcoef BivariatePolynomials<Tscalar,Tcoef>::evaluate_at (Tscalar u, Tscalar v) const
{
	Tcoef r (0);
	Tscalar u2 = u*u;
	Tscalar u3 = u2*u;
	Tscalar u4 = u3*u;
	Tscalar v2 = v*v;
	Tscalar v3 = v2*v;
	Tscalar v4 = v3*v;

	r += coefs[0];
	r += coefs[1] * u;
	r += coefs[2] * v;
	r += coefs[3] * u2;
	r += coefs[4] * u * v;
	r += coefs[5] * v2;
	r += coefs[6] * u3;
	r += coefs[7] * u2 * v;
	r += coefs[8] * u * v2;
	r += coefs[9] * v3;
	r += coefs[10] * u4;
	r += coefs[11] * u3 * v;
	r += coefs[12] * u2 * v2;
	r += coefs[13] * u * v3;
	r += coefs[14] * v4;
	return r;
}

template <typename Tscalar,typename Tcoef>
Tcoef BivariatePolynomials<Tscalar,Tcoef>::evaluate_at (const Geom::Vector<3,Tscalar>& tu, const Geom::Vector<3,Tscalar>& tv, const Geom::Vector<3,Tscalar>& n, const Geom::Vector<3,Tscalar>& eval_dir) const
{
	Tscalar u = tu*eval_dir;
	Tscalar v = tv*eval_dir;
	if (n*eval_dir < 0)
	{
		Tscalar d = sqrt(u*u + v*v);
		u/=d;
		v/=d;
	}
	return evaluate_at(u,v);
}

/*************************************************************************
I/O
**************************************************************************/

template <typename Tscalar,typename Tcoef>
std::ostream & operator << (std::ostream & os, const BivariatePolynomials<Tscalar,Tcoef> & p)
{
	for (int d = 0; d <= p.degree; d++)
	{
		for (int du = d; du >=0 ; du--)
			os << p.get_coef(du,d-du) << "\t";
		os << std::endl;
	}
	return os;
}

/*************************************************************************
operators
**************************************************************************/


/*************************************************************************
fitting
**************************************************************************/


} // namespace Utils

} // namespace CGoGN
