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

	void embed(typename PFP::MAP& map, Dart d, std::vector<unsigned int>& vID, std::vector<unsigned int>& vLev, bool CCW)
	{
		if(isSubdivided())
		{
			std::cout << "embed subdivided edge vertices / current level -> " << map.getCurrentLevel() << std::endl ;

			unsigned int emb0 = vID[children[0]->indices[0]] ;
			unsigned int emb1 = vID[children[0]->indices[1]] ;
			unsigned int emb2 = vID[children[0]->indices[2]] ;

			unsigned int v10 = vID[children[1]->indices[0]] ;
			unsigned int v11 = vID[children[1]->indices[1]] ;
			unsigned int v12 = vID[children[1]->indices[2]] ;
			unsigned int v20 = vID[children[2]->indices[0]] ;
			unsigned int v21 = vID[children[2]->indices[1]] ;
			unsigned int v22 = vID[children[2]->indices[2]] ;
			unsigned int v30 = vID[children[3]->indices[0]] ;
			unsigned int v31 = vID[children[3]->indices[1]] ;
			unsigned int v32 = vID[children[3]->indices[2]] ;

			assert(v11 == emb1) ;
			assert(v12 == emb2) ;
			assert(v20 == emb0) ;
			assert(v22 == emb2) ;
			assert(v30 == emb0) ;
			assert(v31 == emb1) ;

			unsigned int e0L = vLev[children[0]->indices[0]] ;
			unsigned int e1L = vLev[children[0]->indices[1]] ;
			unsigned int e2L = vLev[children[0]->indices[2]] ;

			assert(map.getMaxLevel() + 1 - e0L == map.getCurrentLevel() + 1) ;
			assert(map.getMaxLevel() + 1 - e1L == map.getCurrentLevel() + 1) ;
			assert(map.getMaxLevel() + 1 - e2L == map.getCurrentLevel() + 1) ;
//			assert(e0L == map.getCurrentLevel() + 1 + 1) ;
//			assert(e1L == map.getCurrentLevel() + 1 + 1) ;
//			assert(e2L == map.getCurrentLevel() + 1 + 1) ;

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

			Dart dd0 = map.phi2(d0) ;
			Dart dd1 = map.phi2(d1) ;
			Dart dd2 = map.phi2(d2) ;

			unsigned int e0 = map.getEmbedding(VERTEX, dd0) ;
			unsigned int e1 = map.getEmbedding(VERTEX, dd1) ;
			unsigned int e2 = map.getEmbedding(VERTEX, dd2) ;

			if(e0 == EMBNULL)
			{
				std::cout << "  not visited" << std::endl ;
				map.embedOrbit(VERTEX, dd0, emb0) ;
				map.pushLevel() ;
				for(unsigned int i = map.getCurrentLevel() + 1; i <= map.getMaxLevel(); ++i)
				{
					map.setCurrentLevel(i) ;
					map.embedOrbit(VERTEX, dd0, emb0) ;
				}
				map.popLevel() ;
			}
			else
			{
				std::cout << "  already visited (" << e0 << "," << emb0 << " / " << e1 << "," << emb1 << " / " << e2 << "," << emb2 << ")" << std::endl ;
				assert(e0 == emb0) ;
			}

			if(e1 == EMBNULL)
			{
				std::cout << "  not visited" << std::endl ;
				map.embedOrbit(VERTEX, dd1, emb1) ;
				map.pushLevel() ;
				for(unsigned int i = map.getCurrentLevel() + 1; i <= map.getMaxLevel(); ++i)
				{
					map.setCurrentLevel(i) ;
					map.embedOrbit(VERTEX, dd1, emb1) ;
				}
				map.popLevel() ;
			}
			else
			{
				std::cout << "  already visited (" << e0 << "," << emb0 << " / " << e1 << "," << emb1 << " / " << e2 << "," << emb2 << ")" << std::endl ;
				assert(e1 == emb1) ;
			}

			if(e2 == EMBNULL)
			{
				std::cout << "  not visited" << std::endl ;
				map.embedOrbit(VERTEX, dd2, emb2) ;
				map.pushLevel() ;
				for(unsigned int i = map.getCurrentLevel() + 1; i <= map.getMaxLevel(); ++i)
				{
					map.setCurrentLevel(i) ;
					map.embedOrbit(VERTEX, dd2, emb2) ;
				}
				map.popLevel() ;
			}
			else
			{
				std::cout << "  already visited (" << e0 << "," << emb0 << " / " << e1 << "," << emb1 << " / " << e2 << "," << emb2 << ")" << std::endl ;
				assert(e2 == emb2) ;
			}

			map.decCurrentLevel() ;

			Dart t0 = map.phi_1(d) ;
			map.incCurrentLevel() ;
			t0 = map.phi2(map.phi1(t0)) ;
			children[0]->embed(map, t0, vID, vLev, CCW) ;
			map.decCurrentLevel() ;

			Dart t1 = d ;
			map.incCurrentLevel() ;
			children[1]->embed(map, t1, vID, vLev, !CCW) ;
			map.decCurrentLevel() ;

			Dart t2 = map.phi1(d) ;
			map.incCurrentLevel() ;
			t2 = map.phi1(t2) ;
			children[2]->embed(map, t2, vID, vLev, !CCW) ;
			map.decCurrentLevel() ;

			Dart t3 = map.phi_1(d) ;
			map.incCurrentLevel() ;
			t3 = map.phi_1(t3) ;
			children[3]->embed(map, t3, vID, vLev, !CCW) ;
			map.decCurrentLevel() ;
		}
		else
		{
			if(map.getCurrentLevel() < map.getMaxLevel())
				std::cout << "adaptive subdivision not managed yet" << std::endl ;
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

	void embed(typename PFP::MAP& map, std::vector<unsigned int>& vID, std::vector<unsigned int>& vLev)
	{
		for(unsigned int i = 0; i < roots.size(); ++i)
			roots[i]->embed(map, darts[i], vID, vLev, true) ;
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
