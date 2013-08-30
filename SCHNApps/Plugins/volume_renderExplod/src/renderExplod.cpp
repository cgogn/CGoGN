#include "renderExplod.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* m) :
	mh(m),
	facesScaleFactor(1.0f),
	volumesScaleFactor(1.0f),
	renderEdges(false),
	renderFaces(true),
	faceStyle(FLAT)
{
	m_renderExplod = new Algo::Render::GL2::ExplodeVolumeRender(false, false, false);

	m_renderExplod->setNoClippingPlane();
	m_renderExplod->setExplodeVolumes(1.0f);
	m_renderExplod->setExplodeFaces(1.0f);

	QString positionName;

	QString vec3TypeName = QString::fromStdString(nameOfType(PFP3::VEC3()));

	const AttributeHash& attribs = mh->getAttributesList(VERTEX);
	for(AttributeHash::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
	{
		if(i.value() == vec3TypeName)
		{
			if(positionName != "position")	// try to select an attribute named "position"
				positionName = i.key();		// or anything else if not found
		}
	}
	positionAttribute = mh->getAttribute<PFP3::VEC3, VERTEX>(positionName);

	colorAttribute = mh->getAttribute<PFP3::VEC3, VOLUME>("color");

	updateRender();
}

PerMapParameterSet::~PerMapParameterSet()
{
	delete m_renderExplod;
}

void PerMapParameterSet::updateRender()
{
	PFP3::MAP* m = static_cast<MapHandler<PFP3>*>(mh)->getMap();
	//if(!color.isValid())
	m_renderExplod->updateData<PFP3>(*m, positionAttribute);
}


bool RenderExplodPlugin::enable()
{
	m_dockTab = new RenderExplodDockTab(m_window, this);
	addTabInDock(m_dockTab, "RenderExplod");

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void RenderExplodPlugin::disable()
{

}

void RenderExplodPlugin::redraw(View* view)
{
	ParameterSet* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		PerMapParameterSet* p = params->perMap[m->getName()];

		p->m_renderExplod->setExplodeFaces(p->facesScaleFactor);
		p->m_renderExplod->setExplodeVolumes(p->volumesScaleFactor);

		if(p->renderEdges)
		{
			p->m_renderExplod->drawEdges();
		}
		if(p->renderFaces)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_LIGHTING);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f) ;
			p->m_renderExplod->drawFaces();
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}
}

void RenderExplodPlugin::viewLinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		ParameterSet* params = new ParameterSet();
		h_viewParams.insert(view, params);

		const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
		foreach(MapHandlerGen* mh, maps)
			addManagedMap(view, mh);

		connect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		connect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));

		if(view->isCurrentView())
			m_dockTab->refreshUI(params);
	}
}

void RenderExplodPlugin::viewUnlinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
		foreach(MapHandlerGen* mh, maps)
			removeManagedMap(view, mh);

		ParameterSet* params = h_viewParams[view];
		delete params;
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void RenderExplodPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
		m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderExplodPlugin::mapLinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	addManagedMap(view, m);
}

void RenderExplodPlugin::mapUnlinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	removeManagedMap(view, m);
}

void RenderExplodPlugin::addManagedMap(View* v, MapHandlerGen *m)
{
	connect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	connect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[v];
	PerMapParameterSet* perMap = new PerMapParameterSet(m);

	registerShader(perMap->m_renderExplod->shaderFaces());
	registerShader(perMap->m_renderExplod->shaderLines());

	params->perMap.insert(m->getName(), perMap);

	if(params->selectedMap == NULL || params->perMap.count() == 1)
		changeSelectedMap(v, m);
	else
		m_dockTab->refreshUI(params);
}

void RenderExplodPlugin::removeManagedMap(View *v, MapHandlerGen *m)
{
	disconnect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	disconnect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[v];
	PerMapParameterSet* perMap = params->perMap[m->getName()];

	unregisterShader(perMap->m_renderExplod->shaderFaces());
	unregisterShader(perMap->m_renderExplod->shaderLines());

	delete perMap;
	params->perMap.remove(m->getName());

	if(params->selectedMap == m)
	{
		if(!params->perMap.empty())
			changeSelectedMap(v, m_window->getMap(params->perMap.begin().key()));
		else
			changeSelectedMap(v, NULL);
	}
	else
		m_dockTab->refreshUI(params);
}

void RenderExplodPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(prev)
			disconnect(prev, SIGNAL(attributeAdded(unsigned int, const QString&)), m_dockTab, SLOT(addAttributeToList(unsigned int, const QString&)));
		if(map)
			connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), m_dockTab, SLOT(addAttributeToList(unsigned int, const QString&)));

		m_dockTab->refreshUI(params);
	}
}

void RenderExplodPlugin::changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP3::VEC3> attribute, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet* perMap = params->perMap[map->getName()];
	perMap->positionAttribute = attribute;
	perMap->updateRender();

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeColorAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP3::VEC3> attribute, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
//	params->perMap[map->getName()].colorAttribute = attribute;
	//perMap->colorAttribute = attribute;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeRenderEdges(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->renderEdges = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeRenderFaces(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->renderFaces = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeFacesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->facesScaleFactor = i / 99.0;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeVolumesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->volumesScaleFactor = i / 99.0;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}


void RenderExplodPlugin::attributeModified(unsigned int orbit, QString nameAttr)
{
	MapHandler<PFP3>* mh = static_cast<MapHandler<PFP3>*>(QObject::sender());

	//On cherche la carte et on la met a jour puis updateGL();

	//pour toutes les vues
	 	 	 //on va mettre a jour la carte QObject::senter() si elle est enregistree dans le parameterSet de cette vue



//	if(orbit == VERTEX)
//	{
//		MapHandler<PFP3>* mh = static_cast<MapHandler<PFP3>*>(QObject::sender());
//		if(mh == NULL)
//			return;
//
//		foreach(ParameterSet* params, h_viewParams)
//		{
//			QHash<QString, PerMapParameterSet*>::const_iterator i = params->perMap.constBegin();
//			while (i != params->perMap.constEnd())
//			{
//				PerMapParameterSet* p = params->perMap[mh->getName()];
//
//				p->updateRender();
//
//				++i;
//			}
//
//		}



//		VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP3::VEC3, VERTEX>(nameAttr);
//		if(!position.isValid())
//			return;
//
//		PFP3::MAP* map = mh->getMap();
//
//		m_renderExplod->updateData<PFP3>(*map,position);

//		if(computeNormalLastParameters.contains(map->getName()))
//		{
//			ComputeNormalParameters& params = computeNormalLastParameters[map->getName()];
//			if(params.positionName == nameAttr && params.autoUpdate)
//				computeNormal(map->getName(), params.positionName, params.normalName);
//		}
//	}
}

void RenderExplodPlugin::connectivityModified()
{
	//On cherche la carte et on la met a jour puis updateGL();

	//pour toutes les vues
	 	 	 //on va mettre a jour la carte QObject::senter() si elle est enregistree dans le parameterSet de cette vue


//	MapHandler<PFP3>* mh = static_cast<MapHandler<PFP3>*>(QObject::sender());
//	if(mh == NULL)
//		return;
//
//	VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP3::VEC3, VERTEX>("position");
//	if(!position.isValid())
//		return;
//
//	PFP3::MAP* map = mh->getMap();
//
//	m_renderExplod->updateData<PFP3>(*map,position);

}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderExplodPlugin, RenderExplodPlugin)
#else
Q_EXPORT_PLUGIN2(RenderExplodPluginD, RenderExplodPlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
