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

#include "clipping.h"
#include "Utils/static_assert.h"

/*******************************************************************************
 *														SLOTS
 *******************************************************************************/

void Clipping::slot_drawTopo(bool b)
{
	m_drawTopo = b;
	updateGL();
}

void Clipping::slot_drawVertices(bool b)
{
	m_drawVertices = b;
	updateGL();
}

void Clipping::slot_drawLines(bool b)
{
	m_drawLines = b;
	updateGL();
}

void Clipping::slot_drawFaces(bool b)
{
	m_drawFaces = b;
	updateGL();
}

void Clipping::slot_explodTopoPhi1(double c)
{
	m_coeffTopoExplod[0] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void Clipping::slot_explodTopoPhi2(double c)
{
	m_coeffTopoExplod[1] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void Clipping::slot_explodTopoPhi3(double c)
{
	m_coeffTopoExplod[2] = (float)c;
	m_render_topo->updateData<PFP>(myMap, allDarts, position, m_coeffTopoExplod[0], m_coeffTopoExplod[1], m_coeffTopoExplod[2]);
	updateGL();
}

void Clipping::slot_pushButton_addPlane()
{
	// Create clipping and pickable objects
	int newPlaneId = m_shader->addClipPlane();
	Utils::Pickable* pickable = new Utils::Pickable(m_planeDrawable, newPlaneId);
	m_pickablePlanes.push_back(pickable);

	// Set params
	m_shader->setClipPlaneParamsOrigin(newPlaneId, m_bb.center());
	pickable->translate(m_bb.center());
	pickable->scale(Geom::Vec3f(m_bb.maxSize()*0.5));

	// Update shader sources edits
	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_spinBox_GridResolution(int i)
{
	m_planeDrawable->updatePrecisionDrawing(i);
	updateGL();
}

void Clipping::slot_pushButton_changePlanesColor()
{
	QColor NewColor;
	QColorDialog ColDialog;

	NewColor = ColDialog.getColor();

	int r, g, b;
	NewColor.getRgb(&r, &g, &b);

	m_planeDrawable->setColor(Geom::Vec4f((float)r/255.0, (float)g/255.0, (float)b/255.0, 1.0));
}

void Clipping::slot_pushButton_addSphere()
{
	// Create clipping and pickable objects
	int newSphereId = m_shader->addClipSphere();
	Utils::Pickable* pickable = new Utils::Pickable(m_sphereDrawable, newSphereId);
	m_pickableSpheres.push_back(pickable);

	// Set params
	m_shader->setClipSphereParamsCenter(newSphereId, m_bb.center());
	m_shader->setClipSphereParamsRadius(newSphereId, (m_bb.maxSize())*0.5);
	pickable->translate(m_bb.center());
	pickable->scale(Geom::Vec3f(m_bb.maxSize()*0.5));

	// Update shader sources edits
	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_spinBox_SphereResolution(int i)
{
	m_sphereDrawable->updatePrecisionDrawing(i);
	updateGL();
}

void Clipping::slot_pushButton_changeSpheresColor()
{
	QColor NewColor;
	QColorDialog ColDialog;

	NewColor = ColDialog.getColor();

	int r, g, b;
	NewColor.getRgb(&r, &g, &b);

	m_sphereDrawable->setColor(Geom::Vec4f((float)r/255.0, (float)g/255.0, (float)b/255.0, 1.0));
}

void Clipping::slot_doubleSpinBox_ColorAttenuationFactor(double c)
{
	m_shader->setClipColorAttenuationFactorAbsolute((float)c);
	updateGL();
}

void Clipping::slot_radioButton_ClippingMode(bool b)
{
	if (b)
		m_shader->setClipMode(Utils::ClippingShader::CLIPPING_MODE_AND);
	else
		m_shader->setClipMode(Utils::ClippingShader::CLIPPING_MODE_OR);

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_radioButton_ColorAttenuationMode(bool b)
{
	if (b)
		m_shader->setClipColorAttenuationMode(Utils::ClippingShader::COLOR_ATTENUATION_MODE_LINEAR);
	else
		m_shader->setClipColorAttenuationMode(Utils::ClippingShader::COLOR_ATTENUATION_MODE_QUADRATIC);

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	updateGLMatrices();
}

void Clipping::slot_pushButton_deleteSelectedObject()
{
	if (m_lastPickedObject)
	{
		// Delete clipping object
		if (m_lastPickedObject->checkType<Utils::Grid>())
			m_shader->deleteClipPlane(m_lastPickedObject->id());
		else if (m_lastPickedObject->checkType<Utils::Sphere>())
			m_shader->deleteClipSphere(m_lastPickedObject->id());

		// Delete pickable
		if (m_lastPickedObject->checkType<Utils::Grid>())
		{
			for (size_t i = 0; i < m_pickablePlanes.size(); i++)
			{
				if (m_pickablePlanes[i] == m_lastPickedObject)
				{
					delete m_pickablePlanes[i];
					m_pickablePlanes.erase(m_pickablePlanes.begin() + i);
				}
			}
		}
		else if (m_lastPickedObject->checkType<Utils::Sphere>())
		{
			for (size_t i = 0; i < m_pickableSpheres.size(); i++)
			{
				if (m_pickableSpheres[i] == m_lastPickedObject)
				{
					delete m_pickableSpheres[i];
					m_pickableSpheres.erase(m_pickableSpheres.begin() + i);
				}
			}
		}
		m_lastPickedObject = NULL;

		// Update shader sources edits
		dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
		dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

		updateGLMatrices();

	}
}

void Clipping::button_compile()
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

Clipping::Clipping():
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

void Clipping::initGUI()
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

	setCallBack(dock.spinBox_GridResolution, SIGNAL(valueChanged(int)), SLOT(slot_spinBox_GridResolution(int)));
	setCallBack(dock.pushButton_changePlanesColor, SIGNAL(clicked()), SLOT(slot_pushButton_changePlanesColor()));

	setCallBack(dock.compileButton, SIGNAL(clicked()), SLOT(button_compile()));

	dock.vertexEdit->setPlainText(QString(m_shader->getVertexShaderSrc()));
	dock.fragmentEdit->setPlainText(QString(m_shader->getFragmentShaderSrc()));

	unsigned int planesPrecision1, planesPrecision2;
	m_planeDrawable->getPrecisionDrawing(planesPrecision1, planesPrecision2);
	dock.spinBox_GridResolution->setValue(planesPrecision1);



	setCallBack(dock.pushButton_addSphere, SIGNAL(clicked()), SLOT(slot_pushButton_addSphere()));

	setCallBack(dock.spinBox_SphereResolution, SIGNAL(valueChanged(int)), SLOT(slot_spinBox_SphereResolution(int)));
	setCallBack(dock.pushButton_changeSpheresColor, SIGNAL(clicked()), SLOT(slot_pushButton_changeSpheresColor()));

	unsigned int spheresPrecision1, spheresPrecision2;
	m_sphereDrawable->getPrecisionDrawing(spheresPrecision1, spheresPrecision2);
	dock.spinBox_SphereResolution->setValue(spheresPrecision1);



	setCallBack(dock.doubleSpinBox_ColorAttenuationFactor, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_ColorAttenuationFactor(double)));
	setCallBack(dock.radioButton_ClippingModeAnd, SIGNAL(toggled(bool)), SLOT(slot_radioButton_ClippingMode(bool)));
	setCallBack(dock.radioButton_ColorAttenuationModeLinear, SIGNAL(toggled(bool)), SLOT(slot_radioButton_ColorAttenuationMode(bool)));

	dock.doubleSpinBox_ColorAttenuationFactor->setValue(m_shader->getClipColorAttenuationFactor());
	Utils::ClippingShader::clippingMode clipMode = m_shader->getClipMode();
	if (clipMode == Utils::ClippingShader::CLIPPING_MODE_AND)
		dock.radioButton_ClippingModeAnd->setChecked(true);
	else if (clipMode == Utils::ClippingShader::CLIPPING_MODE_OR)
		dock.radioButton_ClippingModeOr->setChecked(true);
	Utils::ClippingShader::colorAttenuationMode colorAttMode = m_shader->getClipColorAttenuationMode();
	if (colorAttMode == Utils::ClippingShader::COLOR_ATTENUATION_MODE_LINEAR)
		dock.radioButton_ColorAttenuationModeLinear->setChecked(true);
	else if (colorAttMode == Utils::ClippingShader::COLOR_ATTENUATION_MODE_QUADRATIC)
		dock.radioButton_ColorAttenuationModeQuadratic->setChecked(true);

}

void Clipping::cb_Open()
{
	std::string filters("all (*.*)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;

	importMesh(filename) ;
	updateGL() ;
}

void Clipping::importMesh(std::string& filename)
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

	m_shader->setClipColorAttenuationFactorRelative(m_bb.maxSize(), 5.0);
}

/*******************************************************************************
 *														GL FUNCTIONS
 *******************************************************************************/

void Clipping::cb_initGL()
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
	m_sphereDrawable->updatePrecisionDrawing(1);

	// setup clipping picking frame
	m_frameManipulator = new Utils::FrameManipulator();
	m_lastPickedObject = NULL;
}

void Clipping::updateVBOprimitives(int upType)
{
	if(upType & Algo::Render::GL2::TRIANGLES)
		m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES) ;

	if(upType & Algo::Render::GL2::LINES)
		m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES,false) ;

	if(upType & Algo::Render::GL2::POINTS)
		m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS) ;
}

void Clipping::cb_redraw()
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
	{
		if (m_pickablePlanes[i] == m_lastPickedObject)
			glLineWidth(5.0);
		else
			glLineWidth(2.0);
		m_pickablePlanes[i]->draw();
	}
	for (size_t i = 0; i < m_pickableSpheres.size(); i++)
	{
		if (m_pickableSpheres[i] == m_lastPickedObject)
			glLineWidth(5.0);
		else
			glLineWidth(2.0);
		m_pickableSpheres[i]->draw();
	}

	// Display picking frame
	if (m_lastPickedObject)
		m_frameManipulator->draw();

}

void Clipping::cb_keyPress(int code)
{
	switch (code)
	{
		case 16777223 : // Suppr
			slot_pushButton_deleteSelectedObject();
			break;
	}
}

void Clipping::cb_mousePress(int button, int x, int y)
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

		// highlight new axis
		if (lastPickedFrameAxis != newPickedFrameAxis)
			m_frameManipulator->highlight(m_frameManipulatorPickedAxis);
	}

	if (newPickedFrameAxis == 0)	// frame not picked -> pick the pickable objects
	{
		std::vector<Utils::Pickable*> tempo = m_pickablePlanes;
		tempo.insert(tempo.end(), m_pickableSpheres.begin(), m_pickableSpheres.end());
		m_lastPickedObject = Utils::Pickable::pick(tempo, rayA, AB);

		// set FrameManipulator on picked object
		if (m_lastPickedObject)
		{
			m_frameManipulator->setTransformation(m_lastPickedObject->transfo());

			// Lock individual scale axes for the sphere
			if (m_lastPickedObject->checkType<Utils::Grid>())
				m_frameManipulator->unlock(Utils::FrameManipulator::Scales);
			else if (m_lastPickedObject->checkType<Utils::Sphere>())
				m_frameManipulator->lock(Utils::FrameManipulator::Scales);
		}
	}

	// store origin & selected axis on screen projection for easy manipulation.
	m_frameManipulator->storeProjection(m_frameManipulatorPickedAxis);

	updateGL();
}

void Clipping::cb_mouseMove(int buttons, int x, int y)
{
	if (!Shift())
		return;

	bool clippingUpdateNeeded = false;

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

		clippingUpdateNeeded = true;
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

		clippingUpdateNeeded = true;
	}
	// scale selected
	else if (Utils::FrameManipulator::scaleAxis(m_frameManipulatorPickedAxis) )
	{
		float scale = m_frameManipulator->scaleFromMouse(x - m_lastClickedX, y - m_lastClickedY);
		m_frameManipulator->scale(m_frameManipulatorPickedAxis, scale );
		m_lastPickedObject->transfo() = m_frameManipulator->transfo();

		clippingUpdateNeeded = true;
	}

	// Update clipping shape if needed
	if (clippingUpdateNeeded && m_lastPickedObject)
	{
		if (m_lastPickedObject->checkType<Utils::Grid>())
		{
			// Get plane position and normal
			Geom::Vec3f planeNewPos = m_lastPickedObject->getPosition();
			float planeNewAxisScale; // Not used
			Geom::Vec3f planeNewNormal = m_lastPickedObject->getAxisScale(2, planeNewAxisScale); // 2 = Z axis = plane normal

			// Update clipping shape
			m_shader->setClipPlaneParamsAll(m_lastPickedObject->id(), planeNewNormal, planeNewPos);
		}
		else if (m_lastPickedObject->checkType<Utils::Sphere>())
		{
			// Get sphere position and size
			Geom::Vec3f sphereNewPos = m_lastPickedObject->getPosition();
			float sphereNewSize;
			m_lastPickedObject->getAxisScale(0, sphereNewSize); // On the sphere, every axis has the same size, so take 0 as default

			// Update clipping shape
			m_shader->setClipSphereParamsAll(m_lastPickedObject->id(), sphereNewPos, sphereNewSize);
		}
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

    Clipping sqt;
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
