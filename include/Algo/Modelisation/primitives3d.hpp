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

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

//TEMPORAIRE
template <typename PFP>
Dart Primitive3D<PFP>::createOrientedHexa()
{
	Dart base = m_map.newFace(4);

	Dart side1 = m_map.newFace(4);
	m_map.sewFaces(base,side1);

	Dart side2 = m_map.newFace(4);
	m_map.sewFaces(m_map.phi1(base),side2);
	m_map.sewFaces(m_map.phi_1(side1), m_map.phi1(side2));

	Dart side3 = m_map.newFace(4);
	m_map.sewFaces(m_map.phi1(m_map.phi1(base)),side3);
	m_map.sewFaces(m_map.phi_1(side2), m_map.phi1(side3));

	Dart side4 = m_map.newFace(4);
	m_map.sewFaces(m_map.phi_1(base),side4);
	m_map.sewFaces(m_map.phi_1(side3), m_map.phi1(side4));

	m_map.sewFaces(m_map.phi_1(side4), m_map.phi1(side1));

	Dart top = m_map.newFace(4);
	m_map.sewFaces(top,m_map.phi1(m_map.phi1(side1)));
	m_map.sewFaces(m_map.phi_1(top),m_map.phi1(m_map.phi1(side2)));
	m_map.sewFaces(m_map.phi1(m_map.phi1(top)),m_map.phi1(m_map.phi1(side3)));
	m_map.sewFaces(m_map.phi1(top),m_map.phi1(m_map.phi1(side4)));

	return base;
}

template <typename PFP>
Dart Primitive3D<PFP>::HexaGrid1Topo(int nx)
{
	// first cube
	Dart d0 = createOrientedHexa();
	m_tableVertDarts.push_back(d0);
	Dart d1 = m_map.template phi<2112>(d0);

	for (int i=1; i< nx; ++i)
	{
		Dart d2 = createOrientedHexa();
		m_tableVertDarts.push_back(d2);
		m_map.sewVolumes(d1,d2);
		d1 = m_map.template phi<2112>(d2);
	}

	// add last vertex (x=nx)
	d1 = m_map.phi2(d1);
	m_tableVertDarts.push_back(d1);

	return d0;
}

template <typename PFP>
Dart Primitive3D<PFP>::HexaGrid2Topo( int nx, int ny)
{
	// creation premiere ligne
	Dart d0 = HexaGrid1Topo(nx);
	Dart d1 = m_map.template phi<112>(d0);

	for (int i=1; i< ny; ++i)
	{
		// creation ligne suivante
		Dart d2 = HexaGrid1Topo(nx);
		Dart d3 =  m_map.phi2(d2);

		// couture des deux lignes de cubes
		for (int i=0; i< nx; ++i)
		{
			m_map.sewVolumes(d1,d3);
			d1 = m_map.template phi<11232>(d1);
			d3 = m_map.template phi<11232>(d3);
		}
		// passage a lignes suivante
		d1 = m_map.template phi<112>(d2);
	}

	// add last row of vertices (y = ny)

	int index = m_tableVertDarts.size()-(nx+1); // pos of last inserted row of dart
	for (int i = 0; i< nx; ++i)
	{
		Dart dd = m_tableVertDarts[index++];
		dd =m_map.template phi<112>(dd);
		m_tableVertDarts.push_back(dd);	
	}
	// warning last vertex of row has not same dart
	Dart dd = m_tableVertDarts[index++];
	dd =m_map.template phi<211>(dd);
	m_tableVertDarts.push_back(dd);	


	return d0;
}


template <typename PFP>
Dart Primitive3D<PFP>::hexaGrid_topo( int nx, int ny, int nz)
{
	m_kind = HEXAGRID;
	m_nx = nx;
	m_ny = ny;
	m_nz = nz;
	m_tableVertDarts.clear();
	m_tableVertDarts.reserve((nx+1)*(ny+1)*(nz+1));
	

	Dart d0 = HexaGrid2Topo(nx,ny);
	Dart d1 = m_map.template phi<12>(d0);

	for (int i=1; i< nz; ++i)
	{
		// creation grille suivante
		Dart d2 = HexaGrid2Topo(nx,ny);
		Dart d3 = m_map.phi2(m_map.phi_1(d2));
		
		// couture des deux grilles 2D de cubes
		for (int j=0; j< ny; ++j)
		{
			Dart da = d1;
			Dart db = d3;
			for (int k=0; k< nx; ++k)
			{
				m_map.sewVolumes(da,db);
				da = m_map.template phi<11232>(da);
				db = m_map.template phi<11232>(db);
			}
			d1 = m_map.phi_1(d1);
			d1 = m_map.template phi<232>(d1);
			d1 = m_map.phi_1(d1);
			d3 = m_map.template phi<12321>(d3);
		}
		// passage a lignes suivante
		d1 = m_map.template phi<12>(d2);
	}

	// add last slice of vertices to the table
	int nb = (nx+1)*(ny+1);	// nb of vertices in one slice XY
	int index = nb*(nz-1);	// last slice
	for (int i=0; i < nb; ++i)
	{
		Dart dd = m_tableVertDarts[index++];
		dd =m_map.phi2(dd);
		m_tableVertDarts.push_back(dd);
	}

	return d0;
}


template <typename PFP>
void Primitive3D<PFP>::embedHexaGrid( float x, float y, float z)
{
	if (m_kind != HEXAGRID)
	{
		CGoGNerr << "Warning try to embedHexaGrid something that is not a grid of hexahedron"<<CGoGNendl;
		return;
	}

	float dx = x/float(m_nx);
	float dy = y/float(m_ny);
	float dz = z/float(m_nz);

	int nbs = (m_nx+1)*(m_ny+1);

	for(int i=0; i<=m_nz; ++i)
	{
		for(int j=0; j<=m_ny; ++j)
		{
			for(int k=0; k<=m_nx;++k)
			{
				typename PFP::VEC3 pos(-x/2.0f + dx*float(k), -y/2.0f + dy*float(j), -z/2.0f + dz*float(i));
				Dart d = m_tableVertDarts[ i*nbs+j*(m_nx+1)+k ];

				m_map.embedNewCell(VERTEX_ORBIT,d);
				m_positions[d] = pos;
			}
		}
	}
}


template <typename PFP>
void Primitive3D<PFP>::transform(const Geom::Matrix44f& matrice)
{
	for(typename std::vector<Dart>::iterator di=m_tableVertDarts.begin(); di!=m_tableVertDarts.end(); ++di)
	{
		typename PFP::VEC3& pos = m_positions[*di];
		pos = Geom::transform(pos, matrice);
	}

}

template <typename PFP>
void Primitive3D<PFP>::mark(Marker m)
{
	for(typename std::vector<Dart>::iterator di=m_tableVertDarts.begin(); di!=m_tableVertDarts.end(); ++di)
	{
		m_map.markOrbit(0,*di,m);
	}
}

}//end namespace
}//end namespace
}//end namespace
