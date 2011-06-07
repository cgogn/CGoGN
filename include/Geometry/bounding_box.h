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

#ifndef __BOUNDING_BOX__
#define __BOUNDING_BOX__

namespace CGoGN
{

namespace Geom
{

/*
 * Class for the computation of bounding boxes
 */
template <typename VEC>
class BoundingBox
{
	public:
		/**********************************************/
		/*                CONSTRUCTORS                */
		/**********************************************/

		// must initialize the bounding box with one first point
		BoundingBox(const VEC& p);

		/**********************************************/
		/*                 ACCESSORS                  */
		/**********************************************/

		VEC& min();

		const VEC& min() const;

		VEC& max();

		const VEC& max() const;

		typename VEC::DATA_TYPE size(unsigned int coord) const;

		VEC center() const;

		/**********************************************/
		/*                 FUNCTIONS                  */
		/**********************************************/

		// add a point to the bounding box
		void addPoint(const VEC& p);

		// return true if bb intersects the bounding box
		bool intersects(const BoundingBox<VEC>& bb);

		// fusion with the given bounding box
		void fusion(const BoundingBox<VEC>& bb);

		/**********************************************/
		/*             STREAM OPERATORS               */
		/**********************************************/

//		friend std::ostream& operator<<(std::ostream& out, const BoundingBox<VEC>& bb);
//
//		friend std::istream& operator>>(std::istream& in, BoundingBox<VEC>& bb);


		friend std::ostream& operator<<(std::ostream& out, const BoundingBox<VEC>& bb)
		{
			out << bb.min() << " " << bb.max() ;
			return out ;
		}

		friend std::istream& operator>>(std::istream& in, BoundingBox<VEC>& bb)
		{
			in >> bb.min() >> bb.max() ;
			return in ;
		}

	private:
		VEC m_pMin, m_pMax ;
} ;

}
}

#include "bounding_box.hpp"
#endif
