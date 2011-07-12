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

#ifndef __FILTERING_FUNCTORS_H__
#define __FILTERING_FUNCTORS_H__

#include "Topology/generic/functor.h"
#include "Algo/Geometry/intersection.h"

namespace CGoGN
{

namespace Algo
{

namespace Filtering
{

template <typename T>
class FunctorAverage : public virtual FunctorType
{
protected:
	const AttributeHandler<T>& attr ;
	T sum ;
	unsigned int count ;
public:
	FunctorAverage(const AttributeHandler<T>& a) : FunctorType(), attr(a), sum(0), count(0)
	{}
	bool operator()(Dart d)
	{
		sum += attr[d] ;
		++count ;
		return false ;
	}
	inline void reset() { sum = T(0) ; count = 0 ; }
	inline T getSum() { return sum ; }
	inline unsigned int getCount() { return count ; }
	inline T getAverage() { return sum / typename T::DATA_TYPE(count) ; }
} ;

template <typename PFP, typename T>
class FunctorAverageOnSphereBorder : public FunctorMap<typename PFP::MAP>
{
	typedef typename PFP::VEC3 VEC3;
protected:
	const AttributeHandler<T>& attr ;
	const AttributeHandler<VEC3>& position ;
	VEC3 center;
	typename PFP::REAL radius;
	T sum ;
	unsigned int count ;
public:
	FunctorAverageOnSphereBorder(typename PFP::MAP& map, const AttributeHandler<T>& a, const AttributeHandler<VEC3>& p, VEC3& c = VEC3(0), typename PFP::REAL r = 0) :
		FunctorMap<typename PFP::MAP>(map), attr(a), position(p), center(c), radius(r), sum(0), count(0)
	{}
	bool operator()(Dart d)
	{
		typename PFP::REAL alpha = 0;
		Algo::Geometry::intersectionSphereEdge<PFP>(this->map, center, radius, d, position, alpha);
		sum += (1 - alpha) * attr[d] + alpha * attr[this->map.phi1(d)] ;
		++count ;
		return false ;
	}
	inline void reset(VEC3& c, typename PFP::REAL r) { center = c; radius = r; sum = T(0) ; count = 0 ; }
	inline T getSum() { return sum ; }
	inline unsigned int getCount() { return count ; }
	inline T getAverage() { return sum / typename T::DATA_TYPE(count) ; }
} ;

} // namespace Filtering

} // namespace Algo

} // namespace CGoGN

#endif
