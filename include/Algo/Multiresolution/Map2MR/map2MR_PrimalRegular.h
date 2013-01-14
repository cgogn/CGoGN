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

#include "Algo/Multiresolution/filter.h"

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace MR
{

namespace Primal
{

namespace Regular
{

enum FilterType
{
	F_HighPass = 0,
	F_LowPass,
	F_BandPass,
	F_None,
} ;

template <typename PFP>
class Map2MR
{
public:
	typedef typename PFP::MAP MAP ;

protected:
	MAP& m_map;
	bool shareVertexEmbeddings ;

	std::vector<Filter*> synthesisFilters ;
	std::vector<Filter*> analysisFilters ;

	FilterType filter;
	unsigned int thresholdLow;
	unsigned int thresholdHigh;

public:
	Map2MR(MAP& map);

	~Map2MR();

	//if true : tri and quad else quad
	void addNewLevel(bool triQuad = true) ;

	void addNewLevelSqrt3();

	void addNewLevelSqrt2();

	void addSynthesisFilter(Filter* f) { synthesisFilters.push_back(f) ; }
	void addAnalysisFilter(Filter* f) { analysisFilters.push_back(f) ; }

	void clearSynthesisFilters() { synthesisFilters.clear() ; }
	void clearAnalysisFilters() { analysisFilters.clear() ; }

	void analysis() ;
	void synthesis() ;

	void lowPassFiltering(unsigned int cutoff) { thresholdHigh = cutoff; filter = F_LowPass; }
	void highPassFiltering(unsigned int cutoff) { thresholdLow = cutoff; filter = F_HighPass; }
	void bandPassFiltering(unsigned int cutoffLow, unsigned int cutoffHigh) { thresholdLow = cutoffLow; thresholdHigh = cutoffHigh; filter = F_BandPass; }
	void resetFiltering() { thresholdLow = 0; thresholdHigh = 0; filter = F_None; }

} ;

} // namespace Regular

} // namespace Primal

} // namespace MR

} // namespace Surface

} // namespace Algo

} // namespace CGoGN

#include "Algo/Multiresolution/Map2MR/map2MR_PrimalRegular.hpp"

#endif
