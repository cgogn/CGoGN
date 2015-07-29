/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2013, IGG Team, ICube, University of Strasbourg           *
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

namespace Algo
{

namespace Surface
{

namespace Decimation
{

template <typename PFP, typename T>
int decimate(
	typename PFP::MAP& map,
	SelectorType s,
	ApproximatorType a,
	std::vector<VertexAttribute<T, typename PFP::MAP> >& attribs,
	unsigned int nbWantedVertices,
	EdgeAttribute<typename PFP::REAL, typename PFP::MAP>* edgeErrors,
	void (*callback_wrapper)(void*, const void*),
	void* callback_object
)
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

	assert(attribs.size() >= 1 || !"Decimate: not enough attribs provided") ;

	std::vector<ApproximatorGen<PFP>*> approximators ;
	Selector<PFP>* selector = NULL ;

	switch(a)
	{
		case A_QEM :
			approximators.push_back(new Approximator_QEM<PFP>(map, attribs[0])) ;
			break ;
		case A_MidEdge :
			approximators.push_back(new Approximator_MidEdge<PFP,VEC3>(map, attribs[0])) ;
			break ;
		case A_CornerCutting :
			approximators.push_back(new Approximator_CornerCutting<PFP>(map, attribs[0])) ;
			break ;
		case A_TangentPredict1 :
			approximators.push_back(new Approximator_MidEdge<PFP,VEC3>(map, attribs[0])) ;
			break ;
		case A_TangentPredict2 :
			approximators.push_back(new Approximator_MidEdge<PFP,VEC3>(map, attribs[0])) ;
			break ;
		case A_NormalArea :
			approximators.push_back(new Approximator_NormalArea<PFP>(map, attribs[0])) ;
			break ;
		case A_hHalfCollapse :
			approximators.push_back(new Approximator_HalfCollapse<PFP,VEC3>(map, attribs[0])) ;
			break ;
		case A_ColorNaive :	{
			assert(attribs.size() >= 2 || !"Decimate: A_ColorNaive --> not enough attribs provided") ;
			// pos
			Approximator_QEM<PFP>* attrib0Approx = new Approximator_QEM<PFP>(map, attribs[0]);
			approximators.push_back(attrib0Approx) ;
			const AttributeHandler<T, EDGE, MAP>& attrib0ApproxResult = attrib0Approx->getApproximationResultAttribute();
			// col
			approximators.push_back(new Approximator_ColorNaive<PFP>(map, attribs[1], attribs[0], attrib0ApproxResult)) ;
			}
			break ;
		case A_ColorQEMext :
//            {
//			// pos + col
//			assert(attribs.size() >= 2 || !"Decimate: A_ColorQEMext --> not enough attribs provided") ;
//			approximators.push_back(new Approximator_ColorQEMext<PFP>(map, attribs)) ;
//			}
			break;
		case A_GeomColorOpt :
//          {
//			// pos + col
//			assert(attribs.size() >= 2 || !"Decimate: A_GeomColorOpt --> not enough attribs provided") ;
//			approximators.push_back(new Approximator_GeomColOpt<PFP>(map, attribs)) ;
//			}
			break ;
		case A_hQEM :
			// pos
			approximators.push_back(new Approximator_QEMhalfEdge<PFP>(map, attribs[0])) ;
			break ;
		case A_OTHER:
			break;
	}

	switch(s)
	{
		case S_MapOrder :
			selector = new EdgeSelector_MapOrder<PFP>(map) ;
			break ;
		case S_Random :
			selector = new EdgeSelector_Random<PFP>(map) ;
			break ;
		case S_EdgeLength :
			selector = new EdgeSelector_Length<PFP>(map, attribs[0]) ;
			break ;
		case S_QEM : {
			Approximator<PFP, VEC3, EDGE>* approx = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			selector = new EdgeSelector_QEM<PFP>(map, attribs[0], *approx) ;
			}
			break ;
		case S_QEMml : {
			Approximator<PFP, VEC3, EDGE>* approx = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			selector = new EdgeSelector_QEMml<PFP>(map, attribs[0], *approx) ;
			}
			break ;
		case S_Curvature : {
			Approximator<PFP, VEC3, EDGE>* approx = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			selector = new EdgeSelector_Curvature<PFP>(map, attribs[0], *approx) ;
			}
			break ;
		case S_NormalArea : {
			Approximator<PFP, VEC3, EDGE>* approx = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			selector = new EdgeSelector_NormalArea<PFP>(map, attribs[0], *approx) ;
			}
			break ;
		case S_CurvatureTensor : {
			Approximator<PFP, VEC3, EDGE>* approx = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			selector = new EdgeSelector_CurvatureTensor<PFP>(map, attribs[0], *approx) ;
			}
			break ;
		case S_MinDetail : {
			Approximator<PFP, VEC3, EDGE>* approx = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			selector = new EdgeSelector_MinDetail<PFP>(map, attribs[0], *approx) ;
			}
			break ;
		case S_ColorNaive : {
			Approximator<PFP, VEC3, EDGE>* approx0 = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			Approximator<PFP, VEC3, EDGE>* approx1 = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[1]);
			selector = new EdgeSelector_ColorNaive<PFP>(map, attribs[0], attribs[1], *approx0, *approx1) ;
			}
			break ;
		case S_QEMextColor : {
			Approximator<PFP, VEC3, EDGE>* approx0 = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			Approximator<PFP, VEC3, EDGE>* approx1 = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[1]);
			selector = new EdgeSelector_QEMextColor<PFP>(map, attribs[0], attribs[1], *approx0, *approx1) ;
			}
			break ;
		case S_GeomColOptGrad: {
			Approximator<PFP, VEC3, EDGE>* approx0 = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[0]);
			Approximator<PFP, VEC3, EDGE>* approx1 = dynamic_cast<Approximator<PFP, VEC3, EDGE>*>(approximators[1]);
			selector = new EdgeSelector_GeomColOptGradient<PFP>(map, attribs[0], attribs[1], *approx0, *approx1) ;
			}
			break ;
		case S_hQEMml : {
			Approximator<PFP, VEC3, DART>* approx = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[0]);
			selector = new HalfEdgeSelector_QEMml<PFP>(map, attribs[0], *approx) ;
			}
			break ;
		case S_hQEMextColor : {
			Approximator<PFP, VEC3, DART>* approx0 = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[0]);
			Approximator<PFP, VEC3, DART>* approx1 = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[1]);
			selector = new HalfEdgeSelector_QEMextColor<PFP>(map, attribs[0], attribs[1], *approx0, *approx1) ;
			}
			break ;
		case S_hQEMextColorNormal : {
			Approximator<PFP, VEC3, DART>* approx0 = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[0]);
			Approximator<PFP, VEC3, DART>* approx1 = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[1]);
			Approximator<PFP, VEC3, DART>* approx2 = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[2]);
			selector = new HalfEdgeSelector_QEMextColorNormal<PFP>(map, attribs[0], attribs[1], attribs[2], *approx0, *approx1, *approx2) ;
			}
			break ;
		case S_hColorGradient: {
			Approximator<PFP, VEC3, DART>* approx0 = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[0]);
			Approximator<PFP, VEC3, DART>* approx1 = dynamic_cast<Approximator<PFP, VEC3, DART>*>(approximators[1]);
			selector = new HalfEdgeSelector_ColorGradient<PFP>(map, attribs[0], attribs[1], *approx0, *approx1) ;
			}
			break ;
		case S_OTHER:
			break;
	}

	int status = decimate<PFP>(map, selector, approximators, nbWantedVertices, true, edgeErrors, callback_wrapper, callback_object) ;

	delete selector ;

	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		delete (*it) ;

	return status ;
}

template <typename PFP>
int decimate(
	typename PFP::MAP& map,
	Selector<PFP>* selector,
	std::vector<ApproximatorGen<PFP>*>& approximators,
	unsigned int nbWantedVertices,
	bool recomputePriorityList,
	EdgeAttribute<typename PFP::REAL, typename PFP::MAP>* edgeErrors,
	void (*callback_wrapper)(void*, const void*),
	void* callback_object
)
{
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		(*it)->init() ;

	Dart d ;
	if (recomputePriorityList || !selector->nextEdge(d))
	{
		if(!selector->init())
			return -1 ; // init failed
	}

    unsigned int nbVertices = Algo::Topo::getNbOrbits<VERTEX>(map) ;
	bool finished = false ;

	while(!finished)
	{
		if(!selector->nextEdge(d))
			break ; // finished before achieving amount of required vertices

		--nbVertices ;

		Dart d2 = map.phi2(map.phi_1(d)) ;
		Dart dd2 = map.phi2(map.phi_1(map.phi2(d))) ;

		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		{
			(*it)->approximate(d) ;				// compute approximated attributes
			(*it)->saveApprox(d) ;
		}

		selector->updateBeforeCollapse(d) ;		// update selector

		map.collapseEdge(d) ;					// collapse edge

		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
			(*it)->affectApprox(d2);			// affect data to the resulting vertex

		selector->updateAfterCollapse(d2, dd2) ;// update selector

		if(nbVertices <= nbWantedVertices)
			finished = true ;

		// Progress bar support
		if (callback_wrapper != NULL && callback_object != NULL)
			callback_wrapper(callback_object, &nbVertices) ;
	}

	if (edgeErrors != NULL)
		selector->getEdgeErrors(edgeErrors) ;

	return finished == true ? 0 : 1 ; // finished correctly
}

} // namespace Decimation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
