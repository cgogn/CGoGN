#ifndef _LINKVIEWDIALOG_H_
#define _LINKVIEWDIALOG_H_

#include "ui_linkViewDialog.h"

#include "types.h"

class Window;
class Scene;

class LinkViewDialog : public QDialog, Ui::LVDialog{
	Q_OBJECT
public:
	LinkViewDialog(Window* parent, PluginHash* activePlugins, Scene* scene);
	LinkViewDialog(Window* parent, QList<Plugin*> dependingPlugin, Scene* scene);
	~LinkViewDialog(){}

protected:
	enum Mode{LINK, UNLINK};

	QList<Plugin*> l_waitingPlugins;
	Scene* m_scene;
	Mode m_mode;

public slots:
	void button_OK();
	void customContextMenu(const QPoint & pos);
	void showPluginInfo();

};

#endif
