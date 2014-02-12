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

namespace CGoGN
{

/****************************************
 *          DARTS MANAGEMENT            *
 ****************************************/

inline Dart MapMono::newDart()
{
	Dart d = GenericMap::newDart() ;

	for (unsigned int i = 0; i < m_permutation.size(); ++i)
		(*m_permutation[i])[d.index] = d ;
	for (unsigned int i = 0; i < m_permutation_inv.size(); ++i)
		(*m_permutation_inv[i])[d.index] = d ;
	for (unsigned int i = 0; i < m_involution.size(); ++i)
		(*m_involution[i])[d.index] = d ;

	return d ;
}

inline void MapMono::deleteDart(Dart d)
{
	deleteDartLine(d.index) ;
}

inline AttributeContainer& MapMono::getDartContainer()
{
	return m_attribs[DART];
}

/****************************************
 *        RELATIONS MANAGEMENT          *
 ****************************************/

inline void MapMono::addInvolution()
{
	std::stringstream sstm;
	sstm << "involution_" << m_involution.size();
	m_involution.push_back(addRelation(sstm.str()));
}

inline void MapMono::addPermutation()
{
	std::stringstream sstm;
	sstm << "permutation_" << m_permutation.size();
	m_permutation.push_back(addRelation(sstm.str()));
	std::stringstream sstm2;
	sstm2 << "permutation_inv_" << m_permutation_inv.size();
	m_permutation_inv.push_back(addRelation(sstm2.str()));
}

template <int I>
inline Dart MapMono::getInvolution(Dart d) const
{
	return (*m_involution[I])[d.index];
}

template <int I>
inline Dart MapMono::getPermutation(Dart d) const
{
	return (*m_permutation[I])[d.index];
}

template <int I>
inline Dart MapMono::getPermutationInv(Dart d) const
{
	return (*m_permutation_inv[I])[d.index];
}

template <int I>
inline void MapMono::permutationSew(Dart d, Dart e)
{
	Dart f = (*m_permutation[I])[d.index] ;
	Dart g = (*m_permutation[I])[e.index] ;
	(*m_permutation[I])[d.index] = g ;
	(*m_permutation[I])[e.index] = f ;
	(*m_permutation_inv[I])[g.index] = d ;
	(*m_permutation_inv[I])[f.index] = e ;
}

template <int I>
inline void MapMono::permutationUnsew(Dart d)
{
	Dart e = (*m_permutation[I])[d.index] ;
	Dart f = (*m_permutation[I])[e.index] ;
	(*m_permutation[I])[d.index] = f ;
	(*m_permutation[I])[e.index] = e ;
	(*m_permutation_inv[I])[f.index] = d ;
	(*m_permutation_inv[I])[e.index] = e ;
}

template <int I>
inline void MapMono::involutionSew(Dart d, Dart e)
{
	assert((*m_involution[I])[d.index] == d) ;
	assert((*m_involution[I])[e.index] == e) ;
	(*m_involution[I])[d.index] = e ;
	(*m_involution[I])[e.index] = d ;
}

template <int I>
inline void MapMono::involutionUnsew(Dart d)
{
	Dart e = (*m_involution[I])[d.index] ;
	(*m_involution[I])[d.index] = d ;
	(*m_involution[I])[e.index] = e ;
}

inline void MapMono::compactTopoRelations(const std::vector<unsigned int>& oldnew)
{
	for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
	{
		for (unsigned int j = 0; j < m_permutation.size(); ++j)
		{
			Dart d = (*m_permutation[j])[i];
			if (d.index != oldnew[d.index])
				(*m_permutation[j])[i] = Dart(oldnew[d.index]);
		}
		for (unsigned int j = 0; j < m_permutation_inv.size(); ++j)
		{
			Dart d = (*m_permutation_inv[j])[i];
			if (d.index != oldnew[d.index])
				(*m_permutation_inv[j])[i] = Dart(oldnew[d.index]);
		}
		for (unsigned int j = 0; j < m_involution.size(); ++j)
		{
			Dart d = (*m_involution[j])[i];
			if (d.index != oldnew[d.index])
				(*m_involution[j])[i] = Dart(oldnew[d.index]);
		}
	}
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

inline Dart MapMono::begin() const
{
	return Dart::create(m_attribs[DART].begin()) ;
}

inline Dart MapMono::end() const
{
	return Dart::create(m_attribs[DART].end()) ;
}

inline void MapMono::next(Dart& d) const
{
	m_attribs[DART].next(d.index) ;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

template <unsigned int ORBIT>
inline unsigned int MapMono::getEmbedding(Dart d) const
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int d_index = dartIndex(d);

	if (ORBIT == DART)
		return d_index;

	return (*m_embeddings[ORBIT])[d_index] ;
}

template <unsigned int ORBIT>
void MapMono::setDartEmbedding(Dart d, unsigned int emb)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int old = getEmbedding<ORBIT>(d);

	if (old == emb)	// if same emb
		return;		// nothing to do

	if (old != EMBNULL)	// if different
	{
		if(m_attribs[ORBIT].unrefLine(old))	// then unref the old emb
		{
			for (unsigned int t = 0; t < m_nbThreads; ++t)	// clear the markers if it was the
				(*m_markTables[ORBIT][t])[old].clear();		// last unref of the line
		}
	}

	if (emb != EMBNULL)
		m_attribs[ORBIT].refLine(emb);	// ref the new emb

	(*m_embeddings[ORBIT])[dartIndex(d)] = emb ; // finally affect the embedding to the dart
}

template <unsigned int ORBIT>
void MapMono::initDartEmbedding(Dart d, unsigned int emb)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	assert(getEmbedding<ORBIT>(d) == EMBNULL || !"initDartEmbedding called on already embedded dart");

	if(emb != EMBNULL)
		m_attribs[ORBIT].refLine(emb);	// ref the new emb
	(*m_embeddings[ORBIT])[dartIndex(d)] = emb ; // affect the embedding to the dart
}

template <unsigned int ORBIT>
inline void MapMono::copyDartEmbedding(Dart dest, Dart src)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	setDartEmbedding<ORBIT>(dest, getEmbedding<ORBIT>(src));
}

template <unsigned int ORBIT>
inline void MapMono::setOrbitEmbedding(Dart d, unsigned int em)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	FunctorSetEmb<MapMono, ORBIT> fsetemb(*this, em);
	foreach_dart_of_orbit<ORBIT>(d, fsetemb);
}

template <unsigned int ORBIT>
inline void MapMono::initOrbitEmbedding(Dart d, unsigned int em)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	FunctorInitEmb<MapMono, ORBIT> fsetemb(*this, em);
	foreach_dart_of_orbit<ORBIT>(d, fsetemb);
}

template <unsigned int ORBIT>
inline unsigned int MapMono::setOrbitEmbeddingOnNewCell(Dart d)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int em = newCell<ORBIT>();
	setOrbitEmbedding<ORBIT>(d, em);
	return em;
}

template <unsigned int ORBIT>
inline unsigned int MapMono::initOrbitEmbeddingOnNewCell(Dart d)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int em = newCell<ORBIT>();
	initOrbitEmbedding<ORBIT>(d, em);
	return em;
}

template <unsigned int ORBIT>
inline void MapMono::copyCell(Dart d, Dart e)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	unsigned int dE = getEmbedding<ORBIT>(d) ;
	unsigned int eE = getEmbedding<ORBIT>(e) ;
	if(eE != EMBNULL)	// if the source is NULL, nothing to copy
	{
		if(dE == EMBNULL)	// if the dest is NULL, create a new cell
			dE = setOrbitEmbeddingOnNewCell<ORBIT>(d) ;
		copyCell<ORBIT>(dE, eE);	// copy the data
	}
}

template <unsigned int ORBIT>
void MapMono::initAllOrbitsEmbedding(bool realloc)
{
	if(!isOrbitEmbedded<ORBIT>())
		addEmbedding<ORBIT>() ;
	DartMarker<MapMono> mark(*this) ;
	for(Dart d = begin(); d != end(); next(d))
	{
		if(!mark.isMarked(d))
		{
			mark.markOrbit<ORBIT>(d) ;
			if(realloc || getEmbedding<ORBIT>(d) == EMBNULL)
				setOrbitEmbeddingOnNewCell<ORBIT>(d) ;
		}
	}
}

} // namespace CGoGN
