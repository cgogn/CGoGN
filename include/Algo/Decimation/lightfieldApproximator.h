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

#ifndef __LIGHTFIELD_APPROXIMATOR_H__
#define __LIGHTFIELD_APPROXIMATOR_H__

#include "Algo/Decimation/approximator.h"
#include "Topology/generic/mapBrowser.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

template <typename PFP>
class Approximator_FrameHalf : public Approximator<PFP, typename Geom::Matrix<3,3,typename PFP::REAL> >
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	typedef Geom::Matrix<3,3,REAL> MATRIX33 ;
	typedef Geom::Matrix<3,6,REAL> MATRIX36 ;

public:
	Approximator_FrameHalf(MAP& m, VertexAttribute<MATRIX33>& frame, Predictor<PFP, MATRIX33>* pred = NULL) :
		Approximator<PFP, MATRIX33>(m, frame, pred)
	{}
	~Approximator_FrameHalf()
	{}
	ApproximatorType getType() const { return A_LightfieldHalf ; }
	bool init() { return true ; } ;
	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_RGBfunctionsHalf : public Approximator<PFP, typename Geom::Matrix<3,6,typename PFP::REAL> >
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	typedef Geom::Matrix<3,3,REAL> MATRIX33 ;
	typedef Geom::Matrix<3,6,REAL> MATRIX36 ;

protected:
	VertexAttribute<MATRIX33> m_frame ;
	EdgeAttribute<MATRIX33> m_approxFrame ;
	EdgeAttribute<QuadricRGBfunctions<REAL> > m_quadricRGBfunctions ;

public:
	Approximator_RGBfunctionsHalf(MAP& m, VertexAttribute<MATRIX36>& rgbfunctions, Predictor<PFP, MATRIX36>* pred = NULL) :
		Approximator<PFP, MATRIX36>(m, rgbfunctions, pred)
	{ }
	~Approximator_RGBfunctionsHalf	()
	{}
	ApproximatorType getType() const { return A_LightfieldHalf ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_Frame : public Approximator<PFP, typename Geom::Matrix<3,3,typename PFP::REAL> >
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	typedef Geom::Matrix<3,3,REAL> MATRIX33 ;
	typedef Geom::Matrix<3,6,REAL> MATRIX36 ;

protected:
	VertexAttribute<VEC3> m_position ;
	EdgeAttribute<VEC3> m_approxPosition ;

public:
	Approximator_Frame(MAP& m, VertexAttribute<MATRIX33>& frame, Predictor<PFP, MATRIX33>* pred = NULL) :
		Approximator<PFP, MATRIX33>(m, frame, pred)
	{}
	~Approximator_Frame()
	{}
	ApproximatorType getType() const { return A_LightfieldFull ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_RGBfunctions : public Approximator<PFP, typename Geom::Matrix<3,6,typename PFP::REAL> >
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	typedef Geom::Matrix<3,3,REAL> MATRIX33 ;
	typedef Geom::Matrix<3,6,REAL> MATRIX36 ;

protected:
	VertexAttribute<MATRIX33> m_frame ;
	EdgeAttribute<MATRIX33> m_approxFrame ;
	EdgeAttribute<QuadricRGBfunctions<REAL> > m_quadricRGBfunctions ;

public:
	Approximator_RGBfunctions(MAP& m, VertexAttribute<MATRIX36>& rgbfunctions, Predictor<PFP, MATRIX36>* pred = NULL) :
		Approximator<PFP, MATRIX36>(m, rgbfunctions, pred)
	{ }
	~Approximator_RGBfunctions()
	{}
	ApproximatorType getType() const { return A_LightfieldFull ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN

#include "Algo/Decimation/lightfieldApproximator.hpp"

#endif
