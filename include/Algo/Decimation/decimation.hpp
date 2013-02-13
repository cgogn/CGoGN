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

namespace Algo
{

namespace Surface
{

namespace Decimation
{

template <typename PFP>
void decimate(
	typename PFP::MAP& map, SelectorType s, ApproximatorType a,
	std::vector<VertexAttribute<typename PFP::VEC3>* >& attribs, unsigned int nbWantedVertices,
	EdgeAttribute<typename PFP::REAL> *edgeErrors,
	void (*callback_wrapper)(void*, const void*), void* callback_object
)
{
	assert(attribs.size() >= 1 || !"Decimate: not enough attribs provided") ;
	assert(attribs[0]->name() == "position" || !"Decimate: first attribute is not position") ;
	VertexAttribute<typename PFP::VEC3> position = *(attribs[0]) ;

	std::vector<ApproximatorGen<PFP>*> approximators ;
	EdgeSelector<PFP>* selector = NULL ;

	std::vector<VertexAttribute<typename PFP::VEC3>* > *v_approx = NULL ;

	switch(a)
	{
		case A_QEM :
			approximators.push_back(new Approximator_QEM<PFP>(map, attribs)) ;
			break ;
		case A_MidEdge :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, attribs)) ;
			break ;
		case A_CornerCutting :
			approximators.push_back(new Approximator_CornerCutting<PFP>(map, attribs)) ;
			break ;
		case A_TangentPredict1 :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, attribs)) ;
			break ;
		case A_TangentPredict2 :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, attribs)) ;
			break ;
		case A_NormalArea :
			approximators.push_back(new Approximator_NormalArea<PFP>(map, attribs)) ;
			break ;
		case A_hHalfCollapse :
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, attribs)) ;
			break ;
		case A_ColorNaive :
		{
			v_approx = new std::vector<VertexAttribute<typename PFP::VEC3>* >[2] ;

			// pos
			v_approx[0].push_back(attribs[0]) ;
			approximators.push_back(new Approximator_QEM<PFP>(map, v_approx[0])) ;

			// col
			assert(attribs.size() >= 2 || !"Decimate: A_ColorNaive --> not enough attribs provided") ;
			v_approx[1].push_back(attribs[1]) ;
			approximators.push_back(new Approximator_ColorNaive<PFP>(map, v_approx[1])) ;
		}
		break ;
		case A_ColorQEMext :
		{
			// pos + col
			assert(attribs.size() >= 2 || !"Decimate: A_ColorQEMext --> not enough attribs provided") ;
			approximators.push_back(new Approximator_ColorQEMext<PFP>(map, attribs)) ;
		}
		break;
		case A_hQEM :
			// pos
			approximators.push_back(new Approximator_QEMhalfEdge<PFP>(map, attribs)) ;
		break ;
		case A_hLightfieldHalf:
		{
			v_approx = new std::vector<VertexAttribute<typename PFP::VEC3>* >[3] ;

			// pos
			v_approx[0].push_back(attribs[0]) ;
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, v_approx[0])) ;

			// frame
			assert(attribs.size() >= 4 || !"Decimate: A_hLightfieldHalf --> not enough attribs provided") ;
			for (unsigned int i = 0 ; i < 3 ; ++i)
				v_approx[1].push_back(attribs[i+1]) ;
			approximators.push_back(new Approximator_FrameInterpolationHalfEdge<PFP>(map, v_approx[1])) ;

			// hemifunction
			assert(attribs.size() >= 5 || !"Decimate: A_hLightfieldHalf --> not enough attribs provided") ;
			for (unsigned int i = 0 ; i < attribs.size() - 4 ; ++i)
				v_approx[2].push_back(attribs[i+4]) ;
			approximators.push_back(new Approximator_HemiFuncCoefsHalfEdge<PFP>(map, v_approx[2])) ;
		}
		break ;
		case A_Lightfield :
		{
			v_approx = new std::vector<VertexAttribute<typename PFP::VEC3>* >[3] ;

			// pos
			v_approx[0].push_back(attribs[0]) ;
			approximators.push_back(new Approximator_QEM<PFP>(map, v_approx[0])) ;

			// frame
			assert(attribs.size() >= 4 || !"Decimate: A_Lightfield --> not enough attribs provided") ;
			for (unsigned int i = 0 ; i < 3 ; ++i)
				v_approx[1].push_back(attribs[i+1]) ;
			approximators.push_back(new Approximator_FrameInterpolation<PFP>(map, v_approx[1])) ;

			// hemifunction
			assert(attribs.size() >= 5 || !"Decimate: A_Lightfield --> not enough attribs provided") ;
			for (unsigned int i = 0 ; i < attribs.size() - 4 ; ++i)
				v_approx[2].push_back(attribs[i+4]) ;
			approximators.push_back(new Approximator_HemiFuncCoefs<PFP>(map, v_approx[2])) ;
		}
		break ;
	}

	switch(s)
	{
		case S_MapOrder :
			selector = new EdgeSelector_MapOrder<PFP>(map, position, approximators) ;
			break ;
		case S_Random :
			selector = new EdgeSelector_Random<PFP>(map, position, approximators) ;
			break ;
		case S_EdgeLength :
			selector = new EdgeSelector_Length<PFP>(map, position, approximators) ;
			break ;
		case S_QEMml :
			selector = new EdgeSelector_QEMml<PFP>(map, position, approximators) ;
			break ;
		case S_QEM :
			selector = new EdgeSelector_QEM<PFP>(map, position, approximators) ;
			break ;
		case S_Curvature :
			selector = new EdgeSelector_Curvature<PFP>(map, position, approximators) ;
			break ;
		case S_NormalArea :
			selector = new EdgeSelector_NormalArea<PFP>(map, position, approximators) ;
			break ;
		case S_CurvatureTensor :
			selector = new EdgeSelector_CurvatureTensor<PFP>(map, position, approximators) ;
			break ;
		case S_MinDetail :
			selector = new EdgeSelector_MinDetail<PFP>(map, position, approximators) ;
			break ;
		case S_ColorNaive :
			selector = new EdgeSelector_ColorNaive<PFP>(map, position, approximators) ;
			break ;
		case S_QEMextColor :
			selector = new EdgeSelector_QEMextColor<PFP>(map, position, approximators) ;
			break ;
		case S_hQEMextColor :
			selector = new HalfEdgeSelector_QEMextColor<PFP>(map, position, approximators) ;
			break ;
		case S_hQEMml :
			selector = new HalfEdgeSelector_QEMml<PFP>(map, position, approximators) ;
			break ;
		case S_Lightfield :
			selector = new EdgeSelector_Lightfield<PFP>(map, position, approximators) ;
			break ;
		case S_hLightfield :
			selector = new HalfEdgeSelector_Lightfield<PFP>(map, position, approximators) ;
			break ;
		case S_hLightfieldExp :
			selector = new HalfEdgeSelector_LightfieldExp<PFP>(map, position, approximators) ;
			break ;
		case S_hLightfieldKCL :
			selector = new HalfEdgeSelector_LightfieldKCL<PFP>(map, position, approximators) ;
			break ;
	}

	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		(*it)->init() ;

	if(!selector->init())
	{
		delete selector ;

		for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
			delete (*it) ;

		delete[] v_approx ;

		return ;
	}

	if (edgeErrors != NULL)
		selector->getEdgeErrors(edgeErrors) ;

	unsigned int nbVertices = map.template getNbOrbits<VERTEX>() ;
	bool finished = false ;
	Dart d ;

	while(!finished)
	{
		if(!selector->nextEdge(d))
		{
			break ;
		}

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
		{
			finished = true ;
		}

		// Progress bar support
		if (callback_wrapper != NULL && callback_object != NULL)
			callback_wrapper(callback_object, &nbVertices) ;
	}

	delete selector ;

	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		delete (*it) ;

	delete[] v_approx ;
}

} //namespace Decimation

}

} //namespace Algo

} //namespace CGoGN
