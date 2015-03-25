#ifndef _SURFACE_RENDER_PLUGIN_H_
#define _SURFACE_RENDER_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_render_dockTab.h"

#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderPhong.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/pointSprite.h"

namespace CGoGN
{

namespace SCHNApps
{

class MapHandlerGen;

struct MapParameters
{
	enum FaceShadingStyle
	{
		FLAT = 0,
		PHONG = 1
	};

	MapParameters() :
		positionVBO(NULL),
		normalVBO(NULL),
		verticesScaleFactor(1.0f),
		renderVertices(false),
		renderEdges(false),
		renderFaces(true),
		faceStyle(FLAT),
		diffuseColor(0.8f,0.9f,0.7f,0.0f),
		simpleColor(0.0f,0.0f,0.0f,0.0f),
		vertexColor(0.0f,0.0f,1.0f,0.0f)

	{}

	Utils::VBO* positionVBO;
	Utils::VBO* normalVBO;
	float verticesScaleFactor;
	bool renderVertices;
	bool renderEdges;
	bool renderFaces;
	bool renderBoundary;
	FaceShadingStyle faceStyle;

	Geom::Vec4f diffuseColor;
	Geom::Vec4f simpleColor;
	Geom::Vec4f vertexColor;
};

class Surface_Render_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
	friend class Surface_Render_DockTab;

public:
	Surface_Render_Plugin()
	{}

	~Surface_Render_Plugin()
	{}

private:
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

	void schnappsClosing();

public slots:
	// slots for Python calls
	void changePositionVBO(const QString& view, const QString& map, const QString& vbo);
	void changeNormalVBO(const QString& view, const QString& map, const QString& vbo);
	void changeRenderVertices(const QString& view, const QString& map, bool b);
	void changeVerticesScaleFactor(const QString&view, const QString& map, float f);
	void changeRenderEdges(const QString& view, const QString& map, bool b);
	void changeRenderFaces(const QString& view, const QString& map, bool b);
	void changeFacesStyle(const QString& view, const QString& map, int);
	void changeRenderBoundary(const QString& view, const QString& map, bool b);
	void changeFaceColor(const QString& view, const QString& map, float r, float g, float b);
	void changeEdgeColor(const QString& view, const QString& map, float r, float g, float b);
	void changeVertexColor(const QString& view, const QString& map, float r, float g, float b);

protected:
	Surface_Render_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	CGoGN::Utils::ShaderFlat* m_flatShader;
	CGoGN::Utils::ShaderPhong* m_phongShader;
	CGoGN::Utils::ShaderSimpleColor* m_simpleColorShader;
	CGoGN::Utils::PointSprite* m_pointSprite;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
