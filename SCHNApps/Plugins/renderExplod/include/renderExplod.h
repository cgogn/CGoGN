#ifndef _RENDER_PLUGIN_H_
#define _RENDER_PLUGIN_H_

#include "plugin.h"
#include "renderExplodDockTab.h"

#include "Algo/Render/GL2/explodeVolumeRender.h"

namespace CGoGN
{

namespace SCHNApps
{

struct PerMapParameterSet
{
	PerMapParameterSet() :
		positionVBO(NULL),
		colorVBO(NULL),
		facesScaleFactor(1.0f),
		volumesScaleFactor(1.0f),
		renderEdges(false),
		renderFaces(true)
	{}

	PerMapParameterSet(MapHandlerGen* map);

	Utils::VBO* positionVBO;
	Utils::VBO* colorVBO;
	float facesScaleFactor;
	float volumesScaleFactor;
	bool renderEdges;
	bool renderFaces;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	QHash<QString, PerMapParameterSet> perMap;
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

	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);

	void changeSelectedMap(View* view, MapHandlerGen* map);
	void changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo);
	void changeColorVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo);
	void changeRenderEdges(View* view, MapHandlerGen* map, bool b);
	void changeRenderFaces(View* view, MapHandlerGen* map, bool b);
	void changeFacesScaleFactor(View* view, MapHandlerGen* map, int i);
	void changeVolumesScaleFactor(View* view, MapHandlerGen* map, int i);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
