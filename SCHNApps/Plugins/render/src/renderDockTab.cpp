#include "renderDockTab.h"

#include "render.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

RenderDockTab::RenderDockTab(Window* w, RenderPlugin* p) :
	m_window(w),
	m_plugin(p),
	b_refreshingUI(false)
{
	setupUi(this);

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));
	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(combo_normalVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(normalVBOChanged(int)));
	connect(check_renderVertices, SIGNAL(toggled(bool)), this, SLOT(renderVerticesChanged(bool)));
	connect(slider_verticesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(verticesScaleFactorChanged(int)));
	connect(check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(renderEdgesChanged(bool)));
	connect(check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(renderFacesChanged(bool)));
	connect(group_faceShading, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(faceStyleChanged(QAbstractButton*)));
}

void RenderDockTab::refreshUI(ParameterSet* params)
{
	m_currentParams = params;

	b_refreshingUI = true;

	mapList->clear();
	combo_positionVBO->clear();
	combo_normalVBO->clear();

	MapHandlerGen* map = params->selectedMap;

	QHash<QString, PerMapParameterSet*>::const_iterator i = params->perMap.constBegin();
	while (i != params->perMap.constEnd())
	{
		mapList->addItem(i.key());
		if(map != NULL && i.key() == map->getName())
		{
			QList<QListWidgetItem*> item = mapList->findItems(map->getName(), Qt::MatchExactly);
			item[0]->setSelected(true);

			PerMapParameterSet* p = params->perMap[map->getName()];

			QList<Utils::VBO*> vbos = map->getVBOList();
			unsigned int j = 0;
			for(int i = 0; i < vbos.count(); ++i)
			{
				if(vbos[i]->dataSize() == 3)
				{
					combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(vbos[i] == p->positionVBO)
						combo_positionVBO->setCurrentIndex(j);

					combo_normalVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(vbos[i] == p->normalVBO)
						combo_normalVBO->setCurrentIndex(j);

					++j;
				}
			}

			check_renderVertices->setChecked(p->renderVertices);
			slider_verticesScaleFactor->setSliderPosition(p->verticesScaleFactor * 50.0);
			check_renderEdges->setChecked(p->renderEdges);
			check_renderFaces->setChecked(p->renderFaces);
			radio_flatShading->setChecked(p->faceStyle == FLAT);
			radio_phongShading->setChecked(p->faceStyle == PHONG);
		}
		++i;
	}

	b_refreshingUI = false;
}

void RenderDockTab::selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = mapList->selectedItems();
		if(!currentItems.empty())
			m_plugin->changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderDockTab::positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changePositionVBO(view, map, map->getVBO(combo_positionVBO->currentText()), true);
	}
}

void RenderDockTab::normalVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeNormalVBO(view, map, map->getVBO(combo_normalVBO->currentText()), true);
	}
}

void RenderDockTab::renderVerticesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeRenderVertices(view, map, b, true);
	}
}

void RenderDockTab::verticesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeVerticesScaleFactor(view, map, i / 50.0, true);
	}
}

void RenderDockTab::renderEdgesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeRenderEdges(view, map, b, true);
	}
}

void RenderDockTab::renderFacesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeRenderFaces(view, map, b, true);
	}
}

void RenderDockTab::faceStyleChanged(QAbstractButton* b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(radio_flatShading->isChecked())
			m_plugin->changeFacesStyle(view, map, FLAT, true);
		else if(radio_phongShading->isChecked())
			m_plugin->changeFacesStyle(view, map, PHONG, true);
	}
}

void RenderDockTab::addVBOToList(Utils::VBO* vbo)
{
	if(vbo->dataSize() == 3)
	{
		QString name = QString::fromStdString(vbo->name());
		combo_positionVBO->addItem(name);
		combo_normalVBO->addItem(name);
	}
}

} // namespace SCHNApps

} // namespace CGoGN
