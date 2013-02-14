#ifndef _SUBDIVIDESURFACE_PLUGIN_H_
#define _SUBDIVIDESURFACE_PLUGIN_H_

#include "plugin.h"
#include "subdivideSurfaceDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class SubdivideSurfacePlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	SubdivideSurfacePlugin()
	{
		setProvidesRendering(false);
	}

	~SubdivideSurfacePlugin()
	{}

	virtual bool enable();
	virtual void disable() {}

	virtual void redraw(View *view) {}

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

public slots:
	void openSubdivideSurfaceDialog();
	void subdivideSurfaceFromDialog();

	void loopSubdivision(
		const QString& mapName,
		const QString& positionAttributeName = "position"
	);
	void CCSubdivision(
		const QString& mapName,
		const QString& positionAttributeName = "position"
	);
	void trianguleFaces(
		const QString& mapName,
		const QString& positionAttributeName = "position"
	);

//	void loopSubdivision(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position);
//	void CCSubdivision(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position);
//	void trianguleFaces(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position);

private:
	SubdivideSurfaceDialog* m_subdivideSurfaceDialog;

	QAction* m_subdivideSurfaceAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
