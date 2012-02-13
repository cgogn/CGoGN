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
* Web site: http://cgogn.unistra.fr/                                  *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "viewer.h"

Viewer::Viewer() :
	m_renderStyle(FLAT),
	m_drawTopo(true),
	m_drawVertices(false),
	m_drawEdges(false),
	m_drawFaces(true),
	m_drawNormals(false),
	m_render(NULL),
	m_renderTopo(NULL),
	m_phongShader(NULL),
	m_flatShader(NULL),
	m_vectorShader(NULL),
	m_simpleColorShader(NULL),
	m_pointSprite(NULL),
	m_strings(NULL)
{
	normalScaleFactor = 1.0f ;
	vertexScaleFactor = 0.1f ;
	faceShrinkage = 1.0f ;

	colClear = Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f) ;
	colDif = Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f) ;
	colSpec = Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f) ;
	colNormal = Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f) ;
	shininess = 80.0f ;
}

void Viewer::initGUI()
{
    setDock(&dock) ;

    dock.check_drawVertices->setChecked(false) ;
    dock.check_drawEdges->setChecked(false) ;
    dock.check_drawFaces->setChecked(true) ;
    dock.check_drawNormals->setChecked(false) ;

    dock.slider_verticesSize->setVisible(false) ;
    dock.slider_normalsSize->setVisible(false) ;

    dock.slider_verticesSize->setSliderPosition(50) ;
    dock.slider_normalsSize->setSliderPosition(50) ;

	setCallBack( dock.check_drawVertices, SIGNAL(toggled(bool)), SLOT(slot_drawVertices(bool)) ) ;
	setCallBack( dock.slider_verticesSize, SIGNAL(valueChanged(int)), SLOT(slot_verticesSize(int)) ) ;
	setCallBack( dock.check_drawEdges, SIGNAL(toggled(bool)), SLOT(slot_drawEdges(bool)) ) ;
	setCallBack( dock.check_drawFaces, SIGNAL(toggled(bool)), SLOT(slot_drawFaces(bool)) ) ;
	setCallBack( dock.combo_faceLighting, SIGNAL(currentIndexChanged(int)), SLOT(slot_faceLighting(int)) ) ;
	setCallBack( dock.check_drawNormals, SIGNAL(toggled(bool)), SLOT(slot_drawNormals(bool)) ) ;
	setCallBack( dock.slider_normalsSize, SIGNAL(valueChanged(int)), SLOT(slot_normalsSize(int)) ) ;
}

void Viewer::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(2) ;

	setFocal(5.0f) ;

	m_render = new Algo::Render::GL2::MapRender() ;
    m_renderTopo = new Algo::Render::GL2::TopoRender();

	m_positionVBO = new Utils::VBO() ;
	m_normalVBO = new Utils::VBO() ;

	m_phongShader = new Utils::ShaderPhong() ;
	m_phongShader->setAttributePosition(m_positionVBO) ;
	m_phongShader->setAttributeNormal(m_normalVBO) ;
	m_phongShader->setAmbiant(colClear) ;
	m_phongShader->setDiffuse(colDif) ;
	m_phongShader->setSpecular(colSpec) ;
	m_phongShader->setShininess(shininess) ;

	m_flatShader = new Utils::ShaderFlat() ;
	m_flatShader->setAttributePosition(m_positionVBO) ;
	m_flatShader->setAmbiant(colClear) ;
	m_flatShader->setDiffuse(colDif) ;
	m_flatShader->setExplode(faceShrinkage) ;

	m_vectorShader = new Utils::ShaderVectorPerVertex() ;
	m_vectorShader->setAttributePosition(m_positionVBO) ;
	m_vectorShader->setAttributeVector(m_normalVBO) ;
	m_vectorShader->setColor(colNormal) ;

	m_simpleColorShader = new Utils::ShaderSimpleColor() ;
	m_simpleColorShader->setAttributePosition(m_positionVBO) ;
	Geom::Vec4f c(0.1f, 0.1f, 0.1f, 1.0f) ;
	m_simpleColorShader->setColor(c) ;

	m_pointSprite = new Utils::PointSprite() ;
	m_pointSprite->setAttributePosition(m_positionVBO) ;

    m_strings = new Utils::Strings3D(true, Geom::Vec3f(0.1f,0.0f,0.3f));
    storeVerticesInfo();
    m_strings->sendToVBO();

	registerShader(m_phongShader) ;
	registerShader(m_flatShader) ;
	registerShader(m_vectorShader) ;
	registerShader(m_simpleColorShader) ;
	registerShader(m_pointSprite) ;
	registerShader(m_strings);
}

void Viewer::cb_redraw()
{
	//glClearColor(1,1,1,0);
	if(m_drawVertices)
	{
		float size = vertexScaleFactor ;
		m_pointSprite->setSize(size) ;
		m_pointSprite->predraw(Geom::Vec3f(0.0f, 0.0f, 1.0f)) ;
		m_render->draw(m_pointSprite, Algo::Render::GL2::POINTS) ;
		m_pointSprite->postdraw() ;
		m_strings->drawAll(Geom::Vec3f(0.0f, 1.0f, 1.0f));
	}

	if(m_drawEdges)
	{
		glLineWidth(1.0f) ;
		m_render->draw(m_simpleColorShader, Algo::Render::GL2::LINES) ;
	}

	if(m_drawNormals)
	{
		float size = normalBaseSize * normalScaleFactor ;
		m_vectorShader->setScale(size) ;
		glLineWidth(1.0f) ;
		m_render->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
	}

	if(m_drawFaces)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
		glEnable(GL_LIGHTING) ;
		glEnable(GL_POLYGON_OFFSET_FILL) ;
		glPolygonOffset(1.0f, 1.0f) ;
		switch(m_renderStyle)
		{
			case FLAT :
				m_flatShader->setExplode(faceShrinkage) ;
				m_render->draw(m_flatShader, Algo::Render::GL2::TRIANGLES) ;
				break ;
			case PHONG :
				m_render->draw(m_phongShader, Algo::Render::GL2::TRIANGLES) ;
				break ;
		}
		glDisable(GL_POLYGON_OFFSET_FILL) ;
	}

	if(m_drawTopo)
		m_renderTopo->drawTopo();
}

void Viewer::cb_Open()
{
	std::string filters("all (*.*);; trian (*.trian);; ctm (*.ctm);; off (*.off);; ply (*.ply)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;
	if (filename.empty())
		return ;

	importMesh(filename) ;
	updateGL() ;
}

void Viewer::cb_Save()
{
	std::string filters("off (*.off);; map (*.map)") ;
	std::string filename = selectFileSave("Save Mesh", "", filters) ;

	exportMesh(filename);
}

void Viewer::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		std::cout << "shift" << std::endl;

		m_renderTopo->updateData<PFP>(myMap, position, 0.9, 0.9, allDarts);

		Dart d = m_renderTopo->picking<PFP>(myMap, x,  y, allDarts);
		if (d != Dart::nil())
		{
			//statusMsg("dart picked");
			myMap.mergeFaces(d);
		}
		else
		{
			statusMsg("No dart was picked");
		}

	}

	updateGL();
}



void Viewer::importMesh(std::string& filename)
{
	myMap.clear(true) ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if (extension == std::string(".map"))
	{
		myMap.loadMapBin(filename);
		position = myMap.getAttribute<PFP::VEC3>(VERTEX, "position") ;
	}
	else
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMesh<PFP>(myMap, filename.c_str(), attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;
	}


	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES) ;

	m_renderTopo->updateData<PFP>(myMap, position, 0.9, 0.9, allDarts);

	bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	normalBaseSize = bb.diagSize() / 100.0f ;
//	vertexBaseSize = normalBaseSize /5.0f ;

	normal = myMap.getAttribute<PFP::VEC3>(VERTEX, "normal") ;
	if(!normal.isValid())
		normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal") ;

	Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

	m_positionVBO->updateData(position) ;
	m_normalVBO->updateData(normal) ;

	storeVerticesInfo();
	m_strings->sendToVBO();

	setParamObject(bb.maxSize(), bb.center().data()) ;
	updateGLMatrices() ;
}

void Viewer::exportMesh(std::string& filename)
{
	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if (extension == std::string(".map"))
		myMap.saveMapBin(filename);
	else
		Algo::Export::exportOFF<PFP>(myMap, position, filename.c_str(), allDarts) ;
}


void Viewer::storeVerticesInfo()
{
	CellMarker mv(myMap,VERTEX);
	for (Dart d=myMap.begin(); d!=myMap.end(); myMap.next(d))
	{
		if (!mv.isMarked(d))
		{
			mv.mark(d);
			std::stringstream ss;
			ss << d << " : "<< position[d];
			m_strings->addString(ss.str(),position[d]);
		}
	}
}


void Viewer::slot_drawVertices(bool b)
{
	m_drawVertices = b ;
	updateGL() ;
}

void Viewer::slot_verticesSize(int i)
{
	vertexScaleFactor = i / 500.0f ;
	m_strings->setScale(0.02f*i);
	updateGL() ;
}

void Viewer::slot_drawEdges(bool b)
{
	m_drawEdges = b ;
	updateGL() ;
}

void Viewer::slot_drawFaces(bool b)
{
	m_drawFaces = b ;
	updateGL() ;
}

void Viewer::slot_faceLighting(int i)
{
	m_renderStyle = i ;
	updateGL() ;
}

void Viewer::slot_drawNormals(bool b)
{
	m_drawNormals = b ;
	updateGL() ;
}

void Viewer::slot_normalsSize(int i)
{
	normalScaleFactor = i / 50.0f ;
	updateGL() ;
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	QApplication app(argc, argv) ;

	Viewer sqt ;
	sqt.setGeometry(0, 0, 1000, 800) ;
 	sqt.show() ;

	if(argc == 2)
	{
		std::string filename(argv[1]) ;
		sqt.importMesh(filename) ;
	}

	sqt.initGUI() ;

	return app.exec() ;
}

