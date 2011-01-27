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

#include "Topology/generic/autoAttributeHandler.h"

namespace CGoGN
{

namespace Algo
{

namespace Filters2D
{

/**
 * compute new position of vertices from normals (normalAverage & MMSE filters)
 * @param map the map
 */
template <typename PFP>
void computeNewPositionsFromFaceNormals(
	typename PFP::MAP& map,
	const typename PFP::TVEC3& position,
	typename PFP::TVEC3& position2,
	const typename PFP::TREAL& faceArea,
	const typename PFP::TVEC3& faceCentroid,
	const typename PFP::TVEC3& faceNormal,
	const typename PFP::TVEC3& faceNewNormal,
	const FunctorSelect& select)
{
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	CellMarker markV(map, VERTEX_CELL);
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !markV.isMarked(d))
		{
			markV.mark(d);

			// get pos of vertex
			const VEC3& pos_d = position[d] ;

			// traversal of vertices neighbourhood
			VEC3 displ(0) ;
			REAL sumAreas = 0 ;
			Dart dd = d ;
			do
			{
				sumAreas += faceArea[dd] ;
				VEC3 vT = faceCentroid[dd] - pos_d ;
				const VEC3& normal = faceNewNormal[dd] ;
				REAL scal = vT * normal ;
				vT = scal * faceNormal[dd] ;
				displ += faceArea[dd] * vT ;
				dd = map.phi1(map.phi2(dd)) ;
			} while(dd != d) ;
			displ /= sumAreas ;
			position2[d] = pos_d + displ ;
		}
	}
}


template <typename PFP>
void filterAverageNormals(typename PFP::MAP& map, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select)
{
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	AutoAttributeHandler<REAL> faceArea(map, FACE_ORBIT, "faceArea") ;
	AutoAttributeHandler<VEC3> faceNormal(map, FACE_ORBIT, "faceNormal") ;
	AutoAttributeHandler<VEC3> faceCentroid(map, FACE_ORBIT, "faceCentroid") ;

	Algo::Geometry::computeAreaFaces<PFP>(map, position, faceArea, select) ;
	Algo::Geometry::computeNormalFaces<PFP>(map, position, faceNormal, select) ;
	Algo::Geometry::computeCentroidFaces<PFP>(map, position, faceCentroid, select) ;

	AutoAttributeHandler<VEC3> faceNewNormal(map, FACE_ORBIT, "faceNewNormal") ;

	// Compute new normals
	CellMarker markF(map, FACE_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !markF.isMarked(d))
		{
			markF.mark(d);

			REAL sumArea = 0 ;
			VEC3 meanFilter(0) ;

			// traversal of adjacent faces (by edges and vertices)
			Dart d_n = map.phi1(d) ; // next dart in the face
			Dart d_f = map.phi1(map.phi2(d_n)) ; // test dart for end of vertex turning

			Dart e = map.phi2(d) ; // current dart for the traversal
			Dart d_last = e ;
			do
			{
				// get info from face embedding and sum
				sumArea += faceArea[e] ;
				meanFilter += faceArea[e] * faceNormal[e] ;
				e = map.phi_1(e) ;
				e = map.phi2(e) ;

				if(e == d_f)
				{
					e = map.phi2(d_n) ;
					d_n = map.phi1(d_n) ;
					d_f = map.phi1(map.phi2(d_n)) ;
				}
			} while(e != d_last) ;

			// finalize the computation of meanFilter normal
			meanFilter /= sumArea ;
			meanFilter.normalize() ;
			// and store it
			faceNewNormal[d] = meanFilter ;
		}
	}

	// Compute new vertices position
	computeNewPositionsFromFaceNormals<PFP>(
		map, position, position2, faceArea, faceCentroid, faceNormal, faceNewNormal, select
	) ;
}

template <typename PFP>
void filterMMSE(typename PFP::MAP& map, float sigmaN2, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select)
{
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	AutoAttributeHandler<REAL> faceArea(map, FACE_ORBIT, "faceArea") ;
	AutoAttributeHandler<VEC3> faceNormal(map, FACE_ORBIT, "faceNormal") ;
	AutoAttributeHandler<VEC3> faceCentroid(map, FACE_ORBIT, "faceCentroid") ;

	Algo::Geometry::computeAreaFaces<PFP>(map, position, faceArea, select) ;
	Algo::Geometry::computeNormalFaces<PFP>(map, position, faceNormal, select) ;
	Algo::Geometry::computeCentroidFaces<PFP>(map, position, faceCentroid, select) ;

	AutoAttributeHandler<VEC3> faceNewNormal(map, FACE_ORBIT, "faceNewNormal") ;

	// Compute new normals
	CellMarker markF(map,FACE_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if( select(d) && !markF.isMarked(d))
		{
			markF.mark(d);

			// traversal of neighbour vertices
			REAL sumArea = 0 ;
			REAL sigmaX2 = 0 ;
			REAL sigmaY2 = 0 ;
			REAL sigmaZ2 = 0 ;

			VEC3 meanFilter(0) ;

			// traversal of adjacent faces (by edges and vertices)
			Dart d_n = map.phi1(d) ; // next dart in the face
			Dart d_f = map.phi1(map.phi2(d_n)) ; // test dart for end of vertex turning

			Dart e = map.phi2(d) ; // current dart for the traversal
			Dart d_last = e ;
			do
			{
				// get info from face embedding and sum
				REAL area = faceArea[e] ;
				sumArea += area ;
				VEC3 normal = faceNormal[e] ;
				meanFilter += area * normal ;
				sigmaX2 += area * normal[0] * normal[0] ;
				sigmaY2 += area * normal[1] * normal[1] ;
				sigmaZ2 += area * normal[2] * normal[2] ;
				e = map.phi_1(e) ;
				e = map.phi2(e) ;

				if(e == d_f)
				{
					e = map.phi2(d_n) ;
					d_n = map.phi1(d_n) ;
					d_f = map.phi1(map.phi2(d_n)) ;
				}
			} while(e != d_last) ;

			meanFilter /= sumArea ;
			sigmaX2 /= sumArea ;
			sigmaX2 -= meanFilter[0] * meanFilter[0] ;
			sigmaY2 /= sumArea ;
			sigmaY2 -= meanFilter[1] * meanFilter[1] ;
			sigmaZ2 /= sumArea ;
			sigmaZ2 -= meanFilter[2] * meanFilter[2] ;

			VEC3& oldNormal = faceNormal[d] ;
			VEC3 newNormal ;

			if(sigmaX2 < sigmaN2)
				newNormal[0] = meanFilter[0] ;
			else
			{
				newNormal[0] = (1 - (sigmaN2 / sigmaX2)) * oldNormal[0] ;
				newNormal[0] += (sigmaN2 / sigmaX2) * meanFilter[0] ;
			}
			if(sigmaY2 < sigmaN2)
				newNormal[1] = meanFilter[1] ;
			else
			{
				newNormal[1] = (1 - (sigmaN2 / sigmaY2)) * oldNormal[1] ;
				newNormal[1] += (sigmaN2 / sigmaY2) * meanFilter[1] ;
			}
			if(sigmaZ2 < sigmaN2)
				newNormal[2] = meanFilter[2] ;
			else
			{
				newNormal[2] = (1 - (sigmaN2 / sigmaZ2)) * oldNormal[2] ;
				newNormal[2] += (sigmaN2 / sigmaZ2) * meanFilter[2] ;
			}

			newNormal.normalize() ;
			faceNewNormal[d] = newNormal ;
		}
	}

	// Compute new vertices position
	computeNewPositionsFromFaceNormals<PFP>(
		map, position, position2, faceArea, faceCentroid, faceNormal, faceNewNormal, select
	) ;
}

template <typename PFP>
void filterTNBA(typename PFP::MAP& map, float sigmaN2, float SUSANthreshold, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const FunctorSelect& select)
{
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	AutoAttributeHandler<REAL> faceArea(map, FACE_ORBIT, "faceArea") ;
	AutoAttributeHandler<VEC3> faceNormal(map, FACE_ORBIT, "faceNormal") ;
	AutoAttributeHandler<VEC3> faceCentroid(map, FACE_ORBIT, "faceCentroid") ;

	Algo::Geometry::computeAreaFaces<PFP>(map, position, faceArea, select) ;
	Algo::Geometry::computeNormalFaces<PFP>(map, position, faceNormal, select) ;
	Algo::Geometry::computeCentroidFaces<PFP>(map, position, faceCentroid, select) ;

	AutoAttributeHandler<VEC3> faceNewNormal(map, FACE_ORBIT, "faceNewNormal") ;

	// Compute new normals
	long nbTot = 0 ;
	long nbAdapt = 0 ;
	long nbSusan = 0 ;

	CellMarker markF(map, FACE_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if( select(d) && !markF.isMarked(d))
		{
			markF.mark(d);

			const VEC3& normF = faceNormal[d] ;

			// traversal of neighbour vertices
			REAL sumArea = 0 ;
			REAL sigmaX2 = 0 ;
			REAL sigmaY2 = 0 ;
			REAL sigmaZ2 = 0 ;

			VEC3 meanFilter(0) ;
			bool SUSANregion = false ;

			// traversal of adjacent faces (by edges and vertices)
			Dart d_n = map.phi1(d) ; // next dart in the face
			Dart d_f = map.phi1(map.phi2(d_n)) ; // test dart for end of vertex turning

			Dart e = map.phi2(d) ; // current dart for the traversal
			Dart d_last = e ;
			do
			{
				// get info from face embedding and sum
				const VEC3& normal = faceNormal[e] ;

				float angle = Geom::angle(normF, normal) ;
				if(angle <= SUSANthreshold)
				{
					REAL area = faceArea[e] ;
					sumArea += area ;
					meanFilter += area * normal ;
					sigmaX2 += area * normal[0] * normal[0] ;
					sigmaY2 += area * normal[1] * normal[1] ;
					sigmaZ2 += area * normal[2] * normal[2] ;
				}
				else SUSANregion = true ;

				e = map.phi_1(e) ;
				e = map.phi2(e) ;

				if(e == d_f)
				{
					e = map.phi2(d_n) ;
					d_n = map.phi1(d_n) ;
					d_f = map.phi1(map.phi2(d_n)) ;
				}

			} while(e != d_last) ;

			if(SUSANregion)
				++nbSusan ;

			++nbTot ;

			if(sumArea > 0.0f)
			{
				meanFilter /= sumArea ;
				sigmaX2 /= sumArea ;
				sigmaX2 -= meanFilter[0] * meanFilter[0] ;
				sigmaY2 /= sumArea ;
				sigmaY2 -= meanFilter[1] * meanFilter[1] ;
				sigmaZ2 /= sumArea ;
				sigmaZ2 -= meanFilter[2] * meanFilter[2] ;

				VEC3& oldNormal = faceNormal[d] ;
				VEC3 newNormal ;

				bool adapt = false ;
				if(sigmaX2 < sigmaN2)
					newNormal[0] = meanFilter[0] ;
				else
				{
					adapt = true ;
					newNormal[0] = (1 - (sigmaN2 / sigmaX2)) * oldNormal[0] ;
					newNormal[0] += (sigmaN2 / sigmaX2) * meanFilter[0] ;
				}
				if(sigmaY2 < sigmaN2)
					newNormal[1] = meanFilter[1] ;
				else
				{
					adapt = true ;
					newNormal[1] = (1 - (sigmaN2 / sigmaY2)) * oldNormal[1] ;
					newNormal[1] += (sigmaN2 / sigmaY2) * meanFilter[1] ;
				}
				if(sigmaZ2 < sigmaN2)
					newNormal[2] = meanFilter[2] ;
				else
				{
					adapt = true ;
					newNormal[2] = (1 - (sigmaN2 / sigmaZ2)) * oldNormal[2] ;
					newNormal[2] += (sigmaN2 / sigmaZ2) * meanFilter[2] ;
				}
				if(adapt)
					++nbAdapt ;

				newNormal.normalize() ;
				faceNewNormal[d] = newNormal;
			}
			else
			{
				faceNewNormal[d] = normF ;
			}
		}
	}

	// Compute new vertices position
	computeNewPositionsFromFaceNormals<PFP>(
		map, position, position2, faceArea, faceCentroid, faceNormal, faceNewNormal, select
	) ;

//	std::cout <<" susan rate = "<< float(nbSusan)/float(nbTot)<<std::endl;
//	std::cout <<" adaptive rate = "<< float(nbAdapt)/float(nbTot)<<std::endl;
}

template <typename PFP>
void filterVNBA(typename PFP::MAP& map, float sigmaN2, float SUSANthreshold, const typename PFP::TVEC3& position, typename PFP::TVEC3& position2, const typename PFP::TVEC3& normal, const FunctorSelect& select)
{
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	AutoAttributeHandler<REAL> faceArea(map, FACE_ORBIT, "faceArea") ;
	AutoAttributeHandler<VEC3> faceNormal(map, FACE_ORBIT, "faceNormal") ;
	AutoAttributeHandler<VEC3> faceCentroid(map, FACE_ORBIT, "faceCentroid") ;

	Algo::Geometry::computeAreaFaces<PFP>(map, position, faceArea, select) ;
	Algo::Geometry::computeNormalFaces<PFP>(map, position, faceNormal, select) ;
	Algo::Geometry::computeCentroidFaces<PFP>(map, position, faceCentroid, select) ;

	AutoAttributeHandler<REAL> vertexArea(map, VERTEX_ORBIT, "vertexArea") ;
	AutoAttributeHandler<VEC3> faceNewNormal(map, FACE_ORBIT, "faceNewNormal") ;
	AutoAttributeHandler<VEC3> vertexNewNormal(map, VERTEX_ORBIT, "vertexNewNormal") ;

	long nbTot = 0 ;
	long nbAdapt = 0 ;
	long nbSusan = 0 ;

	CellMarker markV(map, VERTEX_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if( select(d) && !markV.isMarked(d))
		{
			markV.mark(d) ;

			const VEC3& normV = normal[d] ;

			// traversal of neighbour vertices
			REAL sumArea = 0 ;
			REAL sigmaX2 = 0 ;
			REAL sigmaY2 = 0 ;
			REAL sigmaZ2 = 0 ;

			VEC3 meanFilter(0) ;

			bool SUSANregion = false ;

			Dart dd = d ;
			do
			{
				Dart e = map.phi2(dd) ;
				const VEC3& neighborNormal = normal[e] ;

				float angle = Geom::angle(normV, neighborNormal) ;
				if( angle <= SUSANthreshold )
				{
					REAL umbArea = 0 ;
			        Dart ee = e ;
			        do
			        {
			        	umbArea += faceArea[ee] ;
			        	ee = map.phi1(map.phi2(ee)) ;
			        } while(ee!=e) ;
			        vertexArea[e] = umbArea ;

			        sumArea += umbArea ;
					sigmaX2 += umbArea * neighborNormal[0] * neighborNormal[0] ;
					sigmaY2 += umbArea * neighborNormal[1] * neighborNormal[1] ;
					sigmaZ2 += umbArea * neighborNormal[2] * neighborNormal[2] ;
					meanFilter += neighborNormal * umbArea ;
				}
				else SUSANregion = true ;

				dd = map.phi1(e) ;
			} while(dd != d) ;

			if(SUSANregion)
				++nbSusan ;

			++nbTot ;

			if(sumArea > 0.0f)
			{
				meanFilter /= sumArea ;
				sigmaX2 /= sumArea ;
				sigmaX2 -= meanFilter[0] * meanFilter[0] ;
				sigmaY2 /= sumArea ;
				sigmaY2 -= meanFilter[1] * meanFilter[1] ;
				sigmaZ2 /= sumArea ;
				sigmaZ2 -= meanFilter[2] * meanFilter[2] ;

				VEC3 newNormal ;
				bool adapt = false ;
				if(sigmaX2 < sigmaN2)
					newNormal[0] = meanFilter[0] ;
				else
				{
					adapt = true ;
					newNormal[0] = (1 - (sigmaN2 / sigmaX2)) * normV[0] ;
					newNormal[0] += (sigmaN2 / sigmaX2) * meanFilter[0] ;
				}
				if(sigmaY2 < sigmaN2)
					newNormal[1] = meanFilter[1] ;
				else
				{
					adapt = true ;
					newNormal[1] = (1 - (sigmaN2 / sigmaY2)) * normV[1] ;
					newNormal[1] += (sigmaN2 / sigmaY2) * meanFilter[1] ;
				}
				if(sigmaZ2 < sigmaN2)
					newNormal[2] = meanFilter[2] ;
				else
				{
					adapt = true ;
					newNormal[2] = (1 - (sigmaN2 / sigmaZ2)) * normV[2] ;
					newNormal[2] += (sigmaN2 / sigmaZ2) * meanFilter[2] ;
				}

				if(adapt)
					++nbAdapt ;

				newNormal.normalize() ;
				vertexNewNormal[d] = newNormal ;
			}
			else
			{
				vertexNewNormal[d] = normV ;
			}
		}
	}

	// Compute face normals from vertex normals
	CellMarker markF(map,FACE_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if (select(d) && !markF.isMarked(d))
		{
			markF.mark(d);
            VEC3 newNormal(0) ;
            REAL totArea = 0 ;
            Dart dd = d ;
            do
            {
            	VEC3 vNorm = vertexNewNormal[dd] ;
            	REAL area = vertexArea[dd] ;
            	totArea += area ;
            	vNorm *= area ;
            	newNormal += vNorm ;
            	dd = map.phi1(dd) ;
            } while(dd != d) ;
            newNormal.normalize() ;
            faceNewNormal[d] = newNormal ;
		}
	}

	// Compute new vertices position
	computeNewPositionsFromFaceNormals<PFP>(
		map, position, position2, faceArea, faceCentroid, faceNormal, faceNewNormal, select
	) ;

//	std::cout <<" susan rate = "<< float(nbSusan)/float(nbTot)<<std::endl;
//	std::cout <<" adaptive rate = "<< float(nbAdapt)/float(nbTot)<<std::endl;
}

} //namespace Filters2D

} //namespace Algo

} //namespace CGoGN
