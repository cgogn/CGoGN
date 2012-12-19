#ifndef _RENDERVECTOR_PLUGIN_H_
#define _RENDERVECTOR_PLUGIN_H_

#include "plugin.h"
#include "ui_renderVector.h"

//#include "Topology/generic/functor.h"
//#include "Topology/generic/parameters.h"
//#include "Topology/map/embeddedMap2.h"

#include "Utils/Shaders/shaderVectorPerVertex.h"


using namespace CGoGN;
using namespace SCHNApps;


struct PerMapParameterSet
{
	PerMapParameterSet() : positionVBO(NULL), vectorVBO(NULL), vectorsScaleFactor(1.0f)
	{}

	PerMapParameterSet(Utils::VBO* pos, Utils::VBO* vec, float s) :
		positionVBO(pos),
		vectorVBO(vec),
		vectorsScaleFactor(s)
	{}

	Utils::VBO* positionVBO;
	Utils::VBO* vectorVBO;
	float vectorsScaleFactor;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	QHash<QString, PerMapParameterSet> perMap;
	MapHandlerGen* selectedMap;
};


class RenderVectorPlugin;

class RenderVectorDockTab : public QWidget, public Ui::RenderVectorWidget
{
public:
	RenderVectorDockTab(RenderVectorPlugin* p) : plugin(p)
	{
		setupUi(this);
	}

	void refreshUI(ParameterSet* params);

private:
	RenderVectorPlugin* plugin;
};


class RenderVectorPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderVectorPlugin() : b_refreshingUI(false)
	{
		setProvidesRendering(true);
	}

	~RenderVectorPlugin()
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
	RenderVectorDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	CGoGN::Utils::ShaderVectorPerVertex* m_vectorShader;

	bool b_refreshingUI;

public slots:
	void cb_selectedMapChanged();
	void cb_positionVBOChanged(int index);
	void cb_vectorVBOChanged(int index);
	void cb_refreshVBOs();
	void cb_vectorsScaleFactorChanged(int i);
};

#endif
