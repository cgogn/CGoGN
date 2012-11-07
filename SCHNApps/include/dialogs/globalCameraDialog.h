#ifndef _GLOBALCAMERADIALOG_H_
#define _GLOBALCAMERADIALOG_H_



#include "ui_globalCameraDialog.h"


#include <QListWidget>
#include <QTreeWidget>
#include <QVariant>
#include <QMenu>


#include "camera.h"
#include "view.h"

class Window;


struct CarryCamera{
	Camera* camera;
	View* view;

	CarryCamera(Camera* camera=NULL, View* view=NULL) : camera(camera), view(view){}
};


namespace Qt{
	extern int UserRole2;
}

namespace myDragDropWidgets{
	enum type {view, camera,scene};
}

class MyCameraMenu : public QMenu{
	Q_OBJECT
public:
	enum ResultType{none,move,share};
	enum Mode{moving=1, sharing=2};


	MyCameraMenu(const QString & title, QWidget * parent = 0, int mode=(moving | sharing));

	ResultType result(){return m_result;}

protected:
	QAction* m_moveAction;
	QAction* m_shareAction;

	ResultType m_result;

protected slots:
	void moveCamera(){m_result=move;}
	void shareCamera(){m_result=share;}
};

class MyDragListWidget : public QListWidget{
	Q_OBJECT
public:
	MyDragListWidget(QWidget* parent=0);
	~MyDragListWidget(){}

protected:
	QMimeData* mimeData(const QList<QListWidgetItem *> items) const;
};

class MyDropTreeWidget : public QTreeWidget{
	Q_OBJECT
public:
	MyDropTreeWidget(QWidget* parent=0);
	~MyDropTreeWidget(){}


protected:
	virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions () const;

};

class GlobalCameraDialog : public QDialog, Ui::GCDialog{
	Q_OBJECT
public:
	GlobalCameraDialog(Window* window);
	~GlobalCameraDialog(){}

	MyDropTreeWidget* treeWidget;
	MyDragListWidget* listWidget;

public slots:
	void treeCurrentItemChanged(QTreeWidgetItem* cur,QTreeWidgetItem* prev);
};


#endif
