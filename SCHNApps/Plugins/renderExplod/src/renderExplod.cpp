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
	MapHandler<PFP3>* mh3 = static_cast<MapHandler<PFP3>*>(mh);
	if(mh3 == NULL)
		return;

	PFP3::MAP* m = mh3->getMap();

	//if(!color.isValid())
	m_renderExplod->updateData<PFP3>(*m,positionAttribute);
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
	//delete m_renderExplod;
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
		{
			PerMapParameterSet* p = new PerMapParameterSet(mh);
			registerShader(p->m_renderExplod->shaderFaces());
			registerShader(p->m_renderExplod->shaderLines());
			params->perMap.insert(mh->getName(), p);
		}
		if (!maps.empty())
			changeSelectedMap(view, maps[0]);

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
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void RenderExplodPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
	{
		ParameterSet* params = h_viewParams[view];
		changeSelectedMap(view, params->selectedMap);
		m_dockTab->refreshUI(h_viewParams[view]);
	}
}

void RenderExplodPlugin::mapLinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));

	connect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	connect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));


	ParameterSet* params = h_viewParams[view];

	PerMapParameterSet* p = new PerMapParameterSet(m);
	registerShader(p->m_renderExplod->shaderFaces());
	registerShader(p->m_renderExplod->shaderLines());
	params->perMap.insert(m->getName(), p);

	if(params->selectedMap == NULL || params->perMap.count() == 1)
		changeSelectedMap(view, m);
	else
		m_dockTab->refreshUI(params);
}

void RenderExplodPlugin::mapUnlinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));

	disconnect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	disconnect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));


	ParameterSet* params = h_viewParams[view];
	params->perMap.remove(m->getName());

	if(params->selectedMap == m)
	{
		if(!params->perMap.empty())
			changeSelectedMap(view, m_window->getMap(params->perMap.begin().key()));
		else
			changeSelectedMap(view, NULL);
	}
	else
		m_dockTab->refreshUI(params);
}


void RenderExplodPlugin::changeSelectedMap(View* view, MapHandlerGen* map, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
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
	params->perMap[map->getName()]->facesScaleFactor = i / 50.0;

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
	params->perMap[map->getName()]->volumesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}


void RenderExplodPlugin::attributeModified(unsigned int orbit, QString nameAttr)
{
//	if(orbit == VERTEX)
//	{
//
//		MapHandler<PFP3>* mh = static_cast<MapHandler<PFP3>*>(QObject::sender());
//		if(mh == NULL)
//			return;
//
//		VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP3::VEC3, VERTEX>(nameAttr);
//		if(!position.isValid())
//			return;
//
//		PFP3::MAP* map = mh->getMap();
//
//		m_renderExplod->updateData<PFP3>(*map,position);
//
////		if(computeNormalLastParameters.contains(map->getName()))
////		{
////			ComputeNormalParameters& params = computeNormalLastParameters[map->getName()];
////			if(params.positionName == nameAttr && params.autoUpdate)
////				computeNormal(map->getName(), params.positionName, params.normalName);
////		}
//	}
}

void RenderExplodPlugin::connectivityModified()
{
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
