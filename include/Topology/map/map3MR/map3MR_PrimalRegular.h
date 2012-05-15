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

#ifndef __MAP3MR_PRIMAL__
#define __MAP3MR_PRIMAL__

#include "Topology/map/embeddedMap3.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversor3.h"

#include "Topology/map/map3MR/filters_Primal.h"
#include "Topology/map/map3MR/schemes_Primal.h"

namespace CGoGN
{

/*! \brief The class of regular 3-map MR
 */

class Map3MR_PrimalRegular : public EmbeddedMap3
{
protected:
	bool shareVertexEmbeddings;

	std::vector<Multiresolution::MRFilter*> synthesisFilters ;
	std::vector<Multiresolution::MRFilter*> analysisFilters ;

	std::vector<Multiresolution::MRScheme*> subdivisionSchemes;

public:
	Map3MR_PrimalRegular();

	virtual std::string mapTypeName() const { return "Map3MR_PrimalRegular"; }

	/*! @name Topological helping functions
	 *
	 *************************************************************************/
	//@{
	void swapEdges(Dart d, Dart e);

	bool isTetrahedron(Dart d);

	void splitSurfaceInVolume(std::vector<Dart>& vd, bool firstSideClosed = true, bool secondSideClosed = false);
	//@}

	void setSharingVertexEmbeddings(bool b) { shareVertexEmbeddings = b; }

	/*! @name Level creation
	 *
	 *************************************************************************/
	//@{
	//!
	/*
	 *
	 */
	void addNewLevelTetraOcta(bool embedNewVertices);

	//!
	/*
	 *
	 */
	void addNewLevelHexa(bool embedNewVertices);

	//!
	/*
	 *
	 */
	void addNewLevel(bool embedNewVertices);
	//@}

	/*! @name Geometry modification
	 *  Analysis / Synthesis
	 *************************************************************************/
	//@{
	//!
	/*
	 *
	 */
	void addSynthesisFilter(Multiresolution::MRFilter* f) { assert(shareVertexEmbeddings); synthesisFilters.push_back(f) ; }

	//!
	/*
	 *
	 */
	void addAnalysisFilter(Multiresolution::MRFilter* f) { assert(shareVertexEmbeddings); analysisFilters.push_back(f) ; }

	//!
	/*
	 *
	 */
	void clearSynthesisFilters() { synthesisFilters.clear() ; }

	//!
	/*
	 *
	 */
	void clearAnalysisFilters() { analysisFilters.clear() ; }

	//!
	/*
	 *
	 */
	void analysis() ;

	//!
	/*
	 *
	 */
	void synthesis() ;
	//@}


	/*! @name Geometry modification
	 *  Subdivision Schemes
	 *************************************************************************/
	//@{

	//!
	/*!
	 *
	 */
	void addSubdivisionScheme(Multiresolution::MRScheme* f) {  assert(!shareVertexEmbeddings); subdivisionSchemes.push_back(f); }

	//!
	/*
	 *
	 */
	void clearSubdivisionSchemes() { subdivisionSchemes.clear() ; }

	//!
	/*
	 *
	 */
	void subdivision() ;
	//@}
};

} //end namespace CGoGN

#endif /* __MAP3MR_PRIMAL__ */

//void splitFaceInSurface(Dart d, Dart e);

//Dart cutEdgeInSurface(Dart d);

//void saveRelationsAroundVertex(Dart d, std::vector<std::pair<Dart, Dart> >& vd);

//void unsewAroundVertex(std::vector<std::pair<Dart, Dart> >& vd);

//Dart quadranguleFace(Dart d);
