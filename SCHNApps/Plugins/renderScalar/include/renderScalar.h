#ifndef _RENDERSCALAR_PLUGIN_H_
#define _RENDERSCALAR_PLUGIN_H_

#include "plugin.h"
#include "renderScalarDockTab.h"

#include "Utils/Shaders/shaderScalarField.h"

namespace CGoGN
{

namespace SCHNApps
{

struct PerMapParameterSet
{
	PerMapParameterSet() :
		positionVBO(NULL)
	{}

	PerMapParameterSet(MapHandlerGen* map);

	Utils::VBO* positionVBO;
	std::vector<Utils::VBO*> scalarVBO;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	QHash<QString, PerMapParameterSet*> perMap;
	MapHandlerGen* selectedMap;
};


class RenderScalarPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderScalarPlugin()
	{
		setProvidesRendering(true);
	}

	~RenderScalarPlugin()
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
	RenderScalarDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	CGoGN::Utils::ShaderScalarField* m_scalarShader;

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

	void changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI = false);
	void changeSelectedScalarsVBO(View* view, MapHandlerGen* map, const std::vector<Utils::VBO*>& vbos, bool fromUI = false);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
