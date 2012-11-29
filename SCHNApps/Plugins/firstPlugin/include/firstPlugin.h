#ifndef _FIRSTPLUGIN_H_
#define _FIRSTPLUGIN_H_

#include "plugin.h"

/**---CGoGN includes **/
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Algo/Render/GL2/topoRender.h"
/**---CGoGN includes **/

/**---Definitions specific to CGoGN ---*/
using namespace CGoGN ;

/**
 * Struct that contains some informations about the types of the manipulated objects
 * Mainly here to be used by the algorithms that are parameterized by it
 */
struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP;
};

typedef PFP::MAP MAP;
typedef PFP::VEC3 VEC3;
/**---Definitions specific to CGoGN ---*/

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
class FirstPlugin : public Plugin
{
	/**
	 * Essential Qt macros.
	 */
	Q_OBJECT
	Q_INTERFACES(Plugin)

public:
	FirstPlugin()
	{}

	~FirstPlugin()
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

	void cb_initGL(Scene *scene);
	virtual void cb_updateMatrix(View* view) {}
	void cb_redraw(Scene *scene);

	virtual void cb_keyPress(Scene* scene, int key) {}
	virtual void cb_keyRelease(Scene* scene, int key) {}
	virtual void cb_mousePress(Scene* scene, int button, int x, int y) {}
	virtual void cb_mouseRelease(Scene* scene, int button, int x, int y) {}
//	virtual void cb_mouseClick(Scene* scene, int button, int x, int y) {}
	virtual void cb_mouseMove(Scene* scene, int buttons, int x, int y) {}
	virtual void cb_wheelEvent(Scene* scene, int delta, int x, int y) {}

	virtual void cb_viewAdded(Scene* s) {}
	virtual void cb_viewRemoved(Scene* s) {}

protected:
	/** Attributes that are specific to this plugin **/
	MAP myMap;

	// attribute for vertices positions
	VertexAttribute<VEC3> position;

	// render (for the topo)
	Algo::Render::GL2::TopoRender *m_render_topo;

	// just for more compact writing
	inline Dart PHI1(Dart d)
	{
		return myMap.phi1(d);
	}
	inline Dart PHI_1(Dart d)
	{
		return myMap.phi_1(d);
	}
	inline Dart PHI2(Dart d)
	{
		return myMap.phi2(d);
	}
	template<int X>
	Dart PHI(Dart d)
	{
		return myMap.phi<X>(d);
	}
	/** Attributes that are specific to this plugin **/
};

#endif // _FIRSTPLUGIN_H_
