#ifndef _SEPARATECAMERADIALOG_H_
#define _SEPARATECAMERADIALOG_H_


#include "ui_cameraSceneDialog.h"

#include <QTreeWidget>
#include <QVariant>

class Scene;
class View;

namespace Qt{
	extern int UserRoleType;
}

namespace myTreeWidget{
	enum type {view, camera};
}

class MyTreeWidget : public QTreeWidget{
	Q_OBJECT
public:
	MyTreeWidget(Scene* originalScene, QWidget* parent=0);
protected:
	virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions () const;
	QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const;

	Scene* m_scene;
};

class CameraSceneDialog : public QDialog, Ui::CSDialog{
	Q_OBJECT
public:
	CameraSceneDialog(Scene* scene, QWidget* parent=0);
	~CameraSceneDialog(){}

	MyTreeWidget* treeWidget;
	Scene* m_scene;

protected slots:
	void newDuplicatedView();
	void removeView();
	void currentItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem* prev);

};


#endif
