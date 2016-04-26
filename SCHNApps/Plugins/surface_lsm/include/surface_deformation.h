#ifndef _SURFACE_DEFORMATION_PLUGIN_H_
#define _SURFACE_DEFORMATION_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_deformation_dockTab.h"

#include "mapHandler.h"

#include "Container/fakeAttribute.h"

#include "NL/nl.h"
#include "Algo/LinearSolving/basic.h"
#include "Eigen/Dense"

namespace CGoGN
{

namespace SCHNApps
{

typedef NoTypeNameAttribute<Eigen::Matrix3f> Eigen_Matrix3f;

struct MapParameters
{
	MapParameters();
	~MapParameters();

	void start(MapHandlerGen* mh);
	void stop(MapHandlerGen* mh);

	VertexAttribute<PFP2::VEC3, PFP2::MAP> positionAttribute;

	CellSelector<PFP2::MAP, VERTEX>* handleSelector;
	CellSelector<PFP2::MAP, VERTEX>* freeSelector;

	bool initialized;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> positionInit;

	VertexAttribute<unsigned int, PFP2::MAP> vIndex;
	unsigned int nb_vertices;

	NLContext nlContext;
};

class Surface_Deformation_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif

	friend class Surface_Deformation_DockTab;

public:
	Surface_Deformation_Plugin() :
		m_draginit(false),
		m_dragging(false)
	{}

	~Surface_Deformation_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view);
	virtual void drawMap(View* view, MapHandlerGen* map) {}

	virtual void keyPress(View* view, QKeyEvent* event);
	virtual void keyRelease(View* , QKeyEvent* ) {}
	virtual void mousePress(View* , QMouseEvent* ) {}
	virtual void mouseRelease(View* , QMouseEvent* ) {}
	virtual void mouseMove(View* view, QMouseEvent* event);
	virtual void wheelEvent(View* , QWheelEvent* ) {}

	virtual void viewLinked(View *view) {}
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	// slots called from MapHandler signals
	void attributeAdded(unsigned int orbit, const QString& name);
	void cellSelectorAdded(unsigned int orbit, const QString& name);
	void cellSelectorRemoved(unsigned int orbit, const QString& name);
	void selectedCellsChanged(CellSelectorGen *cs);

public slots:
	// slots for Python calls
	void changePositionAttribute(const QString& map, const QString& name);
	void changeHandleSelector(const QString& map, const QString& name);
	void changeFreeSelector(const QString& map, const QString& name);

protected:
	void toggleMapDeformation(MapHandlerGen* map);
	void lsm(MapHandlerGen* map);

protected:
	Surface_Deformation_DockTab* m_dockTab;
	QHash<MapHandlerGen*, MapParameters> h_parameterSet;

	bool m_draginit;
	bool m_dragging;
	PFP2::REAL m_dragZ;
	qglviewer::Vec m_dragPrevious;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
