#include "surfaceDeformationDockTab.h"

#include "surfaceDeformation.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

SurfaceDeformationDockTab::SurfaceDeformationDockTab(Window* w, SurfaceDeformationPlugin* p) :
	m_window(w),
	m_plugin(p),
	b_refreshingUI(false)
{
	setupUi(this);

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));
	connect(combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(positionAttributeChanged(int)));
	connect(radio_locked, SIGNAL(toggled(bool)), this, SLOT(selectLockedVertices(bool)));
	connect(radio_handle, SIGNAL(toggled(bool)), this, SLOT(selectHandleVertices(bool)));
}

void SurfaceDeformationDockTab::refreshUI(ParameterSet* params)
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

			radio_locked->setChecked(p->verticesSelectionMode == LOCKED);
			radio_handle->setChecked(p->verticesSelectionMode == HANDLE);
		}
		++i;
	}

	b_refreshingUI = false;
}

void SurfaceDeformationDockTab::selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = mapList->selectedItems();
		if(!currentItems.empty())
			m_plugin->changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void SurfaceDeformationDockTab::positionAttributeChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changePositionAttribute(view, map, map->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText()));
	}
}

void SurfaceDeformationDockTab::selectLockedVertices(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeVerticesSelectionMode(view, map, LOCKED);
	}
}

void SurfaceDeformationDockTab::selectHandleVertices(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		m_plugin->changeVerticesSelectionMode(view, map, HANDLE);
	}
}

void SurfaceDeformationDockTab::addAttributeToList(unsigned int orbit, const QString& nameAttr)
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
