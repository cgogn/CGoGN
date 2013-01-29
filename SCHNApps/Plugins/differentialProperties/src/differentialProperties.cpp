#include "differentialProperties.h"

#include "mapHandler.h"

#include "Algo/Geometry/normal.h"
#include "Algo/Geometry/curvature.h"

bool DifferentialPropertiesPlugin::enable()
{
	m_computeNormalDialog = new ComputeNormalDialog(m_window);
	m_computeCurvatureDialog = new ComputeCurvatureDialog(m_window);

	m_computeNormalAction = new QAction("Compute Normal", this);
	m_computeCurvatureAction = new QAction("Compute Curvature", this);

	addMenuAction("Surface;Differential Properties;Compute Normal", m_computeNormalAction);
	addMenuAction("Surface;Differential Properties;Compute Curvature", m_computeCurvatureAction);

	connect(m_computeNormalAction, SIGNAL(triggered()), this, SLOT(openComputeNormalDialog()));
	connect(m_computeCurvatureAction, SIGNAL(triggered()), this, SLOT(openComputeCurvatureDialog()));

	connect(m_computeNormalDialog, SIGNAL(accepted()), this, SLOT(computeNormal()));
	connect(m_computeNormalDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeNormal()));

	connect(m_computeCurvatureDialog, SIGNAL(accepted()), this, SLOT(computeCurvature()));
	connect(m_computeCurvatureDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeCurvature()));

	return true;
}

void DifferentialPropertiesPlugin::openComputeNormalDialog()
{
	m_computeNormalDialog->show();
}

void DifferentialPropertiesPlugin::openComputeCurvatureDialog()
{
	m_computeCurvatureDialog->show();
}

void DifferentialPropertiesPlugin::computeNormal()
{
	QList<QListWidgetItem*> currentItems = m_computeNormalDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapname));

		QString positionName = m_computeNormalDialog->combo_positionAttribute->currentText();
		QString normalName;
		if(m_computeNormalDialog->normalAttributeName->text().isEmpty())
			normalName = m_computeNormalDialog->combo_normalAttribute->currentText();
		else
			normalName = m_computeNormalDialog->normalAttributeName->text();

		VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionName);
		VertexAttribute<PFP2::VEC3> normal = mh->getAttribute<PFP2::VEC3, VERTEX>(normalName);
		if(!normal.isValid())
			normal = mh->addAttribute<PFP2::VEC3, VERTEX>(normalName);

		PFP2::MAP* map = mh->getMap();
		Algo::Surface::Geometry::computeNormalVertices<PFP2>(*map, position, normal);

		if(m_computeNormalDialog->check_createVBO->checkState() == Qt::Checked)
			mh->createVBO(normal);

		QList<View*> views = mh->getLinkedViews();
		foreach(View* view, views)
			view->updateGL();
	}
}

void DifferentialPropertiesPlugin::computeCurvature()
{
	QList<QListWidgetItem*> currentItems = m_computeCurvatureDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapname));

		QString positionName = m_computeCurvatureDialog->combo_positionAttribute->currentText();
		QString normalName = m_computeCurvatureDialog->combo_normalAttribute->currentText();

		VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionName);
		VertexAttribute<PFP2::VEC3> normal = mh->getAttribute<PFP2::VEC3, VERTEX>(normalName);

		QString KmaxName;
		if(m_computeCurvatureDialog->KmaxAttributeName->text().isEmpty())
			KmaxName = m_computeCurvatureDialog->combo_KmaxAttribute->currentText();
		else
			KmaxName = m_computeCurvatureDialog->KmaxAttributeName->text();
		VertexAttribute<PFP2::VEC3> Kmax = mh->getAttribute<PFP2::VEC3, VERTEX>(KmaxName);
		if(!Kmax.isValid())
			Kmax = mh->addAttribute<PFP2::VEC3, VERTEX>(KmaxName);

		QString kmaxName;
		if(m_computeCurvatureDialog->kmaxAttributeName->text().isEmpty())
			kmaxName = m_computeCurvatureDialog->combo_kmaxAttribute->currentText();
		else
			kmaxName = m_computeCurvatureDialog->kmaxAttributeName->text();
		VertexAttribute<PFP2::REAL> kmax = mh->getAttribute<PFP2::REAL, VERTEX>(kmaxName);
		if(!kmax.isValid())
			kmax = mh->addAttribute<PFP2::REAL, VERTEX>(kmaxName);

		QString KminName;
		if(m_computeCurvatureDialog->KminAttributeName->text().isEmpty())
			KminName = m_computeCurvatureDialog->combo_KminAttribute->currentText();
		else
			KminName = m_computeCurvatureDialog->KminAttributeName->text();
		VertexAttribute<PFP2::VEC3> Kmin = mh->getAttribute<PFP2::VEC3, VERTEX>(KminName);
		if(!Kmin.isValid())
			Kmin = mh->addAttribute<PFP2::VEC3, VERTEX>(KminName);

		QString kminName;
		if(m_computeCurvatureDialog->kminAttributeName->text().isEmpty())
			kminName = m_computeCurvatureDialog->combo_kminAttribute->currentText();
		else
			kminName = m_computeCurvatureDialog->kminAttributeName->text();
		VertexAttribute<PFP2::REAL> kmin = mh->getAttribute<PFP2::REAL, VERTEX>(kminName);
		if(!kmin.isValid())
			kmin = mh->addAttribute<PFP2::REAL, VERTEX>(kminName);

		QString KnormalName;
		if(m_computeCurvatureDialog->KnormalAttributeName->text().isEmpty())
			KnormalName = m_computeCurvatureDialog->combo_KnormalAttribute->currentText();
		else
			KnormalName = m_computeCurvatureDialog->KnormalAttributeName->text();
		VertexAttribute<PFP2::VEC3> Knormal = mh->getAttribute<PFP2::VEC3, VERTEX>(KnormalName);
		if(!Knormal.isValid())
			Knormal = mh->addAttribute<PFP2::VEC3, VERTEX>(KnormalName);

		EdgeAttribute<PFP2::REAL> edgeAngle = mh->getAttribute<PFP2::REAL, EDGE>("edgeAngle");
		if(!edgeAngle.isValid())
			edgeAngle = mh->addAttribute<PFP2::REAL, EDGE>("edgeAngle");

		PFP2::MAP* map = mh->getMap();
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
