#include "firstPlugin.h"

#include "Algo/Geometry/boundingbox.h"


bool FirstPlugin::activate(){
	// creation of 2 new faces: 1 triangle and 1 square
		Dart d1 = myMap.newFace(3);
		Dart d2 = myMap.newFace(4);

		// sew these faces along one of their edge
		myMap.sewFaces(d1, d2);

		// creation of a new attribute on vertices of type 3D vector for position.
		// a handler to this attribute is returned
		position = myMap.addAttribute<VEC3, VERTEX>("position");

		// affect position by moving in the map
		position[d1] = VEC3(0, 0, 0);
		position[PHI1(d1)] = VEC3(2, 0, 0);
		position[PHI_1(d1)] = VEC3(1, 2, 0);
		position[PHI<11>(d2)] = VEC3(0, -2, 0);
		position[PHI_1(d2)] = VEC3(2, -2, 0);

		m_render_topo=NULL;

		return true;
}

void FirstPlugin::disable(){
	if(m_render_topo){
		delete m_render_topo;
	}
}

void FirstPlugin::cb_redraw(Scene* scene){
	m_render_topo->drawTopo();
}

void FirstPlugin::cb_initGL(Scene* scene){
	if(scene){
		//we fit the first (possibly the only) view of the newly liked
		//scene to the content of our map

		//  bounding box of scene
		Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);

		scene->firstViewFitSphere(bb.center()[0], bb.center()[1], bb.center()[2], bb.maxSize());



		m_render_topo = new Algo::Render::GL2::TopoRender() ;

		// render the topo of the map without boundary darts
		SelectorDartNoBoundary<PFP::MAP> nb(myMap);
		m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f, nb);

	}
}


/**
 * If we want to compile this plugin in debug mode,
 * we also define a DEBUG macro at the compilation
 */
#ifndef DEBUG
	//essential Qt function:
	//arguments are
	//	- the complied name of the plugin
	//	- the main class of our plugin (that extends VisualPlugin)
	Q_EXPORT_PLUGIN2(FirstPlugin, FirstPlugin)
#else
	Q_EXPORT_PLUGIN2(FirstPluginD, FirstPlugin)
#endif
