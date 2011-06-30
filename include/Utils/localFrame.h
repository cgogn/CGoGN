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

namespace Utils {

/**
 * Util for rotation of a 3D point (or vector) around a given line (going through the origin) and of a given angle
 * @param axis the rotation axis direction
 * @param angle the rotation angle
 * @param p the point to rotate
 */
template <typename REAL>
Geom::Vector<3,REAL> rotate (Geom::Vector<3,REAL> axis, REAL angle, Geom::Vector<3,REAL> p) ;

/**
 * Util for conversion from spherical to carthesian coordinates.
 * The spherical coordinates are in radius-longitude-latitude
 * @param sph the spherical coordinates
 * @return the carthesian coordinates
 */
template<typename REAL>
Geom::Vector<3,REAL> sphericalToCarth (const Geom::Vector<3,REAL>& sph) ;

/**
 * Util for conversion from carthesian to spherical coordinates.
 * The spherical coordinates are in radius-longitude-latitude
 * @param carth the carthesian coordinates
 * @return the spherical coordinates
 */
template<typename REAL>
Geom::Vector<3,REAL> carthToSpherical (const Geom::Vector<3,REAL>& carth) ;

/**
 * Class for representing a direct right-handed local frame composed of 3 orthonormal vectors T (tangent), B (bitangent) and N (normal).
 * This class can compress/decompress a local frame, switching from its explicit representation (3 vectors) to its compressed representation (1 vector composed of the Euler angles).
 * Usage :
 *  VEC3 T,B,N ;							// current set of orthonormal vectors composing the direct frame.
 *  LocalFrame<PFP> lf(T,B,N) ; 				// Constructor from explicit expression.
 *  if (lf.isOrthoNormalDirect())			// test if the frame is Orthogonal, Normalized and Direct
 *   VEC3 compressed = lf.getCompressed() ;	// Extract compressed frame
 *  LocalFrame<PFP> decompressed(compressed) ;	// Constructor from implicit (compressed) expression.
 *
 * All formulae were provided by "Représentation compacte d'un repère local", june 14th, 2011 by K. Vanhoey
 */
template <typename PFP>
class LocalFrame
{
	typedef typename PFP::REAL REAL ;
	typedef typename Geom::Vector<2,REAL> VEC2 ;
	typedef typename Geom::Vector<3,REAL> VEC3 ;

private: // fields
	/**
	 * The three explicit vectors
	 */
	VEC3 m_T,m_B,m_N ;

public: // methods
	/**
	 * Constructor from explicit data
	 * @param T the tangent vector
	 * @param B the bitangent vector
	 * @param N the normal vector
	 */
	LocalFrame(const VEC3& T, const VEC3& B, const VEC3& N) ;

	/**
	 * Constructor from implicit (compressed representation)
	 * @param compressedFrame an implicit (compressed) version of the local frame (can be produced by localFrame.getCompressed())
	 */
	LocalFrame(const VEC3& compressedFrame) ;

	~LocalFrame() {} ;

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
	bool equals(const LocalFrame<PFP>& lf, REAL epsilon = 1e-6) const ;

	/**
	 * Equality of frames
	 * Identical to calling equals with default epsilon
	 * @return true if frames are identical
	 */
	bool operator==(const LocalFrame<PFP>& lf) const ;

	/**
	 * Inequality of frames
	 * Identical to calling !equals with default epsilon
	 * @return false if frames are identical
	 */
	bool operator!=(const LocalFrame<PFP>& lf) const ;

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
	VEC3& getT() { return m_T ; }
	const VEC3& getT() const { return m_T ; }

	/**
	 * @return current bitangent vector
	 */
	VEC3& getB() { return m_B ; }
	const VEC3& getB() const { return m_B ; }

	/**
	 * @return current normal vector
	 */
	VEC3& getN() { return m_N ; }
	const VEC3& getN() const { return m_N ; }

	friend std::ostream& operator<< (std::ostream &out, const LocalFrame& lf) {
		out << "T : " << lf.m_T << std::endl ;
		out << "B : " << lf.m_B << std::endl ;
		out << "N : " << lf.m_N ;
		return out ;
	} ;

private : // private constants
	// (T,B,N) can be any orthonormal direct frame
	// zeros are not recommended since they can
	// generate zero vectors after a dot product
//	static const REAL Tx = 0.267261 ;
//	static const REAL Ty = 0.534522 ;
//	static const REAL Tz = 0.801784 ;
//	static const REAL Bx = 0.844416 ;
//	static const REAL By = -0.530776 ;
//	static const REAL Bz = 0.0723785 ;
//	static const REAL Nx = 0.464255 ;
//	static const REAL Ny = 0.657695 ;
//	static const REAL Nz = -0.593215 ;
	static const REAL Tx = 0.0766965 ;
	static const REAL Ty = 0.383483 ;
	static const REAL Tz = 0.920358 ;
	static const REAL Bx = -0.760734 ;
	static const REAL By = 0.619202 ;
	static const REAL Bz = -0.194606 ;
	static const REAL Nx = -0.644516 ;
	static const REAL Ny = -0.685222 ;
	static const REAL Nz = 0.339219 ;

} ;

} // Utils

} // CGoGN

#include "localFrame.hpp"

#endif /* LOCALFRAME_H_ */
