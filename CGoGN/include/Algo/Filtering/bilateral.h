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

#include <cmath>
#include "Topology/generic/traversor/traversorCell.h"
#include "Topology/generic/traversor/traversor2.h"
#include "Algo/Geometry/basic.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Filtering
{

template <typename PFP>
void sigmaBilateral(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& normal, typename PFP::REAL& sigmaC, typename PFP::REAL& sigmaS)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	REAL sumLengths = 0.0f;
	REAL sumAngles = 0.0f;
	long nbEdges = 0 ;

	TraversorE<typename PFP::MAP> t(map);
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		sumLengths += Algo::Geometry::edgeLength<PFP>(map, d, position) ;
		sumAngles += Geom::angle(normal[d], normal[map.phi1(d)]) ;
		++nbEdges ;
	}

	// update of returned values
	sigmaC = 1.0f * (sumLengths / REAL(nbEdges));
	sigmaS = 2.5f * (sumAngles / REAL(nbEdges));
}

/**
 * \brief Function applying a bilateral filter smoothing on the mesh.
 * \param map the map of the mesh
 * \param positionIn the current positions container of the mesh
 * \param positionOut the smoothed positions after the function call
 * \param normal the normals
 */
template <typename PFP>
void filterBilateral(
        typename PFP::MAP& map,
        const VertexAttribute<typename PFP::VEC3,typename PFP::MAP>& positionIn,
        VertexAttribute<typename PFP::VEC3,typename PFP::MAP>& positionOut,
        const VertexAttribute<typename PFP::VEC3,typename PFP::MAP>& normal)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

	REAL sigmaC, sigmaS;
    sigmaBilateral<PFP>(map, positionIn, normal, sigmaC, sigmaS) ;

	TraversorV<typename PFP::MAP> t(map) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		if(!map.isBoundaryVertex(d))
		{
			// get normal of vertex
			const VEC3& normal_d = normal[d] ;

			// traversal of incident edges
			REAL sum = 0.0f, normalizer = 0.0f;
			Traversor2VE<typename PFP::MAP> te(map, d) ;
			for(Dart it = te.begin(); it != te.end(); it = te.next())
			{
                VEC3 vec = Algo::Geometry::vectorOutOfDart<PFP>(map, it, positionIn) ;
				REAL h = normal_d * vec;
				REAL t = vec.norm();
				REAL wcs = std::exp((-1.0f * (t * t) / (2.0f * sigmaC * sigmaC)) + (-1.0f * (h * h) / (2.0f * sigmaS * sigmaS)));
				sum += wcs * h ;
				normalizer += wcs ;
			}

            positionOut[d] = positionIn[d] + ((sum / normalizer) * normal_d) ;
		}
		else
            positionOut[d] = positionIn[d] ;
	}
}

template <typename PFP>
void filterSUSAN(typename PFP::MAP& map, float SUSANthreshold, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position, VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& position2, const VertexAttribute<typename PFP::VEC3, typename PFP::MAP>& normal)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL;

	REAL sigmaC, sigmaS;
	sigmaBilateral<PFP>(map, position, normal, sigmaC, sigmaS) ;

	long nbTot = 0 ;
	long nbSusan = 0 ;

	TraversorV<typename PFP::MAP> t(map) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		if(!map.isBoundaryVertex(d))
		{
			// get position & normal of vertex
			const VEC3& pos_d = position[d] ;
			const VEC3& normal_d = normal[d] ;

			// traversal of incident edges
			REAL sum = 0.0f, normalizer = 0.0f;
			bool SUSANregion = false ;
			Traversor2VE<typename PFP::MAP> te(map, d) ;
			for(Dart it = te.begin(); it != te.end(); it = te.next())
			{
				const VEC3& neighborNormal = normal[map.phi1(it)] ;
				REAL angle = Geom::angle(normal_d, neighborNormal);
				if( angle <= SUSANthreshold )
				{
					VEC3 vec = Algo::Geometry::vectorOutOfDart<PFP>(map, it, position) ;
					REAL h = normal_d * vec;
					REAL t = vec.norm();
					REAL wcs = std::exp((-1.0f * (t * t) / (2.0f * sigmaC * sigmaC)) + (-1.0f * (h * h) / (2.0f * sigmaS * sigmaS)));
					sum += wcs * h ;
					normalizer += wcs ;
				}
				else
					SUSANregion = true ;
			}

			if(SUSANregion)
				nbSusan++ ;
			nbTot++ ;

			if (normalizer != 0.0f)
				position2[d] = pos_d + ((sum / normalizer) * normal_d) ;
			else
				position2[d] = pos_d ;
		}
		else
			position2[d] = position[d] ;
	}

//	CGoGNout <<" susan rate = "<< float(nbSusan)/float(nbTot)<<CGoGNendl;
}

} //namespace Filtering

}

} //namespace Algo

} //namespace CGoGN
