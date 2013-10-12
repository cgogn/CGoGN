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
	connect(combo_vertexSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(vertexSelectorChanged(int)));
	connect(combo_edgeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(edgeSelectorChanged(int)));
	connect(combo_faceSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(faceSelectorChanged(int)));
	connect(button_createEmptyMap, SIGNAL(clicked()), this, SLOT(createEmptyMapButtonClicked()));
	connect(button_addCube, SIGNAL(clicked()), this, SLOT(addCubeButtonClicked()));
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

void Surface_Modelisation_DockTab::createEmptyMapButtonClicked()
{
	if(!b_updatingUI)
	{
		m_plugin->createEmptyMap();
	}
}

void Surface_Modelisation_DockTab::addCubeButtonClicked()
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
			m_plugin->addCube(map);
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

void Surface_Modelisation_DockTab::addVertexSelector(const QString& name)
{
	b_updatingUI = true;
	combo_vertexSelector->addItem(name);
	b_updatingUI = false;
}

void Surface_Modelisation_DockTab::removeVertexSelector(const QString& name)
{
	b_updatingUI = true;
	int curIndex = combo_vertexSelector->currentIndex();
	int index = combo_vertexSelector->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_vertexSelector->setCurrentIndex(0);
	combo_vertexSelector->removeItem(index);
	b_updatingUI = false;
}

void Surface_Modelisation_DockTab::addEdgeSelector(const QString& name)
{
	b_updatingUI = true;
	combo_edgeSelector->addItem(name);
	b_updatingUI = false;
}

void Surface_Modelisation_DockTab::removeEdgeSelector(const QString& name)
{
	b_updatingUI = true;
	int curIndex = combo_edgeSelector->currentIndex();
	int index = combo_edgeSelector->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_edgeSelector->setCurrentIndex(0);
	combo_edgeSelector->removeItem(index);
	b_updatingUI = false;
}

void Surface_Modelisation_DockTab::addFaceSelector(const QString& name)
{
	b_updatingUI = true;
	combo_faceSelector->addItem(name);
	b_updatingUI = false;
}

void Surface_Modelisation_DockTab::removeFaceSelector(const QString& name)
{
	b_updatingUI = true;
	int curIndex = combo_faceSelector->currentIndex();
	int index = combo_faceSelector->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_faceSelector->setCurrentIndex(0);
	combo_faceSelector->removeItem(index);
	b_updatingUI = false;
}

void Surface_Modelisation_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionAttribute->clear();
	combo_positionAttribute->addItem("- select attribute -");
	combo_vertexSelector->clear();
	combo_vertexSelector->addItem("- select selector -");
	combo_edgeSelector->clear();
	combo_edgeSelector->addItem("- select selector -");
	combo_faceSelector->clear();
	combo_faceSelector->addItem("- select selector -");

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
		const CellSelectorSet& vertexSelectors = map->getCellSelectorSet(VERTEX);
		for(CellSelectorSet::const_iterator it = vertexSelectors.constBegin(); it != vertexSelectors.constEnd(); ++it)
		{
			combo_vertexSelector->addItem(it.key());
			if(p.vertexSelector && it.key() == p.vertexSelector->getName())
				combo_vertexSelector->setCurrentIndex(i);
			++i;
		}
		i = 1;
		const CellSelectorSet& edgeSelectors = map->getCellSelectorSet(EDGE);
		for(CellSelectorSet::const_iterator it = edgeSelectors.constBegin(); it != edgeSelectors.constEnd(); ++it)
		{
			combo_edgeSelector->addItem(it.key());
			if(p.edgeSelector && it.key() == p.edgeSelector->getName())
				combo_edgeSelector->setCurrentIndex(i);
			++i;
		}
		i = 1;
		const CellSelectorSet& faceSelectors = map->getCellSelectorSet(FACE);
		for(CellSelectorSet::const_iterator it = faceSelectors.constBegin(); it != faceSelectors.constEnd(); ++it)
		{
			combo_faceSelector->addItem(it.key());
			if(p.faceSelector && it.key() == p.faceSelector->getName())
				combo_faceSelector->setCurrentIndex(i);
			++i;
		}
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
