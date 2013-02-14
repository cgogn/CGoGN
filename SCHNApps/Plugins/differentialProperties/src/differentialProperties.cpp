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

	connect(m_computeNormalDialog, SIGNAL(accepted()), this, SLOT(computeNormalFromDialog()));
	connect(m_computeNormalDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeNormalFromDialog()));

	connect(m_computeCurvatureDialog, SIGNAL(accepted()), this, SLOT(computeCurvatureFromDialog()));
	connect(m_computeCurvatureDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeCurvatureFromDialog()));

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

void DifferentialPropertiesPlugin::computeNormalFromDialog()
{
	QList<QListWidgetItem*> currentItems = m_computeNormalDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapName = currentItems[0]->text();

		QString positionName = m_computeNormalDialog->combo_positionAttribute->currentText();
		QString normalName;
		if(m_computeNormalDialog->normalAttributeName->text().isEmpty())
			normalName = m_computeNormalDialog->combo_normalAttribute->currentText();
		else
			normalName = m_computeNormalDialog->normalAttributeName->text();

		bool createVBO = (m_computeNormalDialog->check_createVBO->checkState() == Qt::Checked);

		computeNormal(mapName, positionName, normalName, createVBO);
	}
}

void DifferentialPropertiesPlugin::computeCurvatureFromDialog()
{
	QList<QListWidgetItem*> currentItems = m_computeCurvatureDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapName = currentItems[0]->text();

		QString positionName = m_computeCurvatureDialog->combo_positionAttribute->currentText();
		QString normalName = m_computeCurvatureDialog->combo_normalAttribute->currentText();

		QString KmaxName;
		if(m_computeCurvatureDialog->KmaxAttributeName->text().isEmpty())
			KmaxName = m_computeCurvatureDialog->combo_KmaxAttribute->currentText();
		else
			KmaxName = m_computeCurvatureDialog->KmaxAttributeName->text();

		QString kmaxName;
		if(m_computeCurvatureDialog->kmaxAttributeName->text().isEmpty())
			kmaxName = m_computeCurvatureDialog->combo_kmaxAttribute->currentText();
		else
			kmaxName = m_computeCurvatureDialog->kmaxAttributeName->text();

		QString KminName;
		if(m_computeCurvatureDialog->KminAttributeName->text().isEmpty())
			KminName = m_computeCurvatureDialog->combo_KminAttribute->currentText();
		else
			KminName = m_computeCurvatureDialog->KminAttributeName->text();

		QString kminName;
		if(m_computeCurvatureDialog->kminAttributeName->text().isEmpty())
			kminName = m_computeCurvatureDialog->combo_kminAttribute->currentText();
		else
			kminName = m_computeCurvatureDialog->kminAttributeName->text();

		QString KnormalName;
		if(m_computeCurvatureDialog->KnormalAttributeName->text().isEmpty())
			KnormalName = m_computeCurvatureDialog->combo_KnormalAttribute->currentText();
		else
			KnormalName = m_computeCurvatureDialog->KnormalAttributeName->text();

		bool KmaxVBO = (m_computeCurvatureDialog->check_KmaxCreateVBO->checkState() == Qt::Checked);

		bool kmaxVBO = (m_computeCurvatureDialog->check_kmaxCreateVBO->checkState() == Qt::Checked);

		bool KminVBO = (m_computeCurvatureDialog->check_KminCreateVBO->checkState() == Qt::Checked);

		bool kminVBO = (m_computeCurvatureDialog->check_kminCreateVBO->checkState() == Qt::Checked);

		bool KnormalVBO = (m_computeCurvatureDialog->check_KnormalCreateVBO->checkState() == Qt::Checked);

		computeCurvature(
			mapName, positionName, normalName, KmaxName, kmaxName, KminName, kminName, KnormalName,
			KmaxVBO, kmaxVBO, KminVBO, kminVBO, KnormalVBO
		);
	}
}

void DifferentialPropertiesPlugin::computeNormal(
	const QString& mapName,
	const QString& positionAttributeName,
	const QString& normalAttributeName,
	bool createNormalVBO)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	VertexAttribute<PFP2::VEC3> normal = mh->getAttribute<PFP2::VEC3, VERTEX>(normalAttributeName);
	if(!normal.isValid())
		normal = mh->addAttribute<PFP2::VEC3, VERTEX>(normalAttributeName);

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Geometry::computeNormalVertices<PFP2>(*map, position, normal);

	if(createNormalVBO)
		mh->createVBO(normal);

	QList<View*> views = mh->getLinkedViews();
	foreach(View* view, views)
		view->updateGL();
}

void DifferentialPropertiesPlugin::computeCurvature(
	const QString& mapName,
	const QString& positionAttributeName,
	const QString& normalAttributeName,
	const QString& KmaxAttributeName,
	const QString& kmaxAttributeName,
	const QString& KminAttributeName,
	const QString& kminAttributeName,
	const QString& KnormalAttributeName,
	bool createKmaxVBO,
	bool createkmaxVBO,
	bool createKminVBO,
	bool createkminVBO,
	bool createKnormalVBO)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	VertexAttribute<PFP2::VEC3> normal = mh->getAttribute<PFP2::VEC3, VERTEX>(normalAttributeName);
	if(!normal.isValid())
		return;

	VertexAttribute<PFP2::VEC3> Kmax = mh->getAttribute<PFP2::VEC3, VERTEX>(KmaxAttributeName);
	if(!Kmax.isValid())
		Kmax = mh->addAttribute<PFP2::VEC3, VERTEX>(KmaxAttributeName);

	VertexAttribute<PFP2::REAL> kmax = mh->getAttribute<PFP2::REAL, VERTEX>(kmaxAttributeName);
	if(!kmax.isValid())
		kmax = mh->addAttribute<PFP2::REAL, VERTEX>(kmaxAttributeName);

	VertexAttribute<PFP2::VEC3> Kmin = mh->getAttribute<PFP2::VEC3, VERTEX>(KminAttributeName);
	if(!Kmin.isValid())
		Kmin = mh->addAttribute<PFP2::VEC3, VERTEX>(KminAttributeName);

	VertexAttribute<PFP2::REAL> kmin = mh->getAttribute<PFP2::REAL, VERTEX>(kminAttributeName);
	if(!kmin.isValid())
		kmin = mh->addAttribute<PFP2::REAL, VERTEX>(kminAttributeName);

	VertexAttribute<PFP2::VEC3> Knormal = mh->getAttribute<PFP2::VEC3, VERTEX>(KnormalAttributeName);
	if(!Knormal.isValid())
		Knormal = mh->addAttribute<PFP2::VEC3, VERTEX>(KnormalAttributeName);

	EdgeAttribute<PFP2::REAL> edgeAngle = mh->getAttribute<PFP2::REAL, EDGE>("edgeAngle");
	if(!edgeAngle.isValid())
		edgeAngle = mh->addAttribute<PFP2::REAL, EDGE>("edgeAngle");

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP2>(*map, position, edgeAngle);
	Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles_Projected<PFP2>(*map, 0.02f * mh->getBBdiagSize(), position, normal, edgeAngle, kmax, kmin, Kmax, Kmin, Knormal);

	if(createKmaxVBO) mh->createVBO(Kmax);
	if(createkmaxVBO) mh->createVBO(kmax);
	if(createKminVBO) mh->createVBO(Kmin);
	if(createkminVBO) mh->createVBO(kmin);
	if(createKnormalVBO) mh->createVBO(Knormal);

	QList<View*> views = mh->getLinkedViews();
	foreach(View* view, views)
		view->updateGL();
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(DifferentialPropertiesPlugin, DifferentialPropertiesPlugin)
#else
Q_EXPORT_PLUGIN2(DifferentialPropertiesPluginD, DifferentialPropertiesPlugin)
#endif
