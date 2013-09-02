#include "surface_renderVector_dockTab.h"

#include "surface_renderVector.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_RenderVector_DockTab::Surface_RenderVector_DockTab(SCHNApps* s, Surface_RenderVector_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(list_vectorVBO, SIGNAL(itemSelectionChanged()), this, SLOT(selectedVectorsVBOChanged()));
	connect(slider_vectorsScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(vectorsScaleFactorChanged(int)));
}





void Surface_RenderVector_DockTab::positionVBOChanged(int index)
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

void Surface_RenderVector_DockTab::selectedVectorsVBOChanged()
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			QList<QListWidgetItem*> currentItems = list_vectorVBO->selectedItems();
			QList<Utils::VBO*> vbos;
			foreach(QListWidgetItem* item, currentItems)
				vbos.append(map->getVBO(item->text()));
			m_plugin->h_viewParameterSet[view][map].vectorVBOs = vbos;
			view->updateGL();
		}
	}
}

void Surface_RenderVector_DockTab::vectorsScaleFactorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].vectorsScaleFactor = i / 50.0;
			view->updateGL();
		}
	}
}





void Surface_RenderVector_DockTab::addPositionVBO(QString name)
{
	b_updatingUI = true;
	combo_positionVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::removePositionVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_positionVBO->currentIndex();
	int index = combo_positionVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_positionVBO->setCurrentIndex(0);
	combo_positionVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::addVectorVBO(QString name)
{
	b_updatingUI = true;
	list_vectorVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::removeVectorVBO(QString name)
{
	b_updatingUI = true;
	QList<QListWidgetItem*> vbo = list_vectorVBO->findItems(name, Qt::MatchExactly);
	if(!vbo.empty())
		delete vbo[0];
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_positionVBO->addItem("- select VBO -");

	list_vectorVBO->clear();

	View* view = m_schnapps->getSelectedView();
	MapHandlerGen* map = m_schnapps->getSelectedMap();
	if(view && map)
	{
		const MapParameters& p = m_plugin->h_viewParameterSet[view][map];

		unsigned int i = 0;
		foreach(Utils::VBO* vbo, map->getVBOSet().values())
		{
			if(vbo->dataSize() == 3)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);

				list_vectorVBO->addItem(QString::fromStdString(vbo->name()));
				if(p.vectorVBOs.contains(vbo))
					list_vectorVBO->item(i)->setSelected(true);

				++i;
			}
		}

		slider_vectorsScaleFactor->setSliderPosition(p.vectorsScaleFactor * 50.0);
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
