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

#ifndef __MAP_MONO__
#define __MAP_MONO__

#include "attribmap.h"

namespace CGoGN
{

class MapMono : public AttribMap
{
protected:
	std::vector<AttributeMultiVector<Dart>*> m_permutation;
	std::vector<AttributeMultiVector<Dart>*> m_permutation_inv;
	std::vector<AttributeMultiVector<Dart>*> m_involution;

	virtual Dart newDart()
	{

	}

	void addInvolution()
	{
		std::stringstream sstm;
		sstm << "involution_" << m_involution.size();
		m_involution.push_back(addRelation(sstm.str()));
	}

	void addPermutation()
	{
		std::stringstream sstm;
		sstm << "permutation_" << m_permutation.size();
		m_permutation.push_back(addRelation(sstm.str()));
		std::stringstream sstm2;
		sstm2 << "permutation_inv_" << m_permutation_inv.size();
		m_permutation_inv.push_back(addRelation(sstm2.str()));
	}

	template <int I>
	Dart getInvolution(Dart d)
	{
		return (*m_involution[I])[d.index];
	}

	template <int I>
	Dart getPermutation(Dart d)
	{
		return (*m_permutation[I])[d.index];
	}

	template <int I>
	Dart getPermutationInv(Dart d)
	{
		return (*m_permutation_inv[I])[d.index];
	}

	template <int I>
	void permutationSew(Dart d, Dart e)
	{
		Dart f = (*m_permutation[I])[d.index] ;
		Dart g = (*m_permutation[I])[e.index] ;
		(*m_permutation[I])[d.index] = g ;
		(*m_permutation[I])[e.index] = f ;
		(*m_permutation_inv[I])[g.index] = d ;
		(*m_permutation_inv[I])[f.index] = e ;
	}

	template <int I>
	void permutationUnsew(Dart d)
	{
		Dart e = (*m_permutation[I])[d.index] ;
		Dart f = (*m_permutation[I])[e.index] ;
		(*m_permutation[I])[d.index] = f ;
		(*m_permutation[I])[e.index] = e ;
		(*m_permutation_inv[I])[f.index] = d ;
		(*m_permutation_inv[I])[e.index] = e ;
	}

	template <int I>
	void involutionSew(Dart d, Dart e)
	{
		assert((*m_phi2)[d.index] == d) ;
		assert((*m_phi2)[e.index] == e) ;
		(*m_involution[I])[d.index] = e ;
		(*m_involution[I])[e.index] = d ;
	}

	template <int I>
	void involutionUnsew(Dart d)
	{
		Dart e = (*m_involution[I])[d.index] ;
		(*m_involution[I])[d.index] = d ;
		(*m_involution[I])[e.index] = e ;
	}
} ;

} //namespace CGoGN

#endif
