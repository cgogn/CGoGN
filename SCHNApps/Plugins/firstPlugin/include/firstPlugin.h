#ifndef FIRSTPLUGIN_H_
#define FIRSTPLUGIN_H_



#include "visualPlugin.h"


/**---CGoGN includes **/
	#include "Utils/Qt/qtSimple.h"
	#include "Utils/cgognStream.h"

	#include "Topology/generic/parameters.h"

	#ifdef USE_GMAP
		#include "Topology/gmap/embeddedGMap2.h"
	#else
		#include "Topology/map/embeddedMap2.h"
	#endif

	#include "Algo/Render/GL2/topoRender.h"
/**---CGoGN includes **/


/**---Definitions  specific to CGoGN ---*/
	using namespace CGoGN ;

	/**
	 * Struct that contains some informations about the types of the manipulated objects
	 * Mainly here to be used by the algorithms that are parameterized by it
	 */
	struct PFP: public PFP_STANDARD
	{
		// definition of the type of the map
	#ifdef USE_GMAP
		typedef EmbeddedGMap2 MAP;
	#else
		typedef EmbeddedMap2 MAP;
	#endif
	};

	typedef PFP::MAP MAP;
	typedef PFP::VEC3 VEC3;
/**---Definitions  specific to CGoGN ---*/

/**
 * This class is a basic minimal plugin.
 * All the methods in this class are overloaded methods.
 * In order to create a valid plugin, all the method in this
 * needs to be declared (they are actually overloaded methods
 * from VisualPlugin), even if your plugin doesn't make any
 * drawing.
 */

/**
 * Our plugin must inherit from VisualPlugin,
 * that is a class that itself is an implementation
 * of the Plugin interface (virtual class). It contains
 * many useful and essantial methods.
 */
class FirstPlugin : public VisualPlugin{
	/**
	 * Essential Qt macros.
	 */
	Q_OBJECT
	Q_INTERFACES(Plugin)
public:
	FirstPlugin(){}
	~FirstPlugin(){}

	/**
	 * The classical call back for the initGL method
	 * When a scene will be link to this plugin, it will call
	 * back this method with itself as a parameter.
	 */
	void cb_initGL(Scene* scene);

	/**
	 * The drawing method that needs to be overloaded.
	 * Each time a scene (that is to say, at least one of the
	 * views that is contains) needs to be refresh, it calls back
	 * this method with itself as a parameter
	 */
	void cb_redraw(Scene* scene);

	/**
	 * The plugin's activation method
	 * Each time the main application loads this plugin,
	 * it call this method. Writing this method is
	 * the occasion to initialize the plugin and check certain
	 * conditions.
	 * If this methods return 'false', the plugin load will be aborted.
	 */
	bool activate();
	/**
	 * The plugin's disabling method
	 * Each time the main application will unload the plugin
	 * it will call this method.
	 */
	void disable();


protected:
	/** Attributes that are specific to this plugin **/
		MAP myMap;

		// attribute for vertices positions
		VertexAttribute<VEC3> position;

		// render (for the topo)
		Algo::Render::GL2::TopoRender* m_render_topo;

		// just for more compact writing
		inline Dart PHI1(Dart d) { return myMap.phi1(d); }
		inline Dart PHI_1(Dart d) { return myMap.phi_1(d); }
		inline Dart PHI2(Dart d) { return myMap.phi2(d); }
		template<int X>
		Dart PHI(Dart d) { return myMap.phi<X>(d); }
	/** Attributes that are specific to this plugin **/
};

#endif /* FIRSTPLUGIN_H_ */
