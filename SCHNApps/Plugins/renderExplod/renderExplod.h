#ifndef _RENDER_PLUGIN_H_
#define _RENDER_PLUGIN_H_

#include "plugin.h"
#include "ui_renderExplod.h"

#include "Algo/Render/GL2/explodeVolumeRender.h"

using namespace CGoGN;
using namespace SCHNApps;


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


class RenderExplodPlugin;

class RenderExplodDockTab : public QWidget, public Ui::RenderExplodWidget
{
public:
	RenderExplodDockTab(RenderExplodPlugin* p) : plugin(p)
	{
		setupUi(this);
	}

	void refreshUI(ParameterSet* params);

private:
	RenderExplodPlugin* plugin;
};


class RenderExplodPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderExplodPlugin() : b_refreshingUI(false)
	{
		setProvidesRendering(true);
	}

	~RenderExplodPlugin()
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
	RenderExplodDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	bool b_refreshingUI;

public slots:
	void cb_selectedMapChanged();
	void cb_positionVBOChanged(int index);
	void cb_colorVBOChanged(int index);
	void cb_refreshVBOs();
	void cb_renderEdgesChanged(bool b);
	void cb_renderFacesChanged(bool b);
	void cb_facesScaleFactorChanged(int i);
	void cb_volumesScaleFactorChanged(int i);
};

#endif
