#include "renderScalarDockTab.h"

#include "renderScalar.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

RenderScalarDockTab::RenderScalarDockTab(Window* w, RenderScalarPlugin* p) :
	m_window(w),
	m_plugin(p),
	b_refreshingUI(false)
{
	setupUi(this);

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));
	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(list_scalarVBO, SIGNAL(itemSelectionChanged()), this, SLOT(selectedScalarsVBOChanged()));
}

void RenderScalarDockTab::refreshUI(ParameterSet* params)
{
	m_currentParams = params;

	b_refreshingUI = true;

	mapList->clear();
	combo_positionVBO->clear();
	list_scalarVBO->clear();

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
				unsigned int dataSize = vbos[i]->dataSize();
				if(dataSize == 3)
				{
					combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(vbos[i] == p->positionVBO)
						combo_positionVBO->setCurrentIndex(j);
					++j;
				}
				else if(dataSize == 1)
				{
					list_scalarVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(std::find(p->scalarVBO.begin(), p->scalarVBO.end(), vbos[i]) != p->scalarVBO.end())
						list_scalarVBO->item(j)->setSelected(true);
				}
			}
		}
		++i;
	}

	b_refreshingUI = false;
}

void RenderScalarDockTab::selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = mapList->selectedItems();
		if(!currentItems.empty())
			m_plugin->changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderScalarDockTab::positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changePositionVBO(view, map, map->getVBO(combo_positionVBO->currentText()), true);
	}
}

void RenderScalarDockTab::selectedScalarsVBOChanged()
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		QList<QListWidgetItem*> currentItems = list_scalarVBO->selectedItems();
		std::vector<Utils::VBO*> vbos;
		foreach(QListWidgetItem* item, currentItems)
			vbos.push_back(map->getVBO(item->text()));
		m_plugin->changeSelectedScalarsVBO(view, map, vbos, true);
	}
}

void RenderScalarDockTab::addVBOToList(Utils::VBO* vbo)
{
	QString name = QString::fromStdString(vbo->name());
	unsigned int dataSize = vbo->dataSize();
	if(dataSize == 3)
		combo_positionVBO->addItem(name);
	else if(dataSize == 1)
		list_scalarVBO->addItem(name);
}

} // namespace SCHNApps

} // namespace CGoGN
