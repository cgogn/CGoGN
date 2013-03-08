#ifndef _RENDER_PLUGIN_H_
#define _RENDER_PLUGIN_H_

#include "plugin.h"
#include "renderDockTab.h"

#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderPhong.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/pointSprite.h"

namespace CGoGN
{

namespace SCHNApps
{

enum FaceShadingStyle
{
	FLAT = 0,
	PHONG = 1
};

struct PerMapParameterSet
{
	PerMapParameterSet() :
		positionVBO(NULL),
		normalVBO(NULL),
		verticesScaleFactor(1.0f),
		renderVertices(false),
		renderEdges(false),
		renderFaces(true),
		faceStyle(FLAT)
	{}

	PerMapParameterSet(MapHandlerGen* map);

	Utils::VBO* positionVBO;
	Utils::VBO* normalVBO;
	float verticesScaleFactor;
	bool renderVertices;
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


class RenderPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderPlugin()
	{
		setProvidesRendering(true);
	}

	~RenderPlugin()
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
	RenderDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	CGoGN::Utils::ShaderFlat* m_flatShader;
	CGoGN::Utils::ShaderPhong* m_phongShader;
	CGoGN::Utils::ShaderSimpleColor* m_simpleColorShader;
	CGoGN::Utils::PointSprite* m_pointSprite;

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

	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);

	void changeSelectedMap(View* view, MapHandlerGen* map);

	void changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI = false);
	void changeNormalVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI = false);
	void changeRenderVertices(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changeVerticesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI = false);
	void changeRenderEdges(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changeRenderFaces(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changeFacesStyle(View* view, MapHandlerGen* map, FaceShadingStyle style, bool fromUI = false);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
