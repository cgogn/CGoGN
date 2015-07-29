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

#include "Utils/threadbarrier.h"
#include "Algo/Topo/embedding.h"


#include <vector>

namespace CGoGN
{

template <typename T, unsigned int ORB, typename MAP>
inline void AttributeHandler<T, ORB, MAP>::registerInMap()
{
	std::lock_guard<std::mutex> lockAH(m_map->attributeHandlersMutex);
	m_map->attributeHandlers.insert(std::pair<AttributeMultiVectorGen*, AttributeHandlerGen*>(m_attrib, this)) ;
}

template <typename T, unsigned int ORB, typename MAP>
inline void AttributeHandler<T, ORB, MAP>::unregisterFromMap()
{
	typedef std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator IT ;

	std::lock_guard<std::mutex> lockAH(m_map->attributeHandlersMutex);
	std::pair<IT, IT> bounds = m_map->attributeHandlers.equal_range(m_attrib) ;
	for(IT i = bounds.first; i != bounds.second; ++i)
	{
		if((*i).second == this)
		{
			m_map->attributeHandlers.erase(i) ;
			return ;
		}
	}
	assert(false || !"Should not get here") ;
}

// =================================================================

template <typename T, unsigned int ORB, typename MAP>
AttributeHandler<T, ORB, MAP>::AttributeHandler() :
	AttributeHandlerOrbit<ORB>(false),
	m_map(NULL),
	m_attrib(NULL)
{}

template <typename T, unsigned int ORB, typename MAP>
AttributeHandler<T, ORB, MAP>::AttributeHandler(MAP* m, AttributeMultiVector<T>* amv) :
	AttributeHandlerOrbit<ORB>(false),
	m_map(m),
	m_attrib(amv)
{
	if(m != NULL && amv != NULL && amv->getIndex() != AttributeContainer::UNKNOWN)
	{
		assert(ORB == amv->getOrbit() || !"AttributeHandler: orbit incompatibility") ;
		this->valid = true ;
		registerInMap() ;
	}
	else
		this->valid = false ;
}

template <typename T, unsigned int ORB, typename MAP>
AttributeHandler<T, ORB, MAP>::AttributeHandler(const AttributeHandler<T, ORB, MAP>& ta) :
	AttributeHandlerOrbit<ORB>(ta.valid),
	m_map(ta.m_map),
	m_attrib(ta.m_attrib)
{
	if(this->valid)
		registerInMap() ;
}

//template <typename T, unsigned int ORB, typename MAP>
//template <unsigned int ORBIT2>
//AttributeHandler<T, ORB, MAP>::AttributeHandler(const AttributeHandler<T, ORBIT2, MAP>& h) :
//	AttributeHandlerOrbit<ORB>(h.valid),
//	m_map(h.m_map),
//	m_attrib(h.m_attrib)
//{
//	if(m_attrib->getOrbit() == ORBIT2)
//	{
//		if(this->valid)
//			registerInMap() ;
//	}
//	else
//		this->valid = false;
//}

template <typename T, unsigned int ORB, typename MAP>
inline AttributeHandler<T, ORB, MAP>& AttributeHandler<T, ORB, MAP>::operator=(const AttributeHandler<T, ORB, MAP>& ta)
{
	if(this->valid)
		unregisterFromMap() ;
	m_map = ta.m_map ;
	m_attrib = ta.m_attrib ;
	this->valid = ta.valid ;
	if(this->valid)
		registerInMap() ;
	return *this ;
}

template <typename T, unsigned int ORB, typename MAP>
template <unsigned int ORBIT2>
inline AttributeHandler<T, ORB, MAP>& AttributeHandler<T, ORB, MAP>::operator=(const AttributeHandler<T, ORBIT2, MAP>& ta)
{
	if(this->valid)
		unregisterFromMap() ;
	m_map = ta.map() ;
	m_attrib = ta.getDataVector() ;
	this->valid = ta.isValid() ;
	if(this->valid)
		registerInMap() ;
	return *this ;
}

template <typename T, unsigned int ORB, typename MAP>
AttributeHandler<T, ORB, MAP>::~AttributeHandler()
{
	if(this->valid)
		unregisterFromMap() ;
}

template <typename T, unsigned int ORB, typename MAP>
inline AttributeMultiVector<T>* AttributeHandler<T, ORB, MAP>::getDataVector() const
{
	return m_attrib ;
}

template <typename T, unsigned int ORB, typename MAP>
inline AttributeMultiVectorGen* AttributeHandler<T, ORB, MAP>::getDataVectorGen() const
{
	return m_attrib ;
}

template <typename T, unsigned int ORB, typename MAP>
inline int AttributeHandler<T, ORB, MAP>::getSizeOfType() const
{
	return sizeof(T) ;
}

template <typename T, unsigned int ORB, typename MAP>
inline unsigned int AttributeHandler<T, ORB, MAP>::getOrbit() const
{
	return ORB ;
}

template <typename T, unsigned int ORB, typename MAP>
inline unsigned int AttributeHandler<T, ORB, MAP>::getIndex() const
{
	return m_attrib->getIndex() ;
}

template <typename T, unsigned int ORB, typename MAP>
inline const std::string& AttributeHandler<T, ORB, MAP>::name() const
{
	return m_attrib->getName() ;
}

template <typename T, unsigned int ORB, typename MAP>
inline const std::string& AttributeHandler<T, ORB, MAP>::typeName() const
{
	return m_attrib->getTypeName();
}


template <typename T, unsigned int ORB, typename MAP>
inline unsigned int AttributeHandler<T, ORB, MAP>::nbElements() const
{
	return m_map->template getAttributeContainer<ORB>().size() ;
}

template <typename T, unsigned int ORB, typename MAP>
inline T& AttributeHandler<T, ORB, MAP>::operator[](Cell<ORB> c)
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	unsigned int a = m_map->getEmbedding(c) ;

	if (a == EMBNULL)
		a = Algo::Topo::setOrbitEmbeddingOnNewCell(*m_map, c) ;
	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORB, typename MAP>
inline const T& AttributeHandler<T, ORB, MAP>::operator[](Cell<ORB> c) const
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	unsigned int a = m_map->getEmbedding(c) ;
	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORB, typename MAP>
inline T& AttributeHandler<T, ORB, MAP>::operator[](unsigned int a)
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORB, typename MAP>
inline const T& AttributeHandler<T, ORB, MAP>::operator[](unsigned int a) const
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	return m_attrib->operator[](a) ;
}

template <typename T, unsigned int ORB, typename MAP>
inline unsigned int AttributeHandler<T, ORB, MAP>::insert(const T& elt)
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	unsigned int idx = m_map->template getAttributeContainer<ORB>().insertLine() ;
	m_attrib->operator[](idx) = elt ;
	return idx ;
}

template <typename T, unsigned int ORB, typename MAP>
inline unsigned int AttributeHandler<T, ORB, MAP>::newElt()
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	unsigned int idx = m_map->template getAttributeContainer<ORB>().insertLine() ;
	return idx ;
}

template <typename T, unsigned int ORB, typename MAP>
inline void AttributeHandler<T, ORB, MAP>::setAllValues(const T& v)
{
	for(unsigned int i = begin(); i != end(); next(i))
		m_attrib->operator[](i) = v ;
}

template <typename T, unsigned int ORB, typename MAP>
inline unsigned int AttributeHandler<T, ORB, MAP>::begin() const
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	return m_map->template getAttributeContainer<ORB>().begin() ;
}

template <typename T, unsigned int ORB, typename MAP>
inline unsigned int AttributeHandler<T, ORB, MAP>::end() const
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	return m_map->template getAttributeContainer<ORB>().end() ;
}

template <typename T, unsigned int ORB, typename MAP>
inline void AttributeHandler<T, ORB, MAP>::next(unsigned int& iter) const
{
	assert(this->valid || !"Invalid AttributeHandler") ;
	m_map->template getAttributeContainer<ORB>().next(iter) ;
}


template <typename T, unsigned int ORB, typename MAP>
inline AttributeHandlerIter<T,ORB,MAP> AttributeHandler<T, ORB, MAP>::iterable() const
{
	return AttributeHandlerIter<T,ORB,MAP>(*this);
}


namespace Parallel
{

template <typename FUNC>
class ThreadFunctionAttrib
{
protected:
	std::vector<unsigned int>& m_ids;
	Utils::Barrier& m_sync1;
	Utils::Barrier& m_sync2;
	bool& m_finished;
	unsigned int m_id;
	FUNC m_lambda;
public:
	ThreadFunctionAttrib(FUNC func, std::vector<unsigned int>& vid, Utils::Barrier& s1, Utils::Barrier& s2, bool& finished, unsigned int id):
		m_ids(vid), m_sync1(s1), m_sync2(s2), m_finished(finished), m_id(id), m_lambda(func)
	{
	}

	ThreadFunctionAttrib(const ThreadFunctionAttrib& tf):
		m_ids(tf.m_ids), m_sync1(tf.m_sync1), m_sync2(tf.m_sync2), m_finished(tf.m_finished), m_id(tf.m_id), m_lambda(tf.m_lambda){}

	void operator()()
	{
		while (!m_finished)
		{
			for (std::vector<unsigned int>::const_iterator it = m_ids.begin(); it != m_ids.end(); ++it)
				m_lambda(*it,m_id);
			m_sync1.wait();
			m_sync2.wait();
		}
	}
};



template <typename ATTR, typename FUNC>
void foreach_attribute(ATTR& attribute, FUNC func, unsigned int nbthread)
{
	// thread 0 is for attribute traversal
	unsigned int nbth = nbthread -1;

	std::vector< unsigned int >* vd = new std::vector< unsigned int >[nbth];
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(SIZE_BUFFER_THREAD);

	unsigned int nb = 0;
	unsigned int attIdx = attribute.begin();
	while ((attIdx != attribute.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
	{
		vd[nb%nbth].push_back(attIdx);
		nb++;
		attribute.next(attIdx);
	}
	Utils::Barrier sync1(nbth+1);
	Utils::Barrier sync2(nbth+1);
	bool finished=false;


	std::thread** threads = new std::thread*[nbth];
	ThreadFunctionAttrib<FUNC>** tfs = new ThreadFunctionAttrib<FUNC>*[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
	{
		tfs[i] = new ThreadFunctionAttrib<FUNC>(func, vd[i], sync1,sync2,finished,i);
		threads[i] = new std::thread( std::ref( *(tfs[i]) ) );
	}

	// and continue to traverse the map
	std::vector< unsigned int >* tempo = new std::vector< unsigned int >[nbth];
	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(SIZE_BUFFER_THREAD);

	while (attIdx != attribute.end())
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();
		unsigned int nb = 0;

		while ((attIdx != attribute.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
		{
			tempo[nb%nbth].push_back(attIdx);
			nb++;
			attribute.next(attIdx);
		}
		sync1.wait();// wait for all thread to finish its vector
		for (unsigned int i = 0; i < nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();// everybody refilled then go
	}

	sync1.wait();// wait for all thread to finish its vector
	finished = true;// say finsih to everyone
	sync2.wait(); // just wait for last barrier wait !

	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
		delete tfs[i];
	}
	delete[] tfs;
	delete[] threads;
	delete[] vd;
	delete[] tempo;
}

}

} //namespace CGoGN
