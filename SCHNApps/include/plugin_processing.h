#ifndef _PLUGIN_PROCESSING_H_
#define _PLUGIN_PROCESSING_H_

#include "plugin.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;

class PluginProcessing : public Plugin
{
	Q_OBJECT

public:
	PluginProcessing();
	virtual ~PluginProcessing();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
