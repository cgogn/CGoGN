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

#ifndef __CONVERTTYPE_H_
#define __CONVERTTYPE_H_

#include <assert.h>
#include <vector>

namespace CGoGN {

namespace Utils {

/**
 * Create a ref to a type from a ref from another type
 * No copy only casting. No need to used IN template parameter
 * @param vec input ref
 * @ return a ref on same object with OUT type
 */
template <typename OUT, typename IN>
inline OUT& convertRef(IN& vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return *(reinterpret_cast<OUT*>(&vec));
}

/**
 * Create a const ref to a type from a const ref from another type
 * No copy only casting. No need to used IN template parameter
 * @param vec input ref
 * @ return a ref on same object with OUT type
 */
template <typename OUT, typename IN>
inline const OUT& convertRef(const IN& vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return *(reinterpret_cast<const OUT*>(&vec));
}

/**
 * Create a ptr of a type from a ptr of another type
 * Just a reinterpret cast in fact
 * @param vec input ptr
 * @return a ptr on same object with OUT type
 */
template <typename OUT, typename IN>
inline OUT* convertPtr(IN* vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return reinterpret_cast<OUT*>(vec);
}

/**
 * Create a const ptr of a type from a const ptr of another type
 * Just a reinterpret cast in fact
 * @param vec input ptr
 * @return a ptr on same object with OUT type
 */
template <typename OUT, typename IN>
inline const OUT* convertPtr(const IN* vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return reinterpret_cast<const OUT*>(vec);
}




template <typename OUT, typename IN>
inline std::vector<OUT>& convertVector(std::vector<IN>& vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return *(reinterpret_cast< std::vector<OUT>* >(&vec));
}


template <typename OUT, typename IN>
inline const std::vector<OUT>& convertVector(const std::vector<IN>& vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return *(reinterpret_cast< const std::vector<OUT>* >(&vec));
}





template <typename OUT, typename IN>
inline const std::list<OUT>& convertList(const std::list<IN>& vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return *(reinterpret_cast< const std::list<OUT>* >(&vec));
}


template <typename OUT, typename IN>
inline std::list<OUT>& convertList(std::list<IN>& vec)
{
	assert(sizeof(IN) == sizeof(OUT) || "incompatible size cast");
	return *(reinterpret_cast< std::list<OUT>* >(&vec));
}


} // namespace Utils

} // namespace CGoGN



#endif /* CONVERTTYPE_H_ */
