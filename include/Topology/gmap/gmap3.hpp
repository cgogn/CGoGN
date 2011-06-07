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


inline GMap3::GMap3() : GMap2()
{
	m_beta3 = addRelation("beta3") ;
}

inline std::string GMap3::mapTypeName()
{
	return "GMap3";
}

inline unsigned int GMap3::dimension()
{
	return 3;
}


/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

inline Dart GMap3::newDart()
{
	Dart d = GMap2::newDart() ;
	(*m_beta3)[d.index] = d ;
	return d ;
}

inline Dart GMap3::beta3(Dart d)
{
	return (*m_beta3)[d.index] ;
}

template <int N>
inline Dart GMap3::beta(const Dart d)
{
	assert( (N > 0) || !"negative parameters not allowed in template multi-beta");
	if (N<10)
	{
		switch(N)
		{
		case 0 : return beta0(d) ;
		case 1 : return beta1(d) ;
		case 2 : return beta2(d) ;
		case 3 : return beta2(d) ;
		default : assert(!"Wrong multi-beta relation value") ;
		}
	}
	switch(N%10)
	{
	case 0 : return beta0(beta<N/10>(d)) ;
	case 1 : return beta1(beta<N/10>(d)) ;
	case 2 : return beta2(beta<N/10>(d)) ;
	case 3 : return beta3(beta<N/10>(d)) ;
	default : assert(!"Wrong multi-beta relation value") ;
	}
}

inline Dart GMap3::phi3(Dart d)
{
	Dart e = beta3(d) ;
	if(e == d)
		return d ;
	else
		return beta3(beta0(d)) ;
}

template <int N>
inline Dart GMap3::phi(Dart d)
{
	assert( (N >0) || !"negative parameters not allowed in template multi-phi");
	if (N<10)
	{
		switch(N)
		{
		case 1 : return phi1(d) ;
		case 2 : return phi2(d) ;
		case 3 : return phi3(d) ;
		default : assert(!"Wrong multi-phi relation value") ; return d ;
		}
	}
	switch(N%10)
	{
	case 1 : return phi1(phi<N/10>(d)) ;
	case 2 : return phi2(phi<N/10>(d)) ;
	case 3 : return phi3(phi<N/10>(d)) ;
	default : assert(!"Wrong multi-phi relation value") ; return d ;
	}
}



inline void GMap3::beta3sew(Dart d, Dart e)
{
	assert((*m_beta3)[d.index] == d) ;
	assert((*m_beta3)[e.index] == e) ;
	(*m_beta3)[d.index] = e ;
	(*m_beta3)[e.index] = d ;
}

inline void GMap3::beta3unsew(Dart d)
{
	Dart e = (*m_beta3)[d.index] ;
	(*m_beta3)[d.index] = d ;
	(*m_beta3)[e.index] = e ;
}

inline void GMap3::phi3sew(Dart d, Dart e)
{
	beta3sew(d, beta0(e)) ;
	beta3sew(beta0(d), e) ;
}

inline void GMap3::phi3unsew(Dart d)
{
	beta3unsew(d) ;
	beta3unsew(beta0(d)) ;
}




} // end namespace CGoGN
