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

#include "Topology/generic/dartmarker.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/traversorFactory.h"

namespace CGoGN
{

/****************************************
 *           DARTS MANAGEMENT           *
 ****************************************/

inline Dart GenericMap::newDart()
{
	unsigned int di = m_attribs[DART].insertLine();		// insert a new dart line
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if (m_embeddings[i])							// set all its embeddings
			(*m_embeddings[i])[di] = EMBNULL ;			// to EMBNULL
	}

	return Dart::create(di) ;
}

inline void GenericMap::deleteDartLine(unsigned int index)
{
	m_attribs[DART].removeLine(index) ;	// free the dart line

	for (unsigned int t = 0; t < m_nbThreads; ++t)	// clear markers of
		(*m_markTables[DART][t])[index].clear() ;	// the removed dart

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if (m_embeddings[orbit])									// for each embedded orbit
		{
			unsigned int emb = (*m_embeddings[orbit])[index] ;		// get the embedding of the dart
			if(emb != EMBNULL)
			{
				if(m_attribs[orbit].unrefLine(emb))					// unref the pointed embedding line
				{
					for (unsigned int t = 0; t < m_nbThreads; ++t)	// and clear its markers if it was
						(*m_markTables[orbit][t])[emb].clear() ;	// its last unref (and was thus freed)
				}
			}
		}
	}
}

inline unsigned int GenericMap::copyDartLine(unsigned int index)
{
	unsigned int newindex = m_attribs[DART].insertLine() ;	// create a new dart line
	m_attribs[DART].copyLine(newindex, index) ;				// copy the given dart line
	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if (m_embeddings[orbit])
		{
			unsigned int emb = (*m_embeddings[orbit])[newindex] ;	// add a ref to the cells pointed
			if(emb != EMBNULL)										// by the new dart line
				m_attribs[orbit].refLine(emb) ;
		}
	}
	return newindex ;
}

inline unsigned int GenericMap::getNbDarts()
{
	return m_attribs[DART].size() ;
}

inline bool GenericMap::isDartValid(Dart d)
{
	return !d.isNil() && m_attribs[DART].used(dartIndex(d)) ;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

template <unsigned int ORBIT>
inline bool GenericMap::isOrbitEmbedded() const
{
	return (ORBIT == DART) || (m_embeddings[ORBIT] != NULL) ;
}

inline bool GenericMap::isOrbitEmbedded(unsigned int orbit) const
{
	return (orbit == DART) || (m_embeddings[orbit] != NULL) ;
}

template <unsigned int ORBIT>
inline unsigned int GenericMap::getEmbedding(Dart d) const
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");

	unsigned int d_index = dartIndex(d);

	if (ORBIT == DART)
		return d_index;

	return (*m_embeddings[ORBIT])[d_index] ;
}

template <unsigned int ORBIT>
void GenericMap::setDartEmbedding(Dart d, unsigned int emb)
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
void GenericMap::initDartEmbedding(Dart d, unsigned int emb)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	assert(getEmbedding<ORBIT>(d) == EMBNULL || !"initDartEmbedding called on already embedded dart");
	if(emb != EMBNULL)
		m_attribs[ORBIT].refLine(emb);	// ref the new emb
	(*m_embeddings[ORBIT])[dartIndex(d)] = emb ; // affect the embedding to the dart
}

template <unsigned int ORBIT>
inline void GenericMap::copyDartEmbedding(Dart dest, Dart src)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	setDartEmbedding<ORBIT>(dest, getEmbedding<ORBIT>(src));
}

template <unsigned int ORBIT>
inline unsigned int GenericMap::newCell()
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	return m_attribs[ORBIT].insertLine();
}

template <unsigned int ORBIT>
inline void GenericMap::setOrbitEmbedding(Dart d, unsigned int em)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	FunctorSetEmb<GenericMap, ORBIT> fsetemb(*this, em);
	foreach_dart_of_orbit<ORBIT>(d, fsetemb);
}

template <unsigned int ORBIT>
inline void GenericMap::initOrbitEmbedding(Dart d, unsigned int em)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	FunctorInitEmb<GenericMap, ORBIT> fsetemb(*this, em);
	foreach_dart_of_orbit<ORBIT>(d, fsetemb);
}

template <unsigned int ORBIT>
inline unsigned int GenericMap::setOrbitEmbeddingOnNewCell(Dart d)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	unsigned int em = newCell<ORBIT>();
	setOrbitEmbedding<ORBIT>(d, em);
	return em;
}

template <unsigned int ORBIT>
inline unsigned int GenericMap::initOrbitEmbeddingNewCell(Dart d)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	unsigned int em = newCell<ORBIT>();
	initOrbitEmbedding<ORBIT>(d, em);
	return em;
}

template <unsigned int ORBIT>
inline void GenericMap::copyCell(Dart d, Dart e)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	unsigned int dE = getEmbedding<ORBIT>(d) ;
	unsigned int eE = getEmbedding<ORBIT>(e) ;
	if(eE != EMBNULL)	// if the source is NULL, nothing to copy
	{
		if(dE == EMBNULL)	// if the dest is NULL, create a new cell
			dE = setOrbitEmbeddingOnNewCell<ORBIT>(d) ;
		m_attribs[ORBIT].copyLine(dE, eE) ;	// copy the data
	}
}

template <unsigned int ORBIT>
inline void GenericMap::copyCell(unsigned int i, unsigned int j)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	m_attribs[ORBIT].copyLine(i, j) ;
}

template <unsigned int ORBIT>
inline void GenericMap::initCell(unsigned int i)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded");
	m_attribs[ORBIT].initLine(i) ;
}

template <unsigned int ORBIT>
void GenericMap::initAllOrbitsEmbedding(bool realloc)
{
	if(!isOrbitEmbedded<ORBIT>())
		addEmbedding<ORBIT>() ;
	DartMarker mark(*this) ;
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

/****************************************
 *     QUICK TRAVERSAL MANAGEMENT       *
 ****************************************/

template <unsigned int ORBIT>
inline void GenericMap::enableQuickTraversal()
{

	if(m_quickTraversal[ORBIT] == NULL)
	{
		if(!isOrbitEmbedded<ORBIT>())
			addEmbedding<ORBIT>() ;
		m_quickTraversal[ORBIT] = m_attribs[ORBIT].addAttribute<Dart>("quick_traversal") ;
	}
	updateQuickTraversal<ORBIT>() ;
}

template <unsigned int ORBIT>
inline void GenericMap::updateQuickTraversal()
{
	assert(m_quickTraversal[ORBIT] != NULL || !"updateQuickTraversal on a disabled orbit") ;

//	CellMarker<ORBIT> cm(*this) ;
//	for(Dart d = begin(); d != end(); next(d))
//	{
//		if ((!cm.isMarked(d)) && (!isBoundaryMarkedCurrent(d)))
//		{
//			cm.mark(d) ;
//			(*m_quickTraversal[ORBIT])[getEmbedding<ORBIT>(d)] = d ;
//		}
//	}

	// ensure that we do not try to use quick traversal in Traversors
	AttributeMultiVector<Dart>* qt = m_quickTraversal[ORBIT];
	m_quickTraversal[ORBIT] = NULL;

	// fill the quick travsersal
	TraversorCell<GenericMap,VOLUME> trav(*this);
	for(Dart d = trav.begin(); d != trav.end(); d=trav.next())
	{
		(*qt)[getEmbedding<ORBIT>(d)] = d ;
	}

	// restore ptr
	m_quickTraversal[ORBIT] = qt;

}

template <unsigned int ORBIT>
inline const AttributeMultiVector<Dart>* GenericMap::getQuickTraversal() const
{
	return m_quickTraversal[ORBIT] ;
}

template <unsigned int ORBIT>
inline void GenericMap::disableQuickTraversal()
{
	if(m_quickTraversal[ORBIT] != NULL)
	{
		m_attribs[ORBIT].removeAttribute<Dart>(m_quickTraversal[ORBIT]->getIndex()) ;
		m_quickTraversal[ORBIT] = NULL ;
	}
}


template <typename MAP, unsigned int ORBIT, unsigned int INCI>
inline void GenericMap::enableQuickIncidentTraversal()
{
	if(m_quickLocalIncidentTraversal[ORBIT][INCI] == NULL)
	{
		if(!isOrbitEmbedded<ORBIT>())
			addEmbedding<ORBIT>() ;
		std::stringstream ss;
		ss << "quickLocalIncidentTraversal_" << INCI;
		m_quickLocalIncidentTraversal[ORBIT][INCI] = m_attribs[ORBIT].addAttribute<NoTypeNameAttribute<std::vector<Dart> > >(ss.str()) ;
	}
	updateQuickIncidentTraversal<MAP,ORBIT,INCI>() ;
}

template <typename MAP, unsigned int ORBIT, unsigned int INCI>
inline void GenericMap::updateQuickIncidentTraversal()
{
	assert(m_quickLocalIncidentTraversal[ORBIT][INCI] != NULL || !"updateQuickTraversal on a disabled orbit") ;

	AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* ptrVD = m_quickLocalIncidentTraversal[ORBIT][INCI];
	m_quickLocalIncidentTraversal[ORBIT][INCI] = NULL;

	std::vector<Dart> buffer;
	buffer.reserve(100);

	MAP& map = static_cast<MAP&>(*this);

	TraversorCell<MAP,ORBIT> tra_glob(map);
	for (Dart d = tra_glob.begin(); d != tra_glob.end(); d = tra_glob.next())
	{
		buffer.clear();
		Traversor* tra_loc = TraversorFactory<MAP>::createIncident(map, d, map.dimension(), ORBIT, INCI);
		for (Dart e = tra_loc->begin(); e != tra_loc->end(); e = tra_loc->next())
			buffer.push_back(e);
		delete tra_loc;
		buffer.push_back(NIL);
		std::vector<Dart>& vd = (*ptrVD)[getEmbedding<ORBIT>(d)];
		vd.reserve(buffer.size());
		vd.assign(buffer.begin(),buffer.end());
	}
	m_quickLocalIncidentTraversal[ORBIT][INCI] = ptrVD;
}

template <unsigned int ORBIT, unsigned int INCI>
inline const AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* GenericMap::getQuickIncidentTraversal() const
{
	return m_quickLocalIncidentTraversal[ORBIT][INCI] ;
}

template <unsigned int ORBIT, unsigned int INCI>
inline void GenericMap::disableQuickIncidentTraversal()
{
	if(m_quickLocalIncidentTraversal[ORBIT][INCI] != NULL)
	{
		m_attribs[ORBIT].removeAttribute<Dart>(m_quickLocalIncidentTraversal[ORBIT][INCI]->getIndex()) ;
		m_quickLocalIncidentTraversal[ORBIT][INCI] = NULL ;
	}
}



template <typename MAP, unsigned int ORBIT, unsigned int ADJ>
inline void GenericMap::enableQuickAdjacentTraversal()
{
	if(m_quickLocalAdjacentTraversal[ORBIT][ADJ] == NULL)
	{
		if(!isOrbitEmbedded<ORBIT>())
			addEmbedding<ORBIT>() ;
		std::stringstream ss;
		ss << "quickLocalAdjacentTraversal" << ADJ;
		m_quickLocalAdjacentTraversal[ORBIT][ADJ] = m_attribs[ORBIT].addAttribute<NoTypeNameAttribute<std::vector<Dart> > >(ss.str()) ;
	}
	updateQuickAdjacentTraversal<MAP,ORBIT,ADJ>() ;
}

template <typename MAP, unsigned int ORBIT, unsigned int ADJ>
inline void GenericMap::updateQuickAdjacentTraversal()
{
	assert(m_quickLocalAdjacentTraversal[ORBIT][ADJ] != NULL || !"updateQuickTraversal on a disabled orbit") ;

	AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* ptrVD = m_quickLocalAdjacentTraversal[ORBIT][ADJ];
	m_quickLocalAdjacentTraversal[ORBIT][ADJ] = NULL;

	MAP& map = static_cast<MAP&>(*this);

	std::vector<Dart> buffer;
	buffer.reserve(100);

	TraversorCell<MAP,ORBIT> tra_glob(map);
	for (Dart d = tra_glob.begin(); d != tra_glob.end(); d = tra_glob.next())
	{
		buffer.clear();
		Traversor* tra_loc = TraversorFactory<MAP>::createAdjacent(map, d, map.dimension(), ORBIT, ADJ);
		for (Dart e = tra_loc->begin(); e != tra_loc->end(); e = tra_loc->next())
			buffer.push_back(e);
		buffer.push_back(NIL);
		delete tra_loc;
		std::vector<Dart>& vd = (*ptrVD)[getEmbedding<ORBIT>(d)];
		vd.reserve(buffer.size());
		vd.assign(buffer.begin(),buffer.end());
	}
	m_quickLocalAdjacentTraversal[ORBIT][ADJ] = ptrVD;
}

template <unsigned int ORBIT, unsigned int ADJ>
inline const AttributeMultiVector<NoTypeNameAttribute<std::vector<Dart> > >* GenericMap::getQuickAdjacentTraversal() const
{
	return m_quickLocalAdjacentTraversal[ORBIT][ADJ] ;
}

template <unsigned int ORBIT, unsigned int ADJ>
inline void GenericMap::disableQuickAdjacentTraversal()
{
	if(m_quickLocalAdjacentTraversal[ORBIT][ADJ] != NULL)
	{
		m_attribs[ORBIT].removeAttribute<Dart>(m_quickLocalAdjacentTraversal[ORBIT][ADJ]->getIndex()) ;
		m_quickLocalAdjacentTraversal[ORBIT][ADJ] = NULL ;
	}
}




/****************************************
 *        ATTRIBUTES MANAGEMENT         *
 ****************************************/

inline unsigned int GenericMap::getNbCells(unsigned int orbit)
{
	return m_attribs[orbit].size() ;
}

template <unsigned int ORBIT>
inline AttributeContainer& GenericMap::getAttributeContainer()
{
	return m_attribs[ORBIT] ;
}

template <unsigned int ORBIT>
inline const AttributeContainer& GenericMap::getAttributeContainer() const
{
	return m_attribs[ORBIT] ;
}

inline AttributeContainer& GenericMap::getAttributeContainer(unsigned int orbit)
{
	return m_attribs[orbit] ;
}

inline const AttributeContainer& GenericMap::getAttributeContainer(unsigned int orbit) const
{
	return m_attribs[orbit] ;
}


template <unsigned int ORBIT>
inline AttributeMultiVector<Mark>* GenericMap::getMarkVector(unsigned int thread)
{
	assert(isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit not embedded") ;
	return m_markTables[ORBIT][thread] ;
}

template <unsigned int ORBIT>
inline AttributeMultiVector<unsigned int>* GenericMap::getEmbeddingAttributeVector()
{
	return m_embeddings[ORBIT] ;
}

template <typename R>
bool GenericMap::registerAttribute(const std::string &nameType)
{
	RegisteredBaseAttribute* ra = new RegisteredAttribute<R>;
	if (ra == NULL)
	{
		CGoGNerr << "Erreur enregistrement attribut" << CGoGNendl;
		return false;
	}

	ra->setTypeName(nameType);

	m_attributes_registry_map->insert(std::pair<std::string, RegisteredBaseAttribute*>(nameType,ra));
	return true;
}

/****************************************
 *   EMBEDDING ATTRIBUTES MANAGEMENT    *
 ****************************************/

template <unsigned int ORBIT>
void GenericMap::addEmbedding()
{
	assert(!isOrbitEmbedded<ORBIT>() || !"Invalid parameter: orbit already embedded") ;

	std::ostringstream oss;
	oss << "EMB_" << ORBIT;

	AttributeContainer& dartCont = m_attribs[DART] ;
	AttributeMultiVector<unsigned int>* amv = dartCont.addAttribute<unsigned int>(oss.str()) ;
	m_embeddings[ORBIT] = amv ;

	// set new embedding to EMBNULL for all the darts of the map
	for(unsigned int i = dartCont.begin(); i < dartCont.end(); dartCont.next(i))
		(*amv)[i] = EMBNULL ;
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

template <unsigned int ORBIT>
bool GenericMap::foreach_dart_of_orbit(Dart d, FunctorType& f, unsigned int thread) const
{
	switch(ORBIT)
	{
		case DART:		return f(d);
		case VERTEX: 	return foreach_dart_of_vertex(d, f, thread);
		case EDGE: 		return foreach_dart_of_edge(d, f, thread);
		case FACE: 		return foreach_dart_of_face(d, f, thread);
		case VOLUME: 	return foreach_dart_of_volume(d, f, thread);
		case VERTEX1: 	return foreach_dart_of_vertex1(d, f, thread);
		case EDGE1: 	return foreach_dart_of_edge1(d, f, thread);
		case VERTEX2: 	return foreach_dart_of_vertex2(d, f, thread);
		case EDGE2:		return foreach_dart_of_edge2(d, f, thread);
		case FACE2:		return foreach_dart_of_face2(d, f, thread);
		default: 		assert(!"Cells of this dimension are not handled"); break;
	}
	return false;
}

template <typename MAP, unsigned int ORBIT>
bool GenericMap::foreach_orbit(FunctorType& fonct, unsigned int thread) const
{
	TraversorCell<MAP, ORBIT> trav(*this, true, thread);
	bool found = false;

	for (Dart d = trav.begin(); !found && d != trav.end(); d = trav.next())
	{
		if ((fonct)(d))
			found = true;
	}
	return found;
}

template <typename MAP, unsigned int ORBIT>
unsigned int GenericMap::getNbOrbits() const
{
	FunctorCount fcount;
	foreach_orbit<MAP, ORBIT>(fcount);
	return fcount.getNb();
}

template <typename MAP, unsigned int ORBIT, unsigned int INCIDENT>
unsigned int GenericMap::degree(Dart d) const
{
	assert(ORBIT != INCIDENT || !"degree does not manage adjacency counting") ;
	Traversor* t = TraversorFactory<MAP>::createIncident(*(reinterpret_cast<MAP*>(this)), d, dimension(), ORBIT, INCIDENT) ;
	FunctorCount fcount ;
	t->applyFunctor(fcount) ;
	delete t ;
	return fcount.getNb() ;
}

/****************************************
 *  TOPOLOGICAL ATTRIBUTES MANAGEMENT   *
 ****************************************/

inline AttributeMultiVector<Dart>* GenericMap::addRelation(const std::string& name)
{
	AttributeContainer& cont = m_attribs[DART] ;
	AttributeMultiVector<Dart>* amv = cont.addAttribute<Dart>(name) ;

	// set new relation to fix point for all the darts of the map
	for(unsigned int i = cont.begin(); i < cont.end(); cont.next(i))
		(*amv)[i] = i ;

	return amv ;
}

inline AttributeMultiVector<Dart>* GenericMap::getRelation(const std::string& name)
{
	AttributeContainer& cont = m_attribs[DART] ;
	AttributeMultiVector<Dart>* amv = cont.getDataVector<Dart>(cont.getAttributeIndex(name)) ;
	return amv ;
}


/**************************
 *  BOUNDARY MANAGEMENT   *
 **************************/

template <unsigned int D>
inline void GenericMap::boundaryMark(Dart d)
{
	m_markTables[DART][0]->operator[](dartIndex(d)).setMark(m_boundaryMarkers[D-2]);
}

template <unsigned int D>
inline void GenericMap::boundaryUnmark(Dart d)
{
	m_markTables[DART][0]->operator[](dartIndex(d)).unsetMark(m_boundaryMarkers[D-2]);
}

template <unsigned int D>
inline bool GenericMap::isBoundaryMarked(Dart d) const
{
	return m_markTables[DART][0]->operator[](dartIndex(d)).testMark(m_boundaryMarkers[D-2]);
}


inline bool GenericMap::isBoundaryMarkedCurrent(Dart d) const
{
	return m_markTables[DART][0]->operator[](dartIndex(d)).testMark(m_boundaryMarkers[this->dimension()-2]);
}


inline void GenericMap::boundaryMark2(Dart d)
{
	boundaryMark<2>(d);
}

inline void GenericMap::boundaryUnmark2(Dart d)
{
	boundaryUnmark<2>(d);
}

inline bool GenericMap::isBoundaryMarked2(Dart d) const
{
	return isBoundaryMarked<2>(d);
}

inline void GenericMap::boundaryMark3(Dart d)
{
	boundaryMark<3>(d);
}

inline void GenericMap::boundaryUnmark3(Dart d)
{
	boundaryUnmark<3>(d);
}

inline bool GenericMap::isBoundaryMarked3(Dart d) const
{
	return isBoundaryMarked<3>(d);
}

template <unsigned int ORBIT, unsigned int  DIM>
void GenericMap::boundaryMarkOrbit(Dart d)
{
	FunctorMark<GenericMap> fm(*this, m_boundaryMarkers[DIM-2], m_markTables[DART][0]) ;
	foreach_dart_of_orbit<ORBIT>(d, fm, 0) ;
}

template <unsigned int ORBIT, unsigned int DIM>
void GenericMap::boundaryUnmarkOrbit(Dart d)
{
	FunctorUnmark<GenericMap> fm(*this, m_boundaryMarkers[DIM-2], m_markTables[DART][0]) ;
	foreach_dart_of_orbit<ORBIT>(d, fm, 0) ;
}

template <unsigned int DIM>
void GenericMap::boundaryUnmarkAll()
{
	AttributeContainer& cont = getAttributeContainer<DART>() ;
	for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
		m_markTables[DART][0]->operator[](i).unsetMark(m_boundaryMarkers[DIM-2]);
}


} //namespace CGoGN
