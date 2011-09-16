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

namespace CGoGN
{

inline void GMap2::init()
{
	m_beta2 = addRelation("beta2") ;
}

inline GMap2::GMap2() : GMap1()
{
	init() ;
}

inline std::string GMap2::mapTypeName()
{
	return "GMap2";
}

inline unsigned int GMap2::dimension()
{
	return 2;
}

inline void GMap2::clear(bool removeAttrib)
{
	GMap1::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

inline int GMap2::getDartsPerTriangle()
{
	return 6;
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

inline Dart GMap2::newDart()
{
	Dart d = GMap1::newDart() ;
	(*m_beta2)[d.index] = d ;
	return d ;
}

inline Dart GMap2::beta2(Dart d)
{
	return (*m_beta2)[d.index] ;
}

template <int N>
inline Dart GMap2::beta(const Dart d)
{
	assert( (N > 0) || !"negative parameters not allowed in template multi-beta");
	if (N<10)
	{
		switch(N)
		{
		case 0 : return beta0(d) ;
		case 1 : return beta1(d) ;
		case 2 : return beta2(d) ;
		default : assert(!"Wrong multi-beta relation value") ;
		}
	}
	switch(N%10)
	{
	case 0 : return beta0(beta<N/10>(d)) ;
	case 1 : return beta1(beta<N/10>(d)) ;
	case 2 : return beta2(beta<N/10>(d)) ;
	default : assert(!"Wrong multi-beta relation value") ;
	}
}

inline Dart GMap2::phi2(Dart d)
{
	Dart e = beta2(d) ;
	if(e == d)
		return d ;
	else
		return beta2(beta0(d)) ;
}

template <int N>
inline Dart GMap2::phi(Dart d)
{
	assert( (N >0) || !"negative parameters not allowed in template multi-phi");
	if (N<10)
	{
		switch(N)
		{
		case 1 : return phi1(d) ;
		case 2 : return phi2(d) ;
		default : assert(!"Wrong multi-phi relation value") ; return d ;
		}
	}
	switch(N%10)
	{
	case 1 : return phi1(phi<N/10>(d)) ;
	case 2 : return phi2(phi<N/10>(d)) ;
	default : assert(!"Wrong multi-phi relation value") ; return d ;
	}
}

inline Dart GMap2::alpha0(Dart d)
{
	return phi2(d) ;
}

inline Dart GMap2::alpha1(Dart d)
{
	Dart e = beta1(d);
	Dart f = beta2(e);

	if (f != e)
		return f;

	f = d;
	e = beta2(f);
	while (e != f)
	{
		f = beta1(e);
		e = beta2(f);
	}
	return f;
}

inline Dart GMap2::alpha_1(Dart d)
{
	Dart e = beta2(d);

	if (e != d)
		return beta1(e);

	e = d;
	Dart f = beta1(d);
	while (beta2(f) != f)
	{
		e = beta2(f);
		f = beta1(e);
	}
	return e;
}

inline void GMap2::beta2sew(Dart d, Dart e)
{
	assert((*m_beta2)[d.index] == d) ;
	assert((*m_beta2)[e.index] == e) ;
	(*m_beta2)[d.index] = e ;
	(*m_beta2)[e.index] = d ;
}

inline void GMap2::beta2unsew(Dart d)
{
	Dart e = (*m_beta2)[d.index] ;
	(*m_beta2)[d.index] = d ;
	(*m_beta2)[e.index] = e ;
}

inline void GMap2::phi2sew(Dart d, Dart e)
{
	beta2sew(d, beta0(e)) ;
	beta2sew(beta0(d), e) ;
}

inline void GMap2::phi2unsew(Dart d)
{
	beta2unsew(d) ;
	beta2unsew(beta0(d)) ;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

inline bool GMap2::foreach_dart_of_volume(Dart d, FunctorType& fonct, unsigned int thread)
{
	return foreach_dart_of_cc(d, fonct, thread) ;
}

} // namespace CGoGN
