#ifndef _SURFACE_RENDERVECTOR_PLUGIN_H_
#define _SURFACE_RENDERVECTOR_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderVector_dockTab.h"

#include "Utils/Shaders/shaderVectorPerVertex.h"

namespace CGoGN
{

namespace SCHNApps
{

//struct SVectorInfo
//{
//	Utils::VBO* vbo;
//	QColor color;
//	float scaleFactor;
//	SVectorInfo() :
//		vbo(NULL), color(QColor("red")), scaleFactor(1.0f)
//	{}
//	SVectorInfo(Utils::VBO* ptr) :
//		vbo(ptr), color(QColor("red")), scaleFactor(1.0f)
//	{}
//};

struct MapParameters
{

	MapParameters() :
		positionVBO(NULL)
	{}

	Utils::VBO* positionVBO;
	QList<Utils::VBO*> vectorVBOs;
	QList<QColor> colors;
	QList<float> scaleFactors;

	//float vectorsScaleFactor;


};

class Surface_RenderVector_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
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

public slots:
	// slots for Python calls
	void changePositionVBO(const QString& view, const QString& map, const QString& vbo);
	void addVectorVBO(const QString& view, const QString& map, const QString& vbo);
	void removeVectorVBO(const QString& view, const QString& map, const QString& vbo);
	void changeVectorScaleFactor(const QString& view, const QString& map, const QString& vbo, float f);
	void changeVectorColor(const QString& view, const QString& map, const QString& vbo, const QString& col);

protected:
	Surface_RenderVector_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	CGoGN::Utils::ShaderVectorPerVertex* m_vectorShader;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
