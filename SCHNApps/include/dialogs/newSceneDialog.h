#ifndef _NEWSCENEDIALOG_H_
#define _NEWSCENEDIALOG_H_


#include "ui_newSceneDialog.h"


#include <QListWidget>

namespace myListCameraTypes{
	enum type {view, camera,scene};
}


class Window;

class NewSceneDialog : public QDialog, Ui::NSDialog{
	Q_OBJECT
public:
	NewSceneDialog(Window* parent);
	~NewSceneDialog();

protected:
	Window* m_window;

	QListWidget* m_listWidget;
	QListWidget* m_listWidget2;

public slots:
	void button_OK();
	void new_camera();
	void share_camera();
	void cameraListSelectionChange(QListWidgetItem* cur, QListWidgetItem* prev);
	void linkPlugin(bool b);
	void customContextMenu(const QPoint & pos);
	void showPluginInfo();

};

#endif
