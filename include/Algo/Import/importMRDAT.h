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
bool importMRDAT(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames) ;

template <typename PFP>
class QuadTreeNode
{
public:
	QuadTreeNode()
	{
		for(unsigned int i = 0; i < 3; ++i)
			indices[i] = -1 ;
		for(unsigned int i = 0; i < 4; ++i)
			children[i] = NULL ;
		parent = NULL ;
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
		{
			children[i] = new QuadTreeNode() ;
			children[i]->parent = this ;
		}
	}

	bool isSubdivided()
	{
		return children[0] != NULL ;
	}

	void embed(typename PFP::MAP& map, Dart d, std::vector<unsigned int>& vID, CellMarker& cm, bool CCW)
	{
		if(isSubdivided())
		{
			Dart d0 = map.phi1(d) ;
			Dart d1, d2 ;
			if(CCW)
			{
				d1 = map.phi_1(d) ;
				d2 = d ;
			}
			else
			{
				d1 = d ;
				d2 = map.phi_1(d) ;
			}
			map.incCurrentLevel() ;

			unsigned int emb0 = vID[children[0]->indices[0]] ;
			unsigned int e0 = map.getEmbedding(VERTEX, map.phi2(d0)) ;
			if(!cm.isMarked(map.phi2(d0)))
			{
				assert(e0 == EMBNULL) ;
				map.embedOrbit(VERTEX, map.phi2(d0), emb0) ;
				cm.mark(map.phi2(d0)) ;
			}
			else
				assert(e0 == emb0) ;

			unsigned int emb1 = vID[children[0]->indices[1]] ;
			unsigned int e1 = map.getEmbedding(VERTEX, map.phi2(d1)) ;
			if(!cm.isMarked(map.phi2(d1)))
			{
				assert(e1 == EMBNULL) ;
				map.embedOrbit(VERTEX, map.phi2(d1), emb1) ;
				cm.mark(map.phi2(d1)) ;
			}
			else
				assert(e1 == emb1) ;

			unsigned int emb2 = vID[children[0]->indices[2]] ;
			unsigned int e2 = map.getEmbedding(VERTEX, map.phi2(d2)) ;
			if(!cm.isMarked(map.phi2(d2)))
			{
				assert(e2 == EMBNULL) ;
				map.embedOrbit(VERTEX, map.phi2(d2), emb2) ;
				cm.mark(map.phi2(d2)) ;
			}
			else
				assert(e2 == emb2) ;

			map.decCurrentLevel() ;

			Dart t0 = map.phi_1(d) ;
			map.incCurrentLevel() ;
			t0 = map.phi2(map.phi1(t0)) ;
			children[0]->embed(map, t0, vID, cm, CCW) ;
			map.decCurrentLevel() ;

			Dart t1 = d ;
			map.incCurrentLevel() ;
			children[1]->embed(map, t1, vID, cm, !CCW) ;
			map.decCurrentLevel() ;

			Dart t2 = map.phi1(d) ;
			map.incCurrentLevel() ;
			t2 = map.phi1(t2) ;
			children[2]->embed(map, t2, vID, cm, !CCW) ;
			map.decCurrentLevel() ;

			Dart t3 = map.phi_1(d) ;
			map.incCurrentLevel() ;
			t3 = map.phi_1(t3) ;
			children[3]->embed(map, t3, vID, cm, !CCW) ;
			map.decCurrentLevel() ;
		}
		else
		{
			if(map.getCurrentLevel() < map.getMaxLevel())
				std::cout << "adaptive !!!!!!!" << std::endl ;
		}
	}

	void print()
	{
		std::cout << indices[0] << " " << indices[1] << " " << indices[2] << std::endl ;
		if(isSubdivided())
		{
			for(unsigned int i = 0; i < 4; ++i)
				children[i]->print() ;
		}
	}

	unsigned int indices[3] ;
	QuadTreeNode* children[4] ;
	QuadTreeNode* parent ;
} ;

template <typename PFP>
class QuadTree
{
public:
	std::vector<QuadTreeNode<PFP>*> roots ;
	std::vector<Dart> darts ;

	~QuadTree()
	{
		for(unsigned int i = 0; i < roots.size(); ++i)
			delete roots[i] ;
	}

	void embed(typename PFP::MAP& map, std::vector<unsigned int>& vID)
	{
		CellMarker cm(map, VERTEX) ;
		for(unsigned int i = 0; i < roots.size(); ++i)
			roots[i]->embed(map, darts[i], vID, cm, true) ;
	}

	void print()
	{
		std::cout << "printing quadtree (" << roots.size() << " roots)" << std::endl ;
		for(unsigned int i = 0; i < roots.size(); ++i)
		{
			std::cout << "root " << i << std::endl ;
			roots[i]->print() ;
		}
	}
} ;

} // namespace Import

} // namespace Algo

} // namespace CGoGN

#include "Algo/Import/importMRDAT.hpp"

#endif
