/*
 * attribops.h
 *
 *  Created on: Jul 20, 2010
 *      Author: thery
 */

#ifndef ATTRIBOPS_H_
#define ATTRIBOPS_H_


namespace AttribOps
{

template <typename X, typename PFP>
struct lerpStruct
{
	static X apply( X u, X v, double a)
	{
		return u*a + v*(1.0-a);
	}
};

template <typename PFP>
struct lerpStruct<typename PFP::EVERTEX ,PFP>
{
	static typename PFP::EVERTEX apply(typename PFP::EVERTEX u, typename PFP::EVERTEX v, double a)
	{
		std::cout << "Static Lerp"<< std::endl;
		typename PFP::EVERTEX ec;
		ec.lerp(u,v,a);
		return ec;
	}
};


template < typename X, typename PFP>
X  lerp( X u, X v, double a )
{
	std::cout << "function Lerp"<< std::endl;
	return lerpStruct< X, PFP >::apply( u,v,a );
}



template <typename X, typename PFP>
struct zeroStruct
{
	static X  apply()
	{
		return X(0);
	}
};

template <typename PFP>
struct zeroStruct<typename PFP::EVERTEX ,PFP>
{
	static typename PFP::EVERTEX apply()
	{
		typename PFP::EVERTEX ev;
		ev.zero();
		return ev;
	}
};


template < typename X, typename PFP>
X zero()
{
	return zeroStruct< X, PFP >::apply();
}

}





#endif /* ATTRIBOPS_H_ */
