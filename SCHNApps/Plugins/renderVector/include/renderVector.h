#ifndef _RENDERVECTOR_PLUGIN_H_
#define _RENDERVECTOR_PLUGIN_H_

#include "plugin.h"
#include "renderVectorDockTab.h"

#include "Utils/Shaders/shaderVectorPerVertex.h"

namespace CGoGN
{

namespace SCHNApps
{

struct PerMapParameterSet
{
	PerMapParameterSet() :
		positionVBO(NULL),
		vectorsScaleFactor(1.0f)
	{}

	PerMapParameterSet(MapHandlerGen* map);

	Utils::VBO* positionVBO;
	std::vector<Utils::VBO*> vectorVBO;
	float vectorsScaleFactor;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	QHash<QString, PerMapParameterSet> perMap;
	MapHandlerGen* selectedMap;
};


class RenderVectorPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderVectorPlugin()
	{
		setProvidesRendering(true);
	}

	~RenderVectorPlugin()
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
	RenderVectorDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	CGoGN::Utils::ShaderVectorPerVertex* m_vectorShader;

public slots:
	void viewLinked(View* view, Plugin* plugin);
	void viewUnlinked(View* view, Plugin* plugin);
	void currentViewChanged(View* view);

	void mapLinked(MapHandlerGen* m);
	void mapUnlinked(MapHandlerGen* m);

	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);

	void changeSelectedMap(View* view, MapHandlerGen* map, bool fromUI = false);
	void changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI = false);
	void changeSelectedVectorsVBO(View* view, MapHandlerGen* map, const std::vector<Utils::VBO*>& vbos, bool fromUI = false);
	void changeVectorsScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI = false);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
