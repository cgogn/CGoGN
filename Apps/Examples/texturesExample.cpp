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
#include "Algo/Tiling/Surface/square.h"
#include "Utils/vbo.h" 

TexView::TexView():
	m_obj(myMap),
	m_positionVBO(NULL),
	m_texcoordVBO(NULL),
	m_nbIndices(0),
	m_texture(NULL),
	m_shader(NULL),
	m_shader2(NULL),
	m_phong(true)
{}

TexView::~TexView()
{
	delete m_shader;
	delete m_positionVBO;
	delete m_texcoordVBO;
	delete m_texture;
}

void TexView::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create VBO for position
	m_positionVBO = new Utils::VBO;
	m_texcoordVBO = new Utils::VBO;
	m_normalVBO = new Utils::VBO;

	m_texture = new Utils::Texture<2,Geom::Vec3uc>(GL_UNSIGNED_BYTE);

	if (m_texture->load(m_fileNameTex))
		m_texture->update();
	else
		computeImage();
//		CGoGNerr << "Problem loading image"<< CGoGNendl;

	m_texture->setWrapping(GL_CLAMP_TO_EDGE);
	m_texture->update();

	m_shader = new Utils::ShaderSimpleTexture();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setAttributeTexCoord(m_texcoordVBO);
	m_shader->setTextureUnit(GL_TEXTURE0);
	m_shader->setTexture(m_texture);
	registerShader(m_shader);

	m_shader2 = new Utils::ShaderPhongTexture();
	m_shader2->setAttributePosition(m_positionVBO);
	m_shader2->setAttributeTexCoord(m_texcoordVBO);
	m_shader2->setAttributeNormal(m_normalVBO);
	m_shader2->setTextureUnit(GL_TEXTURE1);
	m_shader2->setTexture(m_texture);
	m_shader2->setShininess(10.0f);
	m_shader2->setAmbient(0.1f);
	m_shader2->setSpecular(Geom::Vec4f(0.5));
	registerShader(m_shader2);

	glEnable(GL_TEXTURE_2D);

	if (!m_obj.hasNormals())
	{
		VertexAttribute<Geom::Vec3f, MAP> normal = myMap.getAttribute<VEC3, VERTEX, MAP>("normal") ;
		if(!normal.isValid())
			normal = myMap.addAttribute<VEC3, VERTEX, MAP>("normal") ;

		Algo::Surface::Geometry::computeNormalVertices<PFP>(myMap, m_obj.m_positions, normal) ;
		m_obj.setNormalAttribute(normal);
	}

	m_nbIndices = m_obj.createSimpleVBO_PTN(m_positionVBO,m_texcoordVBO,m_normalVBO);
}

void TexView::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_phong)
	{
		m_shader2->activeTexture();
		m_shader2->enableVertexAttribs();
		glDrawArrays(GL_TRIANGLES, 0, m_nbIndices);
		m_shader2->disableVertexAttribs();
	}
	else
	{
		m_shader->activeTexture();
		m_shader->enableVertexAttribs();
		glDrawArrays(GL_TRIANGLES, 0, m_nbIndices);
		m_shader->disableVertexAttribs();
	}
}

void TexView::cb_keyPress(int code)
{
	switch(code)
	{
	case 'p':
		m_phong = !m_phong;
	case 'l':
		m_texture->setFiltering(GL_LINEAR);
		break;
	case 'n':
		m_texture->setFiltering(GL_NEAREST);
		break;
	case '1':
		m_shader2->setShininess(10.0f);
		break;
	case '2':
		m_shader2->setShininess(100.0f);
		break;
	case '3':
		m_shader2->setShininess(500.0f);
		break;
	case '4':
		m_shader2->setShininess(25000.0f);
		break;
	case '5':
		m_shader2->setShininess(1.0f);
		m_shader2->setSpecular(Geom::Vec4f(0));
		break;
	}
	updateGL();
}

void TexView::init(const std::string& fnm, const std::string& fnt)
{
	if (fnm.empty()	)
	{
		computeTore();
		m_fileNameTex  = fnt;
	}
	else
	{
		m_fileNameMesh = fnm;
		m_fileNameTex  = fnt;
		std::vector<std::string> attrNames;

		m_obj.import(m_fileNameMesh,attrNames);
	}

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, m_obj.m_positions);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	// send BB info to interface for centering on GL screen
	setParamObject(lWidthObj, lPosObj.data());
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

void TexView::computeTore()
{
#define NB 96

	VertexAttribute<VEC3, MAP> position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");
	VertexAttribute<Geom::Vec2f, MAP> texcoord = myMap.addAttribute<Geom::Vec2f, VERTEX, MAP>("texcoord");
    //Algo::Surface::Modelisation::Polyhedron<PFP> prim(myMap, position);
    Algo::Surface::Tilings::Square::Tore<PFP> prim(myMap, NB, NB);
    prim.embedIntoTore(position, 40.0f,20.0f);
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
			d = myMap.phi<121>(d);
		}
		d = myMap.phi<211>(d);
	}
#undef NB

	m_obj.setPositionAttribute(position);
	m_obj.setTexCoordAttribute(texcoord);
}

int main(int argc, char**argv)
{
	// interface:
	QApplication app(argc, argv);
	TexView tv;

	if (argc == 3)
	{
		tv.init(argv[1], argv[2]);
	}
	else if (argc == 2)
	{
		tv.init("", argv[1]);
	}
	else
	{
		tv.init("","x");
	}

	tv.show();

	// et on attend la fin.
	return app.exec();
}
