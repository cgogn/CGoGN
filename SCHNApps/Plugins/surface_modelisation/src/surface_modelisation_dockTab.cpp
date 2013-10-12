#include "surface_modelisation_dockTab.h"

#include "surface_modelisation.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_Modelisation_DockTab::Surface_Modelisation_DockTab(SCHNApps* s, Surface_Modelisation_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	connect(combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(positionAttributeChanged(int)));
	connect(button_createCube, SIGNAL(clicked()), this, SLOT(createCubeButtonClicked()));
	connect(button_flipEdge, SIGNAL(clicked()), this, SLOT(flipEdgeButtonClicked()));
}





void Surface_Modelisation_DockTab::positionAttributeChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->h_parameterSet[map].positionAttribute = map->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText());
	}
}

void Surface_Modelisation_DockTab::vertexSelectorChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->h_parameterSet[map].vertexSelector = map->getCellSelector<VERTEX>(combo_vertexSelector->currentText());
	}
}

void Surface_Modelisation_DockTab::edgeSelectorChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->h_parameterSet[map].edgeSelector = map->getCellSelector<EDGE>(combo_edgeSelector->currentText());
	}
}

void Surface_Modelisation_DockTab::faceSelectorChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->h_parameterSet[map].faceSelector = map->getCellSelector<FACE>(combo_faceSelector->currentText());
	}
}

void Surface_Modelisation_DockTab::createCubeButtonClicked()
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->createCube(map);
	}
}

void Surface_Modelisation_DockTab::flipEdgeButtonClicked()
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->flipEdge(map);
	}
}





void Surface_Modelisation_DockTab::addVertexAttribute(const QString& nameAttr)
{
	b_updatingUI = true;
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
	const QString& typeAttr = m_schnapps->getSelectedMap()->getAttributeTypeName(VERTEX, nameAttr);
	if(typeAttr == vec3TypeName)
		combo_positionAttribute->addItem(nameAttr);
	b_updatingUI = false;
}

void Surface_Modelisation_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionAttribute->clear();
	combo_positionAttribute->addItem("- select attribute -");

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
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
