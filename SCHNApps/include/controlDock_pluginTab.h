#ifndef _CONTROL_DOCK_PLUGIN_TAB_H_
#define _CONTROL_DOCK_PLUGIN_TAB_H_

#include "ui_controlDock_PluginTabWidget.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Plugin;
class PluginInteraction;
class View;

class ControlDock_PluginTab : public QWidget, public Ui::ControlDock_PluginTabWidget
{
	Q_OBJECT

public:
	ControlDock_PluginTab(SCHNApps* s);
	QString title() { return QString("Plugins"); }

private slots:
	// slots called from UI actions
	void addPluginDirectoryClicked();
	void enableSelectedPluginsClicked();
	void disableSelectedPluginsClicked();
	void pluginCheckStateChanged(QListWidgetItem* item);

	// slots called from SCHNApps signals
	void pluginAvailableAdded(QString name);
	void pluginEnabled(Plugin* plugin);
	void pluginDisabled(Plugin* plugin);
	void selectedViewChanged(View* prev, View* cur);

	// slots called from selected View signals
	void selectedViewPluginLinked(PluginInteraction* plugin);
	void selectedViewPluginUnlinked(PluginInteraction* plugin);

private:
	SCHNApps* m_schnapps;
	bool b_updatingUI;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
