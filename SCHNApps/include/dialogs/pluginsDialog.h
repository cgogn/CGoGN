#ifndef _PLUGINSDIALOG_H_
#define _PLUGINSDIALOG_H_

#include "ui_pluginsDialog.h"

#include "types.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

class PluginsDialog : public QDialog, public Ui::PluginsDialog
{
	Q_OBJECT

public:
	PluginsDialog(Window* window);
	~PluginsDialog();

	void refreshPluginsList();

protected:
	Window* m_window;

	bool b_refreshingUI;

protected slots:
	void registerPluginsDirectory();
	void togglePlugin(QListWidgetItem* item);

	void checkPlugin(Plugin* plugin);
	void uncheckPlugin(Plugin* plugin);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
