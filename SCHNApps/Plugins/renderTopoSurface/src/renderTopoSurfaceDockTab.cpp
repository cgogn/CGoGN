#include "renderTopoSurfaceDockTab.h"

#include "renderTopoSurface.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

RenderTopoSurfaceDockTab::RenderTopoSurfaceDockTab(Window* w, RenderTopoSurfacePlugin* p) :
	m_window(w),
	m_plugin(p),
	b_refreshingUI(false)
{
	setupUi(this);

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));
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

void RenderTopoSurfaceDockTab::refreshUI(ParameterSet* params)
{
	m_currentParams = params;

	b_refreshingUI = true;

	mapList->clear();
	combo_positionAttribute->clear();

	MapHandlerGen* mh = params->selectedMap;

	QHash<QString, PerMapParameterSet*>::const_iterator i = params->perMap.constBegin();
	while (i != params->perMap.constEnd())
	{
		mapList->addItem(i.key());
		if(mh != NULL && i.key() == mh->getName())
		{
			QList<QListWidgetItem*> item = mapList->findItems(mh->getName(), Qt::MatchExactly);
			item[0]->setSelected(true);

			PerMapParameterSet* p = params->perMap[mh->getName()];

			QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

			unsigned int j = 0;
			const AttributeHash& attribs = mh->getAttributesList(VERTEX);
			for(AttributeHash::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
			{
				if(i.value() == vec3TypeName)
				{
					combo_positionAttribute->addItem(i.key());
					if(i.key() == QString::fromStdString(p->positionAttribute.name()))
						combo_positionAttribute->setCurrentIndex(j);

					++j;
				}
			}

			check_drawDarts->setChecked(p->drawDarts);
			combo_dartsColor->setColor(p->dartsColor);
			check_drawPhi1->setChecked(p->drawPhi1);
			combo_phi1Color->setColor(p->phi1Color);
			check_drawPhi2->setChecked(p->drawPhi2);
			combo_phi2Color->setColor(p->phi2Color);
			slider_edgesScaleFactor->setSliderPosition(p->edgesScaleFactor * 100.0);
			slider_facesScaleFactor->setSliderPosition(p->facesScaleFactor * 100.0);
		}
		++i;
	}

	b_refreshingUI = false;
}

void RenderTopoSurfaceDockTab::selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = mapList->selectedItems();
		if(!currentItems.empty())
			m_plugin->changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()), true);
	}
}

void RenderTopoSurfaceDockTab::positionAttributeChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changePositionAttribute(view, map, map->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText()), true);
	}
}

void RenderTopoSurfaceDockTab::drawDartsChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeDrawDarts(view, map, b, true);
	}
}

void RenderTopoSurfaceDockTab::dartsColorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeDartsColor(view, map, combo_dartsColor->color(), true);
	}
}

void RenderTopoSurfaceDockTab::drawPhi1Changed(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeDrawPhi1(view, map, b, true);
	}
}

void RenderTopoSurfaceDockTab::phi1ColorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changePhi1Color(view, map, combo_phi1Color->color(), true);
	}
}

void RenderTopoSurfaceDockTab::drawPhi2Changed(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeDrawPhi2(view, map, b, true);
	}
}

void RenderTopoSurfaceDockTab::phi2ColorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changePhi2Color(view, map, combo_phi2Color->color(), true);
	}
}

void RenderTopoSurfaceDockTab::facesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeEdgesScaleFactor(view, map, i, true);
	}
}

void RenderTopoSurfaceDockTab::edgesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeFacesScaleFactor(view, map, i, true);
	}
}

void RenderTopoSurfaceDockTab::addAttributeToList(unsigned int orbit, const QString& nameAttr)
{
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

	const QString& typeAttr = m_currentParams->selectedMap->getAttributeTypeName(orbit, nameAttr);

	if(typeAttr == vec3TypeName)
	{
		combo_positionAttribute->addItem(nameAttr);
	}
}

} // namespace SCHNApps

} // namespace CGoGN
