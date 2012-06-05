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

#include "Algo/DecimationVolumes/geometryApproximator.h"
#include <time.h>

namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumes
{

/************************************************************************************
 *							Map Order Edge Selector	                                *
 ************************************************************************************/

template <typename PFP>
void EdgeSelector_MapOrder<PFP>::init()
{
	cur = this->m_map.begin() ;

	//TODO Choix de nextOperator
	//this->nextOp = Algo::DecimationVolumique::O_CEdge;
}

template <typename PFP>
bool EdgeSelector_MapOrder<PFP>::nextOperator(Operator<PFP>* op)
{
	if(cur == this->m_map.end())
		return false ;

	return true ;
}

template <typename PFP>
void EdgeSelector_MapOrder<PFP>::updateAfterOperation(Operator<PFP>* op)
{
	MAP& m = this->m_map ;

	//Mise à jour de nextOperator


	//Update the current dart
	if(this->nextOp == Algo::DecimationVolumique::O_CEdge)
	{
		cur = m.begin() ;

		while(!op->canPerform(m,cur,this->m_position))
		{
			//CGoGNout << "update " << cur << CGoGNendl;
			m.next(cur) ;
			if(cur == m.end())
				break ;
		}
	}
}

/************************************************************************************
 *							Random Edge Selector	                                *
 ************************************************************************************/

template <typename PFP>
void EdgeSelector_Random<PFP>::init()
{
	MAP& m = this->m_map ;

	darts.reserve(m.getNbDarts()) ;
	darts.clear() ;

	for(Dart d = m.begin(); d != m.end(); m.next(d))
		darts.push_back(d) ;

	srand(time(NULL)) ;
	int remains = darts.size() ;
	for(unsigned int i = 0; i < darts.size()-1; ++i) // generate the random permutation
	{
		int r = (rand() % remains) + i ;
		// swap ith and rth elements
		Dart tmp = darts[i] ;
		darts[i] = darts[r] ;
		darts[r] = tmp ;
		--remains ;
	}
	cur = 0 ;
	allSkipped = true ;

	//TODO Choix de nextOperator
	this->nextOp = Algo::DecimationVolumique::O_CEdge;
}

template <typename PFP>
bool EdgeSelector_Random<PFP>::nextOperator(Operator<PFP>* op)
{
	if(cur == darts.size() && allSkipped)
		return false ;

	return true ;
}


template <typename PFP>
void EdgeSelector_Random<PFP>::updateAfterOperation(Operator<PFP>* op)
{
	MAP& m = this->m_map ;
	allSkipped = false ;

	if(this->nextOp == Algo::DecimationVolumique::O_CEdge)
	{
//		for(typename std::vector<Dart>::iterator it = darts.begin() ; it != darts.end() ; ++it)
//			CGoGNout << *it << CGoGNendl;

		do
		{
			++cur ;
			if(cur == darts.size())
			{
				//cur = 0 ;
				allSkipped = true ;
			}
		} while(!allSkipped && !op->canPerform(m,darts[cur],this->m_position)) ;
	}
}

///************************************************************************************
// *							Edge Length Selector	                                *
// ************************************************************************************/
//template <typename PFP>
//void EdgeSelector_Length<PFP>::init()
//{
//	MAP& m = this->m_map ;
//
//	edges.clear() ;
//
//	CellMarker eMark(m, EDGE) ;
//	for(Dart d = m.begin(); d != m.end(); m.next(d))
//	{
//		if(!eMark.isMarked(d))
//		{
//			initEdgeInfo(d) ;
//			eMark.mark(d) ;
//		}
//	}
//
//	cur = edges.begin() ; // init the current edge to the first one
//
//	//TODO Choix de nextOperator
//	this->nextOp = Algo::DecimationVolumique::O_CEdge;
//}
//
//template <typename PFP>
//bool EdgeSelector_Length<PFP>::nextOperator(Operator<PFP>* op)
//{
//	if(cur == edges.end() || edges.empty())
//		return false ;
//
//	return true ;
//}
//
//template <typename PFP>
//void EdgeSelector_Length<PFP>::updateBeforeOperation(Operator<PFP>* op)
//{
//	MAP& m = this->m_map;
//	Dart d = op->getEdge();
//
//
//	//Remove all the concerned darts from the multimap
//}
//
//template <typename PFP>
//void EdgeSelector_Length<PFP>::updateAfterOperation(Operator<PFP>* op)
//{
//
//}
//
//template <typename PFP>
//void EdgeSelector_Length<PFP>::initEdgeInfo(Dart d)
//{
//	MAP& m = this->m_map ;
//	EdgeInfo einfo ;
//	if(m.edgeCanCollapse(d))
//		computeEdgeInfo(d, einfo) ;
//	else
//		einfo.valid = false ;
//	edgeInfo[d] = einfo ;
//}

} //end namespace DecimationVolumique
} //end namespace Algo
} //end namespace CGoGN
