#include "differentialProperties.h"

#include "mapHandler.h"

#include "Algo/Geometry/normal.h"


bool DifferentialPropertiesPlugin::enable()
{
	m_computeNormalsDialog = new ComputeNormalsDialog(m_window);

	computeNormalsAction = new QAction("Compute Normals", this);
	computeCurvatureAction = new QAction("Compute Curvature", this);

	addMenuAction("Surface;Differential Properties;Compute Normals", computeNormalsAction);
	addMenuAction("Surface;Differential Properties;Compute Curvature", computeCurvatureAction);

	connect(computeNormalsAction, SIGNAL(triggered()), this, SLOT(cb_openComputeNormalsDialog()));
	connect(computeCurvatureAction, SIGNAL(triggered()), this, SLOT(cb_openComputeNormalsDialog()));

	connect(m_computeNormalsDialog, SIGNAL(accepted()), this, SLOT(cb_computeNormals()));
	connect(m_computeNormalsDialog->button_apply, SIGNAL(clicked()), this, SLOT(cb_computeNormals()));

	return true;
}

void DifferentialPropertiesPlugin::cb_openComputeNormalsDialog()
{
	m_computeNormalsDialog->init();
	m_computeNormalsDialog->show();
}

void DifferentialPropertiesPlugin::cb_computeNormals()
{
	QList<QListWidgetItem*> currentItems = m_computeNormalsDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP>* mh = reinterpret_cast<MapHandler<PFP>*>(m_window->getMap(mapname));
		MAP* map = mh->getMap();

		std::string positionName = m_computeNormalsDialog->combo_positionAttribute->currentText().toUtf8().constData();
		std::string normalName = m_computeNormalsDialog->attributeName->text().toUtf8().constData();
		VertexAttribute<VEC3> position = map->getAttribute<VEC3, VERTEX>(positionName);
		VertexAttribute<VEC3> normal = map->getAttribute<VEC3, VERTEX>(normalName);
		if(!normal.isValid())
			normal = map->addAttribute<VEC3, VERTEX>(normalName);

		Algo::Geometry::computeNormalVertices<PFP>(*map, position, normal);

		if(m_computeNormalsDialog->check_createVBO->checkState() == Qt::Checked)
			mh->createVBO(normal);

		QList<View*> views = m_window->getViewsList();
		foreach(View* view, views)
		{
			if(view->isLinkedToMap(mh))
				view->updateGL();
		}
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(DifferentialPropertiesPlugin, DifferentialPropertiesPlugin)
#else
Q_EXPORT_PLUGIN2(DifferentialPropertiesPluginD, DifferentialPropertiesPlugin)
#endif
