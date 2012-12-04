#ifndef _FIRSTPLUGIN_H_
#define _FIRSTPLUGIN_H_

#include "plugin.h"

/**---CGoGN includes **/
#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderFlat.h"
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
	{
		setProvidesRendering(true);
	}

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

	virtual void redraw(View *view);

	virtual void keyPress(View* view, int key) {}
	virtual void keyRelease(View* view, int key) {}
	virtual void mousePress(View* view, int button, int x, int y) {}
	virtual void mouseRelease(View* view, int button, int x, int y) {}
//	virtual void mouseClick(View* view, int button, int x, int y) {}
	virtual void mouseMove(View* view, int buttons, int x, int y) {}
	virtual void wheelEvent(View* view, int delta, int x, int y) {}

	virtual void viewAdded(View* view);
	virtual void viewRemoved(View* view) {}

protected:
	MAP myMap;
	VertexAttribute<VEC3> position;
	Algo::Render::GL2::MapRender* m_render ;
	Utils::VBO* m_positionVBO ;
	Utils::ShaderFlat* m_flatShader ;
};

#endif // _FIRSTPLUGIN_H_
