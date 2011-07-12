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

#include "Algo/Filtering/functors.h"
#include "Algo/Selection/collector.h"

namespace CGoGN
{

namespace Algo
{

namespace Filtering
{

template <typename PFP>
void filterAveragePositions(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select = SelectorTrue())
{
	typedef typename PFP::VEC3 VEC3 ;

	FunctorAverage<typename PFP::VEC3> fa(position) ;
	Algo::Selection::Collector_OneRing<PFP> c(map) ;

	CellMarker markV(map, VERTEX);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !markV.isMarked(d))
		{
			markV.mark(d);

			c.collectBorder(d) ;
			fa.reset() ;
			c.applyOnBorder(fa) ;
			position2[d] = fa.getAverage() ;
		}
	}
}

} // namespace Filtering

} // namespace Algo

} // namespace CGoGN
