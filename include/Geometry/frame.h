/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef LOCALFRAME_H_
#define LOCALFRAME_H_

#include <cmath>

namespace CGoGN {

namespace Geom {

/**
 * Util for rotation of a 3D point (or vector) around a given line (going through the origin) and of a given angle
 * @param axis the rotation axis direction
 * @param angle the rotation angle
 * @param p the point to rotate
 */
template <typename REAL>
Geom::Vector<3,REAL> rotate (Geom::Vector<3,REAL> axis, REAL angle, Geom::Vector<3,REAL> p) ;

/**
 * Util for conversion from spherical to cartesian coordinates.
 * The spherical coordinates are in radius-longitude-latitude
 * @param sph the spherical coordinates
 * @return the cartesian coordinates
 */
template<typename REAL>
Geom::Vector<3,REAL> sphericalToCart (const Geom::Vector<3,REAL>& sph) ;

/**
 * Util for conversion from cartesian to spherical coordinates.
 * The spherical coordinates are in radius-longitude-latitude
 * @param cart the cartesian coordinates
 * @return the spherical coordinates
 */
template<typename REAL>
Geom::Vector<3,REAL> cartToSpherical (const Geom::Vector<3,REAL>& cart) ;

/**
 * Class for representing a direct right-handed local frame composed of 3 orthonormal vectors T (tangent), B (bitangent) and N (normal).
 * This class can compress/decompress a local frame, switching from its explicit representation (3 vectors) to its compressed representation (1 vector composed of the Euler angles).
 * Usage :
 *  VEC3 X,Y,Z ;							// current set of orthonormal vectors composing the direct frame.
 *  Frame<PFP> lf(X,Y,Z) ; 				// Constructor from explicit expression.
 *  if (lf.isOrthoNormalDirect())			// test if the frame is Orthogonal, Normalized and Direct
 *   VEC3 compressed = lf.getCompressed() ;	// Extract compressed frame
 *  Frame<PFP> decompressed(compressed) ;	// Constructor from implicit (compressed) expression.
 */
template <typename PFP>
class Frame
{
	typedef typename PFP::REAL REAL ;
	typedef typename Geom::Vector<2,REAL> VEC2 ;
	typedef typename Geom::Vector<3,REAL> VEC3 ;

private: // fields
	/**
	 * The three explicit vectors
	 */
	VEC3 m_X,m_Y,m_Z ;

public: // methods
	/**
	 * Constructor from explicit data
	 * @param X the tangent vector
	 * @param Y the bitangent vector
	 * @param Z the normal vector
	 */
	Frame(const VEC3& X, const VEC3& Y, const VEC3& Z) ;

	/**
	 * Constructor from implicit (compressed representation)
	 * @param compressedFrame an implicit (compressed) version of the local frame (can be produced by localFrame.getCompressed())
	 */
	Frame(const VEC3& eulerAngles) ;

	~Frame() {} ;

	/**
	 * Returns a compressed version of the current local frame
	 * A VEC3 is not sufficient to completely define a local frame (if phiN=0, the decompression is not unique).
	 */
	VEC3 getCompressed() const ;

	/**
	 * Tests if the frames are identical
	 * @param lf the frame to compare to the current frame
	 * @param epsilon the authorized deviation
	 * @return true if frames are identical (or deviate less than epsilon)
	 */
	bool equals(const Frame<PFP>& lf, REAL epsilon = 1e-6) const ;

	/**
	 * Equality of frames
	 * Identical to calling equals with default epsilon
	 * @return true if frames are identical
	 */
	bool operator==(const Frame<PFP>& lf) const ;

	/**
	 * Inequality of frames
	 * Identical to calling !equals with default epsilon
	 * @return false if frames are identical
	 */
	bool operator!=(const Frame<PFP>& lf) const ;

	/**
	 * Tests if the frame is direct
	 * @return true if the frame is direct
	 */
	bool isDirect(REAL epsilon = 1e-7) const ;

	/**
	 * Tests if the frame is orthogonal
	 * @return true if the frame is orthogonal
	 */
	bool isOrthogonal(REAL epsilon = 1e-5) const ;

	/**
	 * Tests if the frame is normalized
	 * @return true if the frame is normalized
	 */
	bool isNormalized(REAL epsilon = 1e-5) const ;

	/**
	 * Tests if the frame is direct, normalized and orthogonal
	 * @return true if the frame is direct, normalized and orthogonal
	 */
	bool isOrthoNormalDirect(REAL epsilon = 1e-5) const ;

	/**
	 * @return current tangent vector
	 */
	VEC3& getX() { return m_X ; }
	const VEC3& getX() const { return m_X ; }

	/**
	 * @return current bitangent vector
	 */
	VEC3& getY() { return m_Y ; }
	const VEC3& getY() const { return m_Y ; }

	/**
	 * @return current normal vector
	 */
	VEC3& getZ() { return m_Z ; }
	const VEC3& getZ() const { return m_Z ; }

	friend std::ostream& operator<< (std::ostream &out, const Frame& lf) {
		out << "X : " << lf.m_X << std::endl ;
		out << "Y : " << lf.m_Y << std::endl ;
		out << "Z : " << lf.m_Z ;
		return out ;
	} ;

private : // private constants
	// The reference frame (X,Y,Z) can be any orthonormal
	// direct frame. Zeros are not recommended since they
	// can easily generate zero vectors after a dot product
	// UPDATE : this problem should be handled in compression
//	static const REAL Xx = 0.267261 ;
//	static const REAL Xy = 0.534522 ;
//	static const REAL Xz = 0.801784 ;
//	static const REAL Yx = 0.844416 ;
//	static const REAL Yy = -0.530776 ;
//	static const REAL Yz = 0.0723785 ;
//	static const REAL Zx = 0.464255 ;
//	static const REAL Zy = 0.657695 ;
//	static const REAL Zz = -0.593215 ;
	static const REAL Xx = 0.0766965 ;
	static const REAL Xy = 0.383483 ;
	static const REAL Xz = 0.920358 ;
	static const REAL Yx = -0.760734 ;
	static const REAL Yy = 0.619202 ;
	static const REAL Yz = -0.194606 ;
	static const REAL Zx = -0.644516 ;
	static const REAL Zy = -0.685222 ;
	static const REAL Zz = 0.339219 ;

} ;

} // Geom

} // CGoGN

#include "frame.hpp"

#endif /* LOCALFRAME_H_ */
