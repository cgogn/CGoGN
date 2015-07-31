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
	connect(combo_tangentVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(tangentVBOChanged(int)));
	connect(combo_binormalVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(binormalVBOChanged(int)));
	connect(checkbox_fragInterp, SIGNAL(stateChanged(int)), this, SLOT(fragmentInterpolationChanged(int)));
	connect(button_decimate, SIGNAL(clicked()), this, SLOT(decimateClicked()));
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

void Surface_Radiance_DockTab::tangentVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			m_plugin->h_mapParameterSet[map].tangentVBO = map->getVBO(combo_tangentVBO->currentText());
			foreach (View* v, map->getLinkedViews())
				v->updateGL();
			m_plugin->pythonRecording("changeTangentVBO", "", map->getName(), combo_tangentVBO->currentText());
		}
	}
}

void Surface_Radiance_DockTab::binormalVBOChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			m_plugin->h_mapParameterSet[map].binormalVBO = map->getVBO(combo_binormalVBO->currentText());
			foreach (View* v, map->getLinkedViews())
				v->updateGL();
			m_plugin->pythonRecording("changeBiNormalVBO", "", map->getName(), combo_binormalVBO->currentText());
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

void Surface_Radiance_DockTab::addTangentVBO(QString name)
{
	b_updatingUI = true;
	combo_tangentVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::removeTangentVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_tangentVBO->currentIndex();
	int index = combo_tangentVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_tangentVBO->setCurrentIndex(0);
	combo_tangentVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::addBiNormalVBO(QString name)
{
	b_updatingUI = true;
	combo_binormalVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::removeBiNormalVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_binormalVBO->currentIndex();
	int index = combo_binormalVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_binormalVBO->setCurrentIndex(0);
	combo_binormalVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_Radiance_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_positionVBO->addItem("- select VBO -");
	combo_normalVBO->clear();
	combo_normalVBO->addItem("- select VBO -");
	combo_tangentVBO->clear();
	combo_tangentVBO->addItem("- select VBO -");
	combo_binormalVBO->clear();
	combo_binormalVBO->addItem("- select VBO -");

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
				combo_tangentVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.tangentVBO)
					combo_tangentVBO->setCurrentIndex(i);
				combo_binormalVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.binormalVBO)
					combo_binormalVBO->setCurrentIndex(i);
				++i;
			}
		}

		if (p.radiancePerVertexShader)
			checkbox_fragInterp->setChecked(p.radiancePerVertexShader->getFragInterp());
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
