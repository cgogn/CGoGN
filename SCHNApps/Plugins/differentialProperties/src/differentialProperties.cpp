#include "differentialProperties.h"

#include "mapHandler.h"

#include "Algo/Geometry/normal.h"
#include "Algo/Geometry/curvature.h"


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
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapname));
		PFP2::MAP* map = mh->getMap();

		std::string positionName = m_computeNormalDialog->combo_positionAttribute->currentText().toUtf8().constData();
		std::string normalName;
		if(m_computeNormalDialog->normalAttributeName->text().isEmpty())
			normalName = m_computeNormalDialog->combo_normalAttribute->currentText().toUtf8().constData();
		else
			normalName = m_computeNormalDialog->normalAttributeName->text().toUtf8().constData();
		VertexAttribute<PFP2::VEC3> position = map->getAttribute<PFP2::VEC3, VERTEX>(positionName);
		VertexAttribute<PFP2::VEC3> normal = map->getAttribute<PFP2::VEC3, VERTEX>(normalName);
		if(!normal.isValid())
			normal = map->addAttribute<PFP2::VEC3, VERTEX>(normalName);

		Algo::Surface::Geometry::computeNormalVertices<PFP2>(*map, position, normal);

		if(m_computeNormalDialog->check_createVBO->checkState() == Qt::Checked)
			mh->createVBO(normal);

		QList<View*> views = mh->getLinkedViews();
		foreach(View* view, views)
			view->updateGL();
	}
}

void DifferentialPropertiesPlugin::cb_computeCurvature()
{
	QList<QListWidgetItem*> currentItems = m_computeCurvatureDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapname));
		PFP2::MAP* map = mh->getMap();

		std::string positionName = m_computeCurvatureDialog->combo_positionAttribute->currentText().toUtf8().constData();
		std::string normalName = m_computeCurvatureDialog->combo_normalAttribute->currentText().toUtf8().constData();

		VertexAttribute<PFP2::VEC3> position = map->getAttribute<PFP2::VEC3, VERTEX>(positionName);
		VertexAttribute<PFP2::VEC3> normal = map->getAttribute<PFP2::VEC3, VERTEX>(normalName);

		std::string KmaxName;
		if(m_computeCurvatureDialog->KmaxAttributeName->text().isEmpty())
			KmaxName = m_computeCurvatureDialog->combo_KmaxAttribute->currentText().toUtf8().constData();
		else
			KmaxName = m_computeCurvatureDialog->KmaxAttributeName->text().toUtf8().constData();
		VertexAttribute<PFP2::VEC3> Kmax = map->getAttribute<PFP2::VEC3, VERTEX>(KmaxName);
		if(!Kmax.isValid())
			Kmax = map->addAttribute<PFP2::VEC3, VERTEX>(KmaxName);

		std::string kmaxName;
		if(m_computeCurvatureDialog->kmaxAttributeName->text().isEmpty())
			kmaxName = m_computeCurvatureDialog->combo_kmaxAttribute->currentText().toUtf8().constData();
		else
			kmaxName = m_computeCurvatureDialog->kmaxAttributeName->text().toUtf8().constData();
		VertexAttribute<PFP2::REAL> kmax = map->getAttribute<PFP2::REAL, VERTEX>(kmaxName);
		if(!kmax.isValid())
			kmax = map->addAttribute<PFP2::REAL, VERTEX>(kmaxName);

		std::string KminName;
		if(m_computeCurvatureDialog->KminAttributeName->text().isEmpty())
			KminName = m_computeCurvatureDialog->combo_KminAttribute->currentText().toUtf8().constData();
		else
			KminName = m_computeCurvatureDialog->KminAttributeName->text().toUtf8().constData();
		VertexAttribute<PFP2::VEC3> Kmin = map->getAttribute<PFP2::VEC3, VERTEX>(KminName);
		if(!Kmin.isValid())
			Kmin = map->addAttribute<PFP2::VEC3, VERTEX>(KminName);

		std::string kminName;
		if(m_computeCurvatureDialog->kminAttributeName->text().isEmpty())
			kminName = m_computeCurvatureDialog->combo_kminAttribute->currentText().toUtf8().constData();
		else
			kminName = m_computeCurvatureDialog->kminAttributeName->text().toUtf8().constData();
		VertexAttribute<PFP2::REAL> kmin = map->getAttribute<PFP2::REAL, VERTEX>(kminName);
		if(!kmin.isValid())
			kmin = map->addAttribute<PFP2::REAL, VERTEX>(kminName);

		std::string KnormalName;
		if(m_computeCurvatureDialog->KnormalAttributeName->text().isEmpty())
			KnormalName = m_computeCurvatureDialog->combo_KnormalAttribute->currentText().toUtf8().constData();
		else
			KnormalName = m_computeCurvatureDialog->KnormalAttributeName->text().toUtf8().constData();
		VertexAttribute<PFP2::VEC3> Knormal = map->getAttribute<PFP2::VEC3, VERTEX>(KnormalName);
		if(!Knormal.isValid())
			Knormal = map->addAttribute<PFP2::VEC3, VERTEX>(KnormalName);

		EdgeAttribute<PFP2::REAL> edgeAngle = map->getAttribute<PFP2::REAL, EDGE>("edgeAngle");
		if(!edgeAngle.isValid())
			edgeAngle = map->addAttribute<PFP2::REAL, EDGE>("edgeAngle");
		Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP2>(*map, position, edgeAngle);

		Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles_Projected<PFP2>(*map, 0.01f * mh->getBBdiagSize(), position, normal, edgeAngle, kmax, kmin, Kmax, Kmin, Knormal);

		if(m_computeCurvatureDialog->check_KmaxCreateVBO->checkState() == Qt::Checked)
			mh->createVBO(Kmax);

		if(m_computeCurvatureDialog->check_kmaxCreateVBO->checkState() == Qt::Checked)
			mh->createVBO(kmax);

		if(m_computeCurvatureDialog->check_KminCreateVBO->checkState() == Qt::Checked)
			mh->createVBO(Kmin);

		if(m_computeCurvatureDialog->check_kminCreateVBO->checkState() == Qt::Checked)
			mh->createVBO(kmin);

		if(m_computeCurvatureDialog->check_KnormalCreateVBO->checkState() == Qt::Checked)
			mh->createVBO(Knormal);

		QList<View*> views = mh->getLinkedViews();
		foreach(View* view, views)
			view->updateGL();
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(DifferentialPropertiesPlugin, DifferentialPropertiesPlugin)
#else
Q_EXPORT_PLUGIN2(DifferentialPropertiesPluginD, DifferentialPropertiesPlugin)
#endif
