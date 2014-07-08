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

#include "viewerOBJ.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Utils/vbo.h" 


ObjView::ObjView():
	m_obj(myMap),
	m_positionVBO(NULL),
	m_normalVBO(NULL),
	m_texcoordVBO(NULL),
	m_shader(NULL),
	m_shader2(NULL),
	m_RenderStyle(2),
	m_currentGroupDrawn(0xffffffff)
{}

ObjView::~ObjView()
{
	if (m_shader!=NULL)
		delete m_shader;
	if (m_shader2!=NULL)
		delete m_shader2;
	if (m_phongShader!=NULL)
		delete m_phongShader;

	if (m_positionVBO!=NULL)
		delete m_positionVBO;
	if (m_normalVBO!=NULL)
		delete m_normalVBO;
	if (m_texcoordVBO!=NULL)
		delete m_texcoordVBO;
}


void ObjView::cb_keyPress(int k)
{
	switch(k)
	{
		case 'm':
			m_RenderStyle=0;
			break;
		case 'c':
			m_RenderStyle=1;
			break;
		case 't':
			m_RenderStyle=2;
			break;

	case 'o':
		m_currentGroupDrawn=0xffffffff;
		break;
	case 'O':
		m_currentGroupDrawn=0;
		std::cout << "Group "<< m_obj.objGroupName(m_currentGroupDrawn)<< std::endl;
		break;
	case '-':
		m_currentGroupDrawn--;
		std::cout << "Group "<< m_obj.objGroupName(m_currentGroupDrawn)<< std::endl;
		break;
	case '+':
		m_currentGroupDrawn++;
		std::cout << "Group "<< m_obj.objGroupName(m_currentGroupDrawn)<< std::endl;
		break;

		default:
			break;
	}
	updateGL();
}


void ObjView::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	m_dr = new Utils::Drawer;

	// create VBO for position
	m_positionVBO = new Utils::VBO;
	m_texcoordVBO = new Utils::VBO;
	m_normalVBO = new Utils::VBO;

	m_shader2 = new Utils::ShaderPhongTexture();
	m_shader2->setAttributePosition(m_positionVBO);
	m_shader2->setAttributeTexCoord(m_texcoordVBO);
	m_shader2->setAttributeNormal(m_normalVBO);
	m_shader2->setTextureUnit(GL_TEXTURE1);
	m_shader2->setShininess(10.0f);
	m_shader2->setAmbient(0.1f);
	m_shader2->setSpecular(Geom::Vec4f(0.5));
	registerShader(m_shader2);

	m_phongShader = new Utils::ShaderPhong() ;
	m_phongShader->setAttributePosition(m_positionVBO) ;
	m_phongShader->setAttributeNormal(m_normalVBO) ;
	registerShader(m_phongShader);

	glEnable(GL_TEXTURE_2D);
	m_obj.createGroupMatVBO_PTN(m_positionVBO,m_texcoordVBO,m_normalVBO);
}

void ObjView::drawBB( const Geom::BoundingBox<VEC3>& bb)
{
	const VEC3& mi = bb.min();
	const VEC3& ma = bb.max();

	m_dr->newList(GL_COMPILE_AND_EXECUTE);
	m_dr->lineWidth(3.0f);
	m_dr->begin(GL_LINE_LOOP);
	m_dr->color3f(1.0f,1.0f,0.0f);
	m_dr->vertex3f(mi[0],mi[1],mi[2]);
	m_dr->vertex3f(ma[0],mi[1],mi[2]);
	m_dr->vertex3f(ma[0],ma[1],mi[2]);
	m_dr->vertex3f(mi[0],ma[1],mi[2]);
	m_dr->end();
	m_dr->begin(GL_LINE_LOOP);
	m_dr->color3f(1.0f,1.0f,0.0f);
	m_dr->vertex3f(mi[0],mi[1],ma[2]);
	m_dr->vertex3f(ma[0],mi[1],ma[2]);
	m_dr->vertex3f(ma[0],ma[1],ma[2]);
	m_dr->vertex3f(mi[0],ma[1],ma[2]);
	m_dr->end();
	m_dr->begin(GL_LINES);
	m_dr->color3f(1.0f,1.0f,0.0f);
	m_dr->vertex3f(mi[0],mi[1],mi[2]);
	m_dr->vertex3f(mi[0],mi[1],ma[2]);
	m_dr->vertex3f(mi[0],ma[1],mi[2]);
	m_dr->vertex3f(mi[0],ma[1],ma[2]);
	m_dr->vertex3f(ma[0],ma[1],mi[2]);
	m_dr->vertex3f(ma[0],ma[1],ma[2]);
	m_dr->vertex3f(ma[0],mi[1],mi[2]);
	m_dr->vertex3f(ma[0],mi[1],ma[2]);
	m_dr->end();
	m_dr->endList();

}


void ObjView::cb_redraw()
{


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	unsigned int nb = m_obj.nbObjGroups();
//	const std::vector<Algo::Surface::Import::MaterialOBJ*>& mats = m_obj.getMaterials();


	if (m_currentGroupDrawn<(nb-1))
	{
		std::cout <<  "Draw Object "<< m_currentGroupDrawn << std::endl;
		unsigned int i = m_currentGroupDrawn;
		drawBB(m_obj.getGroupBB(i));
		std::cout << "BB drawn" << std::endl;
		switch(m_RenderStyle)
		{
			case 0: // MONO
			for (unsigned int j=0; j<m_obj.nbSubGroup(i);++j)
			{
					Geom::Vec4f v(0.2f,1.0f,0.4f,0.0f); // color here green
					m_phongShader->setAmbiant(0.2f*v) ;
					m_phongShader->setDiffuse(v) ;
					m_phongShader->setSpecular(v) ;
					m_phongShader->setShininess(10000.0) ;
					m_phongShader->enableVertexAttribs();
					glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
					m_phongShader->disableVertexAttribs();
				}
				break;
			case 1: // COLOR
			for (unsigned int j=0; j<m_obj.nbSubGroup(i);++j)
			{
				const Algo::Surface::Import::MaterialOBJ* mat = m_obj.materialOf(i,j);
					Geom::Vec4f v(mat->diffuseColor[0],mat->diffuseColor[1],mat->diffuseColor[2],0.0f);
					m_phongShader->setAmbiant(0.2f*v) ;
					m_phongShader->setDiffuse(v) ;
					m_phongShader->setSpecular(v) ;
					m_phongShader->setShininess(10000.0) ;
					m_phongShader->enableVertexAttribs();
					glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
					m_phongShader->disableVertexAttribs();

				}
			default:
			for (unsigned int j=0; j<m_obj.nbSubGroup(i);++j)
			{
				const Algo::Surface::Import::MaterialOBJ* mat = m_obj.materialOf(i,j);
					if (mat->textureDiffuse != NULL)
					{
						m_shader2->setTexture(mat->textureDiffuse);
						m_shader2->setShininess(mat->shininess);
						m_shader2->setAmbient(0.8f);
						m_shader2->activeTexture();
						m_shader2->enableVertexAttribs();
						glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
						m_shader2->disableVertexAttribs();
					}
					else
					{
						Geom::Vec4f v;
						v[0] = 0.5f; v[1] = 0.5f; v[2] = 0.5f; v[3] = 0.0f;
						m_phongShader->setAmbiant(v) ;
						v[0] = mat->diffuseColor[0]; v[1] = mat->diffuseColor[1]; v[2] = mat->diffuseColor[2]; v[3] = 0.0f;
						m_phongShader->setDiffuse(v) ;
						v[0] = mat->specularColor[0]; v[1] = mat->specularColor[1]; v[2] = mat->specularColor[2]; v[3] = 0.0f;
						m_phongShader->setSpecular(v) ;
						m_phongShader->setShininess(mat->shininess) ;
						m_phongShader->enableVertexAttribs();
						glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
						m_phongShader->disableVertexAttribs();
					}
				}
		}
	}
	else
	for (unsigned int i=0; i<nb; ++i)
	{

		drawBB(m_obj.getGroupBB(i));

		switch(m_RenderStyle)
		{
			case 0: // MONO
			for (unsigned int j=0; j<m_obj.nbSubGroup(i);++j)
			{
					Geom::Vec4f v(0.2f,1.0f,0.4f,0.0f); // color here green
					m_phongShader->setAmbiant(0.2f*v) ;
					m_phongShader->setDiffuse(v) ;
		//			v[0] = 1.0f; v[1] = 1.0f; v[2] = 1.0f; v[3] = 0.0f; // use this for specular effected
					m_phongShader->setSpecular(v) ;
					m_phongShader->setShininess(10000.0) ;
					m_phongShader->enableVertexAttribs();
					glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
					m_phongShader->disableVertexAttribs();
				}
				break;
			case 1: // COLOR
			for (unsigned int j=0; j<m_obj.nbSubGroup(i);++j)
			{
				const Algo::Surface::Import::MaterialOBJ* mat = m_obj.materialOf(i,j);

					Geom::Vec4f v(mat->diffuseColor[0],mat->diffuseColor[1],mat->diffuseColor[2],0.0f);
					m_phongShader->setAmbiant(0.2f*v) ;
					m_phongShader->setDiffuse(v) ;
		//			v[0] = 1.0f; v[1] = 1.0f; v[2] = 1.0f; v[3] = 0.0f; // use this for specular effected
					m_phongShader->setSpecular(v) ;
					m_phongShader->setShininess(10000.0) ;
					m_phongShader->enableVertexAttribs();
					glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
					m_phongShader->disableVertexAttribs();

				}
			default:
			for (unsigned int j=0; j<m_obj.nbSubGroup(i);++j)
			{
				const Algo::Surface::Import::MaterialOBJ* mat = m_obj.materialOf(i,j);

					if (mat->textureDiffuse != NULL)
					{
						m_shader2->setTexture(mat->textureDiffuse);
						m_shader2->setShininess(mat->shininess);
						m_shader2->setAmbient(0.8f);
						m_shader2->activeTexture();
						m_shader2->enableVertexAttribs();
						glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
						m_shader2->disableVertexAttribs();
					}
					else
					{
						Geom::Vec4f v;
						v[0] = 0.5f; v[1] = 0.5f; v[2] = 0.5f; v[3] = 0.0f;
						m_phongShader->setAmbiant(v) ;
						v[0] = mat->diffuseColor[0]; v[1] = mat->diffuseColor[1]; v[2] = mat->diffuseColor[2]; v[3] = 0.0f;
						m_phongShader->setDiffuse(v) ;
						v[0] = mat->specularColor[0]; v[1] = mat->specularColor[1]; v[2] = mat->specularColor[2]; v[3] = 0.0f;
						m_phongShader->setSpecular(v) ;
						m_phongShader->setShininess(mat->shininess) ;
						m_phongShader->enableVertexAttribs();
						glDrawArrays(GL_TRIANGLES, m_obj.beginIndex(i,j), m_obj.nbIndices(i,j));
						m_phongShader->disableVertexAttribs();
					}
				}
		}
	}
}


void ObjView::init(const std::string& fnm)
{
	std::vector<std::string> attrNames;
	m_obj.import(fnm,attrNames);

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, m_obj.m_positions);
	float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
	Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

	// send BB info to interface for centering on GL screen
	setParamObject(lWidthObj, lPosObj.data());
}


int main(int argc, char**argv)
{
	// interface:
	QApplication app(argc, argv);
	ObjView tv;

	if (argc == 2)
	{
		tv.init(argv[1]);
	}
	else
	{
		CGoGNerr << argv[0] << " objFile"<< CGoGNendl;
	}

	tv.show();


	// et on attend la fin.
	return app.exec();
}
