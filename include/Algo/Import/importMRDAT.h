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

#ifndef __IMPORT_MR_DAT__
#define __IMPORT_MR_DAT__

namespace CGoGN
{

namespace Algo
{

namespace Import
{

template <typename PFP>
bool importMRDAT(typename PFP::MAP& map, const std::string& filename, typename PFP::TVEC3& position) ;

class QuadTreeNode
{
public:
	QuadTreeNode()
	{
		for(unsigned int i = 0; i < 3; ++i)
			indices[i] = -1 ;
		for(unsigned int i = 0; i < 4; ++i)
			children[i] = NULL ;
	}

	~QuadTreeNode()
	{
		for(unsigned int i = 0; i < 4; ++i)
			if(children[i] != NULL)
				delete children[i] ;
	}

	void subdivide()
	{
		assert(children[0] == NULL) ;
		for(unsigned int i = 0; i < 4; ++i)
			children[i] = new QuadTreeNode() ;
	}

	unsigned int indices[3] ;
	QuadTreeNode* children[4] ;
} ;

class QuadTree
{
public:
	std::vector<QuadTreeNode*> roots ;

	~QuadTree()
	{
		for(unsigned int i = 0; i < roots.size(); ++i)
			delete roots[i] ;
	}
} ;

} // namespace Import

} // namespace Algo

} // namespace CGoGN

#include "Algo/Import/importMRDAT.hpp"

#endif
