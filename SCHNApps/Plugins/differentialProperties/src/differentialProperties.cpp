#include "differentialProperties.h"

#include "mapHandler.h"

#include "Algo/Geometry/normal.h"


bool DifferentialPropertiesPlugin::enable()
{
	m_computeNormalDialog = new ComputeNormalDialog(m_window);
	m_computeCurvatureDialog = new ComputeCurvatureDialog(m_window);

	computeNormalAction = new QAction("Compute Normal", this);
	computeCurvatureAction = new QAction("Compute Curvature", this);

	addMenuAction("Surface;Differential Properties;Compute Normal", computeNormalAction);
	addMenuAction("Surface;Differential Properties;Compute Curvature", computeCurvatureAction);

	connect(computeNormalAction, SIGNAL(triggered()), this, SLOT(cb_openComputeNormalDialog()));
	connect(computeCurvatureAction, SIGNAL(triggered()), this, SLOT(cb_openComputeCurvatureDialog()));

	connect(m_computeNormalDialog, SIGNAL(accepted()), this, SLOT(cb_computeNormal()));
	connect(m_computeNormalDialog->button_apply, SIGNAL(clicked()), this, SLOT(cb_computeNormal()));

	connect(m_computeCurvatureDialog, SIGNAL(accepted()), this, SLOT(cb_computeCurvature()));
	connect(m_computeCurvatureDialog->button_apply, SIGNAL(clicked()), this, SLOT(cb_computeCurvature()));

	return true;
}

void DifferentialPropertiesPlugin::cb_openComputeNormalDialog()
{
	m_computeNormalDialog->init();
	m_computeNormalDialog->show();
}

void DifferentialPropertiesPlugin::cb_openComputeCurvatureDialog()
{
	m_computeCurvatureDialog->init();
	m_computeCurvatureDialog->show();
}

void DifferentialPropertiesPlugin::cb_computeNormal()
{
	QList<QListWidgetItem*> currentItems = m_computeNormalDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP>* mh = reinterpret_cast<MapHandler<PFP>*>(m_window->getMap(mapname));
		MAP* map = mh->getMap();

		std::string positionName = m_computeNormalDialog->combo_positionAttribute->currentText().toUtf8().constData();
		std::string normalName;
		if(m_computeNormalDialog->attributeName->text().isEmpty())
			normalName = m_computeNormalDialog->combo_normalAttribute->currentText().toUtf8().constData();
		else
			normalName = m_computeNormalDialog->attributeName->text().toUtf8().constData();
		VertexAttribute<VEC3> position = map->getAttribute<VEC3, VERTEX>(positionName);
		VertexAttribute<VEC3> normal = map->getAttribute<VEC3, VERTEX>(normalName);
		if(!normal.isValid())
			normal = map->addAttribute<VEC3, VERTEX>(normalName);

		Algo::Geometry::computeNormalVertices<PFP>(*map, position, normal);

		if(m_computeNormalDialog->check_createVBO->checkState() == Qt::Checked)
			mh->createVBO(normal);

		QList<View*> views = m_window->getViewsList();
		foreach(View* view, views)
		{
			if(view->isLinkedToMap(mh))
				view->updateGL();
		}
	}
}

void DifferentialPropertiesPlugin::cb_computeCurvature()
{
	QList<QListWidgetItem*> currentItems = m_computeCurvatureDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP>* mh = reinterpret_cast<MapHandler<PFP>*>(m_window->getMap(mapname));
		MAP* map = mh->getMap();

		std::string positionName = m_computeCurvatureDialog->combo_positionAttribute->currentText().toUtf8().constData();
		std::string normalName = m_computeCurvatureDialog->combo_normalAttribute->currentText().toUtf8().constData();

		VertexAttribute<VEC3> position = map->getAttribute<VEC3, VERTEX>(positionName);
		VertexAttribute<VEC3> normal = map->getAttribute<VEC3, VERTEX>(normalName);

		std::string KmaxName;
		if(m_computeCurvatureDialog->KmaxAttributeName->text().isEmpty())
			KmaxName = m_computeCurvatureDialog->combo_KmaxAttribute->currentText().toUtf8().constData();
		else
			KmaxName = m_computeCurvatureDialog->KmaxAttributeName->text().toUtf8().constData();
		VertexAttribute<VEC3> Kmax = map->getAttribute<VEC3, VERTEX>(KmaxName);
		if(!Kmax.isValid())
			Kmax = map->addAttribute<VEC3, VERTEX>(KmaxName);

		std::string KminName;
		if(m_computeCurvatureDialog->KminAttributeName->text().isEmpty())
			KminName = m_computeCurvatureDialog->combo_KminAttribute->currentText().toUtf8().constData();
		else
			KminName = m_computeCurvatureDialog->KminAttributeName->text().toUtf8().constData();
		VertexAttribute<VEC3> Kmin = map->getAttribute<VEC3, VERTEX>(KminName);
		if(!Kmin.isValid())
			Kmin = map->addAttribute<VEC3, VERTEX>(KminName);

		std::string kmaxName;
		if(m_computeCurvatureDialog->kmaxAttributeName->text().isEmpty())
			kmaxName = m_computeCurvatureDialog->combo_kmaxAttribute->currentText().toUtf8().constData();
		else
			kmaxName = m_computeCurvatureDialog->kmaxAttributeName->text().toUtf8().constData();
		VertexAttribute<REAL> kmax = map->getAttribute<REAL, VERTEX>(kmaxName);
		if(!kmax.isValid())
			kmax = map->addAttribute<REAL, VERTEX>(kmaxName);

		std::string kminName;
		if(m_computeCurvatureDialog->kminAttributeName->text().isEmpty())
			kminName = m_computeCurvatureDialog->combo_kminAttribute->currentText().toUtf8().constData();
		else
			kminName = m_computeCurvatureDialog->kminAttributeName->text().toUtf8().constData();
		VertexAttribute<REAL> kmin = map->getAttribute<REAL, VERTEX>(kminName);
		if(!kmin.isValid())
			kmin = map->addAttribute<REAL, VERTEX>(kminName);

		Algo::Geometry::computeCurvatureVertices_NormalCycles<PFP>(*map, position, normal);

		if(m_computeNormalDialog->check_createVBO->checkState() == Qt::Checked)
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
