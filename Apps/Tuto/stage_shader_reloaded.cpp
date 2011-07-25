/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed  in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "stage_shader_reloaded.h"
#include "Utils/static_assert.h"

/*******************************************************************************
 *														SLOTS
 *******************************************************************************/

void StageShaderReloaded::slot_drawTopo(bool b)
{
	m_drawTopo = b;
	updateGL();
}

void StageShaderReloaded::slot_drawVertices(bool b)
{
	m_drawVertices = b;
	updateGL();
}

void StageShaderReloaded::slot_drawLines(bool b)
{
	m_drawLines = b;
	updateGL();
}

void StageShaderReloaded::slot_drawFaces(bool b)
{
	m_drawFaces = b;
	updateGL();
}

void StageShaderReloaded::slot_explodTopoPhi1(double c)
{
	m_coeffTopoExplod[0] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void StageShaderReloaded::slot_explodTopoPhi2(double c)
{
	m_coeffTopoExplod[1] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void StageShaderReloaded::slot_explodTopoPhi3(double c)
{
	m_coeffTopoExplod[2] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void StageShaderReloaded::slot_pushButton_addPlane()
{
	m_shader->setClipPlanesCount(dock.comboBox_PlaneIndex->count() + 1);
	Utils::Pickable* pickable = new Utils::Pickable(m_planeDrawable, dock.comboBox_PlaneIndex->count() + 1);
	m_pickablePlanes.push_back(pickable);

	m_shader->setClipPlaneParamsOrigin(m_bb.center(), dock.comboBox_PlaneIndex->count() + 1 - 1);
	pickable->translate(m_bb.center());

	std::string indexStr;
	std::stringstream ss;
	ss << (dock.comboBox_PlaneIndex->count() + 1);
	indexStr = ss.str();

	std::string str = "Plane " + indexStr;

	dock.comboBox_PlaneIndex->addItem(QString(str.c_str()));

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void StageShaderReloaded::slot_pushButton_deletePlane()
{
	if (m_pickablePlanes.size() > 0)
	{
		if (m_lastPickedObject == m_pickablePlanes.back())
			m_lastPickedObject = NULL;

		delete m_pickablePlanes.back();
		m_pickablePlanes.pop_back();

		m_shader->setClipPlanesCount(dock.comboBox_PlaneIndex->count() - 1);

		dock.comboBox_PlaneIndex->removeItem(dock.comboBox_PlaneIndex->count() - 1);

		dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
		dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

		updateGLMatrices();
	}
}

void StageShaderReloaded::slot_comboBox_PlaneIndexChanged(int newIndex)
{
	if (newIndex >= 0)
	{
		Geom::Vec3f currPlaneVec1 = m_shader->getClipPlaneParamsFirstVec(newIndex);
		dock.doubleSpinBox_PlaneVec1x->setValue(currPlaneVec1[0]);
		dock.doubleSpinBox_PlaneVec1y->setValue(currPlaneVec1[1]);
		dock.doubleSpinBox_PlaneVec1z->setValue(currPlaneVec1[2]);

		Geom::Vec3f currPlaneVec2 = m_shader->getClipPlaneParamsSecondVec(newIndex);
		dock.doubleSpinBox_PlaneVec2x->setValue(currPlaneVec2[0]);
		dock.doubleSpinBox_PlaneVec2y->setValue(currPlaneVec2[1]);
		dock.doubleSpinBox_PlaneVec2z->setValue(currPlaneVec2[2]);

		Geom::Vec3f currPlaneOrigin = m_shader->getClipPlaneParamsOrigin(newIndex);
		dock.doubleSpinBox_PlaneOriginx->setValue(currPlaneOrigin[0]);
		dock.doubleSpinBox_PlaneOriginy->setValue(currPlaneOrigin[1]);
		dock.doubleSpinBox_PlaneOriginz->setValue(currPlaneOrigin[2]);
	}
}

void StageShaderReloaded::slot_doubleSpinBox_PlaneVec1(double c)
{
	if (dock.comboBox_PlaneIndex->currentIndex() >= 0)
	{
		float x = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneVec1x->value();
		float y = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneVec1y->value();
		float z = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneVec1z->value();
		m_shader->setClipPlaneParamsFirstVec(Geom::Vec3f(x, y, z), dock.comboBox_PlaneIndex->currentIndex());
		updateGL();
	}
}

void StageShaderReloaded::slot_doubleSpinBox_PlaneVec2(double c)
{
	if (dock.comboBox_PlaneIndex->currentIndex() >= 0)
	{
		float x = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneVec2x->value();
		float y = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneVec2y->value();
		float z = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneVec2z->value();
		m_shader->setClipPlaneParamsSecondVec(Geom::Vec3f(x, y, z), dock.comboBox_PlaneIndex->currentIndex());
		updateGL();
	}
}

void StageShaderReloaded::slot_doubleSpinBox_PlaneOrigin(double c)
{
	if (dock.comboBox_PlaneIndex->currentIndex() >= 0)
	{
		float x = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneOriginx->value();
		float y = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneOriginy->value();
		float z = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_PlaneOriginz->value();
		m_shader->setClipPlaneParamsOrigin(Geom::Vec3f(x, y, z), dock.comboBox_PlaneIndex->currentIndex());
		updateGL();
	}
}

void StageShaderReloaded::slot_spinBox_GridResolution(int i)
{
	m_planeDrawable->updatePrecisionDrawing(i);
	updateGL();
}

void StageShaderReloaded::slot_doubleSpinBox_GridColor(double c)
{
	float r = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_GridColorR->value();
	float g = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_GridColorG->value();
	float b = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_GridColorB->value();

	m_planeDrawable->setColor(Geom::Vec4f(r, g, b, 1.0));
	updateGL();
}

void StageShaderReloaded::slot_pushButton_addSphere()
{
	m_shader->setClipSpheresCount(dock.comboBox_SphereIndex->count() + 1);
	Utils::Pickable* pickable = new Utils::Pickable(m_sphereDrawable, dock.comboBox_SphereIndex->count() + 1);
	m_pickableSpheres.push_back(pickable);

	m_shader->setClipSphereParamsCenter(m_bb.center(), dock.comboBox_SphereIndex->count() + 1 - 1);
	m_shader->setClipSphereParamsRadius((m_bb.maxSize())*1.0, dock.comboBox_SphereIndex->count() + 1 - 1);
	pickable->translate(m_bb.center());
	pickable->scale(Geom::Vec3f(m_bb.maxSize()*1.0));

	std::string indexStr;
	std::stringstream ss;
	ss << (dock.comboBox_SphereIndex->count() + 1);
	indexStr = ss.str();

	std::string str = "Sphere " + indexStr;

	dock.comboBox_SphereIndex->addItem(QString(str.c_str()));

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void StageShaderReloaded::slot_pushButton_deleteSphere()
{
	if (m_pickableSpheres.size() > 0)
	{
		if (m_lastPickedObject == m_pickableSpheres.back())
			m_lastPickedObject = NULL;

		delete m_pickableSpheres.back();
		m_pickableSpheres.pop_back();

		m_shader->setClipSpheresCount(dock.comboBox_SphereIndex->count() - 1);

		dock.comboBox_SphereIndex->removeItem(dock.comboBox_SphereIndex->count() - 1);

		dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
		dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

		updateGLMatrices();
	}
}

void StageShaderReloaded::slot_comboBox_SphereIndexChanged(int newIndex)
{
	if (newIndex >= 0)
	{
		Geom::Vec3f currSphereCenter = m_shader->getClipSphereParamsCenter(newIndex);
		dock.doubleSpinBox_SphereCenterx->setValue(currSphereCenter[0]);
		dock.doubleSpinBox_SphereCentery->setValue(currSphereCenter[1]);
		dock.doubleSpinBox_SphereCenterz->setValue(currSphereCenter[2]);

		dock.doubleSpinBox_SphereRadius->setValue(m_shader->getClipSphereParamsRadius(newIndex));
	}
}

void StageShaderReloaded::slot_doubleSpinBox_SphereCenter(double c)
{
	int index = dock.comboBox_SphereIndex->currentIndex();
	if (index >= 0)
	{
		float x = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_SphereCenterx->value();
		float y = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_SphereCentery->value();
		float z = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_SphereCenterz->value();
		m_shader->setClipSphereParamsCenter(Geom::Vec3f(x, y, z), index);
		updateGL();
	}
}

void StageShaderReloaded::slot_doubleSpinBox_SphereRadius(double c)
{
	int index = dock.comboBox_SphereIndex->currentIndex();
	if (index >= 0)
	{
		m_shader->setClipSphereParamsRadius((float)c, index);
		m_pickableSpheres[index]->scale(Geom::Vec3f((float)c));
		updateGL();
	}
}

void StageShaderReloaded::slot_spinBox_SphereResolution(int i)
{
	m_sphereDrawable->updatePrecisionDrawing(i);
	updateGL();
}

void StageShaderReloaded::slot_doubleSpinBox_SphereGridColor(double c)
{
	float r = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_SphereGridColorR->value();
	float g = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_SphereGridColorG->value();
	float b = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_SphereGridColorB->value();

	m_sphereDrawable->setColor(Geom::Vec4f(r, g, b, 1.0));
	updateGL();
}

void StageShaderReloaded::slot_doubleSpinBox_ColorAttenuationFactor(double c)
{
	m_shader->setClipColorAttenuationFactor((float)c);
	updateGL();
}

void StageShaderReloaded::slot_horizontalSlider_ClippingMode(int i)
{
	if (i == 0)
		m_shader->setClipMode(Utils::ClippingShader::CLIPPING_MODE_AND);
	else if (i == 1)
		m_shader->setClipMode(Utils::ClippingShader::CLIPPING_MODE_OR);

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void StageShaderReloaded::button_compile()
{
	QString st1 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->vertexEdit->toPlainText();
	QString st2 = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->fragmentEdit->toPlainText();

	m_shader->reloadVertexShaderFromMemory(st1.toStdString().c_str());
	m_shader->reloadFragmentShaderFromMemory(st2.toStdString().c_str());

	m_shader->recompile();
	updateGLMatrices();
}

/*******************************************************************************
 *														Init Functions
 *******************************************************************************/

StageShaderReloaded::StageShaderReloaded():
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

void StageShaderReloaded::initGUI()
{
	CGoGNStream::allToConsole(this) ;
	CGoGNStream::allToStd(true) ;

	setDock(&dock);

	dock.setWindowFlags(Qt::WindowMinimizeButtonHint);

	setCallBack(dock.checkBox_vertices, SIGNAL(toggled(bool)), SLOT(slot_drawVertices(bool)));
	setCallBack(dock.checkBox_lines, SIGNAL(toggled(bool)), SLOT(slot_drawLines(bool)));
	setCallBack(dock.checkBox_faces, SIGNAL(toggled(bool)), SLOT(slot_drawFaces(bool)));
	setCallBack(dock.groupBox_topo, SIGNAL(toggled(bool)), SLOT(slot_drawTopo(bool)));

	setCallBack(dock.explod_phi1, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi1(double)));
	setCallBack(dock.explod_phi2, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi2(double)));
	setCallBack(dock.explod_phi3, SIGNAL(valueChanged(double)), SLOT(slot_explodTopoPhi3(double)));

	setCallBack(dock.pushButton_addPlane, SIGNAL(clicked()), SLOT(slot_pushButton_addPlane()));
	setCallBack(dock.pushButton_deletePlane, SIGNAL(clicked()), SLOT(slot_pushButton_deletePlane()));

	setCallBack(dock.comboBox_PlaneIndex, SIGNAL(currentIndexChanged(int)), SLOT(slot_comboBox_PlaneIndexChanged(int)));

	setCallBack(dock.doubleSpinBox_PlaneVec1x, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneVec1(double)));
	setCallBack(dock.doubleSpinBox_PlaneVec1y, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneVec1(double)));
	setCallBack(dock.doubleSpinBox_PlaneVec1z, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneVec1(double)));

	setCallBack(dock.doubleSpinBox_PlaneVec2x, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneVec2(double)));
	setCallBack(dock.doubleSpinBox_PlaneVec2y, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneVec2(double)));
	setCallBack(dock.doubleSpinBox_PlaneVec2z, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneVec2(double)));

	setCallBack(dock.doubleSpinBox_PlaneOriginx, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneOrigin(double)));
	setCallBack(dock.doubleSpinBox_PlaneOriginy, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneOrigin(double)));
	setCallBack(dock.doubleSpinBox_PlaneOriginz, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_PlaneOrigin(double)));

	setCallBack(dock.spinBox_GridResolution, SIGNAL(valueChanged(int)), SLOT(slot_spinBox_GridResolution(int)));
	setCallBack(dock.doubleSpinBox_GridColorR, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_GridColor(double)));
	setCallBack(dock.doubleSpinBox_GridColorG, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_GridColor(double)));
	setCallBack(dock.doubleSpinBox_GridColorB, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_GridColor(double)));

	setCallBack(dock.compileButton, SIGNAL(clicked()), SLOT(button_compile()) );

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	dock.spinBox_GridResolution->setValue(5); // TODO : Utiliser un getteur
	Geom::Vec3f planesCol = Geom::Vec3f(1.0, 0.0, 0.0);  // TODO : utiliser un getteur
	dock.doubleSpinBox_GridColorR->setValue(planesCol[0]);
	dock.doubleSpinBox_GridColorG->setValue(planesCol[1]);
	dock.doubleSpinBox_GridColorB->setValue(planesCol[2]);



	setCallBack(dock.pushButton_addSphere, SIGNAL(clicked()), SLOT(slot_pushButton_addSphere()));
	setCallBack(dock.pushButton_deleteSphere, SIGNAL(clicked()), SLOT(slot_pushButton_deleteSphere()));

	setCallBack(dock.comboBox_SphereIndex, SIGNAL(currentIndexChanged(int)), SLOT(slot_comboBox_SphereIndexChanged(int)));

	setCallBack(dock.doubleSpinBox_SphereCenterx, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_SphereCenter(double)));
	setCallBack(dock.doubleSpinBox_SphereCentery, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_SphereCenter(double)));
	setCallBack(dock.doubleSpinBox_SphereCenterz, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_SphereCenter(double)));

	setCallBack(dock.doubleSpinBox_SphereRadius, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_SphereRadius(double)));

	setCallBack(dock.spinBox_SphereResolution, SIGNAL(valueChanged(int)), SLOT(slot_spinBox_SphereResolution(int)));
	setCallBack(dock.doubleSpinBox_SphereGridColorR, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_SphereGridColor(double)));
	setCallBack(dock.doubleSpinBox_SphereGridColorG, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_SphereGridColor(double)));
	setCallBack(dock.doubleSpinBox_SphereGridColorB, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_SphereGridColor(double)));

	dock.spinBox_SphereResolution->setValue(3); // TODO : utiliser un getteur
	Geom::Vec3f spheresCol = Geom::Vec3f(0.0, 0.4, 1.0); // TODO : utiliser un getteur
	dock.doubleSpinBox_SphereGridColorR->setValue(spheresCol[0]);
	dock.doubleSpinBox_SphereGridColorG->setValue(spheresCol[1]);
	dock.doubleSpinBox_SphereGridColorB->setValue(spheresCol[2]);



	setCallBack(dock.doubleSpinBox_ColorAttenuationFactor, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_ColorAttenuationFactor(double)));
	setCallBack(dock.horizontalSlider_ClippingMode, SIGNAL(valueChanged(int)), SLOT(slot_horizontalSlider_ClippingMode(int)));

	dock.doubleSpinBox_ColorAttenuationFactor->setValue(m_shader->getClipColorAttenuationFactor());
	Utils::ClippingShader::clippingMode clipMode = m_shader->getClipMode();
	if (clipMode == Utils::ClippingShader::CLIPPING_MODE_AND)
		dock.horizontalSlider_ClippingMode->setValue(0);
	else if (clipMode == Utils::ClippingShader::CLIPPING_MODE_OR)
		dock.horizontalSlider_ClippingMode->setValue(1);

}

void StageShaderReloaded::cb_Open()
{
	std::string filters("all (*.*)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;

	importMesh(filename) ;
	updateGL() ;
}

void StageShaderReloaded::importMesh(std::string& filename)
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

	m_bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	gPosObj = m_bb.center() ;
	float tailleX = m_bb.size(0) ;
	float tailleY = m_bb.size(1) ;
	float tailleZ = m_bb.size(2) ;
	gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;

	m_positionVBO->updateData(position) ;

	setParamObject(gWidthObj, gPosObj.data()) ;

	updateGLMatrices();
}

/*******************************************************************************
 *														GL FUNCTIONS
 *******************************************************************************/

void StageShaderReloaded::cb_initGL()
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

	// setup clipping
	m_shader->insertClippingCode();

	// setup clipping shapes
	m_planeDrawable = new Utils::Grid;
	m_planeDrawable->setColor(Geom::Vec4f(1.0, 0.0, 0.0, 1.0));
	m_planeDrawable->updatePrecisionDrawing(5);
	m_sphereDrawable = new Utils::IcoSphere;
	m_sphereDrawable->setColor(Geom::Vec4f(0.0, 0.4, 1.0, 1.0));
	m_sphereDrawable->updatePrecisionDrawing(3);

	// setup clipping picking frame
	m_frameManipulator = new Utils::FrameManipulator();
	m_lastPickedObject = NULL;
}

void StageShaderReloaded::updateVBOprimitives(int upType)
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

void StageShaderReloaded::cb_redraw()
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

	// Display clipping shapes
	for (size_t i = 0; i < m_pickablePlanes.size(); i++)
		m_pickablePlanes[i]->draw();
	for (size_t i = 0; i < m_pickableSpheres.size(); i++)
		m_pickableSpheres[i]->draw();

	// Display picking frame
	if (m_lastPickedObject)
		m_frameManipulator->draw();

}

void StageShaderReloaded::cb_keyPress(int code)
{

}

void StageShaderReloaded::cb_mousePress(int button, int x, int y)
{
	if (!Shift())
		return;

	m_lastClickedX = x;
	m_lastClickedY = y;

	// get ray of selection
	Geom::Vec3f rayA, rayB;
	float dist = getOrthoScreenRay(x, y, rayA, rayB);
	Geom::Vec3f AB = rayB - rayA;


	unsigned int newPickedFrameAxis = 0;
	unsigned int lastPickedFrameAxis = m_frameManipulatorPickedAxis;
	if (m_lastPickedObject)	// an object is already picked ? => frame is drawn
	{
		// picking the frame -> axis
		newPickedFrameAxis = m_frameManipulator->pick(rayA, AB, dist);
		m_frameManipulatorPickedAxis = newPickedFrameAxis;
	}

	// highlighting new axis
	if (lastPickedFrameAxis != newPickedFrameAxis)
		m_frameManipulator->highlight(m_frameManipulatorPickedAxis);

	if (newPickedFrameAxis == 0)	// frame not picked -> pick the pickable objects
	{
		m_lastPickedObject = Utils::Pickable::pick(m_pickablePlanes, rayA, AB);
		// set FrameManipulator on picked object
		if (m_lastPickedObject)
			m_frameManipulator->setTransformation(m_lastPickedObject->transfo());
	}

	// store origin & selected axis on screen projection for easy manipulation.
	m_frameManipulator->storeProjection(m_frameManipulatorPickedAxis);

	updateGL();
}

void StageShaderReloaded::cb_mouseMove(int buttons, int x, int y)
{
	if (!Shift())
		return;

	// rotation selected ?
	if (Utils::FrameManipulator::rotationAxis(m_frameManipulatorPickedAxis))
	{
		if (buttons & 1)
		{
			float angle = m_frameManipulator->angleFromMouse(x, y, x - m_lastClickedX, y - m_lastClickedY);
			m_frameManipulator->rotate(m_frameManipulatorPickedAxis, angle);
		}
		else if (buttons & 2)
			m_frameManipulator->rotateInScreen(x - m_lastClickedX, y - m_lastClickedY);

		m_lastPickedObject->transfo() = m_frameManipulator->transfo();
	}
	// translation selected
	else if (Utils::FrameManipulator::translationAxis(m_frameManipulatorPickedAxis))
	{
		if (buttons & 1)
		{
			float dist =  m_frameManipulator->distanceFromMouse(x - m_lastClickedX, y - m_lastClickedY);
			m_frameManipulator->translate(m_frameManipulatorPickedAxis, dist);
		}
		else if (buttons & 2)
			m_frameManipulator->translateInScreen(x - m_lastClickedX, y - m_lastClickedY);

		m_lastPickedObject->transfo() = m_frameManipulator->transfo();
	}
	// scale selected
	else if (Utils::FrameManipulator::scaleAxis(m_frameManipulatorPickedAxis) )
	{
		float scale = m_frameManipulator->scaleFromMouse(x - m_lastClickedX, y - m_lastClickedY);
		m_frameManipulator->scale(m_frameManipulatorPickedAxis, scale );
		m_lastPickedObject->transfo() = m_frameManipulator->transfo();
	}

	m_lastClickedX = x;
	m_lastClickedY = y;

	updateGL();

}

/**********************************************************************************************
 *                  MAIN FUNCTION                         								      *
 **********************************************************************************************/

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    StageShaderReloaded sqt;
    sqt.setGeometry(0, 0, 1000, 800);
    sqt.show();

	if(argc == 2)
    {
            std::string filename(argv[1]);
            sqt.importMesh(filename);
    }
	else
	{
		sqt.position = sqt.myMap.addAttribute<PFP::VEC3>(VERTEX, "position");
		Algo::Modelisation::Primitive3D<PFP> prim(sqt.myMap, sqt.position);
		prim.hexaGrid_topo(10,10,10);
		prim.embedHexaGrid(1.0f,1.0f,1.0f);
	}

    sqt.initGUI();

    return app.exec();
}
