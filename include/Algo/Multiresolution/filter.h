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

#ifndef __MR_FILTERS__
#define __MR_FILTERS__

#include <cmath>


namespace CGoGN
{

namespace Algo
{

namespace MR
{

class Filter
{
public:
	Filter() {}
	virtual ~Filter() {}
	virtual void operator() () = 0 ;
} ;

template <typename PFP>
unsigned int vertexLevel(typename PFP::MAP& map, Dart d)
{
	assert(map.getDartLevel(d) <= map.getCurrentLevel() || !"edgeLevel : called with a dart inserted after current level") ;

	unsigned int level = map.getMaxLevel();

	TraversorDartsOfOrbit<typename PFP::MAP,VERTEX> tv(map,d);

	for(Dart dit = tv.begin() ; dit != tv.end() ; dit = tv.next())
	{
		unsigned int ldit = map.getDartLevel(dit) ;
		if(ldit < level)
			level = ldit;
	}

//	Dart dit = d;
//	do
//	{
//		unsigned int ldit = map.getDartLevel(dit) ;
//		if(ldit < level)
//			level = ldit;
//
//		dit = map.phi2(map.phi_1(dit));
//	}
//	while(dit != d);

	return level;
}


template <typename PFP, typename T>
void filterLowPass(typename PFP::MAP& map, VertexAttribute<T>& attIn, unsigned int cutoffLevel)
{
	unsigned int cur = map.getCurrentLevel();
	unsigned int max = map.getMaxLevel();

	map.setCurrentLevel(max);

	TraversorV<typename PFP::MAP> tv(map);
	for (Dart d = tv.begin(); d != tv.end(); d = tv.next())
	{
		if(vertexLevel<PFP>(map,d) > cutoffLevel)
			attIn[d] = T(0.0);
	}

	map.setCurrentLevel(cur);
}

template <typename PFP, typename T>
void filterHighPass(typename PFP::MAP& map, VertexAttribute<T>& attIn, unsigned int cutoffLevel)
{
	unsigned int cur = map.getCurrentLevel();
	unsigned int max = map.getMaxLevel();

	map.setCurrentLevel(max);

	TraversorV<typename PFP::MAP> tv(map);
	for (Dart d = tv.begin(); d != tv.end(); d = tv.next())
	{
		if(vertexLevel<PFP>(map,d) < cutoffLevel)
			attIn[d] = T(0.0);
	}

	map.setCurrentLevel(cur);
}

template <typename PFP, typename T>
void filterBandPass(typename PFP::MAP& map, VertexAttribute<T>& attIn, unsigned int cutoffLevelLow, unsigned int cutoffLevelHigh)
{
	unsigned int cur = map.getCurrentLevel();
	unsigned int max = map.getMaxLevel();

	map.setCurrentLevel(max);

	TraversorV<typename PFP::MAP> tv(map);
	for (Dart d = tv.begin(); d != tv.end(); d = tv.next())
	{
		unsigned int vLevel = vertexLevel<PFP>(map,d);
		if(cutoffLevelLow > vLevel && vLevel < cutoffLevelHigh)
			attIn[d] = T(0.0);
	}

	map.setCurrentLevel(cur);
}

template <typename PFP, typename T>
void frequencyDeformation(typename PFP::MAP& map, VertexAttribute<T>& attIn, unsigned int cutoffLevel)
{
	unsigned int cur = map.getCurrentLevel();
	unsigned int max = map.getMaxLevel();

	map.setCurrentLevel(max);

	TraversorV<typename PFP::MAP> tv(map);
	for (Dart d = tv.begin(); d != tv.end(); d = tv.next())
	{
		if(vertexLevel<PFP>(map,d) == cutoffLevel)
			attIn[d] += T(0.0,0.0,0.2);
	}

	map.setCurrentLevel(cur);
}


} // namespace MR

} // namespace Algo

} // namespace CGoGN

#endif

