#include "surface_deformation_dockTab.h"

#include "surface_deformation.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_Deformation_DockTab::Surface_Deformation_DockTab(SCHNApps* s, Surface_Deformation_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	connect(combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(positionAttributeChanged(int)));
	connect(combo_lockedSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(lockedSelectorChanged(int)));
	connect(combo_handleSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectorChanged(int)));
	connect(button_start_stop, SIGNAL(clicked()), this, SLOT(startStopButtonClicked()));
}





void Surface_Deformation_DockTab::positionAttributeChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			if(index == 0)
				;
			else
				m_plugin->h_parameterSet[map].positionAttribute = map->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText());
		}
	}
}

void Surface_Deformation_DockTab::lockedSelectorChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->h_parameterSet[map].lockedSelector = map->getCellSelector<VERTEX>(combo_lockedSelector->currentText());
	}
}

void Surface_Deformation_DockTab::handleSelectorChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->h_parameterSet[map].handleSelector = map->getCellSelector<VERTEX>(combo_handleSelector->currentText());
	}
}

void Surface_Deformation_DockTab::startStopButtonClicked()
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->toggleMapDeformation(map);
	}
}





void Surface_Deformation_DockTab::addVertexAttribute(const QString& name)
{
	b_updatingUI = true;
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
	const QString& typeAttr = m_schnapps->getSelectedMap()->getAttributeTypeName(VERTEX, name);
	if(typeAttr == vec3TypeName)
		combo_positionAttribute->addItem(name);
	b_updatingUI = false;
}

void Surface_Deformation_DockTab::addVertexSelector(const QString& name)
{
	b_updatingUI = true;
	combo_lockedSelector->addItem(name);
	combo_handleSelector->addItem(name);
	b_updatingUI = false;
}

void Surface_Deformation_DockTab::removeVertexSelector(const QString& name)
{
	b_updatingUI = true;
	int curIndex = combo_lockedSelector->currentIndex();
	int index = combo_lockedSelector->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_lockedSelector->setCurrentIndex(0);
	combo_lockedSelector->removeItem(index);

	curIndex = combo_handleSelector->currentIndex();
	index = combo_handleSelector->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_handleSelector->setCurrentIndex(0);
	combo_handleSelector->removeItem(index);
	b_updatingUI = false;
}

void Surface_Deformation_DockTab::mapParametersInitialized(bool b)
{
	combo_positionAttribute->setEnabled(!b);
	combo_lockedSelector->setEnabled(!b);
	combo_handleSelector->setEnabled(!b);
	if(b) button_start_stop->setText("Stop");
	else button_start_stop->setText("Start");
}

void Surface_Deformation_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionAttribute->clear();
	combo_positionAttribute->addItem("- select attribute -");
	combo_lockedSelector->clear();
	combo_lockedSelector->addItem("- select selector -");
	combo_handleSelector->clear();
	combo_handleSelector->addItem("- select selector -");

	MapHandlerGen* map = m_schnapps->getSelectedMap();

	if(map)
	{
		const MapParameters& p = m_plugin->h_parameterSet[map];

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

		unsigned int i = 1;
		const AttributeSet& attribs = map->getAttributeSet(VERTEX);
		for(AttributeSet::const_iterator it = attribs.constBegin(); it != attribs.constEnd(); ++it)
		{
			if(it.value() == vec3TypeName)
			{
				combo_positionAttribute->addItem(it.key());
				if(p.positionAttribute.isValid() && it.key() == QString::fromStdString(p.positionAttribute.name()))
					combo_positionAttribute->setCurrentIndex(i);

				++i;
			}
		}

		i = 1;
		const CellSelectorSet& selectors = map->getCellSelectorSet(VERTEX);
		for(CellSelectorSet::const_iterator it = selectors.constBegin(); it != selectors.constEnd(); ++it)
		{
			combo_lockedSelector->addItem(it.key());
			if(p.lockedSelector && it.key() == p.lockedSelector->getName())
				combo_lockedSelector->setCurrentIndex(i);

			combo_handleSelector->addItem(it.key());
			if(p.handleSelector && it.key() == p.handleSelector->getName())
				combo_handleSelector->setCurrentIndex(i);

			++i;
		}

		if(p.initialized)
			button_start_stop->setText("Stop");
		else
			button_start_stop->setText("Start");
	}
}

} // namespace SCHNApps

} // namespace CGoGN
