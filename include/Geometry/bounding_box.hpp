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

namespace Geom
{

template <typename VEC>
BoundingBox<VEC>::BoundingBox(const VEC& p) :
m_pMin(p),
m_pMax(p)
{}


template <typename VEC>
VEC& BoundingBox<VEC>::min()
{
	return m_pMin ;
}

template <typename VEC>
const VEC& BoundingBox<VEC>::min() const
{
	return m_pMin ;
}

template <typename VEC>
VEC& BoundingBox<VEC>::max()
{
	return m_pMax ;
}

template <typename VEC>
const VEC& BoundingBox<VEC>::max() const
{
	return m_pMax ;
}

template <typename VEC>
typename VEC::DATA_TYPE BoundingBox<VEC>::size(unsigned int coord) const
{
	assert(coord < m_pMax.dimension()) ;
	return m_pMax[coord] - m_pMin[coord] ;
}

template <typename VEC>
VEC BoundingBox<VEC>::center() const
{
	VEC center = (m_pMax + m_pMin) / typename VEC::DATA_TYPE(2) ;
	return center ;
}

/**********************************************/
/*                 FUNCTIONS                  */
/**********************************************/

template <typename VEC>
void BoundingBox<VEC>::addPoint(const VEC& p)
{
	for(unsigned int i = 0; i < p.dimension(); ++i)
	{
		if(p[i] < m_pMin[i])
			m_pMin[i] = p[i] ;
		if(p[i] > m_pMax[i])
			m_pMax[i] = p[i] ;
	}
}

template <typename VEC>
bool BoundingBox<VEC>::intersects(const BoundingBox<VEC>& bb)
{
	VEC bbmin = bb.min() ;
	VEC bbmax = bb.max() ;
	for(unsigned int i = 0; i < bbmin.size(); ++i)
	{
		if(m_pMax[i] < bbmin[i])
			return false ;
		if(m_pMin[i] > bbmax[i])
			return false ;
	}
	return true ;
}

template <typename VEC>
void BoundingBox<VEC>::fusion(const BoundingBox<VEC>& bb)
{
	VEC bbmin = bb.min() ;
	VEC bbmax = bb.max() ;
	for(unsigned int i = 0; i < bbmin.size(); ++i)
	{
		if(bbmin[i] < m_pMin[i])
			m_pMin[i] = bbmin[i] ;
		if(bbmax[i] > m_pMax[i])
			m_pMax[i] = bbmax[i] ;
	}
}


//template <typename VEC>
//friend std::ostream& BoundingBox<VEC>::operator<<(std::ostream& out, const BoundingBox<VEC>& bb)
//{
//	out << bb.min() << " " << bb.max() ;
//	return out ;
//}
//
//template <typename VEC>
//friend std::istream& BoundingBox<VEC>::operator>>(std::istream& in, BoundingBox<VEC>& bb)
//{
//	in >> bb.min() >> bb.max() ;
//	return in ;
//}

//end namespace
}
}


