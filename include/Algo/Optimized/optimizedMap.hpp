#ifndef __OPTIMIZED_MAP_
#define __OPTIMIZED_MAP_

#include "Topology/generic/functor.h"

namespace CGoGN
{

namespace Algo
{

namespace Optimiz
{

#define OPT_MAX_THREAD 8

template <typename MAP>
class OptimizedMap2
{
	typedef typename MAP::Dart Dart;

protected:
	Dart m_firstVertex[OPT_MAX_THREAD];

	int m_nbVertices;

	Dart m_firstFace[OPT_MAX_THREAD];

	Dart m_endMap;
	Dart m_beginMap;

	int m_nbFaces;

public:
	OptimizedMap2():m_nbVertices(0),m_nbFaces(0)  {}

	void OptimizedForFaces(MAP& the_map);

	void OptimizedForVertices(MAP& the_map);

	void OptimizedBoth(MAP& the_map);

	void foreachFace(FunctorType& funct);

	void foreachVertex(FunctorType& funct);

	void prepareForMT(MAP& the_map);

	void foreachFaceMT2(FunctorType& funct);

	void foreachFaceMT4(FunctorType& funct);

	void foreachFaceMT8(FunctorType& funct);

	void foreachFaceMT2multi(FunctorType& funct1, FunctorType& funct2);

	void foreachFaceMT4multi( FunctorType& funct1,FunctorType& funct2, FunctorType& funct3, FunctorType& funct4);

};


template<typename MAP>
void OptimizedMap2<MAP>::prepareForMT(MAP& the_map)
{
	int nbfbt = m_nbFaces/OPT_MAX_THREAD;

	if (m_nbFaces>0)
	{
		for (int i=1; i<OPT_MAX_THREAD; ++i)
		{
			Dart& d =  m_firstFace[i];
			d = m_firstFace[i-1];
			for (int i=0;i<nbfbt; ++i)
			{
				d++;
			}
		}
	}

	if (m_nbVertices>0)
	{
		for (int i=1; i<OPT_MAX_THREAD; ++i)
		{
			Dart& d =  m_firstVertex[i];
			d = m_firstVertex[i-1];
			for (int i=0;i<nbfbt; ++i)
			{
				d++;
			}
		}
	}
}


template<typename MAP>
void OptimizedMap2<MAP>::OptimizedForFaces(MAP& the_map)
{
	m_endMap = the_map.end();
	m_beginMap = the_map.begin();

	m_nbVertices=0;
	m_nbFaces=0;
	Marker mf = the_map.getNewMarker();

	for (Dart d =the_map.begin(); d != the_map.end(); /* ++ inside */)
	{
		if (!the_map.isMarkedDart(d,mf))
		{
			the_map.markOrbit(2,d,mf);
			Dart dd = d;
			the_map.next(d) ;
			the_map.splice(the_map.begin(), the_map, dd); 
			m_nbFaces++;
		}
		else
			the_map.next(d) ;
	}

	m_firstVertex[0] = the_map.end();
	m_firstFace[0] = the_map.begin();

	the_map.unmarkAll(mf);  
	the_map.releaseMarker(mf);

}


template<typename MAP>
void OptimizedMap2<MAP>::OptimizedForVertices(MAP& the_map)
{
	m_endMap = the_map.end();
	m_beginMap = the_map.begin();

	m_nbFaces=0;
	m_nbVertices=0;
	Marker mv = the_map.getNewMarker();

	for (Dart d =the_map.begin(); d != the_map.end(); /* ++ inside */)
	{
		if (!the_map.isMarkedDart(d,mv))
		{	
			the_map.markOrbit(0,d,mv); 
			Dart dd = d;
			the_map.next(d) ;
			the_map.splice(the_map.begin(), the_map, dd);
			m_nbVertices++;
		}
		else
			the_map.next(d) ;
	}

	m_firstVertex[0] = the_map.begin();
	m_firstFace[0] = the_map.end();

	the_map.unmarkAll(mv);
	the_map.releaseMarker(mv);

}


template<typename MAP>
void OptimizedMap2<MAP>::OptimizedBoth(MAP& the_map)
{
	m_endMap = the_map.end();
	m_beginMap = the_map.begin();

	// dart just afer faces only
	Dart X = the_map.newDart();
	// dart just afer faces & vertices
	Dart Y= the_map.newDart();
	// dart just afer vertices only
	Dart Z= the_map.newDart();	

	the_map.splice(the_map.begin(), the_map, Z);
	the_map.splice(the_map.begin(), the_map, Y);
	the_map.splice(the_map.begin(), the_map, X);

	// ...faces ...X .... faces & vertices .... Y vertices .... Z ... others

	m_nbFaces=0;
	m_nbVertices=0;

	// put one dart per face before X
	Marker mf = the_map.getNewMarker();
	Dart d = Z; d++;
	while ( d != the_map.end())
	{
		if (!the_map.isMarkedDart(d,mf))
		{
			the_map.markOrbit(2,d,mf);
			Dart dd = d;
			the_map.next(d) ;
			the_map.splice(X, the_map, dd); 
			m_nbFaces++;
		}
		else
			the_map.next(d) ;
	}

	// put one dart per vertex before Z
	Marker mv = the_map.getNewMarker();
	d = Z; d++;
	while ( d != the_map.end())
	{
		if (!the_map.isMarkedDart(d,mv))
		{	
			the_map.markOrbit(0,d,mv); 
			Dart dd = d;
			the_map.next(d) ;
			the_map.splice(Z, the_map, dd);
			m_nbVertices++;
		}
		else
			the_map.next(d) ;
	}

	std::cout << "Dart vert before "<< m_nbVertices<<std::endl;

	// move  dart of faces that are not maked before Y 
	d = the_map.begin() ;
	while ( d != X)
	{
		if (!the_map.isMarkedDart(d,mv))
		{	
			the_map.markOrbit(0,d,mv); 
			Dart dd = d;
			the_map.next(d) ;
			the_map.splice(Y, the_map, dd);
			m_nbVertices++;
		}
		else
			the_map.next(d) ;
	}

	std::cout << "Dart vert "<< m_nbVertices<<std::endl;

	m_firstFace[0] = the_map.begin();
	d = X;
	the_map.next(d) ;
	m_firstVertex[0] = d;

	the_map.erase(X);
	the_map.erase(Y);
	the_map.erase(Z);

	the_map.unmarkAll(mf+mv);
	the_map.releaseMarker(mv);
	the_map.releaseMarker(mf);
}


template<typename MAP>
void OptimizedMap2<MAP>::foreachFace(FunctorType& funct)
{
	Dart d = m_firstFace[0];

	for (int i=0; i<m_nbFaces; ++i)
	{
		if (funct(d)) return;
		d++;
	}
}


template<typename MAP>
void OptimizedMap2<MAP>::foreachVertex(FunctorType& funct)
{
	Dart d = m_firstVertex[0];

	for (int i=0; i<m_nbVertices; ++i)
	{
		if (funct(d)) return;
		d++;
	}
}



template<typename MAP>
class ThreadFunctor
{
	typedef typename MAP::Dart Dart;
	
protected:
	Dart m_first;
	int m_nb;
	FunctorType& m_func;
public:
	ThreadFunctor(Dart d, int n,  FunctorType& f): 
	m_first(d), m_nb(n), m_func(f) {}

	void operator()()
	{
		Dart d = m_first;
		for (int i=m_nb; i>0; --i)
		{
			if (m_func(d)) return;
			d++;
		}

	}
};


template<typename MAP>
void OptimizedMap2<MAP>::foreachFaceMT2( FunctorType& funct)
{
	int n = m_nbFaces/2;
	
	boost::thread th1( ThreadFunctor<MAP>(m_firstFace[0],n,funct) );
	n = n + m_nbFaces%2;
	boost::thread th2( ThreadFunctor<MAP>(m_firstFace[4],n,funct) );

	th1.join();
	th2.join();
}


template<typename MAP>
void OptimizedMap2<MAP>::foreachFaceMT4( FunctorType& funct)
{
	int n = m_nbFaces/4;
	
	boost::thread th1( ThreadFunctor<MAP>(m_firstFace[0],n,funct) );
	boost::thread th2( ThreadFunctor<MAP>(m_firstFace[2],n,funct) );
	boost::thread th3( ThreadFunctor<MAP>(m_firstFace[4],n,funct) );
	n = n + m_nbFaces%4;
	boost::thread th4( ThreadFunctor<MAP>(m_firstFace[6],n,funct) );

	th1.join();
	th2.join();
	th3.join();
	th4.join();

}

template<typename MAP>
void OptimizedMap2<MAP>::foreachFaceMT8( FunctorType& funct)
{
	int n = m_nbFaces/8;
	
	boost::thread th1( ThreadFunctor<MAP>(m_firstFace[0],n,funct) );
	boost::thread th2( ThreadFunctor<MAP>(m_firstFace[1],n,funct) );
	boost::thread th3( ThreadFunctor<MAP>(m_firstFace[2],n,funct) );
	boost::thread th4( ThreadFunctor<MAP>(m_firstFace[3],n,funct) );
	boost::thread th5( ThreadFunctor<MAP>(m_firstFace[4],n,funct) );
	boost::thread th6( ThreadFunctor<MAP>(m_firstFace[5],n,funct) );
	boost::thread th7( ThreadFunctor<MAP>(m_firstFace[6],n,funct) );
	n = n + m_nbFaces%8;
	boost::thread th8( ThreadFunctor<MAP>(m_firstFace[7],n,funct) );
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
	th6.join();
	th7.join();
	th8.join();
}



template<typename MAP>
void OptimizedMap2<MAP>::foreachFaceMT2multi( FunctorType& funct1,FunctorType& funct2)
{
	int n = m_nbFaces/2;
	
	boost::thread th1( ThreadFunctor<MAP>(m_firstFace[0],n,funct1) );
	n = n + m_nbFaces%2;
	boost::thread th2( ThreadFunctor<MAP>(m_firstFace[4],n,funct2) );

	th1.join();
	th2.join();
}


template<typename MAP>
void OptimizedMap2<MAP>::foreachFaceMT4multi( FunctorType& funct1,FunctorType& funct2, FunctorType& funct3, FunctorType& funct4)
{
	int n = m_nbFaces/4;
	
	boost::thread th1( ThreadFunctor<MAP>(m_firstFace[0],n,funct1) );
	boost::thread th2( ThreadFunctor<MAP>(m_firstFace[2],n,funct2) );
	boost::thread th3( ThreadFunctor<MAP>(m_firstFace[4],n,funct3) );
	n = n + m_nbFaces%4;
	boost::thread th4( ThreadFunctor<MAP>(m_firstFace[6],n,funct4) );

	th1.join();
	th2.join();
	th3.join();
	th4.join();

}

}//endnamespace
}//endnamespace
}//endnamespace

#endif
