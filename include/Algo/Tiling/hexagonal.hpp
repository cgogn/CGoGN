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

namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Tilings
{

template <typename PFP>
void Square<PFP>::grid(unsigned int x, unsigned int y, bool closed)
{
    // nb vertices
    int nb = (x+1)*(y+1);

    // vertice reservation
    m_tableVertDarts.reserve(nb);

    // creation of quads and storing vertices
    for (unsigned int i = 0; i < y; ++i)
    {
        for (unsigned int j = 1; j <= x; ++j)
        {
            Dart d = m_map.newFace(4, false);
            m_tableVertDarts.push_back(d);
            if (j == x)
                m_tableVertDarts.push_back(m_map.phi1(d));
        }
    }

    // store last row of vertices
    for (unsigned int i = 0; i < x; ++i)
    {
        m_tableVertDarts.push_back( m_map.phi_1(m_tableVertDarts[(y-1)*(x+1) + i]) );
    }
    m_tableVertDarts.push_back( m_map.phi1(m_tableVertDarts[(y-1)*(x+1) +x]) );

    //sewing the quads
    for (unsigned int i = 0; i < y; ++i)
    {
        for (unsigned int j = 0; j < x; ++j)
        {
            if (i > 0) // sew with preceeding row
            {
                int pos = i*(x+1)+j;
                Dart d = m_tableVertDarts[pos];
                Dart e = m_tableVertDarts[pos-(x+1)];
                e = m_map.phi1(m_map.phi1(e));
                m_map.sewFaces(d, e, false);
            }
            if (j > 0) // sew with preceeding column
            {
                int pos = i*(x+1)+j;
                Dart d = m_tableVertDarts[pos];
                d = m_map.phi_1(d);
                Dart e = m_tableVertDarts[pos-1];
                e = m_map.phi1(e);
                m_map.sewFaces(d, e, false);
            }
        }
    }

    if(closed)
        m_map.closeHole(m_tableVertDarts[0]) ;
}

template <typename PFP>
void Square<PFP>::cylinder(unsigned int n, unsigned int z, bool top_closed, bool bottom_closed)
{
    int nb = (n)*(z+1)+2;

    // vertice reservation
    m_tableVertDarts.reserve(nb);

    // creation of quads and storing vertices
    for (unsigned int i = 0; i < z; ++i)
    {
        for (unsigned int j = 0; j < n; ++j)
        {
            Dart d = m_map.newFace(4, false);
            m_tableVertDarts.push_back(d);
        }
    }
    for (unsigned int i = 0; i < n; ++i)
    {
        m_tableVertDarts.push_back( m_map.phi_1(m_tableVertDarts[(z-1)*n+i]) );
    }

    //sewing the quads
    for (unsigned int i = 0; i < z; ++i)
    {
        for (unsigned int j = 0; j < n; ++j)
        {
            if (i > 0) // sew with preceeding row
            {
                int pos = i*n+j;
                Dart d = m_tableVertDarts[pos];
                Dart e = m_tableVertDarts[pos-n];
                e = m_map.phi1(m_map.phi1(e));
                m_map.sewFaces(d, e, false);
            }
            if (j > 0) // sew with preceeding column
            {
                int pos = i*n+j;
                Dart d = m_tableVertDarts[pos];
                d = m_map.phi_1(d);
                Dart e = m_tableVertDarts[pos-1];
                e = m_map.phi1(e);
                m_map.sewFaces(d, e, false);
            }
            else
            {
                int pos = i*n;
                Dart d = m_tableVertDarts[pos];
                d = m_map.phi_1(d);
                Dart e = m_tableVertDarts[pos+(n-1)];
                e = m_map.phi1(e);
                m_map.sewFaces(d, e, false);
            }
        }
    }

    if (bottom_closed)
    {
        Dart d = m_tableVertDarts[0];
        if(m_map.closeHole(d, false))
        {
            d = m_map.phi2(d);
            if(m_map.faceDegree(d) > 3)
            {
                Modelisation::trianguleFace<PFP>(m_map, d);
                m_tableVertDarts.push_back(m_map.phi_1(d));
            }
        }
    }
    else
        m_map.closeHole(m_tableVertDarts[0]);

    if (top_closed)
    {
        Dart d =  m_map.phi_1(m_tableVertDarts[n*z]);
        if(m_map.closeHole(d, false))
        {
            d = m_map.phi2(d);
            if(m_map.faceDegree(d) > 3)
            {
                Modelisation::trianguleFace<PFP>(m_map, d);
                m_tableVertDarts.push_back(m_map.phi_1(d));
            }
        }
    }
    else
        m_map.closeHole(m_map.phi_1(m_tableVertDarts[n*z]));
}

template <typename PFP>
void Square<PFP>::tore(unsigned int n, unsigned int m)
{
    cylinder_topo(n, m, false, false);

    // just finish to sew
    for(unsigned int i = 0; i < n; ++i)
    {
        Dart d = m_tableVertDarts[i];
        Dart e = m_tableVertDarts[(m*n)+i];
        e = m_map.phi_1(e);
        m_map.sewFaces(d, e);
    }

    // remove the last n vertex darts that are no more necessary (sewed with n first)
    // memory not freed (but will be when destroy the Polyhedron), not important ??
    m_tableVertDarts.resize(m*n);
}

template <typename PFP>
void Square<PFP>::embedGrid(VertexAttribute<VEC3>& position, float x, float y, float z)
{
    float dx = x/float(m_nx);
    float dy = y/float(m_ny);
    float dz = z/float(m_nz);

    unsigned int nbs = (m_nx+1)*(m_ny+1);

    for(unsigned int i = 0; i <= m_nz; ++i)
    {
        for(unsigned int j = 0; j <= m_ny; ++j)
        {
            for(unsigned int k = 0; k <= m_nx; ++k)
            {
                typename PFP::VEC3 pos(-x/2.0f + dx*float(k), -y/2.0f + dy*float(j), -z/2.0f + dz*float(i));
                Dart d = m_tableVertDarts[ i*nbs+j*(m_nx+1)+k ];

                m_map.template setOrbitEmbeddingOnNewCell<VERTEX>(d);
                m_positions[d] = pos;
            }
        }
    }
}


//template <typename PFP>
//void triangular(typename PFP::MAP& map, unsigned int rows, unsigned int columns)
//{
//    unsigned int nbt = rows * columns;
//    std::vector<Dart> m_tableVertDarts;
//    m_tableVertDarts.reserve(nbt);

//    // creation of triangles around circunference and storing vertices
//    for (unsigned int i = 0; i <= nbt; ++i)
//    {
//        Dart d = map.newFace(3, false);
//        m_tableVertDarts.push_back(d);
//    }

//    for(unsigned int y = 0 ; y < rows-1 ; ++y)
//    {
//        for(unsigned int x = 0 ; x < columns-1 ; ++x)
//        {
//            Dart d = m_tableVertDarts[i];
//            Dart e = m_tableVertDarts[i+1];

//        }
//    }

//}

//triangular -> grid -> open or closed (closeHole)
//triangular -> cylinder -> grid + finish sewing -> open or closed (closeHole) -> triangule face
//triangular -> tore -> cylinder + finish sewing


} // namespace Tilings

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
