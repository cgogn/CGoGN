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

namespace Algo
{

namespace Filters2D
{


template <typename PFP>
void filterTaubin(typename PFP::MAP& map, typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select)
{
	typedef typename PFP::VEC3 VEC3 ;

	const float lambda = 0.6307 ;
	const float mu = -0.6732 ;

	CellMarkerNoUnmark mv(map, VERTEX);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !mv.isMarked(d))
		{
			mv.mark(d);

			// get pos of vertex
			const VEC3& pos_d = position[d] ;

			// traversal of vertices neighborhood
			VEC3 l(0) ;
			int nbE = 0 ;
			Dart dd = d ;
			do
			{
				Dart e = map.phi2(dd) ;
				l += position[e] - pos_d ;
				nbE++ ;
				dd = map.phi1(e) ;
			} while(dd != d) ;

			l /= float(nbE) ;
			l *= lambda ;

			position2[d] = pos_d + l ;
		}
	}

	// unshrinking step
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && mv.isMarked(d))
		{
			mv.unmark(d);

			// get pos of vertex
			const VEC3& pos_d = position2[d] ;

			// traversal of vertices neighborhood
			VEC3 l(0) ;
			int nbE = 0 ;
			Dart dd = d ;
			do
			{
				Dart e = map.phi2(dd) ;
				l += position2[e] - pos_d ;
				nbE++ ;
				dd = map.phi1(e) ;
			} while(dd != d) ;

			l /= float(nbE) ;
			l *= mu ;

			position[d] = pos_d + l ;
		}
	}
}

} //namespace Filters2D

} //namespace Algo

} //namespace CGoGN
