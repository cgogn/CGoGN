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
#ifndef ATTRIBOPS_H_
#define ATTRIBOPS_H_

#include "Utils/cgognStream.h"

namespace CGoGN
{

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
		CGoGNout << "Static Lerp"<< CGoGNendl;
		typename PFP::EVERTEX ec;
		ec.lerp(u,v,a);
		return ec;
	}
};


template < typename X, typename PFP>
X  lerp( X u, X v, double a )
{
	CGoGNout << "function Lerp"<< CGoGNendl;
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


} // end namespace
}




#endif /* ATTRIBOPS_H_ */
