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

namespace Decimation
{

template <typename PFP>
void decimate(
	typename PFP::MAP& map, SelectorType s, ApproximatorType a,
	AttributeHandler<typename PFP::VEC3, VERTEX>& position, unsigned int nbWantedVertices, const FunctorSelect& selected
)
{
	std::vector<ApproximatorGen<PFP>*> approximators ;
	EdgeSelector<PFP>* selector = NULL ;

	switch(a)
	{
		case A_QEM :
			approximators.push_back(new Approximator_QEM<PFP>(map, position)) ;
			break ;
		case A_QEMhalfEdge :
			approximators.push_back(new Approximator_QEMhalfEdge<PFP>(map, position)) ;
			break ;
		case A_MidEdge :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, position)) ;
			break ;
		case A_CornerCutting :
			approximators.push_back(new Approximator_CornerCutting<PFP>(map, position)) ;
			break ;
		case A_TangentPredict1 :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, position)) ;
			break ;
		case A_TangentPredict2 :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, position)) ;
			break ;
		case A_HalfCollapse :
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, position)) ;
			break ;
		case A_LightfieldFull :
		{
			approximators.push_back(new Approximator_QEMhalfEdge<PFP>(map, position)) ;
			/*
			PFP::TVEC3 frame[3] ;
			frame[0] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "frame_T") ; // Tangent
			frame[1] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "frame_B") ; // Bitangent
			frame[2] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "frame_N") ; // Normal
			for (unsigned int i = 0 ; i < 3 ; ++i)
				if (!frame[i].isValid()) {
					CGoGNerr << "In function decimate : frame[" << i << "] is not valid" << CGoGNendl ;
				}

			AttributeHandler<typename PFP::VEC3> colorPTM[6] ;
			colorPTM[0] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "colorPTM_a") ;
			colorPTM[1] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "colorPTM_b") ;
			colorPTM[2] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "colorPTM_c") ;
			colorPTM[3] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "colorPTM_d") ;
			colorPTM[4] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "colorPTM_e") ;
			colorPTM[5] = map.template getAttribute<typename PFP::VEC3>(VERTEX, "colorPTM_f") ;
			for (unsigned int i = 0 ; i < 6 ; ++i)
				if (!colorPTM[i].isValid()) {
					CGoGNerr << "In function decimate : colorPTM[" << i << "] is not valid" << CGoGNendl ;
				}
			*/
			AttributeHandler<Geom::Matrix<3,3,typename PFP::REAL>, VERTEX> frame = map.template getAttribute<Geom::Matrix<3,3,typename PFP::REAL>, VERTEX>("frame") ;
			AttributeHandler<Geom::Matrix<3,6,typename PFP::REAL>, VERTEX> RGBfunctions = map.template getAttribute<Geom::Matrix<3,6,typename PFP::REAL>, VERTEX>("colorPTM") ;
			approximators.push_back(new Approximator_Frame<PFP>(map, frame)) ;
			approximators.push_back(new Approximator_RGBfunctions<PFP>(map, RGBfunctions)) ;
			break ;
		}
		case A_LightfieldHalf :
		{
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, position)) ;
			AttributeHandler<Geom::Matrix<3,3,typename PFP::REAL>, VERTEX> frame = map.template getAttribute<Geom::Matrix<3,3,typename PFP::REAL>, VERTEX>("frame") ;
			AttributeHandler<Geom::Matrix<3,6,typename PFP::REAL>, VERTEX> RGBfunctions = map.template getAttribute<Geom::Matrix<3,6,typename PFP::REAL>, VERTEX>("colorPTM") ;
			approximators.push_back(new Approximator_FrameHalf<PFP>(map, frame)) ;
			approximators.push_back(new Approximator_RGBfunctionsHalf<PFP>(map, RGBfunctions)) ;
			break ;
		}
	}

	switch(s)
	{
		case S_MapOrder :
			selector = new EdgeSelector_MapOrder<PFP>(map, position, approximators, selected) ;
			break ;
		case S_Random :
			selector = new EdgeSelector_Random<PFP>(map, position, approximators, selected) ;
			break ;
		case S_EdgeLength :
			selector = new EdgeSelector_Length<PFP>(map, position, approximators, selected) ;
			break ;
		case S_QEMml :
			selector = new EdgeSelector_QEMml<PFP>(map, position, approximators, selected) ;
			break ;
		case S_QEM :
			selector = new EdgeSelector_QEM<PFP>(map, position, approximators, selected) ;
			break ;
		case S_Curvature :
			selector = new EdgeSelector_Curvature<PFP>(map, position, approximators, selected) ;
			break ;
		case S_MinDetail :
			selector = new EdgeSelector_Random<PFP>(map, position, approximators, selected) ;
			break ;
		case S_hLightfield :
			selector = new HalfEdgeSelector_Lightfield<PFP>(map, position, approximators, selected) ;
			break ;
		case S_hQEMml :
			selector = new HalfEdgeSelector_QEMml<PFP>(map, position, approximators, selected) ;
			break ;
	}

	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		(*it)->init() ;

	if(!selector->init())
		return ;

	unsigned int nbVertices = map.template getNbOrbits<VERTEX>() ;
	bool finished = false ;
	Dart d ;

	while(!finished)
	{
//		CGoGNout << "Countdown : " ;
//		CGoGNout << std::setprecision(8) << (nbVertices - nbWantedVertices) << "\r" << CGoGNflush ;

		if(!selector->nextEdge(d)) {
//			CGoGNout << CGoGNendl << "out" << CGoGNendl ;
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

		if(nbVertices <= nbWantedVertices) {
			finished = true ;
//			CGoGNout << CGoGNendl << "done" << CGoGNendl ;
		}
	}

	delete selector ;
	for(typename std::vector<ApproximatorGen<PFP>*>::iterator it = approximators.begin(); it != approximators.end(); ++it)
		delete (*it) ;
}

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN
