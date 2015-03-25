#include "surface_renderScalar_dockTab.h"

#include "surface_renderScalar.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_RenderScalar_DockTab::Surface_RenderScalar_DockTab(SCHNApps* s, Surface_RenderScalar_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(list_scalarVBO, SIGNAL(itemSelectionChanged()), this, SLOT(selectedScalarVBOChanged()));
	connect(combo_colorMap, SIGNAL(currentIndexChanged(int)), this, SLOT(colorMapChanged(int)));
	connect(slider_expansion, SIGNAL(valueChanged(int)), this, SLOT(expansionChanged(int)));
}





void Surface_RenderScalar_DockTab::positionVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].positionVBO = map->getVBO(combo_positionVBO->currentText());
			view->updateGL();
			m_plugin->pythonRecording("changePositionVBO", "", view->getName(), map->getName(), combo_positionVBO->currentText());
		}
	}
}

void Surface_RenderScalar_DockTab::selectedScalarVBOChanged()
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			MapParameters& p = m_plugin->h_viewParameterSet[view][map];

			QList<QListWidgetItem*> selectedItems = list_scalarVBO->selectedItems();
			if(!selectedItems.empty())
			{
				Utils::VBO* vbo = map->getVBO(selectedItems[0]->text());
				p.scalarVBO = vbo;

				if(vbo)
				{
					m_plugin->pythonRecording("changeScalarVBO", "", view->getName(), map->getName(), selectedItems[0]->text());
					MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(map);
					const VertexAttribute<PFP2::REAL, PFP2::MAP>& attr = mh->getAttribute<PFP2::REAL, VERTEX>(QString::fromStdString(vbo->name()));
					p.scalarMin = std::numeric_limits<float>::max();
					p.scalarMax = std::numeric_limits<float>::min();
					for(unsigned int i = attr.begin(); i != attr.end(); attr.next(i))
					{
						p.scalarMin = attr[i] < p.scalarMin ? attr[i] : p.scalarMin;
						p.scalarMax = attr[i] > p.scalarMax ? attr[i] : p.scalarMax;
					}
				}
			}
			else
				p.scalarVBO = NULL;
			view->updateGL();
		}
	}
}

void Surface_RenderScalar_DockTab::colorMapChanged(int index)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].colorMap = index;
			view->updateGL();
			m_plugin->pythonRecording("changeColorMap", "", view->getName(), map->getName(), index);
		}
	}
}

void Surface_RenderScalar_DockTab::expansionChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].expansion = i;
			view->updateGL();
			m_plugin->pythonRecording("changeExpansion", "", view->getName(), map->getName(), i);
		}
	}
}





void Surface_RenderScalar_DockTab::addPositionVBO(QString name)
{
	b_updatingUI = true;
	combo_positionVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_RenderScalar_DockTab::removePositionVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_positionVBO->currentIndex();
	int index = combo_positionVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_positionVBO->setCurrentIndex(0);
	combo_positionVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_RenderScalar_DockTab::addScalarVBO(QString name)
{
	b_updatingUI = true;
	list_scalarVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_RenderScalar_DockTab::removeScalarVBO(QString name)
{
	b_updatingUI = true;
	QList<QListWidgetItem*> vbo = list_scalarVBO->findItems(name, Qt::MatchExactly);
	if(!vbo.empty())
		delete vbo[0];
	b_updatingUI = false;
}

void Surface_RenderScalar_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_positionVBO->addItem("- select VBO -");

	list_scalarVBO->clear();

	View* view = m_schnapps->getSelectedView();
	MapHandlerGen* map = m_schnapps->getSelectedMap();
	if(view && map)
	{
		const MapParameters& p = m_plugin->h_viewParameterSet[view][map];

		unsigned int i = 1;
		unsigned int j = 0;
		foreach(Utils::VBO* vbo, map->getVBOSet().values())
		{
			unsigned int dataSize = vbo->dataSize();
			if(dataSize == 3)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbo->name()));
				if (vbo == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);
				++i;
			}
			else if(dataSize == 1)
			{
				list_scalarVBO->addItem(QString::fromStdString(vbo->name()));
				if (vbo == p.scalarVBO)
					list_scalarVBO->item(j)->setSelected(true);	
				++j;
			}
		}

		combo_colorMap->setCurrentIndex(p.colorMap);
		slider_expansion->setSliderPosition(p.expansion);
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
