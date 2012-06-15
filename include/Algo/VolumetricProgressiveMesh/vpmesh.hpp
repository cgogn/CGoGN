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

#include "Algo/Geometry/localFrame.h"

namespace CGoGN
{

namespace Algo
{

namespace VPMesh
{

template <typename PFP>
VolumetricProgressiveMesh<PFP>::VolumetricProgressiveMesh(
		MAP& map, DartMarker& inactive,
		Algo::DecimationVolumes::SelectorType s, Algo::DecimationVolumes::ApproximatorType a,
		VertexAttribute<typename PFP::VEC3>& position
	) :
	m_map(map), positionsTable(position), inactiveMarker(inactive), dartSelect(inactiveMarker)
{
	CGoGNout << "  creating approximator and predictor.." << CGoGNflush ;
	switch(a)
	{
		case Algo::DecimationVolumes::A_QEM :
			m_approximators.push_back(new Algo::DecimationVolumes::Approximator_QEM<PFP>(m_map, positionsTable)) ;
			break ;
		default :
			CGoGNout << "not yet implemented" << CGoGNendl;
			break;
	}
	CGoGNout << "..done" << CGoGNendl ;

	CGoGNout << "  creating selector.." << CGoGNflush ;
	switch(s)
	{
		case Algo::DecimationVolumes::S_QEM :
			m_selector = new Algo::DecimationVolumes::EdgeSelector_QEM<PFP>(m_map, positionsTable, m_approximators, dartSelect) ;
			break ;
		default:
			CGoGNout << "not yet implemented" << CGoGNendl;
			break;
	}
	CGoGNout << "..done" << CGoGNendl ;

	m_initOk = true ;

	CGoGNout << "  initializing approximators.." << CGoGNflush ;
	for(typename std::vector<Algo::DecimationVolumes::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
	{
		if(! (*it)->init())
			m_initOk = false ;
		if((*it)->getApproximatedAttributeName() == "position")
			m_positionApproximator = reinterpret_cast<Algo::DecimationVolumes::Approximator<PFP, VEC3>*>(*it) ;
	}
	CGoGNout << "..done" << CGoGNendl ;

	CGoGNout << "  initializing predictors.." << CGoGNflush ;
	for(typename std::vector<Algo::DecimationVolumes::PredictorGen<PFP>*>::iterator it = m_predictors.begin(); it != m_predictors.end(); ++it)
		if(! (*it)->init())
			m_initOk = false ;
	CGoGNout << "..done" << CGoGNendl ;

	CGoGNout << "  initializing selector.." << CGoGNflush ;
	m_initOk = m_selector->init() ;
	CGoGNout << "..done" << CGoGNendl ;


	m_nodes = new Algo::DecimationVolumes::OperatorList<PFP>(m_map);
}

template <typename PFP>
VolumetricProgressiveMesh<PFP>::~VolumetricProgressiveMesh()
{
	for(unsigned int i = 0; i < m_nodes.size(); ++i)
		delete m_nodes[i] ;
	if(m_selector)
		delete m_selector ;
	for(typename std::vector<Algo::DecimationVolumes::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
		delete (*it) ;
	for(typename std::vector<Algo::DecimationVolumes::PredictorGen<PFP>*>::iterator it = m_predictors.begin(); it != m_predictors.end(); ++it)
		delete (*it) ;
}

template <typename PFP>
void VolumetricProgressiveMesh<PFP>::createPM(unsigned int percentWantedVertices)
{
	unsigned int nbVertices = m_map.template getNbOrbits<VERTEX>() ;
	unsigned int nbWantedVertices = nbVertices * percentWantedVertices / 100 ;
	CGoGNout << "  creating PM (" << nbVertices << " vertices).." << /* flush */ CGoGNendl ;

	bool finished = false ;
	Dart d ;
	while(!finished)
	{
		//Next Operator to perform
		Algo::DecimationVolumes::Operator<PFP> *op;

		if((op = m_selector->nextOperator()) == NULL)
			break;

		m_nodes->add(op);

		for(typename std::vector<Algo::DecimationVolumes::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
		{
			(*it)->approximate(op) ;					// compute approximated attributes with its associated detail
			(*it)->saveApprox(op) ;
		}

		//Update the selector before performing operation
		if(!m_selector->updateBeforeOperation(op))
			break;

		nbVertices -= op->collapse(m_map, positionsTable);

		for(typename std::vector<Algo::DecimationVolumes::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
			(*it)->affectApprox(op);				// affect data to the resulting vertex

		m_selector->updateAfterOperation(op) ;	// update selector

		if(nbVertices <= nbWantedVertices)
			finished = true ;
	}

	m_selector->finish() ;

	delete m_selector ;
	m_selector = NULL ;

	m_level = m_nodes->size() ;
	CGoGNout << "..done (" << nbVertices << " vertices)" << CGoGNendl ;
}

template <typename PFP>
void VolumetricProgressiveMesh<PFP>::gotoLevel(unsigned int l)
{
	unsigned int i=0;
	if(l == m_level || l > m_nodes->size() || l < 0)
		return ;

	if(l > m_level)
		for(i=m_level ; i<l ; i++)
			m_nodes->coarsen(positionsTable);
	else
		for(i=l ; i<m_level ; i++)
			m_nodes->refine(positionsTable);

	m_level = i;
}


} //namespace VPMesh

} //namespace Algo

} //namespace CGoGN
