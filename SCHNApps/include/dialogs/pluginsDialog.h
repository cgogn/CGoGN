#ifndef _PLUGINSDIALOG_H_
#define _PLUGINSDIALOG_H_

#include "ui_pluginsDialog.h"

#include "types.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

struct PluginInfo
{
	PluginInfo(const QString& path, const QString& name) :
		pluginPath(path),
		pluginName(name)
	{}

	PluginInfo() {}

	QString pluginPath;
	QString pluginName;
};

class PluginsDialog : public QDialog, public Ui::PluginsDialog
{
	Q_OBJECT

public:
	PluginsDialog(Window* window);
	~PluginsDialog();

protected:
	Window* m_window;
	QHash<QTreeWidgetItem*, PluginInfo> m_listedPlugins;

//	bool restoreState();

private:
	bool init;

	enum EntryType
	{
		DIR = 1,
		FILE,
		FILE_DIR
	};

protected slots:
	void cb_addPlugins();
	void cb_removePlugins();
	void cb_addPluginsDirectory();

	void cb_togglePlugin(QTreeWidgetItem* item, int column);

//	void customContextMenu(const QPoint & pos);

//	void cb_acceptDialog();

//	void showPluginInfo();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
