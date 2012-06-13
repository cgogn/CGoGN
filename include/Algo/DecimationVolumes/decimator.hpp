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
	VertexAttribute<typename PFP::VEC3>& position, unsigned int percentWantedVertices, const FunctorSelect& selected
)
{
	std::vector<ApproximatorGen<PFP>*> approximators ;
	EdgeSelector<PFP>* selector = NULL ;

	//choose the Approximator
	switch(a)
	{
		case A_QEM :
			approximators.push_back(new Approximator_QEM<PFP>(map, position)) ;
			break ;
		default :
			CGoGNout << "not yet implemented" << CGoGNendl;
			break;
	}

	//choose the Selector
	switch(s)
	{
		case S_QEM :
			selector = new EdgeSelector_QEM<PFP>(map, position, approximators, selected) ;
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
	unsigned int nbWantedVertices = nbVertices * percentWantedVertices / 100 ;
	CGoGNout << " decimate (" << nbVertices << " vertices).." << /* flush */ CGoGNendl ;
	bool finished = false ;

	Dart d;

	while(!finished)
	{
		//Next Operator to perform
		Operator<PFP> *op;

		//if(!selector->nextOperator(&op))
		if((op = selector->nextOperator()) == NULL)
			break;

		// compute approximated attributes
		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		{
			(*it)->approximate(op) ;
			(*it)->saveApprox(op) ;
		}

		//Update the selector before performing operation
		if(!selector->updateBeforeOperation(op))
			break;

		//Perform the topological operation and
		//compute the number of resulting cells
		nbVertices -= op->perform(map, position);

		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
			(*it)->affectApprox(op);			// affect data to the resulting vertex

		//Update the embedded position and
		//search the next operation to perform
		selector->updateAfterOperation(op);

		if(nbVertices <= nbWantedVertices)
			finished = true ;

		delete op;
	}

	selector->finish() ;

	CGoGNout << "..done (" << nbVertices << " vertices)" << CGoGNendl ;

	delete selector ;

	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		delete (*it) ;
}

} //namespace DecimationVolumes

} //namespace Algo

} //namespace CGoGN

//		if(!selector->nextEdge(d))
//			break ;
//
//
//		Dart d2 = map.phi2(map.phi_1(d)) ;
//		Dart dd2 = map.phi2(map.phi_1(map.phi2(d))) ;
//
//		std::cout << "bin a contracter : " << d << std::endl;
//		std::cout << "voisin d2 : " << d2 << std::endl;
//		std::cout << "voisin dd2 : " << dd2 << std::endl;
//
//		--nbVertices ;
//
//		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
//		{
//			(*it)->approximate(d) ;				// compute approximated attributes
//			(*it)->saveApprox(d) ;
//		}
//
//		selector->updateBeforeCollapse(d) ;		// update selector
//
//		map.collapseEdge(d) ;					// collapse edge
//
//		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
//			(*it)->affectApprox(d2);			// affect data to the resulting vertex
//
//		selector->updateAfterCollapse(d2, dd2) ;// update selector
//
//		if(nbVertices <= nbWantedVertices)
//			finished = true ;

