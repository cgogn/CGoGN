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

namespace Triangular
{

/*! Grid
 *************************************************************************/

template <typename PFP>
void Grid<PFP>::grid(unsigned int x, unsigned int y, bool close)
{
    // nb vertices
	int nb = (x+1)*(y+1);

	// vertices reservation
    this->m_tableVertDarts.reserve(nb);

    // creation of triangles and storing vertices
    for (unsigned int i = 0; i < y; ++i)
    {
        for (unsigned int j = 1; j <= x; ++j)
        {
            Dart d = this->m_map.newFace(3, false);
			Dart d2 = this->m_map.newFace(3, false);
			this->m_map.sewFaces(this->m_map.phi1(d), this->m_map.phi_1(d2), false);
			this->m_tableVertDarts.push_back(d);

            if (j == x)
				this->m_tableVertDarts.push_back(d2);
        }
    }

    // store last row of vertices
    for (unsigned int i = 0; i < x; ++i)
    {
		this->m_tableVertDarts.push_back(this->m_map.phi_1(this->m_tableVertDarts[(y-1)*(x+1) + i]));
	}
	this-> m_tableVertDarts.push_back(this->m_map.phi1(this->m_tableVertDarts[(y-1)*(x+1) +x]));


	//sewing pairs of triangles
    for (unsigned int i = 0; i < y; ++i)
    {
        for (unsigned int j = 0; j < x; ++j)
        {

			if (i > 0) // sew with preceeding row
			{
				int pos = i*(x+1)+j;
				Dart d = this->m_tableVertDarts[pos];
				Dart e = this->m_tableVertDarts[pos-(x+1)];
				e = this->m_map.phi_1(this->m_map.phi2(this->m_map.phi1(e)));
				this->m_map.sewFaces(d, e, false);
			}
			if (j > 0) // sew with preceeding column
			{
				int pos = i*(x+1)+j;
				Dart d = this->m_tableVertDarts[pos];
				d = this->m_map.phi_1(d);
				Dart e = this->m_tableVertDarts[pos-1];
				e = this->m_map.phi1(this->m_map.phi2(this->m_map.phi1(e)));
				this->m_map.sewFaces(d, e, false);
			}
        }
    }

    if(close)
        this->m_map.closeHole(this->m_tableVertDarts[0]) ;

	this->m_dart = this->m_tableVertDarts[0];
}

template <typename PFP>
void Grid<PFP>::embedIntoGrid(VertexAttribute<VEC3>& position, float x, float y, float z)
{
	float dx = x / float(this->m_nx);
	float dy = y / float(this->m_ny);

	for(unsigned int i = 0; i <= this->m_ny; ++i)
    {
        for(unsigned int j = 0; j <= this->m_nx;++j)
		{
			position[this->m_tableVertDarts[i*(this->m_nx+1)+j]] = VEC3(dx*float(j) + dx*0.5f*float(i), dy*float(i) * sqrtf(3.0f)/2.0f, z);
        }
    }
}

template <typename PFP>
void Grid<PFP>::embedIntoTwistedStrip(VertexAttribute<VEC3>& position, float radius_min, float radius_max, float turns)
{
	float alpha = float(2.0*M_PI/this->m_ny);
	float beta = turns/float(this->m_ny);

	float radius = (radius_max + radius_min)/2.0f;
	float rdiff = (radius_max - radius_min)/2.0f;

	for(unsigned int i = 0; i <= this->m_ny; ++i)
	{
		for(unsigned int j = 0; j <= this->m_nx; ++j)
		{
			float rw = -rdiff + float(j)*2.0f*rdiff/float(this->m_nx);
			float r = radius + rw*cos(beta*float(i));
			VEC3 pos(r*cos(alpha*float(i)), r*sin(alpha*float(i)), rw*sin(beta*float(i)));
			position[this->m_tableVertDarts[ i*(this->m_nx+1)+j ] ] = pos;
		}
	}

}

template <typename PFP>
void Grid<PFP>::embedIntoHelicoid(VertexAttribute<VEC3>& position, float radius_min,  float radius_max, float maxHeight, float nbTurn, int orient)
{
	float alpha = float(2.0*M_PI/this->m_nx)*nbTurn;
	float hS = maxHeight/this->m_nx;

	// 	float radius = (radius_max + radius_min)/2.0f;
	// 	float rdiff = (radius_max - radius_min)/2.0f;

	for(unsigned int i = 0; i <= this->m_ny; ++i)
	{
		for(unsigned int j = 0; j <= this->m_nx; ++j)
		{
			// 			float r = radius_max + radius_min*cos(beta*float(j));
			float r,x,y;
			// 			if(i==1) {
			// 				r = radius_max;
			// 			}
			// 			else {
			r = radius_min+(radius_max-radius_min)*float(i)/float(this->m_ny);
			// 			}
			x = orient*r*sin(alpha*float(j));
			y = orient*r*cos(alpha*float(j));

			VEC3 pos(x, y, j*hS);
			Dart d = this->m_tableVertDarts[i*(this->m_nx+1)+j];
			position[d] = pos;
		}
	}
}

/*! Cylinder
 *************************************************************************/

template <typename PFP>
void Cylinder<PFP>::cylinder(unsigned int n, unsigned int z)
{
	// nb vertices
	int nb = (n)*(z+1)+2;

	// vertices reservation
	this->m_tableVertDarts.reserve(nb);

	// creation of triangles and storing vertices
	for (unsigned int i = 0; i < z; ++i)
	{
		for (unsigned int j = 0; j < n; ++j)
		{
			Dart d = this->m_map.newFace(3, false);
			Dart d2 = this->m_map.newFace(3, false);
			this->m_map.sewFaces(this->m_map.phi1(d), this->m_map.phi_1(d2), false);
			this->m_tableVertDarts.push_back(d);
		}
	}

	// store last row of vertices
	for (unsigned int i = 0; i < n; ++i)
	{
		this->m_tableVertDarts.push_back(this->m_map.phi_1(this->m_tableVertDarts[(z-1)*n+i]));
	}

	//sewing pairs of triangles
	for (unsigned int i = 0; i < z; ++i)
	{
		for (unsigned int j = 0; j < n; ++j)
		{

			if (i > 0) // sew with preceeding row
			{
				int pos = i*n+j;
				Dart d = this->m_tableVertDarts[pos];
				Dart e = this->m_tableVertDarts[pos-n];
				e = this->m_map.phi_1(this->m_map.phi2(this->m_map.phi1(e)));
				this->m_map.sewFaces(d, e, false);
			}
			if (j > 0) // sew with preceeding column
			{
				int pos = i*n+j;
				Dart d = this->m_tableVertDarts[pos];
				d = this->m_map.phi_1(d);
				Dart e = this->m_tableVertDarts[pos-1];
				e = this->m_map.phi1(this->m_map.phi2(this->m_map.phi1(e)));
				this->m_map.sewFaces(d, e, false);
			}
			else
			{
				int pos = i*n;
				Dart d = this->m_tableVertDarts[pos];
				d = this->m_map.phi_1(d);
				Dart e = this->m_tableVertDarts[pos+(n-1)];
				e = this->m_map.phi1(this->m_map.phi2(this->m_map.phi1(e)));
				this->m_map.sewFaces(d, e, false);
			}
		}
	}

	if(m_top_closed)
		closeTop();

	if(m_bottom_closed)
		closeBottom();

	this->m_dart = this->m_tableVertDarts.front();
}

template <typename PFP>
void Cylinder<PFP>::closeTop()
{
	this->m_map.closeHole(this->m_map.phi1(this->m_map.phi2(this->m_tableVertDarts[this->m_nx*this->m_nz])));
	m_top_closed = true;
}

template <typename PFP>
void Cylinder<PFP>::triangleTop()
{
	if (m_top_closed)
	{
		Dart d =  this->m_map.phi1(this->m_map.phi2(this->m_tableVertDarts[this->m_nx*this->m_nz]));
		this->m_map.fillHole(d);

		d = this->m_map.phi2(d);
		if(this->m_map.faceDegree(d) > 3)
		{
			Algo::Surface::Modelisation::trianguleFace<PFP>(this->m_map, d);
			//this->m_tableVertDarts.push_back(this->m_map.phi_1(d));
			m_topVertDart = this->m_map.phi_1(d);
		}

		m_top_triangulated = true;
	}
}

template <typename PFP>
void Cylinder<PFP>::closeBottom()
{
	this->m_map.closeHole(this->m_tableVertDarts[0]);
	m_bottom_closed = true;
}

template <typename PFP>
void Cylinder<PFP>::triangleBottom()
{
	if (m_bottom_closed)
	{
		Dart d = this->m_tableVertDarts[0];
		this->m_map.fillHole(d);

		d = this->m_map.phi2(d);
		if(this->m_map.faceDegree(d) > 3)
		{
			Algo::Surface::Modelisation::trianguleFace<PFP>(this->m_map, d);
			//this->m_tableVertDarts.push_back(this->m_map.phi_1(d));
			m_bottomVertDart = this->m_map.phi_1(d);
		}

		m_bottom_triangulated = true;
	}
}

template <typename PFP>
void Cylinder<PFP>::embedIntoCylinder(VertexAttribute<VEC3>& position, float bottom_radius, float top_radius, float height)
{
	float alpha = float(2.0*M_PI/this->m_nx);
	float dz = height/float(this->m_nz);

	for(unsigned int i = 0; i <= this->m_nz; ++i)
	{
		float a = float(i)/float(this->m_nz);
		float radius = a*top_radius + (1.0f-a)*bottom_radius;
		for(unsigned int j = 0; j < this->m_nx; ++j)
		{

			float x = radius*cos(alpha*float(j));
			float y = radius*sin(alpha*float(j));
			position[this->m_tableVertDarts[i*(this->m_nx)+j] ] = VEC3(x, y, -height/2 + dz*float(i));
		}
	}

	//int indexUmbrella = this->m_nx*(this->m_nz+1);

	if (m_bottom_triangulated)
	{
		//position[this->m_tableVertDarts[indexUmbrella++] ] = VEC3(0.0f, 0.0f, height/2 );
		position[m_bottomVertDart] = VEC3(0.0f, 0.0f, -height/2 );
	}

	if (m_top_triangulated)
	{
		//position[this->m_tableVertDarts[indexUmbrella] ] = VEC3(0.0f, 0.0f, -height/2 );
		position[m_topVertDart] = VEC3(0.0f, 0.0f, height/2 );
	}
}

template <typename PFP>
void Cylinder<PFP>::embedIntoSphere(VertexAttribute<VEC3>& position, float radius)
{
	float alpha = float(2.0*M_PI/this->m_nx);
	float beta = float(M_PI/(this->m_nz+2));

	for(unsigned int i = 0; i <= this->m_nz; ++i)
	{
		for(unsigned int j = 0; j < this->m_nx; ++j)
		{
			float h = float(radius * sin(-M_PI/2.0+(i+1)*beta));
			float rad = float(radius * cos(-M_PI/2.0+(i+1)*beta));

			float x = rad*cos(alpha*float(j));
			float y = rad*sin(alpha*float(j));

			position[this->m_tableVertDarts[i*(this->m_nx)+j] ] = VEC3(x, y, h );
		}
	}

	// bottom  pole
	if (m_bottom_triangulated)
	{
		//position[this->m_tableVertDarts[this->m_nx*(this->m_nz+1)] ] = VEC3(0.0f, 0.0f, radius);
		position[m_bottomVertDart] = VEC3(0.0f, 0.0f, -radius);
	}

	//  top pole
	if (m_top_triangulated)
	{
		//position[this->m_tableVertDarts[this->m_nx*(this->m_nz+1)+1] ] = VEC3(0.0f, 0.0f, -radius);
		position[m_topVertDart] = VEC3(0.0f, 0.0f, radius);
	}
}

template <typename PFP>
void Cylinder<PFP>::embedIntoCone(VertexAttribute<VEC3>& position, float radius, float height)
{
	if(m_top_closed && m_top_triangulated)
	{
		float alpha = float(2.0*M_PI/this->m_nx);
		float dz = height/float(this->m_nz+1);
		for( unsigned int i = 0; i <= this->m_nz; ++i)
		{
			for(unsigned int j = 0; j < this->m_nx; ++j)
			{
				float rad = radius * float(this->m_nz+1-i)/float(this->m_nz+1);
				float h = -height/2 + dz*float(i);
				float x = rad*cos(alpha*float(j));
				float y = rad*sin(alpha*float(j));

				position[this->m_tableVertDarts[i*(this->m_nx)+j] ] = VEC3(x, y, h);
			}
		}

		//int indexUmbrella = this->m_nx*(this->m_nz+1);
		if (m_bottom_triangulated)
		{
			//position[this->m_tableVertDarts[indexUmbrella] ] = VEC3(0.0f, 0.0f, -height/2 );
			position[m_bottomVertDart] = VEC3(0.0f, 0.0f, -height/2 );
		}

		//  top always closed in cone
		//position[this->m_tableVertDarts[indexUmbrella++] ] = VEC3(0.0f, 0.0f, height/2 );
		position[m_topVertDart] = VEC3(0.0f, 0.0f, height/2 );
	}

}

} // namespace Triangular

} // namespace Tilings

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
