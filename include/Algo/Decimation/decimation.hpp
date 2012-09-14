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
	VertexAttribute<typename PFP::VEC3>& position, unsigned int nbWantedVertices, const FunctorSelect& selected,
	void (*callback_wrapper)(void*, const void*), void* callback_object
)
{
	std::vector<ApproximatorGen<PFP>*> approximators ;
	EdgeSelector<PFP>* selector = NULL ;

	std::vector<VertexAttribute<typename PFP::VEC3>* > v_pos ;
	v_pos.push_back(&position) ;
	switch(a)
	{
		case A_QEM :
			approximators.push_back(new Approximator_QEM<PFP>(map, v_pos)) ;
			break ;
		case A_MidEdge :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, v_pos)) ;
			break ;
		case A_CornerCutting :
			approximators.push_back(new Approximator_CornerCutting<PFP>(map, v_pos)) ;
			break ;
		case A_TangentPredict1 :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, v_pos)) ;
			break ;
		case A_TangentPredict2 :
			approximators.push_back(new Approximator_MidEdge<PFP>(map, v_pos)) ;
			break ;
		case A_hHalfCollapse :
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, v_pos)) ;
			break ;
		case A_ColorNaive :
		{
			// pos
			approximators.push_back(new Approximator_QEM<PFP>(map, v_pos)) ;

			// col
			std::vector<VertexAttribute<typename PFP::VEC3>* > v_col ;
			VertexAttribute<typename PFP::VEC3> colors = map.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
			v_col.push_back(&colors) ;
			approximators.push_back(new Approximator_ColorNaive<PFP>(map, v_col)) ;
		}
		break ;
		case A_ColorQEMext :
		{
			// pos+col
			std::vector<VertexAttribute<typename PFP::VEC3>* > v_poscol ;
			v_poscol.push_back(&position) ; // pos
			VertexAttribute<typename PFP::VEC3> colors = map.template getAttribute<typename PFP::VEC3, VERTEX>("color") ;
			v_poscol.push_back(&colors) ; // col
			approximators.push_back(new Approximator_ColorQEMext<PFP>(map, v_poscol)) ;
		}
		break;
		case A_hQEM :
			approximators.push_back(new Approximator_QEMhalfEdge<PFP>(map, v_pos)) ;
		break ;
		case A_hLightfieldHalf :
		{
			// pos
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, v_pos)) ;

			// frame
			std::vector<VertexAttribute<typename PFP::VEC3>* > v_frame ;
			VertexAttribute<typename PFP::VEC3> FT = map.template getAttribute<typename PFP::VEC3, VERTEX>("frameT") ;
			VertexAttribute<typename PFP::VEC3> FB = map.template getAttribute<typename PFP::VEC3, VERTEX>("frameB") ;
			VertexAttribute<typename PFP::VEC3> FN = map.template getAttribute<typename PFP::VEC3, VERTEX>("frameN") ;
			v_frame.push_back(&FT) ;
			v_frame.push_back(&FB) ;
			v_frame.push_back(&FN) ;
			approximators.push_back(new Approximator_FrameHalf<PFP>(map, v_frame)) ;

//			TODO
//			// function coefs
//			std::vector<VertexAttribute<typename PFP::VEC3>* > v_coefs ;
//			VertexAttribute<typename PFP::VEC3> coefs = map.template getAttribute<typename PFP::VEC3, VERTEX>("SLFcoefs_0") ;
//			v_frame.push_back(&coefs) ;
//			approximators.push_back(new Approximator_LFcoefs<PFP>(map, v_coefs)) ;
		}
		break ;
		/*case A_LightfieldHalf:
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, position)) ;

			// Get all frame embeddings
			std::vector<VertexAttribute<PFP::VEC3>* > vertexAttributesFrame ;
			VertexAttribute<PFP::VEC3> frame0 = myMap.getAttribute<PFP::VEC3, VERTEX>("frameT") ; 	vertexAttributesFrame.push_back(&frame0) ;
			VertexAttribute<PFP::VEC3> frame1 = myMap.getAttribute<PFP::VEC3, VERTEX>("frameB") ; 	vertexAttributesFrame.push_back(&frame1) ;
			VertexAttribute<PFP::VEC3> frame2 = myMap.getAttribute<PFP::VEC3, VERTEX>("frameN") ;	vertexAttributesFrame.push_back(&frame2) ;
			approximators.push_back(new Approximator_FrameHalf<PFP>(map, vertexAttributesFrame)) ; // TODO

			// Get all coefficient embeddings
			const unsigned int K = 200 ;
			std::vector<VertexAttribute<PFP::VEC3>* > vertexAttributesCoefs ;
			VertexAttribute<PFP::VEC3> *PBcoefs = new VertexAttribute<PFP::VEC3>[K] ;
			for (unsigned int k = 0 ; k < K ; ++k)
			{
				std::stringstream s ;
				s << "PBcoefs" << k ;
				PBcoefs[k] = myMap.getAttribute<PFP::VEC3, VERTEX>(s.str()) ;
				if (!PBcoefs[k].isValid())
					break ;
				vertexAttributesCoefs.push_back(&PBcoefs[k]) ;
			}
			const bool& sh = vertexAttributesCoefs.empty() ; // sh or pb
			VertexAttribute<PFP::VEC3> *SHcoefs = new VertexAttribute<PFP::VEC3>[K] ;
			for (unsigned int k = 0 ; k < K ; ++k)
			{
				std::stringstream s ;
				s << "SHcoefs" << k ;
				SHcoefs[k] = myMap.getAttribute<PFP::VEC3, VERTEX>(s.str()) ;
				if (!SHcoefs[k].isValid())
					break ;
				vertexAttributesCoefs.push_back(&SHcoefs[k]) ;
			}
			approximators.push_back(new Approximator_LightfieldCoefsHalf<PFP>(map, vertexAttributesCoefs, sh)) ; // TODO
			break ;
		case A_LightfieldFull_deprecated :
		{
			approximators.push_back(new Approximator_QEMhalfEdge<PFP>(map, position)) ;
			/ *
			PFP::TVEC3 frame[3] ;
			frame[0] = map.template getAttribute<typename PFP::VEC3, VERTEX>("frame_T") ; // Tangent
			frame[1] = map.template getAttribute<typename PFP::VEC3, VERTEX>("frame_B") ; // Bitangent
			frame[2] = map.template getAttribute<typename PFP::VEC3, VERTEX>("frame_N") ; // Normal
			for (unsigned int i = 0 ; i < 3 ; ++i)
				if (!frame[i].isValid()) {
					CGoGNerr << "In function decimate : frame[" << i << "] is not valid" << CGoGNendl ;
				}

			VertexAttribute<typename PFP::VEC3> colorPTM[6] ;
			colorPTM[0] = map.template getAttribute<typename PFP::VEC3, VERTEX>("colorPTM_a") ;
			colorPTM[1] = map.template getAttribute<typename PFP::VEC3, VERTEX>("colorPTM_b") ;
			colorPTM[2] = map.template getAttribute<typename PFP::VEC3, VERTEX>("colorPTM_c") ;
			colorPTM[3] = map.template getAttribute<typename PFP::VEC3, VERTEX>("colorPTM_d") ;
			colorPTM[4] = map.template getAttribute<typename PFP::VEC3, VERTEX>("colorPTM_e") ;
			colorPTM[5] = map.template getAttribute<typename PFP::VEC3, VERTEX>("colorPTM_f") ;
			for (unsigned int i = 0 ; i < 6 ; ++i)
				if (!colorPTM[i].isValid()) {
					CGoGNerr << "In function decimate : colorPTM[" << i << "] is not valid" << CGoGNendl ;
				}
			*
			VertexAttribute<Geom::Matrix<3,3,typename PFP::REAL> > frame = map.template getAttribute<Geom::Matrix<3,3,typename PFP::REAL>, VERTEX>("frame") ;
			VertexAttribute<Geom::Matrix<3,6,typename PFP::REAL> > RGBfunctions = map.template getAttribute<Geom::Matrix<3,6,typename PFP::REAL>, VERTEX>("colorPTM") ;
			approximators.push_back(new Approximator_Frame_deprecated<PFP>(map, frame)) ;
			approximators.push_back(new Approximator_RGBfunctions_deprecated<PFP>(map, RGBfunctions)) ;
			break ;
		}
		case A_LightfieldHalf_deprecated :
		{
			approximators.push_back(new Approximator_HalfCollapse<PFP>(map, position)) ;
			VertexAttribute<Geom::Matrix<3,3,typename PFP::REAL> > frame = map.template getAttribute<Geom::Matrix<3,3,typename PFP::REAL>, VERTEX>("frame") ;
			VertexAttribute<Geom::Matrix<3,6,typename PFP::REAL> > RGBfunctions = map.template getAttribute<Geom::Matrix<3,6,typename PFP::REAL>, VERTEX>("colorPTM") ;
			approximators.push_back(new Approximator_FrameHalf_deprecated<PFP>(map, frame)) ;
			approximators.push_back(new Approximator_RGBfunctionsHalf_deprecated<PFP>(map, RGBfunctions)) ;
			break ;
		}*/
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
			selector = new EdgeSelector_MinDetail<PFP>(map, position, approximators, selected) ;
			break ;
		case S_ColorNaive :
			selector = new EdgeSelector_ColorNaive<PFP>(map, position, approximators, selected) ;
			break ;
		case S_hQEMml :
			selector = new HalfEdgeSelector_QEMml<PFP>(map, position, approximators, selected) ;
			break ;
		case S_hLightfield :
			selector = new HalfEdgeSelector_Lightfield<PFP>(map, position, approximators, selected) ;
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
}

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN
