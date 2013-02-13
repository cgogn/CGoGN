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
	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(combo_colorVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(colorVBOChanged(int)));
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
	combo_positionVBO->clear();
	combo_colorVBO->clear();

	MapHandlerGen* map = params->selectedMap;

	QHash<QString, PerMapParameterSet>::const_iterator i = params->perMap.constBegin();
	while (i != params->perMap.constEnd())
	{
		mapList->addItem(i.key());
		if(map != NULL && i.key() == map->getName())
		{
			QList<QListWidgetItem*> item = mapList->findItems(map->getName(), Qt::MatchExactly);
			item[0]->setSelected(true);

			PerMapParameterSet& p = params->perMap[map->getName()];

			QList<Utils::VBO*> vbos = map->getVBOList();
			unsigned int j = 0;
			for(int i = 0; i < vbos.count(); ++i)
			{
				if(vbos[i]->dataSize() == 3)
				{
					combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(vbos[i] == p.positionVBO)
						combo_positionVBO->setCurrentIndex(j);

					combo_colorVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(vbos[i] == p.colorVBO)
						combo_colorVBO->setCurrentIndex(j);

					++j;
				}
			}

			check_renderEdges->setChecked(p.renderEdges);
			check_renderFaces->setChecked(p.renderFaces);
			slider_facesScaleFactor->setSliderPosition(p.facesScaleFactor * 50.0);
			slider_volumesScaleFactor->setSliderPosition(p.volumesScaleFactor * 50.0);
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

void RenderExplodDockTab::positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changePositionVBO(view, map, map->getVBO(combo_positionVBO->currentText()));
	}
}

void RenderExplodDockTab::colorVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeColorVBO(view, map, map->getVBO(combo_colorVBO->currentText()));
	}
}

void RenderExplodDockTab::renderEdgesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeRenderEdges(view, map, b);
	}
}

void RenderExplodDockTab::renderFacesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeRenderFaces(view, map, b);
	}
}

void RenderExplodDockTab::facesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeFacesScaleFactor(view, map, i);
	}
}

void RenderExplodDockTab::volumesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeVolumesScaleFactor(view, map, i);
	}
}

} // namespace SCHNApps

} // namespace CGoGN
