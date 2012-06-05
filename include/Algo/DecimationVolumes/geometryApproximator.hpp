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

#include "Algo/DecimationVolumes/selector.h"
#include "Algo/Geometry/centroid.h"

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

/************************************************************************************
 *							      Centroid		                                    *
 ************************************************************************************/
template <typename PFP>
void Approximator_Centroid<PFP>::init()
{

}

template <typename PFP>
void Approximator_Centroid<PFP>::approximate(Operator<PFP> *op)
{
	Dart d = op->getEdge();
	VEC3 a = this->m_attrV[d];

	CGoGNout << "d=" << d << " a=" << a << CGoGNendl;

	// get some darts
	Dart dd = this->m_map.phi2(d) ;

	switch(op->getType())
	{
		case O_CVolume : {
			a = Algo::Geometry::volumeCentroid<PFP>(this->m_map, d, this->m_attrV);
			break;
		}
		case O_CFace : {
			a = Algo::Geometry::faceCentroid<PFP>(this->m_map, d, this->m_attrV);
			break;
		}
		case O_CEdge : {
			// get the contracted edge vertices positions
			VEC3 v2 = this->m_attrV[dd] ;

			// Compute the approximated position
			a =  (a + v2) / REAL(2) ;
			break;
		}
		default : {
			CGoGNerr << "Approximate : reducer does not exist" << CGoGNendl;
			break;
		}
	}

	this->m_app = a;
}


} //end namespace DecimationVolumes

} //end namespace Algo

} //end namespace CGoGN
