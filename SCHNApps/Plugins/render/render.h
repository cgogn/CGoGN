#ifndef _RENDER_PLUGIN_H_
#define _RENDER_PLUGIN_H_

#include "plugin.h"
#include "ui_render.h"

#include "Topology/generic/functor.h"
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

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

struct ParameterSet
{
	ParameterSet() :
		verticesScaleFactor(1.0f),
		renderVertices(false),
		renderEdges(false),
		renderFaces(true),
		faceStyle(FLAT)
	{}

	float verticesScaleFactor;
	bool renderVertices;
	bool renderEdges;
	bool renderFaces;
	FaceShadingStyle faceStyle;
};


class RenderDockTab : public QWidget, public Ui::RenderWidget
{
public:
	RenderDockTab() { setupUi(this); }

	void refreshUI(ParameterSet* params);
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

	virtual void keyPress(View* view, int key) {}
	virtual void keyRelease(View* view, int key) {}
	virtual void mousePress(View* view, int button, int x, int y) {}
	virtual void mouseRelease(View* view, int button, int x, int y) {}
	virtual void mouseMove(View* view, int buttons, int x, int y) {}
	virtual void wheelEvent(View* view, int delta, int x, int y) {}

	virtual void viewLinked(View* view);
	virtual void viewUnlinked(View* view);
	virtual void currentViewChanged(View* view);

	virtual void mapLinked(View* view, MapHandlerGen* m) {}
	virtual void mapUnlinked(View* view, MapHandlerGen* m) {}

protected:
	RenderDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	CGoGN::Utils::ShaderFlat* m_flatShader;
	CGoGN::Utils::ShaderPhong* m_phongShader;
	CGoGN::Utils::ShaderSimpleColor* m_simpleColorShader;
	CGoGN::Utils::PointSprite* m_pointSprite;

public slots:
	void cb_renderVerticesChanged(bool b);
	void cb_verticesScaleFactorChanged(int i);
	void cb_renderEdgesChanged(bool b);
	void cb_renderFacesChanged(bool b);
	void cb_faceStyleChanged(QAbstractButton* b);
};

#endif
