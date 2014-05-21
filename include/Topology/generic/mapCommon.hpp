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
template <unsigned int ORBIT, unsigned int INCIDENT>
unsigned int MapCommon<MAP_IMPL>::degree(Dart d) const
{
	assert(ORBIT != INCIDENT || !"degree does not manage adjacency counting") ;
	Traversor* t = TraversorFactory<MapCommon<MAP_IMPL> >::createIncident(*this, d, this->dimension(), ORBIT, INCIDENT) ;
	unsigned int cpt = 0;
	t->apply([&] (Dart) { ++cpt; }) ;
	delete t ;
	return cpt;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
bool MapCommon<MAP_IMPL>::sameOrbit(Cell<ORBIT> c1, Cell<ORBIT> c2, unsigned int thread) const
{
	TraversorDartsOfOrbit<MapCommon<MAP_IMPL>, ORBIT> tradoo(*this, c1, thread);
	for (Dart x = tradoo.begin(); x != tradoo.end(); x = tradoo.next())
	{
		if (x == c2.dart)
			return true;
	}
	return false;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline unsigned int MapCommon<MAP_IMPL>::getEmbedding(Cell<ORBIT> c) const
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	if (ORBIT == DART)
		return this->dartIndex(c.dart);

	return (*this->m_embeddings[ORBIT])[this->dartIndex(c.dart)] ;
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
			for (unsigned int t = 0; t < this->m_nbThreadMarkers; ++t)	// clear the markers if it was the
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
inline void MapCommon<MAP_IMPL>::setOrbitEmbedding(Cell<ORBIT> c, unsigned int em)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	this->foreach_dart_of_orbit(c, [&] (Dart d) { this->setDartEmbedding<ORBIT>(d, em); });
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::initOrbitEmbedding(Cell<ORBIT> c, unsigned int em)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	this->foreach_dart_of_orbit(c, [&] (Dart d) { this->initDartEmbedding<ORBIT>(d, em); });
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline unsigned int MapCommon<MAP_IMPL>::setOrbitEmbeddingOnNewCell(Cell<ORBIT> c)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int em = this->template newCell<ORBIT>();
	setOrbitEmbedding<ORBIT>(c, em);
	return em;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline unsigned int MapCommon<MAP_IMPL>::initOrbitEmbeddingOnNewCell(Cell<ORBIT> d)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int em = this->template newCell<ORBIT>();
	initOrbitEmbedding<ORBIT>(d, em);
	return em;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::copyCellAttributes(Cell<ORBIT> d, Cell<ORBIT> e)
{
	assert(this->template isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	unsigned int dE = getEmbedding<ORBIT>(d) ;
	unsigned int eE = getEmbedding<ORBIT>(e) ;
	if(eE != EMBNULL)	// if the source is NULL, nothing to copy
	{
		if(dE == EMBNULL)	// if the dest is NULL, create a new cell
			dE = setOrbitEmbeddingOnNewCell<ORBIT>(d) ;
		this->m_attribs[ORBIT].copyLine(dE, eE) ;	// copy the data
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
template <unsigned int DIM, unsigned int ORBIT>
void MapCommon<MAP_IMPL>::boundaryMarkOrbit(Cell<ORBIT> c)
{
	this->foreach_dart_of_orbit(c, [&] (Dart d) { this->boundaryMark<DIM>(d); });
}

template <typename MAP_IMPL>
template <unsigned int DIM, unsigned int ORBIT>
void MapCommon<MAP_IMPL>::boundaryUnmarkOrbit(Cell<ORBIT> c)
{
	this->foreach_dart_of_orbit(c, [&] (Dart d)	{ this->boundaryUnmark<DIM>(d);	});
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

template <typename MAP_IMPL>
inline bool MapCommon<MAP_IMPL>::isBoundaryMarked(unsigned int dim, Dart d) const
{
	switch(dim)
	{
		case 2 : return isBoundaryMarked<2>(d) ; break ;
		case 3 : return isBoundaryMarked<3>(d) ; break ;
		default : return false ; break ;
	}
}

template <typename MAP_IMPL>
template <unsigned int DIM>
void MapCommon<MAP_IMPL>::boundaryUnmarkAll()
{
	AttributeContainer& cont = this->m_attribs[DART] ;
	for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
		this->m_markTables[DART][0]->operator[](i).unsetMark(this->m_boundaryMarkers[DIM-2]);
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
	AttributeMultiVector<T>* amv = this->m_attribs[ORBIT].template addAttribute<T>(nameAttr) ;
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

template <typename MAP_IMPL>
inline DartAttribute<Dart, MAP_IMPL> MapCommon<MAP_IMPL>::getInvolution(unsigned int i)
{
	return DartAttribute<Dart, MAP_IMPL>(this, this->getInvolutionAttribute(i));
}

template <typename MAP_IMPL>
inline DartAttribute<Dart, MAP_IMPL> MapCommon<MAP_IMPL>::getPermutation(unsigned int i)
{
	return DartAttribute<Dart, MAP_IMPL>(this, this->getPermutationAttribute(i));
}

template <typename MAP_IMPL>
inline DartAttribute<Dart, MAP_IMPL> MapCommon<MAP_IMPL>::getPermutationInv(unsigned int i)
{
	return DartAttribute<Dart, MAP_IMPL>(this, this->getPermutationInvAttribute(i));
}

/****************************************
 *     QUICK TRAVERSAL MANAGEMENT       *
 ****************************************/

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::enableQuickTraversal()
{
	if(this->m_quickTraversal[ORBIT] == NULL)
	{
		if(!this->template isOrbitEmbedded<ORBIT>())
			this->template addEmbedding<ORBIT>() ;
		this->m_quickTraversal[ORBIT] = this->m_attribs[ORBIT].template addAttribute<Dart>("quick_traversal") ;
	}
	updateQuickTraversal<ORBIT>() ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::updateQuickTraversal()
{
	assert(this->m_quickTraversal[ORBIT] != NULL || !"updateQuickTraversal on a disabled orbit") ;

	CellMarker<MapCommon<MAP_IMPL>, ORBIT> cm(*this) ;
	for(Dart d = this->begin(); d != this->end(); this->next(d))
	{
		if(!cm.isMarked(d))
		{
			cm.mark(d) ;
			(*this->m_quickTraversal[ORBIT])[getEmbedding<ORBIT>(d)] = d ;
		}
	}
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline const AttributeMultiVector<Dart>* MapCommon<MAP_IMPL>::getQuickTraversal() const
{
	return this->m_quickTraversal[ORBIT] ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT>
inline void MapCommon<MAP_IMPL>::disableQuickTraversal()
{
	if(this->m_quickTraversal[ORBIT] != NULL)
	{
		this->m_attribs[ORBIT].template removeAttribute<Dart>(this->m_quickTraversal[ORBIT]->getIndex()) ;
		this->m_quickTraversal[ORBIT] = NULL ;
	}
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int INCI>
inline void MapCommon<MAP_IMPL>::enableQuickIncidentTraversal()
{
	if(this->m_quickLocalIncidentTraversal[ORBIT][INCI] == NULL)
	{
		if(!this->template isOrbitEmbedded<ORBIT>())
			this->template addEmbedding<ORBIT>() ;
		std::stringstream ss;
		ss << "quickIncidentTraversal_" << INCI;
		this->m_quickLocalIncidentTraversal[ORBIT][INCI] = this->m_attribs[ORBIT].template addAttribute<NoTypeNameAttribute<std::vector<Dart> > >(ss.str()) ;
	}
	updateQuickIncidentTraversal<ORBIT,INCI>() ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int INCI>
inline void MapCommon<MAP_IMPL>::updateQuickIncidentTraversal()
{
	assert(this->m_quickLocalIncidentTraversal[ORBIT][INCI] != NULL || !"updateQuickTraversal on a disabled orbit") ;

	AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* ptrVD = this->m_quickLocalIncidentTraversal[ORBIT][INCI];
	this->m_quickLocalIncidentTraversal[ORBIT][INCI] = NULL;

	std::vector<Dart> buffer;
	buffer.reserve(100);

	TraversorCell<MapCommon<MAP_IMPL>, ORBIT> tra_glob(*this);
	for (Dart d = tra_glob.begin(); d != tra_glob.end(); d = tra_glob.next())
	{
		buffer.clear();
		Traversor* tra_loc = TraversorFactory<MapCommon<MAP_IMPL> >::createIncident(*this, d, this->dimension(), ORBIT, INCI);
		for (Dart e = tra_loc->begin(); e != tra_loc->end(); e = tra_loc->next())
			buffer.push_back(e);
		delete tra_loc;
		buffer.push_back(NIL);
		std::vector<Dart>& vd = (*ptrVD)[getEmbedding<ORBIT>(d)];
		vd.reserve(buffer.size());
		vd.assign(buffer.begin(), buffer.end());
	}

	this->m_quickLocalIncidentTraversal[ORBIT][INCI] = ptrVD;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int INCI>
inline const AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* MapCommon<MAP_IMPL>::getQuickIncidentTraversal() const
{
	return this->m_quickLocalIncidentTraversal[ORBIT][INCI] ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int INCI>
inline void MapCommon<MAP_IMPL>::disableQuickIncidentTraversal()
{
	if(this->m_quickLocalIncidentTraversal[ORBIT][INCI] != NULL)
	{
		this->m_attribs[ORBIT].template removeAttribute<Dart>(this->m_quickLocalIncidentTraversal[ORBIT][INCI]->getIndex()) ;
		this->m_quickLocalIncidentTraversal[ORBIT][INCI] = NULL ;
	}
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int ADJ>
inline void MapCommon<MAP_IMPL>::enableQuickAdjacentTraversal()
{
	if(this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] == NULL)
	{
		if(!this->template isOrbitEmbedded<ORBIT>())
			this->template addEmbedding<ORBIT>() ;
		std::stringstream ss;
		ss << "quickAdjacentTraversal" << ADJ;
		this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] = this->m_attribs[ORBIT].template addAttribute<NoTypeNameAttribute<std::vector<Dart> > >(ss.str()) ;
	}
	updateQuickAdjacentTraversal<ORBIT, ADJ>() ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int ADJ>
inline void MapCommon<MAP_IMPL>::updateQuickAdjacentTraversal()
{
	assert(this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] != NULL || !"updateQuickTraversal on a disabled orbit") ;

	AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* ptrVD = this->m_quickLocalAdjacentTraversal[ORBIT][ADJ];
	this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] = NULL;

	std::vector<Dart> buffer;
	buffer.reserve(100);

	TraversorCell<MapCommon<MAP_IMPL>, ORBIT> tra_glob(*this);
	for (Dart d = tra_glob.begin(); d != tra_glob.end(); d = tra_glob.next())
	{
		buffer.clear();
		Traversor* tra_loc = TraversorFactory<MapCommon<MAP_IMPL> >::createAdjacent(*this, d, this->dimension(), ORBIT, ADJ);
		for (Dart e = tra_loc->begin(); e != tra_loc->end(); e = tra_loc->next())
			buffer.push_back(e);
		buffer.push_back(NIL);
		delete tra_loc;
		std::vector<Dart>& vd = (*ptrVD)[getEmbedding<ORBIT>(d)];
		vd.reserve(buffer.size());
		vd.assign(buffer.begin(),buffer.end());
	}

	this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] = ptrVD;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int ADJ>
inline const AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* MapCommon<MAP_IMPL>::getQuickAdjacentTraversal() const
{
	return this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] ;
}

template <typename MAP_IMPL>
template <unsigned int ORBIT, unsigned int ADJ>
inline void MapCommon<MAP_IMPL>::disableQuickAdjacentTraversal()
{
	if(this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] != NULL)
	{
		this->m_attribs[ORBIT].template removeAttribute<Dart>(this->m_quickLocalAdjacentTraversal[ORBIT][ADJ]->getIndex()) ;
		this->m_quickLocalAdjacentTraversal[ORBIT][ADJ] = NULL ;
	}
}

} // namespace CGoGN
