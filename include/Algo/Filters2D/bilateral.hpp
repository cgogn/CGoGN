/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <math.h>

namespace CGoGN
{

namespace Algo
{

namespace Filters2D
{


template <typename PFP>
void sigmaBilateral(typename PFP::MAP& map, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal, float& sigmaC, float& sigmaS, const FunctorSelect& select)
{
	typedef typename PFP::VEC3 VEC3 ;

	float sumLengths = 0.0f ;
	float sumAngles = 0.0f ;
	long nbEdges = 0 ;

	CellMarker mv(map, VERTEX) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !mv.isMarked(d))
		{
			mv.mark(d);

			Dart dd = d ;
			do
			{
				// compute length and angle if not yet computed
				Dart e = map.phi2(dd) ;
				if(!mv.isMarked(e))
				{
					VEC3 vec = Algo::Geometry::vectorOutOfDart<PFP>(map, dd, position) ;
					sumLengths += vec.norm() ;
					sumAngles += Geom::angle(normal[d], normal[e]) ;
					++nbEdges ;
				}
				// step to next face around vertex
				dd = map.phi1(e) ;
			} while(dd != d) ;
		}
	}

	// update of returned values
	sigmaC = 1.0f * ( sumLengths / float(nbEdges) ) ;
	sigmaS = 2.5f * ( sumAngles / float(nbEdges) ) ;
}

template <typename PFP>
void filterBilateral(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select)
{
	typedef typename PFP::VEC3 VEC3 ;

	float sigmaC, sigmaS ;
	sigmaBilateral<PFP>(map, position, normal, sigmaC, sigmaS, select) ;

	CellMarker mv(map, VERTEX) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !mv.isMarked(d))
		{
			mv.mark(d);

			// get position & normal of vertex
			const VEC3& pos_d = position[d] ;
			const VEC3& normal_d = normal[d] ;

			// traversal of neighbour vertices
			float sum = 0.0f, normalizer = 0.0f ;
			Dart dd = d ;
			do
			{
				VEC3 vec = Algo::Geometry::vectorOutOfDart<PFP>(map, dd, position) ;
				float h = normal_d * vec ;
				float t = vec.norm() ;
				float wcs = exp( ( -1.0f * (t * t) / (2.0f * sigmaC * sigmaC) ) + ( -1.0f * (h * h) / (2.0f * sigmaS * sigmaS) ) ) ;
				sum += wcs * h ;
				normalizer += wcs ;
				dd = map.phi1(map.phi2(dd)) ;
			} while (dd != d) ;

			position2[d] = pos_d + ((sum / normalizer) * normal_d) ;
		}
	}
}

template <typename PFP>
void filterSUSAN(typename PFP::MAP& map, float SUSANthreshold, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select)
{
	typedef typename PFP::VEC3 VEC3 ;

	float sigmaC, sigmaS ;
	sigmaBilateral<PFP>(map, position, normal, sigmaC, sigmaS, select) ;

	long nbTot = 0 ;
	long nbSusan = 0 ;

	CellMarker mv(map, VERTEX);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !mv.isMarked(d))
		{
			mv.mark(d);

			// get position & normal of vertex
			const VEC3& pos_d = position[d] ;
			const VEC3& normal_d = normal[d] ;

			// traversal of neighbour vertices
			float sum = 0.0f, normalizer = 0.0f ;
			bool SUSANregion = false ;
			Dart dd = d ;
			do
			{
				Dart e = map.phi2(dd) ;
				const VEC3& neighborNormal = normal[e] ;
				float angle = Geom::angle(normal_d, neighborNormal) ;
				if( angle <= SUSANthreshold )
				{
					VEC3 vec = Algo::Geometry::vectorOutOfDart<PFP>(map, dd, position) ;
					float h = normal_d * vec ;
					float t = vec.norm() ;
					float wcs = exp( ( -1.0f * (t * t) / (2.0f * sigmaC * sigmaC) ) + ( -1.0f * (h * h) / (2.0f * sigmaS * sigmaS) ) );
					sum += wcs * h ;
					normalizer += wcs ;
				}
				else
					SUSANregion = true ;

				dd = map.phi1(e) ;
			} while(dd != d) ;

			if(SUSANregion)
				nbSusan++ ;
			nbTot++ ;

			if (normalizer != 0.0f)
				position2[d] = pos_d + ((sum / normalizer) * normal_d) ;
			else
				position2[d] = pos_d ;
		}
	}

//	CGoGNout <<" susan rate = "<< float(nbSusan)/float(nbTot)<<CGoGNendl;
}

} //namespace Filters2D

} //namespace Algo

} //namespace CGoGN
