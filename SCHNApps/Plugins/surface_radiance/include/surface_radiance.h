#ifndef _SURFACE_RADIANCE_PLUGIN_H_
#define _SURFACE_RADIANCE_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_radiance_dockTab.h"

#include "Utils/sphericalHarmonics.h"
#include "Utils/Shaders/shaderRadiancePerVertex.h"

#include "Algo/Decimation/decimation.h"

namespace CGoGN
{

namespace SCHNApps
{

struct MapParameters
{
	MapParameters() :
		positionVBO(NULL),
		normalVBO(NULL),
		radianceTexture(NULL),
		paramVBO(NULL),
		positionApproximator(NULL),
		normalApproximator(NULL),
		radianceApproximator(NULL),
		selector(NULL)
	{}

	Utils::VBO* positionVBO;
	Utils::VBO* normalVBO;

	CGoGN::Utils::ShaderRadiancePerVertex* radiancePerVertexShader;

	VertexAttribute<Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>, PFP2::MAP> radiance;
	Utils::Texture<2, Geom::Vec3f>* radianceTexture;
	VertexAttribute<Geom::Vec2i, PFP2::MAP> param;
	Utils::VBO* paramVBO;

	DartAttribute<PFP2::REAL, PFP2::MAP> halfedgeError;

	Algo::Surface::Decimation::Approximator_HalfCollapse<PFP2, PFP2::VEC3>* positionApproximator;
	Algo::Surface::Decimation::Approximator_HalfCollapse<PFP2, PFP2::VEC3>* normalApproximator;
	Algo::Surface::Decimation::Approximator_HalfCollapse<PFP2, Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3> >* radianceApproximator;

	Algo::Surface::Decimation::Selector<PFP2>* selector;
};

class Surface_Radiance_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

	friend class Surface_Radiance_DockTab;

public:
	Surface_Radiance_Plugin()
	{}

	~Surface_Radiance_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view) {}
	virtual void drawMap(View* view, MapHandlerGen* map);

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

	virtual void viewLinked(View *view) {}
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedViewChanged(View* prev, View* cur);
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	// slots called from MapHandler signals
	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);
	void attributeModified(unsigned int orbit, QString nameAttr);

	void importFromFileDialog();

public slots:
	// slots for Python calls
	void changePositionVBO(const QString& map, const QString& vbo);
	void changeNormalVBO(const QString& map, const QString& vbo);
	MapHandlerGen* importFromFile(const QString& fileName);
	void decimate(const QString& mapName, const QString& positionAttributeName, const QString& normalAttributeName);

protected:
	Surface_Radiance_DockTab* m_dockTab;
	QHash<MapHandlerGen*, MapParameters> h_mapParameterSet;

	QAction* m_importAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif // _SURFACE_RADIANCE_PLUGIN_H_
