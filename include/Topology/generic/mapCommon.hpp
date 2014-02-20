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
 *           DARTS TRAVERSALS           *
 ****************************************/

template <typename MAP_IMPL>
template <unsigned int ORBIT, typename MAP>
bool MapCommon<MAP_IMPL>::foreach_orbit(FunctorType& fonct, unsigned int thread)
{
	TraversorCell<MAP, ORBIT> trav(*static_cast<MAP*>(this), true, thread);
	bool found = false;

	for (Dart d = trav.begin(); !found && d != trav.end(); d = trav.next())
	{
		if ((fonct)(d))
			found = true;
	}
	return found;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, typename MAP>
unsigned int MapCommon<MAP_IMPL>::getNbOrbits()
{
	FunctorCount fcount;
	foreach_orbit<ORBIT, MAP>(fcount);
	return fcount.getNb();
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int INCIDENT>
unsigned int MapCommon<MAP_IMPL>::degree(Dart d) const
{
	assert(ORBIT != INCIDENT || !"degree does not manage adjacency counting") ;
	Traversor* t = TraversorFactory<MAP_IMPL>::createIncident(*this, d, this->dimension(), ORBIT, INCIDENT) ;
	FunctorCount fcount ;
	t->applyFunctor(fcount) ;
	delete t ;
	return fcount.getNb() ;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline unsigned int MapCommon<MAP_IMPL>::getEmbedding(Dart d) const
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int d_index = this->dartIndex(d);

	if (ORBIT == DART)
		return d_index;

	return (*this->m_embeddings[ORBIT])[d_index] ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
void MapCommon<MAP_IMPL>::setDartEmbedding(Dart d, unsigned int emb)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int old = getEmbedding<ORBIT>(d);

	if (old == emb)	// if same emb
		return;		// nothing to do

	if (old != EMBNULL)	// if different
	{
		if(this->m_attribs[ORBIT].unrefLine(old))	// then unref the old emb
		{
			for (unsigned int t = 0; t < this->m_nbThreads; ++t)	// clear the markers if it was the
				(*this->m_markTables[ORBIT][t])[old].clear();		// last unref of the line
		}
	}

	if (emb != EMBNULL)
		this->m_attribs[ORBIT].refLine(emb);	// ref the new emb

	(*this->m_embeddings[ORBIT])[this->dartIndex(d)] = emb ; // finally affect the embedding to the dart
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
void MapCommon<MAP_IMPL>::initDartEmbedding(Dart d, unsigned int emb)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	assert(getEmbedding<ORBIT>(d) == EMBNULL || !"initDartEmbedding called on already embedded dart");

	if(emb != EMBNULL)
		this->m_attribs[ORBIT].refLine(emb);	// ref the new emb
	(*this->m_embeddings[ORBIT])[this->dartIndex(d)] = emb ; // affect the embedding to the dart
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::copyDartEmbedding(Dart dest, Dart src)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	setDartEmbedding<ORBIT>(dest, getEmbedding<ORBIT>(src));
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::setOrbitEmbedding(Dart d, unsigned int em)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	FunctorSetEmb<MapCommon<MAP_IMPL>, ORBIT> fsetemb(*this, em);
	this->template foreach_dart_of_orbit<ORBIT>(d, fsetemb);
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::initOrbitEmbedding(Dart d, unsigned int em)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	FunctorInitEmb<MapCommon<MAP_IMPL>, ORBIT> fsetemb(*this, em);
	this->template foreach_dart_of_orbit<ORBIT>(d, fsetemb);
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline unsigned int MapCommon<MAP_IMPL>::setOrbitEmbeddingOnNewCell(Dart d)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int em = this->template newCell<ORBIT>();
	setOrbitEmbedding<ORBIT>(d, em);
	return em;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline unsigned int MapCommon<MAP_IMPL>::initOrbitEmbeddingOnNewCell(Dart d)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int em = this->template newCell<ORBIT>();
	initOrbitEmbedding<ORBIT>(d, em);
	return em;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::copyCell(Dart d, Dart e)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	unsigned int dE = getEmbedding<ORBIT>(d) ;
	unsigned int eE = getEmbedding<ORBIT>(e) ;
	if(eE != EMBNULL)	// if the source is NULL, nothing to copy
	{
		if(dE == EMBNULL)	// if the dest is NULL, create a new cell
			dE = setOrbitEmbeddingOnNewCell<ORBIT>(d) ;
		copyCell<ORBIT>(dE, eE);	// copy the data
	}
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
void MapCommon<MAP_IMPL>::initAllOrbitsEmbedding(bool realloc)
{
	if(!this->template isOrbitEmbedded<ORBIT>())
		this->template addEmbedding<ORBIT>() ;
	DartMarker<MapCommon<MAP_IMPL> > mark(*this) ;
	for(Dart d = this->begin(); d != this->end(); this->next(d))
	{
		if(!mark.isMarked(d))
		{
			mark.markOrbit<ORBIT>(d) ;
			if(realloc || getEmbedding<ORBIT>(d) == EMBNULL)
				setOrbitEmbeddingOnNewCell<ORBIT>(d) ;
		}
	}
}

/**************************
 *  BOUNDARY MANAGEMENT   *
 **************************/

template <typename MAP_IMPL>
template <unsigned int DIM>
inline void MapCommon<MAP_IMPL>::boundaryMark(Dart d)
{
	this->m_markTables[DART][0]->operator[](this->dartIndex(d)).setMark(this->m_boundaryMarkers[DIM-2]);
}

template <typename MAP_IMPL>
template <unsigned int DIM>
inline void MapCommon<MAP_IMPL>::boundaryUnmark(Dart d)
{
	this->m_markTables[DART][0]->operator[](this->dartIndex(d)).unsetMark(this->m_boundaryMarkers[DIM-2]);
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int  DIM>
void MapCommon<MAP_IMPL>::boundaryMarkOrbit(Dart d)
{
	FunctorMark<MAP_IMPL> fm(*this, this->m_boundaryMarkers[DIM-2], this->m_markTables[DART][0]) ;
	this->template foreach_dart_of_orbit<ORBIT>(d, fm, 0) ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int DIM>
void MapCommon<MAP_IMPL>::boundaryUnmarkOrbit(Dart d)
{
	FunctorUnmark<MAP_IMPL> fm(*this, this->m_boundaryMarkers[DIM-2], this->m_markTables[DART][0]) ;
	this->template foreach_dart_of_orbit<ORBIT>(d, fm, 0) ;
}

template <typename MAP_IMPL>
template <unsigned int DIM>
inline bool MapCommon<MAP_IMPL>::isBoundaryMarked(Dart d) const
{
	return this->m_markTables[DART][0]->operator[](this->dartIndex(d)).testMark(this->m_boundaryMarkers[DIM-2]);
}

template <typename MAP_IMPL>
inline bool MapCommon<MAP_IMPL>::isBoundaryMarkedCurrent(Dart d) const
{
	return this->m_markTables[DART][0]->operator[](this->dartIndex(d)).testMark(this->m_boundaryMarkers[this->dimension()-2]);
}

/****************************************
 *        ATTRIBUTES MANAGEMENT         *
 ****************************************/

template <typename MAP_IMPL>
template <typename T, unsigned int ORBIT>
inline AttributeHandler<T, ORBIT, MAP_IMPL> MapCommon<MAP_IMPL>::addAttribute(const std::string& nameAttr)
{
	if(!this->template isOrbitEmbedded<ORBIT>())
		this->template addEmbedding<ORBIT>() ;
	AttributeMultiVector<T>* amv = this->m_attribs[ORBIT].addAttribute<T>(nameAttr) ;
	return AttributeHandler<T, ORBIT, MAP_IMPL>(this, amv) ;
}

template <typename MAP_IMPL>
template <typename T, unsigned int ORBIT>
inline bool MapCommon<MAP_IMPL>::removeAttribute(AttributeHandler<T, ORBIT, MAP_IMPL>& attr)
{
	assert(attr.isValid() || !"Invalid attribute handler") ;
	if(this->m_attribs[attr.getOrbit()].template removeAttribute<T>(attr.getIndex()))
	{
		AttributeMultiVectorGen* amv = attr.getDataVector();
		typedef std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator IT ;
		std::pair<IT, IT> bounds = this->attributeHandlers.equal_range(amv) ;
		for(IT i = bounds.first; i != bounds.second; ++i)
			(*i).second->setInvalid() ;
		this->attributeHandlers.erase(bounds.first, bounds.second) ;
		return true ;
	}
	return false ;
}

template <typename MAP_IMPL>
template <typename T, unsigned int ORBIT>
inline AttributeHandler<T ,ORBIT, MAP_IMPL> MapCommon<MAP_IMPL>::getAttribute(const std::string& nameAttr)
{
	AttributeMultiVector<T>* amv = this->m_attribs[ORBIT].template getDataVector<T>(nameAttr) ;
	return AttributeHandler<T, ORBIT, MAP_IMPL>(this, amv) ;
}

template <typename MAP_IMPL>
inline AttributeMultiVectorGen* MapCommon<MAP_IMPL>::getAttributeVectorGen(unsigned int orbit, const std::string& nameAttr)
{
	return this->m_attribs[orbit].getVirtualDataVector(nameAttr) ;
}

template <typename MAP_IMPL>
template <typename T, unsigned int ORBIT>
inline AttributeHandler<T ,ORBIT, MAP_IMPL> MapCommon<MAP_IMPL>::checkAttribute(const std::string& nameAttr)
{
	AttributeHandler<T, ORBIT, MAP_IMPL> att = this->getAttribute<T,ORBIT>(nameAttr);
	if (!att.isValid())
		att = this->addAttribute<T, ORBIT>(nameAttr);
	return att;
}

template <typename MAP_IMPL>
template <typename T, unsigned int ORBIT>
inline bool MapCommon<MAP_IMPL>::swapAttributes(AttributeHandler<T, ORBIT, MAP_IMPL>& attr1, AttributeHandler<T, ORBIT, MAP_IMPL>& attr2)
{
	assert((attr1.isValid() && attr2.isValid()) || !"Invalid attribute handler") ;
//	assert(attr1.getOrbit() == attr2.getOrbit() || !"Cannot swap attributes of different orbits") ;
//	unsigned int orbit = attr1.getOrbit() ;
	unsigned int index1 = attr1.getIndex() ;
	unsigned int index2 = attr2.getIndex() ;
	if(index1 != index2)
		return this->m_attribs[ORBIT].swapAttributes(index1, index2) ;
	return false ;
}

template <typename MAP_IMPL>
template <typename T, unsigned int ORBIT>
inline bool MapCommon<MAP_IMPL>::copyAttribute(AttributeHandler<T, ORBIT, MAP_IMPL>& dst, AttributeHandler<T, ORBIT, MAP_IMPL>& src)
{
	assert((dst.isValid() && src.isValid()) || !"Invalid attribute handler") ;
//	unsigned int orbit = dst.getOrbit() ;
//	assert(orbit == src.getOrbit() || !"Cannot copy attributes of different orbits") ;
	unsigned int index_dst = dst.getIndex() ;
	unsigned int index_src = src.getIndex() ;
	if(index_dst != index_src)
		return this->m_attribs[ORBIT].copyAttribute(index_dst, index_src) ;
	return false ;
}

/****************************************
 *               UTILITIES              *
 ****************************************/

template <typename MAP_IMPL>
template <unsigned int ORBIT>
unsigned int MapCommon<MAP_IMPL>::computeIndexCells(AttributeHandler<unsigned int, ORBIT, MAP_IMPL>& idx)
{
	AttributeContainer& cont = this->m_attribs[ORBIT] ;
	unsigned int cpt = 0 ;
	for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
		idx[i] = cpt++ ;
	return cpt ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
void MapCommon<MAP_IMPL>::bijectiveOrbitEmbedding()
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded") ;

	AttributeHandler<int, ORBIT, MAP_IMPL> counter = addAttribute<int, ORBIT>("tmpCounter") ;
	counter.setAllValues(int(0)) ;

	DartMarker<MAP_IMPL> mark(*this) ;
	for(Dart d = this->begin(); d != this->end(); this->next(d))
	{
		if(!mark.isMarked(d))
		{
			mark.template markOrbit<ORBIT>(d) ;
			unsigned int emb = getEmbedding<ORBIT>(d) ;
			if (emb != EMBNULL)
			{
				if (counter[d] > 0)
				{
					unsigned int newEmb = setOrbitEmbeddingOnNewCell<ORBIT>(d) ;
					copyCell<ORBIT>(newEmb, emb) ;
				}
				counter[d]++ ;
			}
		}
	}

	removeAttribute(counter) ;
}

} // namespace CGoGN
