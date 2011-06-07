/*
 * stage_shader.cpp
 *
 *  Created on: 6 juin 2011
 *      Author: untereiner
 */

#include "stage_shader.h"

/*******************************************************************************
 *														SLOTS
 *******************************************************************************/

void StageShader::slot_drawTopo(bool b)
{
	m_drawTopo = b;
	updateGL();
}

void StageShader::slot_drawVertices(bool b)
{
	m_drawVertices = b;
	updateGL();
}


void StageShader::slot_drawLines(bool b)
{
	m_drawLines = b;
	updateGL();
}


void StageShader::slot_drawFaces(bool b)
{
	m_drawFaces = b;
	updateGL();
}

void StageShader::slot_explodTopoPhi1(double c)
{
	m_coeffTopoExplod[0] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void StageShader::slot_explodTopoPhi2(double c)
{
	m_coeffTopoExplod[1] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void StageShader::slot_explodTopoPhi3(double c)
{
	m_coeffTopoExplod[2] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void StageShader::button_compile()
{
	QString st1 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->vertexEdit->toPlainText();
	QString st2 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->fragmentEdit->toPlainText();
	QString st3 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->geometryEdit->toPlainText();

	m_shader->reloadVertexShaderFromMemory(st1.toStdString().c_str());
	m_shader->reloadFragmentShaderFromMemory(st2.toStdString().c_str());
	m_shader->reloadGeometryShaderFromMemory(st3.toStdString().c_str());

	m_shader->recompile();
	updateGLMatrices();
}

/*******************************************************************************
 *														QT TRICK
 *******************************************************************************/

void StageShader::dockWidget_topLevelChanged(bool isFloating)
{
	if(isFloating)
	{
		dockWidget()->setWindowFlags(Qt::Window);

		// setWindowFlags calls setParent() when changing the flags for a window, causing the widget to be hidden.
		// You must call show() to make the widget visible again
		dockWidget()->show();
	}
}

/*******************************************************************************
 *														Init Functions
 *******************************************************************************/

StageShader::StageShader():
	m_drawVertices(false),
	m_drawLines(true),
	m_drawFaces(true),
	m_drawTopo(false),
	m_render(NULL),
	m_render_topo(NULL),
	m_positionVBO(NULL),
	m_shader(NULL)
{
	m_coeffTopoExplod = Geom::Vec3f(0.9,0.9,0.9);
}

void StageShader::initGUI()
{
	CGoGNStream::allToConsole(this) ;
	CGoGNStream::allToStd(false) ;

	setDock(&dock);

	dock.setWindowFlags(Qt::WindowMinimizeButtonHint);

	setCallBack(dock.checkBox_vertices, SIGNAL(toggled(bool)), SLOT(slot_drawVertices(bool)));
	setCallBack(dock.checkBox_lines, SIGNAL(toggled(bool)), SLOT(slot_drawLines(bool)));
	setCallBack(dock.checkBox_faces, SIGNAL(toggled(bool)), SLOT(slot_drawFaces(bool)));
	setCallBack( dock.groupBox_topo, SIGNAL(toggled(bool)), SLOT(slot_drawTopo(bool)));

	setCallBack(dock.explod_phi1, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi1(double)));
	setCallBack(dock.explod_phi2, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi2(double)));
	setCallBack(dock.explod_phi3, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi3(double)));

	setCallBack(&dock, SIGNAL(topLevelChanged(bool)), SLOT(dockWidget_topLevelChanged(bool)));

	setCallBack(dock.compileButton, SIGNAL(clicked()), SLOT(button_compile()) );

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));
	dock.geometryEdit->setPlainText(QString(m_shader->getGeometryShaderSrc()));

}

void StageShader::cb_Open()
{
	std::string filters("all (*.*)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;

	importMesh(filename) ;
	updateGL() ;
}

void StageShader::importMesh(std::string& filename)
{
	std::vector<std::string> attrNames ;

	std::string extension;
	size_t pos;

	pos = filename.rfind(".");    // position of "." in filename
	 extension = filename.substr(pos);

	if(extension == std::string(".tet"))
	{
		if(!Algo::Import::importTet<PFP>(myMap,filename.c_str(),attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		else
			position = myMap.getAttribute<PFP::VEC3>(VERTEX , attrNames[0]) ;
	}
	else if(extension == std::string(".ts"))
	{
		if(!Algo::Import::importTs<PFP>(myMap,filename.c_str(),attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		else
			position = myMap.getAttribute<PFP::VEC3>(VERTEX , attrNames[0]) ;
	}

	updateVBOprimitives(Algo::Render::GL2::TRIANGLES | Algo::Render::GL2::LINES | Algo::Render::GL2::POINTS) ;

	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	gPosObj = bb.center() ;
	float tailleX = bb.size(0) ;
	float tailleY = bb.size(1) ;
	float tailleZ = bb.size(2) ;
	gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;

	m_positionVBO->updateData(position) ;

	setParamObject(gWidthObj, gPosObj.data()) ;

	updateGLMatrices() ;
}

/*******************************************************************************
 *														GL FUNCTIONS
 *******************************************************************************/

void StageShader::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();
	m_render_topo = new Algo::Render::GL2::Topo3RenderMapD();

	// create VBO for position
	m_positionVBO = new Utils::VBO();

	// using simple shader with color
	m_shader = new Utils::ShaderSimpleColor();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));

	registerShader(m_shader);
}

void StageShader::updateVBOprimitives(int upType)
{
	if(upType & Algo::Render::GL2::TRIANGLES)
	{
		m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES) ;
	}

	if(upType & Algo::Render::GL2::LINES)
	{
		m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES,false) ;
	}

	if(upType & Algo::Render::GL2::POINTS)
	{
		m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS) ;
	}
}

void StageShader::cb_redraw()
{
	if(m_drawVertices)
	{
		m_shader->setColor(Geom::Vec4f(0.,0.,1.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::POINTS);
	}

	if(m_drawLines)
	{
		glLineWidth(2.0f);
		m_shader->setColor(Geom::Vec4f(1.,1.,0.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::LINES);
	}

	if(m_drawFaces)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		m_shader->setColor(Geom::Vec4f(0.,1.,0.,0.));
		m_render->draw(m_shader, Algo::Render::GL2::TRIANGLES);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	if (m_drawTopo)
		m_render_topo->drawTopo();

}

void StageShader::cb_keyPress(int code)
{

}

void StageShader::cb_mousePress(int button, int x, int y)
{

}

/**********************************************************************************************
 *							                                      MAIN FUNCTION                         								                *
 **********************************************************************************************/

int main(int argc, char** argv)
{
    QApplication app(argc, argv) ;

    StageShader sqt ;
    sqt.setGeometry(0, 0, 1000, 800) ;
    sqt.show() ;

	if(argc ==  2)
    {
            std::string filename(argv[1]) ;
            sqt.importMesh(filename) ;
    }
	else
	{
		sqt.position = sqt.myMap.addAttribute<PFP::VEC3>(VERTEX, "position");
		Algo::Modelisation::Primitive3D<PFP> prim(sqt.myMap, sqt.position);
		prim.hexaGrid_topo(10,10,10);
		prim.embedHexaGrid(1.0f,1.0f,1.0f);
	}

    sqt.initGUI() ;

    return app.exec() ;
}
