#ifndef _VOLUME_IMPORT_PLUGIN_H_
#define _VOLUME_IMPORT_PLUGIN_H_

#include "plugin_processing.h"

namespace CGoGN
{

namespace SCHNApps
{

class Volume_Import_Plugin : public PluginProcessing
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	Volume_Import_Plugin()
	{}

	~Volume_Import_Plugin()
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
