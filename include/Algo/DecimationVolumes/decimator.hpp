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

template <typename PFP>
void decimate(
	typename PFP::MAP& map, SelectorType s, ApproximatorType a,
	VertexAttribute<typename PFP::VEC3>& position, unsigned int nbWantedVertices, const FunctorSelect& selected
)
{
	std::vector<ApproximatorGen<PFP>*> approximators ;
	Selector<PFP>* selector = NULL ;

	//choose the Approximator
	switch(a)
	{
		case A_Centroid :
			approximators.push_back(new Approximator_Centroid<PFP>(map, position)) ;
			break ;
		default :
			CGoGNout << "not yet implemented" << CGoGNendl;
			break;
	}

	//choose the Selector
	switch(s)
	{
		case S_MapOrder :
			selector = new EdgeSelector_MapOrder<PFP>(map, position, approximators, selected);
			break ;
		case S_Random :
			selector = new EdgeSelector_Random<PFP>(map, position, approximators, selected);
			break ;
		default:
			CGoGNout << "not yet implemented" << CGoGNendl;
			break;
	}

	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		(*it)->init() ;

	if(!selector->init())
		return ;


	unsigned int nbVertices = map.template getNbOrbits<VERTEX>() ;
	bool finished = false ;

	while(!finished)
	{
		//Next Operator to perform
		Operator<PFP> *op;

		if(!selector->nextOperator(op)) //a changer de nom
			break;

//		 switch(selector->nextOperatorType())
//		 {
//		  	 case O_CEdge :
//		  	 	 op = new CollapseEdgeOperator<PFP>(selector->nextCell(), approximators);
//		  	 	 break;
//		  	 case O_CFace :
//		  	 	 break;
//		  	 case O_CVolume:
//		  		 break;
//		 }

		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		{
			(*it)->approximate(op) ;				// compute approximated attributes
		}

		//Update the selector before performing operation
		selector->updateBeforeOperation(op);

		//Perform the topological operation and
		//compute the number of resulting cells
		nbVertices -= op->perform(map, position);

		//for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		//	(*it)->affectApprox(op);			// affect data to the resulting vertex

		//Update the embedded position and
		//search the next operation to perform
		selector->updateAfterOperation(op);

		if(nbVertices <= nbWantedVertices)
			finished = true ;

		delete op;
	}

	selector->finish() ;

	delete selector ;

	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		delete (*it) ;
}

} //namespace DecimationVolumes

} //namespace Algo

} //namespace CGoGN
