#ifndef _RENDERVECTOR_PLUGIN_H_
#define _RENDERVECTOR_PLUGIN_H_

#include "plugin.h"
#include "ui_renderVector.h"

#include "Utils/Shaders/shaderVectorPerVertex.h"


using namespace CGoGN;
using namespace SCHNApps;


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

	void setRefreshingUI(bool b) { b_refreshingUI = b; }

protected:
	RenderVectorDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	CGoGN::Utils::ShaderVectorPerVertex* m_vectorShader;

	bool b_refreshingUI;

public slots:
	void mapLinked(MapHandlerGen* m);
	void mapUnlinked(MapHandlerGen* m);
	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);

	void changeSelectedMap(View* view, MapHandlerGen* map);
	void changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo);
	void changeSelectedVectorsVBO(View* view, MapHandlerGen* map, const std::vector<Utils::VBO*>& vbos);
	void changeVectorsScaleFactor(View* view, MapHandlerGen* map, int i);

	void cb_selectedMapChanged();
	void cb_positionVBOChanged(int index);
	void cb_selectedVectorsVBOChanged();
	void cb_vectorsScaleFactorChanged(int i);
};

#endif
