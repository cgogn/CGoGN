#include "firstPlugin.h"
#include "Algo/Geometry/boundingbox.h"

bool FirstPlugin::enable()
{
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

	m_render_topo = new Algo::Render::GL2::TopoRender() ;

	SelectorDartNoBoundary<PFP::MAP> nb(myMap);
	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f, nb);

	return true;
}

void FirstPlugin::disable()
{
	if (m_render_topo)
	{
		delete m_render_topo;
	}
}

void FirstPlugin::redraw(View* view)
{
	m_render_topo->drawTopo();

//	const float nbSteps = 200.0;
//	glBegin(GL_QUAD_STRIP);
//	for (float i = 0; i < nbSteps; ++i)
//	{
//		float ratio = i/nbSteps;
//		float angle = 21.0*ratio;
//		float c = cos(angle);
//		float s = sin(angle);
//		float r1 = 1.0 - 0.8f*ratio;
//		float r2 = 0.8f - 0.8f*ratio;
//		float alt = ratio - 0.5f;
//		const float nor = 0.5f;
//		const float up = sqrt(1.0-nor*nor);
//		glColor3f(1.0-ratio, 0.2f , ratio);
//		glNormal3f(nor*c, up, nor*s);
//		glVertex3f(r1*c, alt, r1*s);
//		glVertex3f(r2*c, alt+0.05f, r2*s);
//	}
//	glEnd();
}

void FirstPlugin::viewAdded(View* view)
{
	// bounding box of scene
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);

	PFP::VEC3& min = bb.min();
	PFP::VEC3& max = bb.max();
	qglviewer::Vec minV(min[0], min[1], min[2]);
	qglviewer::Vec maxV(max[0], max[1], max[2]);

	view->setSceneBoundingBox(minV, maxV);
}

/**
 * If we want to compile this plugin in debug mode,
 * we also define a DEBUG macro at the compilation
 */
#ifndef DEBUG
// essential Qt function:
// arguments are
//  - the complied name of the plugin
//  - the main class of our plugin
Q_EXPORT_PLUGIN2(FirstPlugin, FirstPlugin)
#else
Q_EXPORT_PLUGIN2(FirstPluginD, FirstPlugin)
#endif
