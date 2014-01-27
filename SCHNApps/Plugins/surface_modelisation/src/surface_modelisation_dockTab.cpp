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

    generalOperations << "Create empty map" << "Create new face" << "Add cube" << "Merge volumes" << "Split surface" << "Extrude region";
    vertexOperations << "Split vertex" << "Delete vertex";
    edgeOperations << "Cut edge" << "Uncut edge" << "Collapse edge" << "Flip edge" << "Flip back edge";
    faceOperations << "Split face" << "Merge faces" << "Delete face" << "Sew faces" << "Unsew faces" << "Extrude face" << "Extrude face following a path";

	connect(combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(positionAttributeChanged(int)));
	connect(combo_vertexSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(vertexSelectorChanged(int)));
	connect(combo_edgeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(edgeSelectorChanged(int)));
	connect(combo_faceSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(faceSelectorChanged(int)));
    connect(button_applyGeneralOperation, SIGNAL(clicked()), this, SLOT(applyGeneralOperationButtonClicked()));
    connect(button_applyVertexOperation, SIGNAL(clicked()), this, SLOT(applyVertexOperationButtonClicked()));
    connect(button_applyEdgeOperation, SIGNAL(clicked()), this, SLOT(applyEdgeOperationButtonClicked()));
    connect(button_applyFaceOperation, SIGNAL(clicked()), this, SLOT(applyFaceOperationButtonClicked()));
    connect(button_start, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
    connect(button_accept, SIGNAL(clicked()), this, SLOT(acceptButtonClicked()));
    connect(button_cancel, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
}

/* -------------------------- Position and selectors -----------------------------------*/
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

/* -------------------------------- Operations -----------------------------------------*/

void Surface_Modelisation_DockTab::applyGeneralOperationButtonClicked()
{
    if(!b_updatingUI)
    {
        if (generalOperations.indexOf(combo_generalOperation->currentText()) == 0) // Create empty map
            m_plugin->createEmptyMap();
        else
        {
            MapHandlerGen* map = m_schnapps->getSelectedMap();
            if(map)
            {
                switch ( generalOperations.indexOf(combo_generalOperation->currentText()) )
                {
                    case 1: // Create new face
                        m_plugin->createNewFace(map);
                        break;
                    case 2: // Add cube
                        m_plugin->addCube(map);
                        break;
                    case 3: // Merge volumes
                        m_plugin->mergeVolumes(map);
                        break;
                    case 4: // Split surface
                        m_plugin->splitSurface(map);
                        break;
                    case 5: // Extrude region
                        m_plugin->extrudeRegion(map);
                        break;
                }
            }
        }
    }
}

void Surface_Modelisation_DockTab::applyVertexOperationButtonClicked()
{
    if(!b_updatingUI)
    {
         MapHandlerGen* map = m_schnapps->getSelectedMap();
         if(map)
         {
            switch ( vertexOperations.indexOf(combo_vertexOperation->currentText()) )
            {
                case 0: // Split vertex
                    m_plugin->splitVertex(map);
                    break;
                case 1: // Delete vertex
                    m_plugin->deleteVertex(map);
                    break;
            }
        }
    }
}

void Surface_Modelisation_DockTab::applyEdgeOperationButtonClicked()
{
    if(!b_updatingUI)
    {
         MapHandlerGen* map = m_schnapps->getSelectedMap();
         if(map)
         {
            switch ( edgeOperations.indexOf(combo_edgeOperation->currentText()) )
            {
                case 0: // Cut edge
                    m_plugin->cutEdge(map);
                    break;
                case 1: // Uncut edge
                    m_plugin->uncutEdge(map);
                    break;
                case 2: // Collapse edge
                    m_plugin->collapseEdge(map);
                    break;
                case 3: // Flip edge
                    m_plugin->flipEdge(map);
                    break;
                case 4: // Flip back edge
                    m_plugin->flipBackEdge(map);
                    break;
            }
        }
    }
}

void Surface_Modelisation_DockTab::applyFaceOperationButtonClicked()
{
    if(!b_updatingUI)
    {
         MapHandlerGen* map = m_schnapps->getSelectedMap();
         if(map)
         {
            switch ( faceOperations.indexOf(combo_faceOperation->currentText()) )
            {
                case 0: // Split face
                    m_plugin->splitFace(map);
                    break;
                case 1: // Merge faces
                    m_plugin->mergeFaces(map);
                    break;
                case 2: // Delete face
                    m_plugin->deleteFace(map);
                    break;
                case 3: // Sew faces
                    m_plugin->sewFaces(map);
                    break;
                case 4: // Unsew faces
                    m_plugin->unsewFaces(map);
                    break;
                case 5: // Extrude face
                    m_plugin->extrudeFace(map);
                    break;
                case 6 : // Extrude face following a path
                    m_plugin->pathExtrudeFace(map);
                    break;
            }
        }
    }
}
/* -------------------------------- Clicks -----------------------------------------*/

void Surface_Modelisation_DockTab::startButtonClicked()
{
    m_plugin->collect = true;
}

void Surface_Modelisation_DockTab::acceptButtonClicked()
{
    m_plugin->collect = false;
}

void Surface_Modelisation_DockTab::cancelButtonClicked()
{
    m_plugin->collect = false;
    m_plugin->collectedVertices.clear();
}
/* -------------------------------- Updates -----------------------------------------*/

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
