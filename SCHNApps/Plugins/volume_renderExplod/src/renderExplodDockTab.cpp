#include "renderExplodDockTab.h"

#include "renderExplod.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

RenderExplodDockTab::RenderExplodDockTab(Window* w, RenderExplodPlugin* p) :
	m_window(w),
	m_plugin(p),
	b_refreshingUI(false)
{
	setupUi(this);

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));

	connect(combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(positionAttributeChanged(int)));
	connect(combo_colorAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(colorAttributeChanged(int)));

	connect(check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(renderEdgesChanged(bool)));
	connect(check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(renderFacesChanged(bool)));

	connect(slider_facesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(facesScaleFactorChanged(int)));
	connect(slider_volumesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(volumesScaleFactorChanged(int)));
}

void RenderExplodDockTab::refreshUI(ParameterSet* params)
{
	m_currentParams = params;

	b_refreshingUI = true;

	mapList->clear();
	combo_positionAttribute->clear();
	combo_colorAttribute->clear();

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

//					combo_colorAttribute->addItem(i.key());
//					if(i.key() == QString::fromStdString(p.colorAttribute.name()))
//						combo_colorAttribute->setCurrentIndex(j);

					++j;
				}
			}

			check_renderEdges->setChecked(p->renderEdges);
			check_renderFaces->setChecked(p->renderFaces);
			slider_facesScaleFactor->setSliderPosition(p->facesScaleFactor * 50.0);
			slider_volumesScaleFactor->setSliderPosition(p->volumesScaleFactor * 50.0);
		}
		++i;
	}

	b_refreshingUI = false;
}

void RenderExplodDockTab::selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = mapList->selectedItems();
		if(!currentItems.empty())
			m_plugin->changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderExplodDockTab::positionAttributeChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changePositionAttribute(view, map, map->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText()), true);
	}
}

void RenderExplodDockTab::colorAttributeChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeColorAttribute(view, map, map->getAttribute<PFP2::VEC3, VERTEX>(combo_colorAttribute->currentText()), true);
	}
}

void RenderExplodDockTab::renderEdgesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeRenderEdges(view, map, b, true);
	}
}

void RenderExplodDockTab::renderFacesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeRenderFaces(view, map, b, true);
	}
}

void RenderExplodDockTab::facesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeFacesScaleFactor(view, map, i, true);
	}
}

void RenderExplodDockTab::volumesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeVolumesScaleFactor(view, map, i, true);
	}
}

void RenderExplodDockTab::addAttributeToList(unsigned int orbit, const QString& nameAttr)
{
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

	const QString& typeAttr = m_currentParams->selectedMap->getAttributeTypeName(orbit, nameAttr);

	if(typeAttr == vec3TypeName)
	{
		combo_positionAttribute->addItem(nameAttr);
		combo_colorAttribute->addItem(nameAttr);
	}
}

} // namespace SCHNApps

} // namespace CGoGN
