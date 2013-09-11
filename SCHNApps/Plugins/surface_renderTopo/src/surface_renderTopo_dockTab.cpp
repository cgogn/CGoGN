#include "surface_renderTopo_dockTab.h"

#include "surface_renderTopo.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_RenderTopo_DockTab::Surface_RenderTopo_DockTab(SCHNApps* s, Surface_RenderTopo_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	connect(combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(positionAttributeChanged(int)));
	connect(check_drawDarts, SIGNAL(toggled(bool)), this, SLOT(drawDartsChanged(bool)));
	connect(combo_dartsColor, SIGNAL(currentIndexChanged(int)), this, SLOT(dartsColorChanged(int)));
	connect(check_drawPhi1, SIGNAL(toggled(bool)), this, SLOT(drawPhi1Changed(bool)));
	connect(combo_phi1Color, SIGNAL(currentIndexChanged(int)), this, SLOT(phi1ColorChanged(int)));
	connect(check_drawPhi2, SIGNAL(toggled(bool)), this, SLOT(drawPhi2Changed(bool)));
	connect(combo_phi2Color, SIGNAL(currentIndexChanged(int)), this, SLOT(phi2ColorChanged(int)));
	connect(slider_edgesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(edgesScaleFactorChanged(int)));
	connect(slider_facesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(facesScaleFactorChanged(int)));
}





void Surface_RenderTopo_DockTab::positionAttributeChanged(int index)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].positionAttribute = map->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText());
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::drawDartsChanged(bool b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].drawDarts = b;
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::dartsColorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].dartsColor = combo_dartsColor->color();
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::drawPhi1Changed(bool b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].drawPhi1 = b;
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::phi1ColorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].phi1Color = combo_phi1Color->color();
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::drawPhi2Changed(bool b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].drawPhi2 = b;
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::phi2ColorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].phi2Color = combo_phi2Color->color();
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::facesScaleFactorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].facesScaleFactor = i / 100.0f;
			view->updateGL();
		}
	}
}

void Surface_RenderTopo_DockTab::edgesScaleFactorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].edgesScaleFactor = i / 100.0f;
			view->updateGL();
		}
	}
}





void Surface_RenderTopo_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionAttribute->clear();
	combo_positionAttribute->addItem("- select attribute -");

	View* view = m_schnapps->getSelectedView();
	MapHandlerGen* map = m_schnapps->getSelectedMap();

	if(view && map)
	{
		const MapParameters& p = m_plugin->h_viewParameterSet[view][map];

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

		unsigned int i = 1;
		const AttributeSet& attribs = map->getAttributeSet(VERTEX);
		for(AttributeSet::const_iterator it = attribs.constBegin(); it != attribs.constEnd(); ++it)
		{
			if(it.value() == vec3TypeName)
			{
				combo_positionAttribute->addItem(it.key());
				if(it.key() == QString::fromStdString(p.positionAttribute.name()))
					combo_positionAttribute->setCurrentIndex(i);

				++i;
			}
		}

		check_drawDarts->setChecked(p.drawDarts);
		combo_dartsColor->setColor(p.dartsColor);
		check_drawPhi1->setChecked(p.drawPhi1);
		combo_phi1Color->setColor(p.phi1Color);
		check_drawPhi2->setChecked(p.drawPhi2);
		combo_phi2Color->setColor(p.phi2Color);
		slider_edgesScaleFactor->setSliderPosition(p.edgesScaleFactor * 100.0);
		slider_facesScaleFactor->setSliderPosition(p.facesScaleFactor * 100.0);
	}

	b_updatingUI = false;
}

void Surface_RenderTopo_DockTab::addAttributeToList(unsigned int orbit, const QString& nameAttr)
{
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
	const QString& typeAttr = m_schnapps->getSelectedMap()->getAttributeTypeName(orbit, nameAttr);
	if(typeAttr == vec3TypeName)
		combo_positionAttribute->addItem(nameAttr);
}

} // namespace SCHNApps

} // namespace CGoGN
