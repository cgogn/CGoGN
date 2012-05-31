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

#ifndef __MAP2MR_PM__
#define __MAP2MR_PM__

#include "Topology/map/embeddedMap2.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2.h"


#include "Algo/Decimation/selector.h"
#include "Algo/Decimation/edgeSelector.h"
#include "Algo/Decimation/geometryApproximator.h"
#include "Algo/Decimation/geometryPredictor.h"
#include "Algo/Decimation/lightfieldApproximator.h"


namespace CGoGN
{

namespace Algo
{

namespace Multiresolution
{

template <typename PFP>
class Map2MR_PM
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

private:
	MAP& m_map ;
	VertexAttribute<VEC3>& m_position;
	bool shareVertexEmbeddings ;

	//SelectorUnmarked dartSelect ;

	bool m_initOk ;

	Algo::Decimation::EdgeSelector<PFP>* m_selector ;
	std::vector<Algo::Decimation::ApproximatorGen<PFP>*> m_approximators ;
	std::vector<Algo::Decimation::PredictorGen<PFP>*> m_predictors ;

	Algo::Decimation::Approximator<PFP, VEC3>* m_positionApproximator ;

public:
	Map2MR_PM(MAP& map, VertexAttribute<VEC3>& position,
			Algo::Decimation::SelectorType s, Algo::Decimation::ApproximatorType a) ;

	~Map2MR_PM();

	//add a coarser level
	void addNewLevel(unsigned int percentWantedVertices);

	//create the progressive meshes
	void createPM(unsigned int percentWantedVertices);

	//coarsen the mesh -> analysis
	void coarsen() ;

	//refine the mesh -> synthesis
	void refine() ;

	bool initOk() { return m_initOk; }
} ;

} // namespace Multiresolution

} // namespace Algo

} // namespace CGoGN


#include "Algo/Multiresolution/map2MR/map2MR_PM.hpp"

#endif
