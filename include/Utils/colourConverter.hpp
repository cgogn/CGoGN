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

namespace CGoGN {
namespace Utils {

template<typename REAL>
ColourConverter<REAL>::ColourConverter(VEC3 col, enum ColourEncoding enc) :
	RGB(NULL),
	Luv(NULL),
	XYZ(NULL)
{
	originalEnc = enc ;

	switch(originalEnc) {
		case(C_RGB):
			RGB = new VEC3(col) ;
			break ;

		case (C_Luv) :
			Luv = new VEC3(col) ;
			break ;

		case (C_XYZ) :
			XYZ = new VEC3(col) ;
			break ;
	}
}

template<typename REAL>
Geom::Vector<3,REAL> ColourConverter<REAL>::getRGB() {
	if (RGB == NULL)
		convert(originalEnc,C_RGB) ;

	return *RGB ;
}

template<typename REAL>
Geom::Vector<3,REAL> ColourConverter<REAL>::getLuv() {
	if (Luv == NULL)
		convert(originalEnc,C_Luv) ;

	return *Luv ;
}

template<typename REAL>
Geom::Vector<3,REAL> ColourConverter<REAL>::getXYZ() {
	if (XYZ == NULL) {
		convert(originalEnc,C_XYZ) ;
	}

	return *XYZ ;
}

template<typename REAL>
void ColourConverter<REAL>::convertRGBtoXYZ() {
	Geom::Matrix<3,3,REAL> M ;

	M(0,0) = 0.412453 ;
	M(0,1) = 0.357580 ;
	M(0,2) = 0.180423 ;

	M(1,0) = 0.212671 ;
	M(1,1) = 0.715160 ;
	M(1,2) = 0.072169 ;

	M(2,0) = 0.019334 ;
	M(2,1) = 0.119193 ;
	M(2,2) = 0.950227 ;

	VEC3 c = M * (*RGB) ;

	XYZ = new VEC3(c) ;
}

template<typename REAL>
void ColourConverter<REAL>::convertXYZtoRGB() {
	Geom::Matrix<3,3,REAL> M ;

	M(0,0) = 3.240479 ;
	M(0,1) = -1.537150 ;
	M(0,2) = -0.498535 ;

	M(1,0) = -0.969256 ;
	M(1,1) = 1.875992 ;
	M(1,2) = 0.041556 ;

	M(2,0) = 0.055648 ;
	M(2,1) = -0.204043 ;
	M(2,2) = 1.057311 ;

	VEC3 c = M * (*XYZ) ;

	RGB = new VEC3(c) ;
}

template<typename REAL>
void ColourConverter<REAL>::convertXYZtoLuv() {
	REAL L,u,v ;

	REAL &X = (*XYZ)[0] ;
	REAL &Y = (*XYZ)[1] ;
	REAL &Z = (*XYZ)[2] ;

	if (Y > 0.008856)
		L = 116.0f * pow(Y,1.0f/3.0) - 16 ;
	else // near black
		L = 903.3 * Y ;

	REAL den = X + 15.0 * Y + 3 * Z ;
	REAL u1 = (4.0 * X) / den ;
	REAL v1 = (9.0 * X) / den ;
	u = 13*L * (u1 - 0.197832) ;
	v = 13*L * (v1 - 0.468340) ;

	Luv = new VEC3(L,u,v) ;
}

template<typename REAL>
void ColourConverter<REAL>::convertLuvToXYZ() {
	REAL X,Y,Z ;

	REAL &L = (*Luv)[0] ;
	REAL &u = (*Luv)[1] ;
	REAL &v = (*Luv)[2] ;

	if (L > 8.0)
		Y = pow(((L+16.0) / 116.0),3) ;
	else // near black
		Y = L / 903.3 ;

	REAL den = 13.0 * L ;
	REAL u1 = u/den + 0.197832 ;
	REAL v1 = v/den + 0.468340 ;
	den = 4.0*v1 ;
	X = Y * 9.0*u1 / den ;
	Z = Y * (12-3*u1-20*v1) / den ;

	XYZ = new VEC3(X,Y,Z) ;
}

template<typename REAL>
bool ColourConverter<REAL>::convert(enum ColourEncoding from, enum ColourEncoding to) {
	switch(from) {
		case(C_RGB) :
			switch (to) {
				case (C_XYZ) :
					if (XYZ == NULL)
						convertRGBtoXYZ() ;
					break ;
				case (C_Luv) :
					if (XYZ == NULL)
						convertRGBtoXYZ() ;
					if (Luv == NULL)
						convertXYZtoLuv() ;
					break ;
				default :
					std::cerr << "Colour conversion not supported" << std::endl ;
					return false ;
			}
			break ;

		case(C_Luv) :
			switch(to) {
				case(C_RGB) : {
					if (RGB == NULL) {
						if (XYZ == NULL)
							convertLuvToXYZ() ;
						convertXYZtoRGB() ;
					}
					break ;
				}
				case(C_XYZ) : {
					if (XYZ == NULL)
						convertLuvToXYZ() ;
					break ;
				}
				default :
					std::cerr << "Colour conversion not supported" << std::endl ;
					return false ;
			}
			break ;

		case(C_XYZ) :
			switch (to) {
				case(C_RGB) :
					if (RGB==NULL)
						convertXYZtoRGB() ;
					break ;
				case(C_Luv) :
					if (Luv == NULL)
						convertXYZtoLuv() ;
					break ;
				default :
					std::cerr << "Colour conversion not supported" << std::endl ;
					return false ;
			}
			break ;
		default :
			std::cerr << "Colour conversion not supported" << std::endl ;
			return false ;
	}

	return true ;
}



} // namespace Utils
} // namespace CGoGN


