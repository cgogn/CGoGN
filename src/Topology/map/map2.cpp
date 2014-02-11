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

#include "Topology/map/map2.h"
#include "Topology/generic/traversorCell.h"
#include "Topology/generic/dartmarker.h"

namespace CGoGN
{

//void Map2::rdfi(Dart t, DartMarker& m1, DartMarker& m2)
//{
//	Dart p = NIL;
//	while (!(p == NIL && (t == NIL || (m1.isMarked(t) || m2.isMarked(t)) ) ) )
//	{
//		if (t == NIL || (m1.isMarked(t) || m2.isMarked(t)))
//		{
//			if (m2.isMarked(p))			// pop
//			{
//				Dart q = phi2(p);		//	q = p->s1;
//				unsigned int pi=dartIndex(p);
//				(*m_phi2)[pi]=t;		//	p->s1 = t;
//				t = p;
//				p = q;
//	     	}
//			else						// swing
//	     	{
//				m2.mark(p);				//	p->val = 2;
//				Dart q = phi1(p);		//	q = p->s0;
//				unsigned int pi=dartIndex(p);
//				(*m_phi1)[pi]=t;		//	p->s0 = t;
//				t = phi2(p);			//	t = p->s1;
//				(*m_phi2)[pi]=q;		//	p->s1 = q;
//			}
//		}
//		else							 // push
//		{
//			m1.mark(t);					//	t->val = 1;
//			Dart q = phi1(t);			//	q = t->s0;
//			unsigned int ti=dartIndex(t);
//			(*m_phi1)[ti]=p;			//	t->s0 = p;
//			p = t;
//			t = q;
//		}
//	}
//}

} // namespace CGoGN
