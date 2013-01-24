#ifndef _SUBDIVIDESURFACE_PLUGIN_H_
#define _SUBDIVIDESURFACE_PLUGIN_H_

#include "plugin.h"

#include "subdivideSurfaceDialog.h"


using namespace CGoGN;
using namespace SCHNApps;


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

	virtual void keyPress(View* view, int key) {}
	virtual void keyRelease(View* view, int key) {}
	virtual void mousePress(View* view, int button, int x, int y) {}
	virtual void mouseRelease(View* view, int button, int x, int y) {}
	virtual void mouseMove(View* view, int buttons, int x, int y) {}
	virtual void wheelEvent(View* view, int delta, int x, int y) {}

public slots:
	void openSubdivideSurfaceDialog();
	void subdivideSurface();

	void loopSubdivision(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position);
	void CCSubdivision(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position);
	void trianguleFaces(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position);

private:
	SubdivideSurfaceDialog* m_subdivideSurfaceDialog;

	QAction* m_subdivideSurfaceAction;
};

#endif
