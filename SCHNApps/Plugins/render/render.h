#ifndef _RENDER_PLUGIN_H_
#define _RENDER_PLUGIN_H_

#include "plugin.h"
#include "ui_render.h"

#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderPhong.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/pointSprite.h"


using namespace CGoGN;
using namespace SCHNApps;


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

	QHash<QString, PerMapParameterSet> perMap;
	MapHandlerGen* selectedMap;
};


class RenderPlugin;

class RenderDockTab : public QWidget, public Ui::RenderWidget
{
public:
	RenderDockTab(RenderPlugin* p) : plugin(p)
	{
		setupUi(this);
	}

	void refreshUI(ParameterSet* params);

private:
	RenderPlugin* plugin;
};


class RenderPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderPlugin() : b_refreshingUI(false)
	{
		setProvidesRendering(true);
	}

	~RenderPlugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void redraw(View *view);

	virtual void keyPress(View* view, int key) {}
	virtual void keyRelease(View* view, int key) {}
	virtual void mousePress(View* view, int button, int x, int y) {}
	virtual void mouseRelease(View* view, int button, int x, int y) {}
	virtual void mouseMove(View* view, int buttons, int x, int y) {}
	virtual void wheelEvent(View* view, int delta, int x, int y) {}

	virtual void viewLinked(View* view);
	virtual void viewUnlinked(View* view);
	virtual void currentViewChanged(View* view);

	virtual void mapLinked(View* view, MapHandlerGen* m);
	virtual void mapUnlinked(View* view, MapHandlerGen* m);

	void setRefreshingUI(bool b) { b_refreshingUI = b; }

protected:
	RenderDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	CGoGN::Utils::ShaderFlat* m_flatShader;
	CGoGN::Utils::ShaderPhong* m_phongShader;
	CGoGN::Utils::ShaderSimpleColor* m_simpleColorShader;
	CGoGN::Utils::PointSprite* m_pointSprite;

	bool b_refreshingUI;

public slots:
	void cb_selectedMapChanged();
	void cb_positionVBOChanged(int index);
	void cb_normalVBOChanged(int index);
	void cb_refreshVBOs();
	void cb_renderVerticesChanged(bool b);
	void cb_verticesScaleFactorChanged(int i);
	void cb_renderEdgesChanged(bool b);
	void cb_renderFacesChanged(bool b);
	void cb_faceStyleChanged(QAbstractButton* b);
};

#endif
