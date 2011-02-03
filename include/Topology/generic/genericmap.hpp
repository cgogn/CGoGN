/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

namespace CGoGN
{

inline bool GenericMap::chechXmlNode(xmlNodePtr node, const std::string& name)
{
	return (strcmp((char*)(node->name),(char*)(name.c_str())) == 0);
}

/****************************************
 *           DARTS MANAGEMENT           *
 ****************************************/

inline Dart GenericMap::newDart()
{
	Dart d = Dart::create(m_attribs[DART_ORBIT].insertLine());
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		if (m_embeddings[i])
			(*m_embeddings[i])[d.index] = EMBNULL ;
	return d ;
}

inline void GenericMap::deleteDart(Dart d)
{
	m_attribs[DART_ORBIT].removeLine(d.index) ;
	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if (m_embeddings[orbit])
		{
			unsigned int emb = (*m_embeddings[orbit])[d.index] ;
			if(emb != EMBNULL)
				m_attribs[orbit].unrefLine(emb) ;
		}
		m_markerTables[DART_ORBIT]->operator[](d.index).clear() ;
	}
}

inline bool GenericMap::isDartValid(Dart d)
{
	return !d.isNil() && m_attribs[DART_ORBIT].used(d.index) ;
}

inline unsigned int GenericMap::getNbDarts()
{
	return m_attribs[DART_ORBIT].size() ;
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

inline bool GenericMap::isOrbitEmbedded(unsigned int orbit) const
{
	return (orbit == DART_ORBIT) || (m_embeddings[orbit] != NULL) ;
}

inline AttribMultiVect<unsigned int>* GenericMap::getEmbeddingAttributeVector(unsigned int orbit) const
{
	return m_embeddings[orbit] ;
}

inline unsigned int GenericMap::nbEmbeddings() const
{
	unsigned int nb = 0;
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		if (isOrbitEmbedded(i))
			++nb;
	return nb ;
}

inline unsigned int GenericMap::getEmbedding(Dart d, unsigned int orbit)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");

	if (orbit == DART_ORBIT)
		return d.index;

	unsigned int emb = (*m_embeddings[orbit])[d.index] ;
	if(emb != EMBNULL)
		return emb ;

	class FunctorGetLazyEmb: public FunctorType
	{
	protected:
		GenericMap& m_map;
		unsigned int m_orbit;
		AttribMultiVect<unsigned int>* m_emb;
		unsigned int m_val;
		std::vector<Dart> m_darts;

	public:
		FunctorGetLazyEmb(GenericMap& map, unsigned int orbit):
			m_map(map), m_orbit(orbit), m_val(EMBNULL)
		{
			m_emb = m_map.getEmbeddingAttributeVector(orbit) ;
			m_darts.reserve(8) ;
		}

		bool operator()(Dart d)
		{
			if ((*m_emb)[d.index] != EMBNULL)
			{
				m_val = (*m_emb)[d.index];
				return true;
			}
			m_darts.push_back(d);
			return false;
		}

		unsigned int getVal()
		{
			if(m_val != EMBNULL)
			{
				AttribContainer& cont = m_map.getAttributeContainer(m_orbit) ;
				for(std::vector<Dart>::iterator it = m_darts.begin(); it != m_darts.end(); ++it)
				{
					(*m_emb)[it->index] = m_val;
					cont.refLine(m_val) ;
				}
			}

			return m_val;
		}
	};

	FunctorGetLazyEmb fle(*this, orbit);
	foreach_dart_of_orbit(orbit, d, fle);

	return fle.getVal();
}

inline unsigned int GenericMap::getDartEmbedding(unsigned int orbit, Dart d)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");

	if (orbit == DART_ORBIT)
		return d.index;

	return (*m_embeddings[orbit])[d.index] ;
}

inline void GenericMap::copyDartEmbedding(unsigned int orbit, Dart d, Dart e)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	setDartEmbedding(orbit, d, getDartEmbedding(orbit, e));
}

inline unsigned int GenericMap::newCell(unsigned int orbit)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	return m_attribs[orbit].insertLine();
}

inline void GenericMap::embedOrbit(unsigned int orbit, Dart d, unsigned int em)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	FunctorSetEmb<GenericMap> fsetemb(*this,orbit,em);
	foreach_dart_of_orbit(orbit, d, fsetemb);
}

inline unsigned int GenericMap::embedNewCell(unsigned int orbit, Dart d)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	unsigned int em = newCell(orbit);
//	setDartEmbedding(orbit, d, em) ; // possible ?? utile ??
	embedOrbit(orbit, d, em);
	return em;
}

inline void GenericMap::copyCell(unsigned int orbit, Dart d, Dart e)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	unsigned int dE = getEmbedding(d, orbit) ;
	unsigned int eE = getEmbedding(e, orbit) ;
	if(eE != EMBNULL)	// if the source is NULL, nothing to copy
	{
		if(dE == EMBNULL)	// if the dest is NULL, create a new cell
		{
			dE = embedNewCell(orbit, d) ;
		}
		m_attribs[orbit].copyLine(dE, eE) ;	// copy the data
	}
}

inline void GenericMap::copyCell(unsigned int orbit, unsigned int i, unsigned int j)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	m_attribs[orbit].copyLine(i,j) ;
}

inline void GenericMap::initCell(unsigned int orbit, unsigned int i)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	m_attribs[orbit].initLine(i) ;
}

/****************************************
 *        ATTRIBUTES MANAGEMENT         *
 ****************************************/

template <typename T>
inline AttribMultiVect<T>& GenericMap::getAttributeVector(unsigned int idAttr)
{
	assert(idAttr != AttribContainer::UNKNOWN) ;
	return m_attribs[AttribContainer::orbitAttr(idAttr)].getDataVector<T>(AttribContainer::indexAttr(idAttr)) ;
}

inline AttribMultiVect<Mark>* GenericMap::getMarkerVector(unsigned int orbit)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	return m_markerTables[orbit] ;
}

inline AttribMultiVectGen& GenericMap::getMultiVec(unsigned int idAttr)
{
	assert(idAttr != AttribContainer::UNKNOWN) ;
	return m_attribs[AttribContainer::orbitAttr(idAttr)].getVirtualDataVector(AttribContainer::indexAttr(idAttr)) ;
}

inline AttribContainer& GenericMap::getAttributeContainer(unsigned int orbit)
{
//	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	return m_attribs[orbit] ;
}

/****************************************
 *          MARKERS MANAGEMENT          *
 ****************************************/

inline Marker GenericMap::getNewMarker(unsigned int cell)
{
	assert(isOrbitEmbedded(cell) || !"Try to get a marker on non embedded cell! ") ;
	return m_orbMarker[cell].getNewMarker(cell) ;
}

inline void GenericMap::releaseMarker(Marker m)
{
	unsigned int cell = m.getCell();
	assert(isOrbitEmbedded(cell) || !"Try to release a marker on non embedded cell! ") ;
	m_orbMarker[cell].releaseMarker(m) ;
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

inline Dart GenericMap::begin()
{
	return Dart::create(m_attribs[DART_ORBIT].begin());
}

inline Dart GenericMap::end()
{
	return Dart::create(m_attribs[DART_ORBIT].end()) ;
}

inline void GenericMap::next(Dart& d)
{
	m_attribs[DART_ORBIT].next(d.index) ;
}

} //namespace CGoGN
