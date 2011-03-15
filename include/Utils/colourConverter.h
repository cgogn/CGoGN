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

#ifndef __COLOURCONVERTER_H__
#define __COLOURCONVERTER_H__

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"

namespace CGoGN {

namespace Utils {

/**
 * Class for colour conversions between the enumerated colour spaces.
 * Usage :
 *  VEC3 colRGB ; 								// current colour in RGB for example
 *  ColourConverter<REAL> cc(colRGB,C_RGB) ;	// Tell constructor you provided RGB
 *  VEC3 colLuv = cc.getLuv() ; 				// ask whatever supported colour type you require
 *
 * All conversion formulae were provided by "Notes about color", january 5th, 2011 by B. Sauvage
 */
template <typename REAL>
class ColourConverter
{

public: // types
	typedef Geom::Vector<3,REAL> VEC3 ;
	enum ColourEncoding { C_RGB, C_XYZ, C_Luv } ;

public: // methods
	ColourConverter(VEC3 col, enum ColourEncoding enc) ;
	~ColourConverter() {} ;

	VEC3 getRGB() ;
	VEC3 getLuv() ;
	VEC3 getXYZ() ;

public: // members
	enum ColourEncoding originalEnc ;

private: // private members
	VEC3 *RGB ;
	VEC3 *Luv ;
	VEC3 *XYZ ;

	bool convert(enum ColourEncoding from, enum ColourEncoding to) ;
	void convertRGBtoXYZ() ;
	void convertXYZtoLuv() ;
	void convertLuvToXYZ() ;
	void convertXYZtoRGB() ;


} ;

} // namespace Utils

} // namespace CGoGN

#include "colourConverter.hpp"

#endif // __COLOURCONVERTER_H__
