#include "surface_render_dockTab.h"

#include "surface_render.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_Render_DockTab::Surface_Render_DockTab(SCHNApps* s, Surface_Render_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	m_currentColorDial(0),
	b_updatingUI(false)

{
	setupUi(this);

	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(combo_normalVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(normalVBOChanged(int)));
	connect(combo_colorVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(colorVBOChanged(int)));
	connect(check_renderVertices, SIGNAL(toggled(bool)), this, SLOT(renderVerticesChanged(bool)));
	connect(slider_verticesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(verticesScaleFactorChanged(int)));
	connect(slider_verticesScaleFactor, SIGNAL(sliderPressed()), this, SLOT(verticesScaleFactorPressed()));
	connect(check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(renderEdgesChanged(bool)));
	connect(check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(renderFacesChanged(bool)));
	connect(group_faceShading, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(faceStyleChanged(QAbstractButton*)));
	connect(check_renderBoundary, SIGNAL(toggled(bool)), this, SLOT(renderBoundaryChanged(bool)));
	connect(check_doubleSided, SIGNAL(toggled(bool)), this, SLOT(renderBackfaceChanged(bool)));

	m_colorDial  = new QColorDialog(m_diffuseColor,NULL);
	connect(dcolorButton,SIGNAL(clicked()),this,SLOT(diffuseColorClicked()));
	connect(scolorButton,SIGNAL(clicked()),this,SLOT(simpleColorClicked()));
	connect(vcolorButton,SIGNAL(clicked()),this,SLOT(vertexColorClicked()));
	connect(bfcolorButton, SIGNAL(clicked()), this, SLOT(backColorClicked()));
	connect(bothcolorButton, SIGNAL(clicked()), this, SLOT(bothColorClicked()));
	connect(m_colorDial,SIGNAL(accepted()),this,SLOT(colorSelected()));

}





void Surface_Render_DockTab::positionVBOChanged(int index)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].basePSradius = map->getBBdiagSize() / (2 * std::sqrt(map->getNbOrbits(EDGE)));
			m_plugin->h_viewParameterSet[view][map].positionVBO = map->getVBO(combo_positionVBO->currentText());
			view->updateGL();
			m_plugin->pythonRecording("changePositionVBO", "", view->getName(), map->getName(), combo_positionVBO->currentText());
// TODO write this slot ?			
//			m_plugin->pythonRecording("changeVerticesBaseSize", "",view->getName(), map->getName(), h_viewParameterSet[view][map].basePSradius);
		}
	}
}

void Surface_Render_DockTab::normalVBOChanged(int index)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].normalVBO = map->getVBO(combo_normalVBO->currentText());
			view->updateGL();
			m_plugin->pythonRecording("changeNormalVBO", "", view->getName(), map->getName(), combo_normalVBO->currentText());
		}
	}
}

void Surface_Render_DockTab::colorVBOChanged(int index)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].colorVBO = map->getVBO(combo_colorVBO->currentText());
			view->updateGL();
			m_plugin->pythonRecording("changeColorVBO", "", view->getName(), map->getName(), combo_colorVBO->currentText());
		}
	}
}

void Surface_Render_DockTab::renderVerticesChanged(bool b)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			if (b)
				m_plugin->h_viewParameterSet[view][map].basePSradius = map->getBBdiagSize() / (2 * std::sqrt(map->getNbOrbits(EDGE)));

			m_plugin->h_viewParameterSet[view][map].renderVertices = b;
			view->updateGL();
			m_plugin->pythonRecording("changeRenderVertices", "", view->getName(), map->getName(), b);
		}
	}
}


void Surface_Render_DockTab::verticesScaleFactorPressed()
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].basePSradius = map->getBBdiagSize() / (2 * std::sqrt(map->getNbOrbits(EDGE)));
		}
	}
}


void Surface_Render_DockTab::verticesScaleFactorChanged(int i)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].verticesScaleFactor = i / 50.0; 
			view->updateGL();
			m_plugin->pythonRecording("changeVerticesScaleFactor", "", view->getName(), map->getName(), i / 50.0);
		}
	}
}

void Surface_Render_DockTab::renderEdgesChanged(bool b)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].renderEdges = b;
			view->updateGL();
			m_plugin->pythonRecording("changeRenderEdges", "", view->getName(), map->getName(), b);
		}
	}
}

void Surface_Render_DockTab::renderFacesChanged(bool b)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].renderFaces = b;
			view->updateGL();
			m_plugin->pythonRecording("changeRenderFaces", "", view->getName(), map->getName(), b);
		}
	}
}

void Surface_Render_DockTab::faceStyleChanged(QAbstractButton* b)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			if (radio_flatShading->isChecked())
				m_plugin->h_viewParameterSet[view][map].faceStyle = MapParameters::FLAT;
			else if (radio_phongShading->isChecked())
				m_plugin->h_viewParameterSet[view][map].faceStyle = MapParameters::PHONG;
			view->updateGL();
			m_plugin->pythonRecording("changeFacesStyle", "", view->getName(), map->getName(), m_plugin->h_viewParameterSet[view][map].faceStyle);
		}
	}
}

void Surface_Render_DockTab::renderBoundaryChanged(bool b)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].renderBoundary = b;
			view->updateGL();
			m_plugin->pythonRecording("changeRenderBoundary", "", view->getName(), map->getName(), b);
		}
	}
}



void Surface_Render_DockTab::renderBackfaceChanged(bool b)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].renderBackfaces = b;
			view->updateGL();
			m_plugin->pythonRecording("changeRenderBackfaces", "", view->getName(), map->getName(), b);
		}
	}
}

void Surface_Render_DockTab::diffuseColorClicked()
{
	m_currentColorDial = 1;
	m_colorDial->show();
	m_colorDial->setCurrentColor(m_diffuseColor);
}

void Surface_Render_DockTab::simpleColorClicked()
{
	m_currentColorDial = 2;
	m_colorDial->show();
	m_colorDial->setCurrentColor(m_simpleColor);
}

void Surface_Render_DockTab::vertexColorClicked()
{
	m_currentColorDial = 3;
	m_colorDial->show();
	m_colorDial->setCurrentColor(m_vertexColor);
}

void Surface_Render_DockTab::backColorClicked()
{
	m_currentColorDial = 4;
	m_colorDial->show();
	m_colorDial->setCurrentColor(m_backColor);
}

void Surface_Render_DockTab::bothColorClicked()
{
	m_currentColorDial = 5;
	m_colorDial->show();
	m_colorDial->setCurrentColor(m_diffuseColor);

}


void Surface_Render_DockTab::colorSelected()
{
	QColor col = m_colorDial->currentColor();
	if (m_currentColorDial == 1)
	{
		m_diffuseColor = col;
		dcolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");
		bothcolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");

		Geom::Vec4f rgbCol(1.0/255.0*m_diffuseColor.red(), 1.0/255.0*m_diffuseColor.green(),1.0/255.0*m_diffuseColor.blue(),0.0f);

		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].diffuseColor = rgbCol;
			view->updateGL();
			m_plugin->pythonRecording("changeFaceColor", "", view->getName(), map->getName(), rgbCol[0], rgbCol[1], rgbCol[2]);
		}
	}

	if (m_currentColorDial == 2)
	{
		m_simpleColor = col;
		scolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");

		Geom::Vec4f rgbCol(1.0/255.0*m_simpleColor.red(), 1.0/255.0*m_simpleColor.green(),1.0/255.0*m_simpleColor.blue(),0.0f);

		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].simpleColor = rgbCol;
			view->updateGL();
			m_plugin->pythonRecording("changeEdgeColor", "", view->getName(), map->getName(), rgbCol[0], rgbCol[1], rgbCol[2]);
		}
	}

	if (m_currentColorDial == 3)
	{
		m_vertexColor = col;
		vcolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");

		Geom::Vec4f rgbCol(1.0/255.0*m_vertexColor.red(), 1.0/255.0*m_vertexColor.green(),1.0/255.0*m_vertexColor.blue(),0.0f);

		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].vertexColor = rgbCol;
			view->updateGL();
			m_plugin->pythonRecording("changeVertexColor", "", view->getName(), map->getName(), rgbCol[0], rgbCol[1], rgbCol[2]);
		}
	}

	if (m_currentColorDial == 4)
	{
		m_backColor = col;
		bfcolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");

		Geom::Vec4f rgbCol(1.0 / 255.0*m_backColor.red(), 1.0 / 255.0*m_backColor.green(), 1.0 / 255.0*m_backColor.blue(), 0.0f);

		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].backColor = rgbCol;
			view->updateGL();
			m_plugin->pythonRecording("changeBackColor", "", view->getName(), map->getName(), rgbCol[0], rgbCol[1], rgbCol[2]);
		}
	}

	if (m_currentColorDial == 5)
	{
		m_backColor = col;
		bfcolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");

		m_diffuseColor = col;
		dcolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");

		bothcolorButton->setStyleSheet("QPushButton { background-color:" + col.name() + "}");

		Geom::Vec4f rgbCol(1.0 / 255.0*m_backColor.red(), 1.0 / 255.0*m_backColor.green(), 1.0 / 255.0*m_backColor.blue(), 0.0f);

		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			m_plugin->h_viewParameterSet[view][map].backColor = rgbCol;
			m_plugin->h_viewParameterSet[view][map].diffuseColor = rgbCol;
			view->updateGL();
			m_plugin->pythonRecording("changeBackColor", "", view->getName(), map->getName(), rgbCol[0], rgbCol[1], rgbCol[2]);
			m_plugin->pythonRecording("changeFaceColor", "", view->getName(), map->getName(), rgbCol[0], rgbCol[1], rgbCol[2]);
		}
	}
}





void Surface_Render_DockTab::addPositionVBO(QString name)
{
	b_updatingUI = true;
	combo_positionVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_Render_DockTab::removePositionVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_positionVBO->currentIndex();
	int index = combo_positionVBO->findText(name, Qt::MatchExactly);
	if (curIndex == index)
		combo_positionVBO->setCurrentIndex(0);
	combo_positionVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_Render_DockTab::addNormalVBO(QString name)
{
	b_updatingUI = true;
	combo_normalVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_Render_DockTab::removeNormalVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_normalVBO->currentIndex();
	int index = combo_normalVBO->findText(name, Qt::MatchExactly);
	if (curIndex == index)
		combo_normalVBO->setCurrentIndex(0);
	combo_normalVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_Render_DockTab::addColorVBO(QString name)
{
	b_updatingUI = true;
	combo_colorVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_Render_DockTab::removeColorVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_colorVBO->currentIndex();
	int index = combo_colorVBO->findText(name, Qt::MatchExactly);
	if (curIndex == index)
		combo_colorVBO->setCurrentIndex(0);
	combo_colorVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_Render_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_positionVBO->addItem("- select VBO -");

	combo_normalVBO->clear();
	combo_normalVBO->addItem("- select VBO -");

	combo_colorVBO->clear();
	combo_colorVBO->addItem("- select VBO -");

	View* view = m_schnapps->getSelectedView();
	MapHandlerGen* map = m_schnapps->getSelectedMap();

	if (view && map)
	{
		const MapParameters& p = m_plugin->h_viewParameterSet[view][map];

		unsigned int i = 1;
		foreach(Utils::VBO* vbo, map->getVBOSet().values())
		{
			if (vbo->dataSize() == 3)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbo->name()));
				if (vbo == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);

				combo_normalVBO->addItem(QString::fromStdString(vbo->name()));
				if (vbo == p.normalVBO)
					combo_normalVBO->setCurrentIndex(i);

				combo_colorVBO->addItem(QString::fromStdString(vbo->name()));
				if (vbo == p.colorVBO)
					combo_colorVBO->setCurrentIndex(i);

				++i;
			}
		}

		check_renderVertices->setChecked(p.renderVertices);
		slider_verticesScaleFactor->setSliderPosition(p.verticesScaleFactor * 50.0);
		check_renderEdges->setChecked(p.renderEdges);
		check_renderFaces->setChecked(p.renderFaces);
		radio_flatShading->setChecked(p.faceStyle == MapParameters::FLAT);
		radio_phongShading->setChecked(p.faceStyle == MapParameters::PHONG);

		m_diffuseColor = QColor(255 * p.diffuseColor[0], 255 * p.diffuseColor[1], 255 * p.diffuseColor[2]);
		dcolorButton->setStyleSheet("QPushButton { background-color:" + m_diffuseColor.name() + " }");
		bothcolorButton->setStyleSheet("QPushButton { background-color:" + m_diffuseColor.name() + "}");

		m_simpleColor = QColor(255 * p.simpleColor[0], 255 * p.simpleColor[1], 255 * p.simpleColor[2]);
		scolorButton->setStyleSheet("QPushButton { background-color:" + m_simpleColor.name() + " }");

		m_vertexColor = QColor(255 * p.vertexColor[0], 255 * p.vertexColor[1], 255 * p.vertexColor[2]);
		vcolorButton->setStyleSheet("QPushButton { background-color:" + m_vertexColor.name() + " }");

		m_backColor = QColor(255 * p.backColor[0], 255 * p.backColor[1], 255 * p.backColor[2]);
		bfcolorButton->setStyleSheet("QPushButton { background-color:" + m_backColor.name() + " }");
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
