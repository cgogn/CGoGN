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

#include "texturesExample.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Utils/vbo.h" 

TexView::TexView():
	m_render(NULL),
	m_positionVBO(NULL),
	m_texcoordVBO(NULL),
	m_texture(NULL),
	m_shader(NULL),
	m_modeMask(false),
	m_fileName("")
{}

TexView::~TexView()
{
	delete m_render;
	delete m_shader;
	delete m_positionVBO;
	delete m_texcoordVBO;
	delete m_texture;
}

void TexView::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO;
	m_texcoordVBO = new Utils::VBO;

	m_texture = new Utils::Texture<2,Geom::Vec3uc>(GL_UNSIGNED_BYTE);
	computeImage();
	m_texture->update();

	m_mask = new Utils::Texture<2,float>(GL_FLOAT);
	m_mask->create(Geom::Vec2ui(256,256));
	createMask(8);
	m_mask->update();

	m_shader = new Utils::ShaderSimpleTexture();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setAttributeTexCoord(m_texcoordVBO);
	m_shader->setTextureUnit(GL_TEXTURE0);
	m_shader->setTexture(m_texture);
	registerShader(m_shader);

	m_shader2 = new Utils::ShaderTextureMask();
	m_shader2->setAttributePosition(m_positionVBO);
	m_shader2->setAttributeTexCoord(m_texcoordVBO);
	m_shader2->setTextureUnits(GL_TEXTURE0,GL_TEXTURE1);
	m_shader2->setTextures(m_texture,m_mask);
	registerShader(m_shader2);

	glEnable(GL_TEXTURE_2D);

	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::TRIANGLES);
}

void TexView::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
	if (m_shader)
	{
		if (m_modeMask)
		{
			m_shader2->activeTextures();
			m_render->draw(m_shader2, Algo::Render::GL2::TRIANGLES);
		}
		else
		{
			m_shader->activeTexture();
			m_render->draw(m_shader, Algo::Render::GL2::TRIANGLES);
		}
	}
}

void TexView::cb_keyPress(int code)
{
	switch(code)
	{
	case 'l':
		m_texture->setFiltering(GL_LINEAR);
		break;
	case 'n':
		m_texture->setFiltering(GL_NEAREST);
		break;
	case 'm':
		m_modeMask = !m_modeMask;
		if (m_modeMask)
		{
			createMask(16);
			m_mask->update();
		}
		break;
	case 'M':
		m_modeMask = !m_modeMask;
		if (m_modeMask)
		{
			createMask(8);
			m_mask->update();
		}
		break;
	case 's':
		m_texture->subSample2<Geom::Vec3d>();
		m_texture->update();
		break;
	case 't':
		m_texture->rotate90(3);
		m_texture->update();
		break;


	case 'r':
		m_texture->load(m_fileName);
		m_texture->update();
		break;
	}
	updateGL();
}

void TexView::cb_Open()
{
	std::string filename = selectFile("Open Image","/tmp");
	if (!filename.empty())
	{
		m_fileName = filename;
		if (m_texture->load(filename))
		{
			m_texture->update();
			updateGL();
		}
		else
			CGoGNerr << "Problem loading image"<< CGoGNendl;
	}
	else
	{
		computeImage();
		m_texture->update();
		updateGL();
	}
}

void TexView::createMask(unsigned int nb)
{
	if (nb ==0)
		return;

	unsigned int sz0 = m_mask->size()[0]/nb;
	unsigned int sz1 = m_mask->size()[1]/nb;

	for (unsigned int j=0; j<m_mask->size()[1]; ++j)
	{
		for (unsigned int i=0; i<m_mask->size()[0]; ++i)
		{
			bool b1 = (i/sz0)%2 ==0;
			bool b2 = (j/sz1)%2 ==0;
			if (b1!=b2)
				(*m_mask)(i,j)=1.0f;
			else
				(*m_mask)(i,j)= 0.0f;
		}
	}
}

void TexView::computeImage()
{
	std::vector<Geom::Vec3f> colorTable;
	colorTable.push_back(Geom::Vec3uc(255,0,0));
	colorTable.push_back(Geom::Vec3uc(0,255,0));
	colorTable.push_back(Geom::Vec3uc(0,0,255));
	colorTable.push_back(Geom::Vec3uc(255,255,0));
	colorTable.push_back(Geom::Vec3uc(255,0,255));
	colorTable.push_back(Geom::Vec3uc(0,255,255));
	colorTable.push_back(Geom::Vec3uc(255,255,255));

	m_texture->create(Geom::Vec2ui(512,512));

#define WIDTHCHECKER 20

	for (unsigned int ki = 0; ki < m_texture->size()[0]; ++ki)
	{
		for (unsigned int kj = 0; kj <  m_texture->size()[1]; ++kj)
		{
			unsigned int kc = ((kj/WIDTHCHECKER)*m_texture->size()[0] + (ki/WIDTHCHECKER))%7;
			(*m_texture)(ki,kj) =colorTable[kc];
		}
	}
#undef WIDTHCHECKER
}

int main(int argc, char**argv)
{
	// interface:
	QApplication app(argc, argv);
	TexView tv;

	PFP::MAP& m = tv.myMap;

	VertexAttribute<VEC3> position = m.addAttribute<VEC3, VERTEX>("position");
	VertexAttribute<Geom::Vec2f> texcoord = m.addAttribute<Geom::Vec2f, VERTEX>("texcoord");

#define NB 96

	Algo::Surface::Modelisation::Polyhedron<PFP> prim(m, position);
	prim.tore_topo(NB, NB);
	prim.embedTore(40.0f,20.0f);

	Dart d = prim.getDart();
	for(unsigned int i=0; i<NB; ++i)
	{
		for(unsigned int j=0; j<NB; ++j)
		{
			float a;
			if (i<=NB/2)
				a = (2.0f/NB)*i;
			else
				a = (2.0f/NB)*(NB-i);
			float b;
			if (j<=NB/2)
				b = (2.0f/NB)*j;
			else
				b =(2.0f/NB)*(NB-j);

			texcoord[d] = Geom::Vec2f(a,b);
			d = m.phi<121>(d);
		}
		d = m.phi<211>(d);
	}

#undef NB

    //  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(m, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	tv.setParamObject(lWidthObj, lPosObj.data());
	// show 1 pour GL context
	tv.show();

	// update des VBO (position et texture coord)
	tv.m_positionVBO->updateData(position);
	tv.m_texcoordVBO->updateData(texcoord);

	// show final pour premier redraw
	tv.show();

	// et on attend la fin.
	return app.exec();
}
