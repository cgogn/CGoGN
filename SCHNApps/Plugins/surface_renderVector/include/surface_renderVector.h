#ifndef _SURFACE_RENDERVECTOR_PLUGIN_H_
#define _SURFACE_RENDERVECTOR_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderVector_dockTab.h"

#include "Utils/Shaders/shaderVectorPerVertex.h"

namespace CGoGN
{

namespace SCHNApps
{

struct MapParameters
{
	MapParameters() :
		positionVBO(NULL),
		vectorsScaleFactor(1.0f)
	{}

	Utils::VBO* positionVBO;
	QList<Utils::VBO*> vectorVBOs;
	float vectorsScaleFactor;
};

class Surface_RenderVector_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

	friend class Surface_RenderVector_DockTab;

public:
	Surface_RenderVector_Plugin()
	{}

	~Surface_RenderVector_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view) {}
	virtual void drawMap(View* view, MapHandlerGen* map);

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

	virtual void viewLinked(View *view) {}
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedViewChanged(View* prev, View* cur);
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	// slots called from MapHandler signals
	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);

public slots:
	// slots for Python calls
	void changePositionVBO(const QString& view, const QString& map, const QString& vbo);
	void addVectorVBO(const QString& view, const QString& map, const QString& vbo);
	void removeVectorVBO(const QString& view, const QString& map, const QString& vbo);
	void changeVectorsScaleFactor(const QString& view, const QString& map, float f);

protected:
	Surface_RenderVector_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	CGoGN::Utils::ShaderVectorPerVertex* m_vectorShader;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
