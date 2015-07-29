#include "volume_render.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

//PerMapParameterSet::PerMapParameterSet(MapHandlerGen* m) :
//	mh(m),
//	facesScaleFactor(1.0f),
//	volumesScaleFactor(1.0f),
//	renderEdges(false),
//	renderFaces(true),
//	faceStyle(FLAT)
//{
//	m_renderExplod = new Algo::Render::GL2::ExplodeVolumeRender(false, false, false);

//	m_renderExplod->setNoClippingPlane();
//	m_renderExplod->setExplodeVolumes(1.0f);
//	m_renderExplod->setExplodeFaces(1.0f);

//	QString positionName;

//	QString vec3TypeName = QString::fromStdString(nameOfType(PFP3::VEC3()));

//	const AttributeHash& attribs = mh->getAttributesList(VERTEX);
//	for(AttributeHash::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
//	{
//		if(i.value() == vec3TypeName)
//		{
//			if(positionName != "position")	// try to select an attribute named "position"
//				positionName = i.key();		// or anything else if not found
//		}
//	}
//	positionAttribute = mh->getAttribute<PFP3::VEC3, VERTEX>(positionName);

//	colorAttribute = mh->getAttribute<PFP3::VEC3, VOLUME>("color");

//	updateRender();
//}

//PerMapParameterSet::~PerMapParameterSet()
//{
//	delete m_renderExplod;
//}

//void PerMapParameterSet::updateRender()
//{
//	PFP3::MAP* m = static_cast<MapHandler<PFP3>*>(mh)->getMap();
//	//if(!color.isValid())
//	m_renderExplod->updateData<PFP3>(*m, positionAttribute);
//}


bool Volume_Render_Plugin::enable()
{
	m_dockTab = new RenderExplodDockTab(m_window, this);
	addTabInDock(m_dockTab, "RenderExplod");

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void Volume_Render_Plugin::disable()
{

}

void Volume_Render_Plugin::drawMap(View* view, MapHandlerGen* map)
{

}


void Volume_Render_Plugin::selectedViewChanged(View *prev, View *cur)
{

}

void Volume_Render_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{

}

void Volume_Render_Plugin::mapAdded(MapHandlerGen *map)
{

}

void Volume_Render_Plugin::mapRemoved(MapHandlerGen *map)
{

}



void Volume_Render_Plugin::changeVerticesScaleFactor(const QString& view, const QString& map, float f)
{

}

void Volume_Render_Plugin::changeRenderEdges(const QString& view, const QString& map, bool b)
{

}

void Volume_Render_Plugin::changeRenderFaces(const QString& view, const QString& map, bool b)
{

}

void Volume_Render_Plugin::changeFacesStyle(const QString& view, const QString& map, MapParameters::FaceShadingStyle style)
{

}
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
    #ifndef DEBUG
		Q_EXPORT_PLUGIN2(Volume_Render_Plugin, Volume_Render_Plugin)
    #else
		Q_EXPORT_PLUGIN2(Volume_Render_PluginD, Volume_Render_Plugin)
    #endif
#endif


} // namespace SCHNApps

} // namespace CGoGN
