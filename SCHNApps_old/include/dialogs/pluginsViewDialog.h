#ifndef _PLUGINSVIEWDIALOG_H_
#define _PLUGINSVIEWDIALOG_H_

#include "ui_pluginsViewDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class View;
class Plugin;

class PluginsViewDialog : public QDialog, Ui::PluginsViewDialog
{
	Q_OBJECT

public:
	PluginsViewDialog(Window* window, View* view);
	~PluginsViewDialog();

private:
	Window* m_window;
	View* m_view;
	bool b_refreshingUI;

public slots:
	void selectedPluginsChanged();
	void selectPlugin(View* view, Plugin* plugin);
	void deselectPlugin(View* view, Plugin* plugin);
	void addPluginToList(Plugin* p);
	void removePluginFromList(Plugin* p);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
