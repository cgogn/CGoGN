#include "renderVectorDockTab.h"

#include "renderVector.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

RenderVectorDockTab::RenderVectorDockTab(Window* w, RenderVectorPlugin* p) :
	m_window(w),
	m_plugin(p),
	b_refreshingUI(false)
{
	setupUi(this);

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));
	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(list_vectorVBO, SIGNAL(itemSelectionChanged()), this, SLOT(selectedVectorsVBOChanged()));
	connect(slider_vectorsScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(vectorsScaleFactorChanged(int)));
}

void RenderVectorDockTab::refreshUI(ParameterSet* params)
{
	m_currentParams = params;

	b_refreshingUI = true;

	mapList->clear();
	combo_positionVBO->clear();
	list_vectorVBO->clear();

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

					list_vectorVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(std::find(p->vectorVBO.begin(), p->vectorVBO.end(), vbos[i]) != p->vectorVBO.end())
						list_vectorVBO->item(j)->setSelected(true);

					++j;
				}
			}

			slider_vectorsScaleFactor->setSliderPosition(p->vectorsScaleFactor * 50.0);
		}
		++i;
	}

	b_refreshingUI = false;
}

void RenderVectorDockTab::selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = mapList->selectedItems();
		if(!currentItems.empty())
			m_plugin->changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderVectorDockTab::positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changePositionVBO(view, map, map->getVBO(combo_positionVBO->currentText()), true);
	}
}

void RenderVectorDockTab::selectedVectorsVBOChanged()
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
		{
			QList<QListWidgetItem*> currentItems = list_vectorVBO->selectedItems();
			std::vector<Utils::VBO*> vbos;
			foreach(QListWidgetItem* item, currentItems)
				vbos.push_back(map->getVBO(item->text()));
			m_plugin->changeSelectedVectorsVBO(view, map, vbos, true);
		}
	}
}

void RenderVectorDockTab::vectorsScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeVectorsScaleFactor(view, map, i, true);
	}
}

void RenderVectorDockTab::addVBOToList(Utils::VBO* vbo)
{
	if(vbo->dataSize() == 3)
	{
		QString name = QString::fromStdString(vbo->name());
		combo_positionVBO->addItem(name);
		list_vectorVBO->addItem(name);
	}
}

} // namespace SCHNApps

} // namespace CGoGN
