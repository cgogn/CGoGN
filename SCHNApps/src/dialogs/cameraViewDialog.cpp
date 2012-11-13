#include "cameraViewDialog.h"

#include "camera.h"
#include "cameraOptionDialog.h"



Q_DECLARE_METATYPE(Camera*);

CameraViewDialog::CameraViewDialog(View* view, QWidget* parent) : QDialog(parent),
	view(view),
	autoText(false)
{
	this->setupUi(this);

	connect(add_Button,SIGNAL(clicked()), this, SLOT(addCamera()));
	connect(remove_Button, SIGNAL(clicked()), this, SLOT(removeCamera()));
	connect(settings_Button, SIGNAL(clicked()), this, SLOT(settingsCamera()));

	if(view){
		QList<Camera*> cameraList= view->cameras();
		foreach(Camera* c, cameraList){
			QVariant var;
			var.setValue(c);

			QListWidgetItem* item= new QListWidgetItem(listWidget);
			item->setData(Qt::UserRole, var);
			autoText=true;
			item->setText(c->getName());
			autoText=false;
			item->setFlags (item->flags () | Qt::ItemIsEditable);

			if(view->currentCamera()==c){
				listWidget->setCurrentItem(item);
			}

		}

		connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
				this, SLOT(changeCurrentCamera(QListWidgetItem*, QListWidgetItem*)));
		connect(listWidget, SIGNAL(itemChanged ( QListWidgetItem *)),
				this, SLOT(cameraNameChange(QListWidgetItem *)));
	}


}

CameraViewDialog::~CameraViewDialog(){}

void CameraViewDialog::addCamera(){
	if(view){
		Camera* c= view->addCamera();
		QVariant var;
		var.setValue(c);

		QListWidgetItem* item= new QListWidgetItem(listWidget);
		autoText=true;
		item->setData(Qt::UserRole, var);
		item->setText(c->getName());
		autoText=false;
		item->setFlags (item->flags () | Qt::ItemIsEditable);
	}
}

void CameraViewDialog::removeCamera(){
	QListWidgetItem* item;
	if(view && (item=listWidget->currentItem()) && listWidget->count()>1){
		Camera* c= item->data(Qt::UserRole).value<Camera*>();

		delete item;

		view->removeCamera(c);
	}
}

void CameraViewDialog::changeCurrentCamera(QListWidgetItem * current, QListWidgetItem * previous){
	if(current && view){
		Camera* c= current->data(Qt::UserRole).value<Camera*>();

		view->setCurrentCamera(c);
	}
}

void CameraViewDialog::cameraNameChange(QListWidgetItem * item){
	if(!autoText && item){
		Camera* c= item->data(Qt::UserRole).value<Camera*>();
		if(c){
			c->setName(item->text());
		}
	}
}

void CameraViewDialog::settingsCamera(){
	QListWidgetItem* item= listWidget->currentItem();
	if(view && item){
		Camera* c= item->data(Qt::UserRole).value<Camera*>();

		CameraOptionDialog cod(c,this);
		cod.exec();
	}
}
