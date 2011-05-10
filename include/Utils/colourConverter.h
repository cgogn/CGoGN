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
#include "Utils/cgognStream.h"

namespace CGoGN {

namespace Utils {

/**
 * Supported colour spaces
 */
enum ColourEncoding
{
	C_RGB = 0,
	C_XYZ = 1,
	C_Luv = 2,
	C_Lab = 3
} ;

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

public: // methods
	/**
	 * Constructor
	 * @param col a VEC3 colour
	 * @param enc the colour space of provided colour
	 */
	ColourConverter(VEC3 col, enum ColourEncoding enc) ;
	/**
	 * Destructor
	 */
	~ColourConverter() {} ;

	/**
	 * getR
	 * @return original value (in its original space)
	 */
	VEC3 getOriginal() ;
	/**
	 * getR
	 * @return enc value of provided colour
	 */
	VEC3 getColour(enum ColourEncoding enc) ;
	/**
	 * getR
	 * @return RGB value of provided colour
	 */
	VEC3 getRGB() ;
	/**
	 * getR
	 * @return Luv value of provided colour
	 */
	VEC3 getLuv() ;
	/**
	 * getR
	 * @return Lab value of provided colour
	 */
	VEC3 getLab() ;
	/**
	 * getR
	 * @return XYZ value of provided colour
	 */
	VEC3 getXYZ() ;

public: // members
	/**
	 * Colour space of original (unaltered) data
	 */
	enum ColourEncoding originalEnc ;

private: // private members
	VEC3 *RGB ;
	VEC3 *Luv ;
	VEC3 *Lab ;
	VEC3 *XYZ ;

	bool convert(enum ColourEncoding from, enum ColourEncoding to) ;
	void convertRGBtoXYZ() ;
	void convertXYZtoRGB() ;

	void convertXYZtoLuv() ;
	void convertLuvToXYZ() ;

	void convertXYZtoLab() ;
	void convertLabToXYZ() ;

private: // private constants

	// D65 reference white
	static const REAL Xn = 0.950456 ;
	static const REAL Yn = 1.0 ;
	static const REAL Zn = 1.088754 ;

	static const REAL un = 0.197832 ;
	static const REAL vn = 0.468340 ;

} ;

} // namespace Utils

} // namespace CGoGN

#include "colourConverter.hpp"

#endif // __COLOURCONVERTER_H__
