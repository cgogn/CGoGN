#include "surface_radiance_dockTab.h"

#include "surface_radiance.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_Radiance_DockTab::Surface_Radiance_DockTab(SCHNApps* s, Surface_Radiance_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(combo_normalVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(normalVBOChanged(int)));
}





void Surface_Radiance_DockTab::positionVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			m_plugin->h_mapParameterSet[map].positionVBO = map->getVBO(combo_positionVBO->currentText());
		}
	}
}

void Surface_Radiance_DockTab::normalVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			m_plugin->h_mapParameterSet[map].normalVBO = map->getVBO(combo_normalVBO->currentText());
		}
	}
}





void Surface_Radiance_DockTab::addPositionVBO(QString name)
{
	b_updatingUI = true;
	combo_positionVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::removePositionVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_positionVBO->currentIndex();
	int index = combo_positionVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_positionVBO->setCurrentIndex(0);
	combo_positionVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::addNormalVBO(QString name)
{
	b_updatingUI = true;
	combo_normalVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::removeNormalVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_normalVBO->currentIndex();
	int index = combo_normalVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_normalVBO->setCurrentIndex(0);
	combo_normalVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_positionVBO->addItem("- select VBO -");
	combo_normalVBO->clear();
	combo_normalVBO->addItem("- select VBO -");

	MapHandlerGen* map = m_schnapps->getSelectedMap();
	if(map)
	{
		const MapParameters& p = m_plugin->h_mapParameterSet[map];

		unsigned int i = 1;
		foreach(Utils::VBO* vbo, map->getVBOSet().values())
		{
			unsigned int dataSize = vbo->dataSize();
			if(dataSize == 3)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);
				combo_normalVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.normalVBO)
					combo_normalVBO->setCurrentIndex(i);
				++i;
			}
		}
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
