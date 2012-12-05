#ifndef _DEMO_PLUGIN_H_
#define _DEMO_PLUGIN_H_

#include "plugin.h"
#include "ui_demo.h"

#include "Topology/generic/functor.h"
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"

#include "Utils/Shaders/shaderFlat.h"


struct PFP: public CGoGN::PFP_STANDARD
{
	// definition of the map
	typedef CGoGN::EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP;
typedef PFP::VEC3 VEC3;


class DemoDockTab : public QWidget, public Ui::DemoWidget
{
public:
	DemoDockTab() { setupUi(this); }
};


struct TabParams
{
	bool b_showVertices;
	bool b_showEdges;
};


/**
 * This class is a basic minimal plugin.
 * All the methods in this class are overloaded methods.
 * In order to create a valid plugin, all the method in this
 * needs to be declared (they are actually overloaded methods
 * from VisualPlugin), even if your plugin doesn't make any
 * drawing.
 */

/**
 * Our plugin must inherit from Plugin
 */
class DemoPlugin : public Plugin
{
	/**
	 * Essential Qt macros.
	 */
	Q_OBJECT
	Q_INTERFACES(Plugin)

public:
	DemoPlugin()
	{
		setProvidesRendering(true);
	}

	~DemoPlugin()
	{}

	/**
	 * The plugin's enable method
	 * Each time the main application loads this plugin,
	 * it call this method. Writing this method is
	 * the occasion to initialize the plugin and check certain
	 * conditions.
	 * If this methods return 'false', the plugin load will be aborted.
	 */
	bool enable();

	/**
	 * The plugin's disable method
	 * Each time the main application will unload the plugin
	 * it will call this method.
	 */
	void disable();

	virtual void redraw(View *view);

	virtual void keyPress(View* view, int key) {}
	virtual void keyRelease(View* view, int key) {}
	virtual void mousePress(View* view, int button, int x, int y) {}
	virtual void mouseRelease(View* view, int button, int x, int y) {}
//	virtual void mouseClick(View* view, int button, int x, int y) {}
	virtual void mouseMove(View* view, int buttons, int x, int y) {}
	virtual void wheelEvent(View* view, int delta, int x, int y) {}

	virtual void viewLinked(View* view) {}
	virtual void viewUnlinked(View* view) {}

protected:
	CGoGN::Utils::ShaderFlat* m_flatShader ;
};

#endif // _FIRSTPLUGIN_H_
