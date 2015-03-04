#ifndef _VOLUME_RENDER_PLUGIN_H_
#define _VOLUME_RENDER_PLUGIN_H_

#include "plugin_interaction.h"
#include "volume_render_dockTab.h"

#include "Algo/Render/GL2/explodeVolumeRender.h"

namespace CGoGN
{

namespace SCHNApps
{

class MapHandlerGen;

struct MapParameters
{
  enum FaceShadingStyle
  {
    FLAT = 0,
    SMOOTH = 1
  };

  MapParameters():
    m_renderExplod(NULL),
    facesScaleFactor(1.0f),
    volumesScaleFactor(1.0f),
    renderEdges(false),
    renderFaces(true),
    faceStyle(FLAT)
  {}

  Algo::Render::GL2::ExplodeVolumeRender* m_renderExplod;
  //  VertexAttribute<PFP3::VEC3> positionAttribute;
  //  VolumeAttribute<PFP3::VEC3> colorAttribute;

  float facesScaleFactor;
  float volumesScaleFactor;
  bool renderEdges;
  bool renderFaces;
  FaceShadingStyle faceStyle;

  //  	void updateRender();

};



class Volume_Render_Plugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
        Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
	friend class Volume_Render_DockTab;

public:
	Volume_Render_Plugin()
	{}

	~Volume_Render_Plugin()
	{}

private:
	virtual bool enable();
	virtual void disable();

	virtual void redraw(View *view);
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

public slots:
	// slots for Python calls
	void changeVerticesScaleFactor(const QString&view, const QString& map, float f);
	void changeRenderEdges(const QString& view, const QString& map, bool b);
	void changeRenderFaces(const QString& view, const QString& map, bool b);
	void changeFacesStyle(const QString& view, const QString& map, MapParameters::FaceShadingStyle style);


protected:
	Volume_Render_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
