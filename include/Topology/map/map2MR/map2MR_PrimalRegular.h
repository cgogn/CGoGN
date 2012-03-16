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

#ifndef __MAP2MR_PRIMAL_REGULAR__
#define __MAP2MR_PRIMAL_REGULAR__

#include "Topology/map/embeddedMap2.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor2.h"

#include "Topology/map/map2MR/filters_Primal.h"

namespace CGoGN
{

class Map2MR_PrimalRegular : public EmbeddedMap2
{
protected:
	bool shareVertexEmbeddings ;

	std::vector<Multiresolution::MRFilter*> synthesisFilters ;
	std::vector<Multiresolution::MRFilter*> analysisFilters ;

public:
	Map2MR_PrimalRegular() ;

	std::string mapTypeName() { return "Map2MR_PrimalRegular" ; }

	void addNewLevel(bool embedNewVertices) ;

	void addSynthesisFilter(Multiresolution::MRFilter* f) { synthesisFilters.push_back(f) ; }
	void addAnalysisFilter(Multiresolution::MRFilter* f) { analysisFilters.push_back(f) ; }

	void clearSynthesisFilters() { synthesisFilters.clear() ; }
	void clearAnalysisFilters() { analysisFilters.clear() ; }

	void analysis() ;
	void synthesis() ;
} ;

} // namespace CGoGN

#endif
