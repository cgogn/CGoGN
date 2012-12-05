#ifndef _PLUGINDIALOG_H_
#define _PLUGINDIALOG_H_

#include "ui_pluginDialog.h"

#include "types.h"

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

class PluginDialog : public QDialog, public Ui::PluginDialog
{
	Q_OBJECT

public:
	PluginDialog(Window* window);
	~PluginDialog();

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

#endif
