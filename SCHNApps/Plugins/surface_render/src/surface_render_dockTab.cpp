#include "surface_render_dockTab.h"

#include "surface_render.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

SurfaceRender_DockTab::SurfaceRender_DockTab(SCHNApps* s, SurfaceRenderPlugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(combo_normalVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(normalVBOChanged(int)));
	connect(check_renderVertices, SIGNAL(toggled(bool)), this, SLOT(renderVerticesChanged(bool)));
	connect(slider_verticesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(verticesScaleFactorChanged(int)));
	connect(check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(renderEdgesChanged(bool)));
	connect(check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(renderFacesChanged(bool)));
	connect(group_faceShading, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(faceStyleChanged(QAbstractButton*)));
}

void SurfaceRender_DockTab::positionVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].positionVBO = map->getVBO(combo_positionVBO->currentText());
			view->updateGL();
		}
	}
}

void SurfaceRender_DockTab::normalVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].normalVBO = map->getVBO(combo_normalVBO->currentText());
			view->updateGL();
		}
	}
}

void SurfaceRender_DockTab::renderVerticesChanged(bool b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].renderVertices = b;
			view->updateGL();
		}
	}
}

void SurfaceRender_DockTab::verticesScaleFactorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].verticesScaleFactor = i / 50.0;
			view->updateGL();
		}
	}
}

void SurfaceRender_DockTab::renderEdgesChanged(bool b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].renderEdges = b;
			view->updateGL();
		}
	}
}

void SurfaceRender_DockTab::renderFacesChanged(bool b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].renderFaces = b;
			view->updateGL();
		}
	}
}

void SurfaceRender_DockTab::faceStyleChanged(QAbstractButton* b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			if(radio_flatShading->isChecked())
				m_plugin->h_viewParameterSet[view][map].faceStyle = MapParameters::FLAT;
			else if(radio_phongShading->isChecked())
				m_plugin->h_viewParameterSet[view][map].faceStyle = MapParameters::PHONG;
			view->updateGL();
		}
	}
}





void SurfaceRender_DockTab::addPositionVBO(QString name)
{
	b_updatingUI = true;
	combo_positionVBO->addItem(name);
	b_updatingUI = false;
}

void SurfaceRender_DockTab::removePositionVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_positionVBO->currentIndex();
	int index = combo_positionVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_positionVBO->setCurrentIndex(0);
	combo_positionVBO->removeItem(index);
	b_updatingUI = false;
}

void SurfaceRender_DockTab::addNormalVBO(QString name)
{
	b_updatingUI = true;
	combo_normalVBO->addItem(name);
	b_updatingUI = false;
}

void SurfaceRender_DockTab::removeNormalVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_normalVBO->currentIndex();
	int index = combo_normalVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_normalVBO->setCurrentIndex(0);
	combo_normalVBO->removeItem(index);
	b_updatingUI = false;
}

void SurfaceRender_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_normalVBO->clear();

	combo_positionVBO->addItem("- select VBO -");
	combo_normalVBO->addItem("- select VBO -");

	View* view = m_schnapps->getSelectedView();
	MapHandlerGen* map = m_schnapps->getSelectedMap();

	if(view && map)
	{
		const MapParameters& p = m_plugin->h_viewParameterSet[view][map];

		unsigned int i = 1;
		foreach(Utils::VBO* vbo, map->getVBOSet().values())
		{
			if(vbo->dataSize() == 3)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);

				combo_normalVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.normalVBO)
					combo_normalVBO->setCurrentIndex(i);

				++i;
			}
		}

		check_renderVertices->setChecked(p.renderVertices);
		slider_verticesScaleFactor->setSliderPosition(p.verticesScaleFactor * 50.0);
		check_renderEdges->setChecked(p.renderEdges);
		check_renderFaces->setChecked(p.renderFaces);
		radio_flatShading->setChecked(p.faceStyle == MapParameters::FLAT);
		radio_phongShading->setChecked(p.faceStyle == MapParameters::PHONG);
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
