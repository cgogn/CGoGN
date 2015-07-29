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

#include "wallPaper.h"
#include "Algo/Tiling/Surface/square.h"

using namespace CGoGN ;

void MyQT::cb_initGL()
{
	position = myMap.addAttribute<PFP::VEC3, VERTEX, MAP>("position");
	Algo::Surface::Tilings::Square::Cylinder<PFP> c(myMap,50,50);
	c.embedIntoCylinder(position,0.5f,0.7f,1.0f);
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);
	setParamObject(lWidthObj,lPosObj.data());

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();
	m_positionVBO->updateData(position);

	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(1.0,0.0,0.0,0.));

	m_render->initPrimitives<PFP>(myMap, Algo::Render::GL2::LINES);

	registerShader(m_shader);


	// FOR WALL PAPER

	m_textureWP = new Utils::Texture<2,Geom::Vec3uc>(GL_UNSIGNED_BYTE);
	if (!m_textureWP->load("../../SCHNApps/resources/cgogn/cgogn2.png"))
		computeTexture();
	m_textureWP->update();
	m_textureWP->setWrapping(GL_CLAMP_TO_EDGE);


	m_textureWP2 = new Utils::Texture<2,Geom::Vec3uc>(GL_UNSIGNED_BYTE);
	if (!m_textureWP2->load("../../SCHNApps/resources/icons/cameras.png"))
		computeTexture();
	m_textureWP2->update();
	m_textureWP2->setWrapping(GL_CLAMP_TO_EDGE);


	m_shaderWP = new Utils::ShaderWallPaper();
	m_shaderWP->setTextureUnit(GL_TEXTURE0);
	m_shaderWP->setTexture(m_textureWP);
	registerShader(m_shaderWP);

	glEnable(GL_TEXTURE_2D);


	std::cout << Utils::GLSLShader::MAJOR_OGL_CORE << " / "<< Utils::GLSLShader::MINOR_OGL_CORE << std::endl;
	std::cout << glGetString(GL_VERSION)<< std::endl;
}

void MyQT::cb_redraw()
{
	// FOR WALL PAPER
	m_render->draw(m_shader, Algo::Render::GL2::LINES);

	m_shaderWP->setTexture(m_textureWP);
	m_shaderWP->draw();

	m_shaderWP->drawFront(this->getWidth(), this->getHeight(),150,10,200,200,m_textureWP2);
}


void MyQT::cb_keyPress(int keycode)
{
	switch(keycode)
	{
	case 'l' :
		if (m_textureWP->load("../../SCHNApps/resources/cgogn/cgogn2.png"))
			m_textureWP->update();
		m_textureWP->setWrapping(GL_CLAMP_TO_EDGE);
		break;

	case 'c' :
		computeTexture();
		m_textureWP->update();
		m_textureWP->setWrapping(GL_CLAMP_TO_EDGE);
		break;
	}
	updateGL();
}



void MyQT::computeTexture()
{
	std::vector<Geom::Vec3f> colorTable;
	colorTable.push_back(Geom::Vec3uc(255,0,0));
	colorTable.push_back(Geom::Vec3uc(0,255,0));
	colorTable.push_back(Geom::Vec3uc(0,0,255));
	colorTable.push_back(Geom::Vec3uc(255,255,0));
	colorTable.push_back(Geom::Vec3uc(255,0,255));
	colorTable.push_back(Geom::Vec3uc(0,255,255));
	colorTable.push_back(Geom::Vec3uc(255,255,255));

	m_textureWP->create(Geom::Vec2ui(16,16));

#define WIDTHCHECKER 2
	for (unsigned int ki = 0; ki < m_textureWP->size()[0]; ++ki)
	{
		for (unsigned int kj = 0; kj <  m_textureWP->size()[1]; ++kj)
		{
			unsigned int kc = ((kj/WIDTHCHECKER)*m_textureWP->size()[0] + (ki/WIDTHCHECKER))%7;
			(*m_textureWP)(ki,kj) =colorTable[kc];
		}
	}
#undef WIDTHCHECKER
}

int main(int argc, char **argv)
{
	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

	sqt.show();

	return app.exec();
}
