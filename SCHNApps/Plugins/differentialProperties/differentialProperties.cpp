#include "differentialProperties.h"

#include "mapHandler.h"

#include "Algo/Geometry/normal.h"

bool DifferentialPropertiesPlugin::enable()
{
	m_computeNormalsDialog = new ComputeNormalsDialog();

	computeNormalsAction = new QAction("import", this);
	addMenuAction("Surface;DifferentialProperties;Compute Normals", computeNormalsAction);
	connect(computeNormalsAction, SIGNAL(triggered()), this, SLOT(cb_computeNormals()));
	return true;
}

void DifferentialPropertiesPlugin::cb_computeNormals()
{
	m_computeNormalsDialog->mapList->clear();
	m_computeNormalsDialog->attributeName->setText("normal");
	const QList<MapHandlerGen*>& maps = m_window->getMapsList();
	foreach(MapHandlerGen* map, maps)
		m_computeNormalsDialog->mapList->addItem(map->getName());

	m_computeNormalsDialog->show();
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(DifferentialPropertiesPlugin, DifferentialPropertiesPlugin)
#else
Q_EXPORT_PLUGIN2(DifferentialPropertiesPluginD, DifferentialPropertiesPlugin)
#endif
