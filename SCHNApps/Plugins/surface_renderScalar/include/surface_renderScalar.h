#ifndef _SURFACE_RENDERSCALAR_PLUGIN_H_
#define _SURFACE_RENDERSCALAR_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderScalar_dockTab.h"

#include "Utils/Shaders/shaderScalarField.h"

namespace CGoGN
{

namespace SCHNApps
{

enum ColorMap {
	BlueWhiteRed = 0,
	CyanWhiteRed = 1,
	BlueCyanGreenYellowRed = 2,
	BlueGreenRed = 3
};

struct MapParameters
{
	MapParameters() :
		positionVBO(NULL),
		scalarVBO(NULL),
		scalarMin(0.0f),
		scalarMax(0.0f),
		colorMap(0),
		expansion(0)
	{}

	Utils::VBO* positionVBO;
	Utils::VBO* scalarVBO;
	float scalarMin, scalarMax;
	int colorMap;
	int expansion;
};

class Surface_RenderScalar_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
	friend class Surface_RenderScalar_DockTab;

public:
	Surface_RenderScalar_Plugin()
	{}

	~Surface_RenderScalar_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view) {}
	virtual void drawMap(View* view, MapHandlerGen* map);

	virtual void keyPress(View* , QKeyEvent* ) {}
	virtual void keyRelease(View* , QKeyEvent* ) {}
	virtual void mousePress(View* , QMouseEvent* ) {}
	virtual void mouseRelease(View* , QMouseEvent* ) {}
	virtual void mouseMove(View* , QMouseEvent* ) {}
	virtual void wheelEvent(View* , QWheelEvent* ) {}

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
	void attributeModified(unsigned int orbit, QString nameAttr);

public slots:
	// slots for Python calls
	void changePositionVBO(const QString& view, const QString& map, const QString& vbo);
	void changeScalarVBO(const QString& view, const QString& map, const QString& vbo);
	void changeColorMap(const QString& view, const QString& map, int c);
	void changeExpansion(const QString& view, const QString& map, int i);


protected:
	Surface_RenderScalar_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	CGoGN::Utils::ShaderScalarField* m_scalarShader;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
