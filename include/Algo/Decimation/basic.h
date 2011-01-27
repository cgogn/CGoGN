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

#ifndef __DECIMATION_BASIC_H_
#define __DECIMATION_BASIC_H_

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

template <typename PFP>
bool edgeCanCollapse(typename PFP::MAP& map, Dart d, AttributeHandler<unsigned int>& valences)
{
	Dart dd = map.phi2(d);
	Dart dp = map.phi_1(d);
	Dart ddp = map.phi_1(dd);

	// Check valency conditions

	unsigned int val_vd = valences[d];
	unsigned int val_vdd = valences[dd];
	unsigned int val_vd1 = valences[dp];
	unsigned int val_vdd1 = valences[ddp];

	if(val_vd + val_vdd < 8 || val_vd + val_vdd > 11 || val_vd1  < 5 || val_vdd1 < 5)
		return false;

	// Check vertex sharing condition

	unsigned int vu1[32]; // pas de vector mais un tableau (find a la main, mais pas d'allocation par reserve)
	val_vd -= 3; // evite le -3 dans la boucle
	val_vdd -= 3;
	Dart vit1 = map.alpha1(map.alpha1(d));

	for (unsigned int i = 0; i< val_vd; ++i)
	{
		unsigned int ve = map.getEmbedding(map.phi2(vit1),VERTEX_ORBIT);
		vu1[i] = ve;
		vit1 = map.alpha1(vit1);
	}

	val_vd--; // pour le parcours avec while >=0

	Dart vit2 = map.alpha1(map.alpha1(dd));
	for (unsigned int i = 0; i< val_vdd; ++i)
	{
		unsigned int ve = map.getEmbedding(map.phi2(vit2),VERTEX_ORBIT);

		int j = val_vd;
		while ( j>=0)
		{
			if (vu1[j] == ve)
				return false;
			--j;
		}
		vit2 = map.alpha1(vit2);
	}

	return true;
}

template <typename PFP>
bool edgeCanCollapse(typename PFP::MAP& map, Dart d)
{
	Dart dd = map.phi2(d);
	Dart dp = map.phi_1(d);
	Dart ddp = map.phi_1(dd);

	// Check valency conditions

	int val_vd = 0 ;
	Dart tmp = d ;
	do { ++val_vd ;	tmp = map.alpha1(tmp) ; } while(tmp != d) ;
	int val_vdd = 0 ;
	tmp = dd ;
	do { ++val_vdd ; tmp = map.alpha1(tmp) ; } while(tmp != dd) ;
	int val_vdp = 0 ;
	tmp = dp ;
	do { ++val_vdp ; tmp = map.alpha1(tmp) ; } while(tmp != dp) ;
	int val_vddp = 0 ;
	tmp = ddp ;
	do { ++val_vddp ; tmp = map.alpha1(tmp) ; } while(tmp != ddp) ;

	if(val_vd + val_vdd < 8 || val_vd + val_vdd > 11 || val_vdp < 5 || val_vddp < 5)
		return false;

	// Check vertex sharing condition

	unsigned int vu1[32]; // pas de vector mais un tableau (find a la main, mais pas d'allocation par reserve)
	val_vd -= 3; // evite le -3 dans la boucle
	val_vdd -= 3;
	Dart vit1 = map.alpha1(map.alpha1(d));

	for (int i = 0; i < val_vd; ++i)
	{
		unsigned int ve = map.getEmbedding(map.phi2(vit1),VERTEX_ORBIT);
		vu1[i] = ve;
		vit1 = map.alpha1(vit1);
	}

	val_vd--; // pour le parcours avec while >=0

	Dart vit2 = map.alpha1(map.alpha1(dd));
	for (int i = 0; i < val_vdd; ++i)
	{
		unsigned int ve = map.getEmbedding(map.phi2(vit2),VERTEX_ORBIT);

		int j = val_vd;
		while (j >= 0)
		{
			if (vu1[j] == ve)
				return false;
			--j;
		}
		vit2 = map.alpha1(vit2);
	}

	return true;
}

} // namespace Decimation

} // namespace Algo

} // namespace CGoGN

#endif
