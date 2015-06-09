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
	connect(checkbox_fragInterp, SIGNAL(stateChanged(int)), this, SLOT(fragmentInterpolationChanged(int)));
	connect(button_decimate, SIGNAL(clicked()), this, SLOT(decimateClicked()));
	connect(button_computeDistance, SIGNAL(clicked()), this, SLOT(computeDistanceClicked()));
}





void Surface_Radiance_DockTab::positionVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			m_plugin->h_mapParameterSet[map].positionVBO = map->getVBO(combo_positionVBO->currentText());
			foreach (View* v, map->getLinkedViews())
				v->updateGL();
			m_plugin->pythonRecording("changePositionVBO", "", map->getName(), combo_positionVBO->currentText());
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
			foreach (View* v, map->getLinkedViews())
				v->updateGL();
			m_plugin->pythonRecording("changeNormalVBO", "", map->getName(), combo_normalVBO->currentText());
		}
	}
}

void Surface_Radiance_DockTab::fragmentInterpolationChanged(int state)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			m_plugin->h_mapParameterSet[map].radiancePerVertexShader->setFragInterp(state == Qt::Checked);
			foreach (View* v, map->getLinkedViews())
				v->updateGL();
		}
	}
}

void Surface_Radiance_DockTab::decimateClicked()
{
	m_plugin->decimate(
		m_schnapps->getSelectedMap()->getName(),
		combo_positionVBO->currentText(),
		combo_normalVBO->currentText(),
		slider_decimationGoal->value() / 100.0f,
		checkbox_halfCollapse->checkState() == Qt::Checked
	);

	m_plugin->pythonRecording("decimate", "",
		m_schnapps->getSelectedMap()->getName(),
		combo_positionVBO->currentText(),
		combo_normalVBO->currentText(),
		slider_decimationGoal->value() / 100.0f,
		checkbox_halfCollapse->checkState() == Qt::Checked);
}

void Surface_Radiance_DockTab::computeDistanceClicked()
{
	m_plugin->computeDistance(
		combo_object1->currentText(),
		combo_object2->currentText()
	);
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

void Surface_Radiance_DockTab::addObject(QString name)
{
	b_updatingUI = true;
	combo_object1->addItem(name);
	combo_object2->addItem(name);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::removeObject(QString name)
{
	b_updatingUI = true;
	int curIndex1 = combo_object1->currentIndex();
	int index1 = combo_object1->findText(name, Qt::MatchExactly);
	if(curIndex1 == index1)
		combo_object1->setCurrentIndex(0);
	combo_object1->removeItem(index1);
	int curIndex2 = combo_object2->currentIndex();
	int index2 = combo_object2->findText(name, Qt::MatchExactly);
	if(curIndex2 == index2)
		combo_object2->setCurrentIndex(0);
	combo_object2->removeItem(index2);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_positionVBO->addItem("- select VBO -");
	combo_normalVBO->clear();
	combo_normalVBO->addItem("- select VBO -");
	combo_object1->clear();
	combo_object1->addItem("- select map -");
	combo_object2->clear();
	combo_object2->addItem("- select map -");

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

		checkbox_fragInterp->setChecked(p.radiancePerVertexShader->getFragInterp());
	}

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
	{
		combo_object1->addItem(map->getName());
		combo_object2->addItem(map->getName());
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
