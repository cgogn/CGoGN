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

#ifndef __MULTI3ATTRIBS_H_
#define __MULTI3ATTRIBS_H_

#include "Topology/generic/cells.h"

namespace CGoGN
{

//forward
template <typename T1, typename T2, typename T3>
struct RefCompo3Type;


template <typename T1, typename T2, typename T3>
struct Compo3Type
{
	T1 m_v1;
	T2 m_v2;
	T3 m_v3;


	Compo3Type(){}
	Compo3Type(double z ): m_v1(z), m_v2(z), m_v3(z) {}
	Compo3Type(const RefCompo3Type<T1,T2,T3>& v);

	Compo3Type<T1,T2,T3>& operator =(const Compo3Type<T1,T2,T3>& comp);

	Compo3Type<T1,T2,T3> operator+(const Compo3Type<T1,T2,T3>& v) const;
	Compo3Type<T1,T2,T3> operator-(const Compo3Type<T1,T2,T3>& v) const;
	Compo3Type<T1,T2,T3> operator/(double d) const;
	Compo3Type<T1,T2,T3> operator*(double d) const;
	Compo3Type<T1,T2,T3>& operator+=(const Compo3Type<T1,T2,T3>& v);
	Compo3Type<T1,T2,T3>& operator-=(const Compo3Type<T1,T2,T3>& v);
	Compo3Type<T1,T2,T3>& operator*=(double d);
	Compo3Type<T1,T2,T3>& operator/=(double d);

};


template <typename T1, typename T2, typename T3>
struct RefCompo3Type
{
	T1& m_v1;
	T2& m_v2;
	T3& m_v3;

	RefCompo3Type(T1& v1, T2& v2, T3& v3): m_v1(v1), m_v2(v2), m_v3(v3) {}
	RefCompo3Type (Compo3Type<T1,T2,T3>& comp);

	RefCompo3Type<T1,T2,T3>& operator=(const RefCompo3Type<T1,T2,T3>& v);
	RefCompo3Type<T1,T2,T3>& operator=(Compo3Type<T1,T2,T3> v);

	Compo3Type<T1,T2,T3> operator+(const RefCompo3Type<T1,T2,T3>& v) const;
	Compo3Type<T1,T2,T3> operator-(const RefCompo3Type<T1,T2,T3>& v) const;
	Compo3Type<T1,T2,T3> operator/(double d) const;
	Compo3Type<T1,T2,T3> operator*(double d) const;
	RefCompo3Type<T1,T2,T3>& operator+=(const RefCompo3Type<T1,T2,T3>& v);
	RefCompo3Type<T1,T2,T3>& operator-=(const RefCompo3Type<T1,T2,T3>& v);
	RefCompo3Type<T1,T2,T3>& operator*=(double d);
	RefCompo3Type<T1,T2,T3>& operator/=(double d);
};




template <typename T1, typename T2, typename T3, unsigned int ORB, typename MAP>
class Cell3Attributes: public AttributeHandlerGen
{

	AttributeHandler<T1, ORB, MAP>& m_h1;
	AttributeHandler<T2, ORB, MAP>& m_h2;
	AttributeHandler<T3, ORB, MAP>& m_h3;
public:
	typedef Compo3Type<T1,T2,T3> DATA_TYPE;
	typedef RefCompo3Type<T1,T2,T3> REF_DATA_TYPE;

	Cell3Attributes(AttributeHandler<T1, ORB, MAP>& h1,
					AttributeHandler<T2, ORB, MAP>& h2,
					AttributeHandler<T3, ORB, MAP>& h3):
		AttributeHandlerGen(true),
		m_h1(h1), m_h2(h2), m_h3(h3) {}

	static const unsigned int ORBIT = ORB;
	virtual int getSizeOfType() const {return m_h1.getSizeOfType()+m_h2.getSizeOfType()+m_h3.getSizeOfType(); }
	virtual unsigned int getOrbit() const {return ORB;}
	virtual const std::string& name() const { return m_h1.name(); }
	virtual const std::string& typeName() const { return m_h1.typeName();}
	virtual AttributeMultiVectorGen* getDataVectorGen() const { return NULL;}

	RefCompo3Type<T1,T2,T3> operator[](unsigned int a)
	{
		return RefCompo3Type<T1,T2,T3>(m_h1[a],m_h2[a],m_h3[a]);
	}

	RefCompo3Type<T1,T2,T3> operator[](Cell<ORB> c)
	{
		return RefCompo3Type<T1,T2,T3>(m_h1[c],m_h2[c],m_h3[c]);
	}

	const RefCompo3Type<T1,T2,T3> operator[](unsigned int a) const
	{
		return RefCompo3Type<T1,T2,T3>(m_h1[a],m_h2[a],m_h3[a]);
	}

	const RefCompo3Type<T1,T2,T3> operator[](Cell<ORB> c) const
	{
		return RefCompo3Type<T1,T2,T3>(m_h1[c],m_h2[c],m_h3[c]);
	}

};

template <typename T1, typename T2, typename T3, typename MAP>
using Vertex3Attributes = Cell3Attributes<T1, T2, T3, VERTEX, MAP>;

template <typename T1, typename T2, typename T3, typename MAP>
using Edge3Attributes = Cell3Attributes<T1, T2, T3, EDGE, MAP>;

template <typename T1, typename T2, typename T3, typename MAP>
using Face3Attributes = Cell3Attributes<T1, T2, T3, FACE, MAP>;

template <typename T1, typename T2, typename T3, typename MAP>
using Volume3Attributes = Cell3Attributes<T1, T2, T3, VOLUME, MAP>;


/// implementation

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3>& Compo3Type<T1,T2,T3>::operator =(const Compo3Type<T1,T2,T3>& comp)
{
	m_v1 = comp.m_v1;
	m_v2 = comp.m_v2;
	m_v3 = comp.m_v3;
	return *this;
}


template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3>::Compo3Type(const RefCompo3Type<T1, T2, T3> &v)
{
	m_v1 = v.m_v1;
	m_v2 = v.m_v2;
	m_v3 = v.m_v3;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> Compo3Type<T1,T2,T3>::operator+(const Compo3Type<T1,T2,T3>& v) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 + v.m_v1;
	res.m_v2 = this->m_v2 + v.m_v2;
	res.m_v3 = this->m_v3 + v.m_v3;
	return res ;
}



template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> Compo3Type<T1,T2,T3>::operator-(const Compo3Type<T1,T2,T3>& v) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 - v.m_v1;
	res.m_v2 = this->m_v2 - v.m_v2;
	res.m_v3 = this->m_v3 - v.m_v3;
	return res ;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> Compo3Type<T1,T2,T3>::operator/(double d) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 / d;
	res.m_v2 = this->m_v2 / d;
	res.m_v3 = this->m_v3 / d;
	return res ;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> Compo3Type<T1,T2,T3>::operator*(double d) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 * d;
	res.m_v2 = this->m_v2 * d;
	res.m_v3 = this->m_v3 * d;
		return res ;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3>& Compo3Type<T1,T2,T3>::operator+=(const Compo3Type<T1,T2,T3>& v)
{
	m_v1 += v.m_v1;
	m_v2 += v.m_v2;
	m_v3 += v.m_v3;
	return *this;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3>& Compo3Type<T1,T2,T3>::operator-=(const Compo3Type<T1,T2,T3>& v)
{
	m_v1 += v.m_v1;
	m_v2 -= v.m_v2;
	m_v3 -= v.m_v3;
	return *this;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3>& Compo3Type<T1,T2,T3>::operator*=(double d)
{
	m_v1 *= d;
	m_v2 *= d;
	m_v3 *= d;
	return *this;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3>& Compo3Type<T1,T2,T3>::operator/=(double d)
{
	m_v1 /= d;
	m_v2 /= d;
	m_v3 /= d;
	return *this;
}


/// Ref version

template < typename T1, typename T2, typename T3>
RefCompo3Type<T1,T2,T3>::RefCompo3Type (Compo3Type<T1,T2,T3>& comp):
	m_v1(comp.m_v1),
	m_v2(comp.m_v2),
	m_v3(comp.m_v3)
{
}

template < typename T1, typename T2, typename T3>
RefCompo3Type<T1,T2,T3>& RefCompo3Type<T1,T2,T3>::operator=(const RefCompo3Type<T1,T2,T3>& v)
{
	m_v1 = v.m_v1;
	m_v2 = v.m_v2;
	m_v3 = v.m_v3;
	return *this;
}

template < typename T1, typename T2, typename T3>
RefCompo3Type<T1,T2,T3>& RefCompo3Type<T1,T2,T3>::operator=(Compo3Type<T1,T2,T3> v)
{
	m_v1 = v.m_v1;
	m_v2 = v.m_v2;
	m_v3 = v.m_v3;
	return *this;
}


template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> RefCompo3Type<T1,T2,T3>::operator+(const RefCompo3Type<T1,T2,T3>& v) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 + v.m_v1;
	res.m_v2 = this->m_v2 + v.m_v2;
	res.m_v3 = this->m_v3 + v.m_v3;
	return res ;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> RefCompo3Type<T1,T2,T3>::operator-(const RefCompo3Type<T1,T2,T3>& v) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 - v.m_v1;
	res.m_v2 = this->m_v2 - v.m_v2;
	res.m_v3 = this->m_v3 - v.m_v3;
	return res ;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> RefCompo3Type<T1,T2,T3>::operator/(double d) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 / d;
	res.m_v2 = this->m_v2 / d;
	res.m_v3 = this->m_v3 / d;
	return res ;
}

template < typename T1, typename T2, typename T3>
Compo3Type<T1,T2,T3> RefCompo3Type<T1,T2,T3>::operator*(double d) const
{
	Compo3Type<T1,T2,T3> res ;
	res.m_v1 = this->m_v1 * d;
	res.m_v2 = this->m_v2 * d;
	res.m_v3 = this->m_v3 * d;
	return res ;
}

template < typename T1, typename T2, typename T3>
RefCompo3Type<T1,T2,T3>& RefCompo3Type<T1,T2,T3>::operator+=(const RefCompo3Type<T1,T2,T3>& v)
{
	m_v1 += v.m_v1;
	m_v2 += v.m_v2;
	m_v3 += v.m_v3;
	return *this;
}

template < typename T1, typename T2, typename T3>
RefCompo3Type<T1,T2,T3>& RefCompo3Type<T1,T2,T3>::operator-=(const RefCompo3Type<T1,T2,T3>& v)
{
	m_v1 -= v.m_v1;
	m_v2 -= v.m_v2;
	m_v3 -= v.m_v3;
	return *this;
}

template < typename T1, typename T2, typename T3>
RefCompo3Type<T1,T2,T3>& RefCompo3Type<T1,T2,T3>::operator*=(double d)
{
	m_v1 *= d;
	m_v2 *= d;
	m_v3 *= d;
	return *this;
}

template < typename T1, typename T2, typename T3>
RefCompo3Type<T1,T2,T3>& RefCompo3Type<T1,T2,T3>::operator/=(double d)
{
	m_v1 /= d;
	m_v2 /= d;
	m_v3 /= d;
	return *this;
}


} // namespace CGoGN

#endif /* MULTIATTRIBS_H_ */
