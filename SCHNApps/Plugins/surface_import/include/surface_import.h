#ifndef _IMPORTSURFACE_PLUGIN_H_
#define _IMPORTSURFACE_PLUGIN_H_

#include "plugin_processing.h"

namespace CGoGN
{

namespace SCHNApps
{

class ImportSurfacePlugin : public PluginProcessing
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	ImportSurfacePlugin()
	{}

	~ImportSurfacePlugin()
	{}

private:
	virtual bool enable();
	virtual void disable() {}

public slots:
	MapHandlerGen* importFromFile(const QString& fileName);
	void importFromFileDialog();

private:
	QAction* importAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif

