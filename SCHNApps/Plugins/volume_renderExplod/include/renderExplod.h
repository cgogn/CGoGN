#ifndef _RENDER_PLUGIN_H_
#define _RENDER_PLUGIN_H_

#include "plugin.h"
#include "renderExplodDockTab.h"

#include "Algo/Render/GL2/explodeVolumeRender.h"

namespace CGoGN
{

namespace SCHNApps
{

enum FaceShadingStyle
{
	FLAT = 0,
	SMOOTH = 1
};

struct PerMapParameterSet
{
	PerMapParameterSet(MapHandlerGen* mh);
	~PerMapParameterSet();

	void updateRender();

	MapHandlerGen* mh;
	Algo::Render::GL2::ExplodeVolumeRender* m_renderExplod;
	VertexAttribute<PFP3::VEC3> positionAttribute;
	VolumeAttribute<PFP3::VEC3> colorAttribute;

	float facesScaleFactor;
	float volumesScaleFactor;
	bool renderEdges;
	bool renderFaces;
	FaceShadingStyle faceStyle;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	QHash<QString, PerMapParameterSet*> perMap;
	MapHandlerGen* selectedMap;
};


class RenderExplodPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderExplodPlugin()
	{
		setProvidesRendering(true);
	}

	~RenderExplodPlugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void redraw(View *view);

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

protected:
	RenderExplodDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

public slots:
	void viewLinked(View* view, Plugin* plugin);
	void viewUnlinked(View* view, Plugin* plugin);
	void currentViewChanged(View* view);

	void mapLinked(MapHandlerGen* m);
	void mapUnlinked(MapHandlerGen* m);

protected:
	void addManagedMap(View *v, MapHandlerGen* m);
	void removeManagedMap(View *v, MapHandlerGen* m);

public slots:
	void changeSelectedMap(View* view, MapHandlerGen* map);

	void changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP3::VEC3> attribute, bool fromUI = false);
	void changeColorAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP3::VEC3> attribute, bool fromUI = false);
	void changeRenderEdges(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changeRenderFaces(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changeFacesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI = false);
	void changeVolumesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI = false);

	void attributeModified(unsigned int orbit, QString nameAttr);
	void connectivityModified();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
