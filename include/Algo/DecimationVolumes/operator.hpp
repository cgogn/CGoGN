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

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

/************************************************************************************
 *							Collapse Edge Operator	                                *
 ************************************************************************************/
template <typename PFP>
unsigned int CollapseEdgeOperator<PFP>::perform(MAP& m, VertexAttribute<typename PFP::VEC3>& position)
{
	unsigned int nbCell = 0;

	m.collapseEdge(this->m_edge);
	++nbCell;

	return nbCell;
}

template <typename PFP>
bool CollapseEdgeOperator<PFP>::canPerform(MAP &m ,Dart d, VertexAttribute<typename PFP::VEC3>& position)
{
	bool canCollapse = true;

//	Dart e = d;
//	do
//	{
//		//isBoundaryVolume
//		if(m.isBoundaryVolume(e))
//		{
//			canCollapse = false;
//		}
//		//l'un de ces 2 voisins existe
//		else if(m.phi3(m.phi2(m.phi1(e))) != m.phi2(m.phi1(e)) && m.phi3(m.phi2(m.phi_1(e))) != m.phi2(m.phi_1(e)))
//		{
//			//l'un de ces 2 voisins est au bord
//			if(m.isBoundaryVolume(m.phi3(m.phi2(m.phi1(e)))) || m.isBoundaryVolume(m.phi3(m.phi2(m.phi_1(e)))))
//			{
//				canCollapse = false;
//
//			}
//		}
//		else
//		{
//			//Edge Criteria Valide
//			if(m.edgeDegree(m.phi1(m.phi2(m.phi_1(e)))) < 3)
//				canCollapse = false;
//			elseframe
//			{
//				//Is inverted
//				Dart a = m.phi3(m.phi2(m.phi1(e)));
//				Dart b = m.phi1(m.phi3(m.phi2(m.phi_1(e))));
//
//				typename PFP::VEC3::DATA_TYPE v1;
//				typename PFP::VEC3::DATA_TYPE v2;
//
//				v1 = Algo::Geometry::tetrahedronSignedVolume<PFP>(m,a,position);
//
//				if (v1 < 0)
//					canCollapse = false;
//
//				v2 = Algo::Geometry::tetrahedronSignedVolume<PFP>(m,b,position);
//				if (v2 < 0)
//					canCollapse = false;
//
//				//CGoGNout << " v2 = " << v2;
//			}
//		}
//
//		e = m.alpha2(e);
//	}while ( e != d && canCollapse);

	return canCollapse;
}


} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN
