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

#ifndef __VPMESH__
#define __VPMESH__

#include "Algo/DecimationVolumes/operator.h"
#include "Algo/DecimationVolumes/selector.h"
#include "Algo/DecimationVolumes/edgeSelector.h"
#include "Algo/DecimationVolumes/geometryApproximator.h"
//#include "Algo/DecimationVolumes/geometryPredictor.h"

#include "Utils/quantization.h"

namespace CGoGN
{

namespace Algo
{

namespace VPMesh
{

template <typename PFP>
class VolumetricProgressiveMesh
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& positionsTable ;

	DartMarker& inactiveMarker ;
	SelectorUnmarked dartSelect ;

	Algo::DecimationVolumes::EdgeSelector<PFP>* m_selector ;
	std::vector<Algo::DecimationVolumes::ApproximatorGen<PFP>*> m_approximators ;
	std::vector<Algo::DecimationVolumes::PredictorGen<PFP>*> m_predictors ;
	Algo::DecimationVolumes::OperatorList<PFP>* m_nodes;
	unsigned int m_level ;

	Algo::DecimationVolumes::Approximator<PFP, VEC3>* m_positionApproximator ;

	bool m_initOk ;


public:
	VolumetricProgressiveMesh(
		MAP& map, DartMarker& inactive,
		Algo::DecimationVolumes::SelectorType s, Algo::DecimationVolumes::ApproximatorType a,
		VertexAttribute<typename PFP::VEC3>& position
	) ;

	~VolumetricProgressiveMesh() ;

	bool initOk() { return m_initOk ; }

	void createPM(unsigned int percentWantedVertices) ;
	void gotoLevel(unsigned int l) ;
	unsigned int& currentLevel() { return m_level ; }
	unsigned int nbSplits() { return m_nodes.size() ; }

} ;

} //namespace PMesh

} //namespace Algo

} //namespace CGoGN

#include "Algo/VolumetricProgressiveMesh/vpmesh.hpp"

#endif
