#include "cameraSceneDialog.h"

#include "scene.h"
#include "view.h"
#include "camera.h"


#include <QMetaType>
#include <QMessageBox>

Q_DECLARE_METATYPE(Camera*)
Q_DECLARE_METATYPE(View*)

QDataStream &operator<<(QDataStream &out, Camera* &c){
	return out << (quintptr)(c);
}

QDataStream &operator>>(QDataStream &in, Camera* &c){
	return in >> reinterpret_cast<quintptr&>(c);
}


MyTreeWidget::MyTreeWidget(Scene* originalScene, QWidget* parent) : QTreeWidget(parent),
		m_scene(originalScene)
{
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);

	QTreeWidgetItem* root= invisibleRootItem();
	root->setFlags(root->flags() & ~Qt::ItemIsDropEnabled);
}

bool MyTreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action){
	QTreeWidgetItem *item;


	Camera* c;
	QByteArray ba= data->data("Camera*");
	QDataStream st(&ba, QIODevice::ReadOnly);
	while(!st.atEnd()){
		st >> c;
		QTreeWidgetItem* curItem= this->currentItem();
		QTreeWidgetItem* curParent= curItem->parent();
		if(curParent!=parent){
			View* cur_view= curParent->data(0,Qt::UserRole).value<View*>();
			cur_view->takeCamera(c);
			delete (curItem);
			if(curParent->childCount()<1){
				m_scene->deleteView(cur_view);
			}
		}
		else{
			delete (curItem);
		}

		if(parent==NULL){
			return false;
		}
		int t= parent->data(0,Qt::UserRoleType).value<int>();
		if (t==myTreeWidget::view){
			View* view= parent->data(0,Qt::UserRole).value<View*>();
			if(parent->childCount()==0 || !view){
				QVariant var;
				View* newView= m_scene->addNewView(c);
				var.setValue(newView);
				parent->setData(0,Qt::UserRole, var);
				parent->setText(0,newView->getName());
				newView->enableSceneCameraGesture(m_scene->countViews()>1 || view->countCameras()>1);
			}
			else{
				view->insertCamera(index,c);
				view->enableSceneCameraGesture(m_scene->countViews()>1 || view->countCameras()>1);
			}


			item= new QTreeWidgetItem();
			parent->insertChild(index,item);
			QVariant var,varType;
			var.setValue(c);
			varType.setValue((int)(myTreeWidget::camera));
			item->setData(0,Qt::UserRole, var);
			item->setData(0,Qt::UserRoleType,varType);
			if(c->isShared()){
				item->setText(0,c->getName()+" (shared)");
			}
			else{
				item->setText(0,c->getName());
			}
			item->setFlags((item->flags() | Qt::ItemIsDragEnabled) & ~Qt::ItemIsDropEnabled);
			parent->setExpanded(true);
		}
		else{
		}
	}

	return true;
}

QStringList MyTreeWidget::mimeTypes() const{
    QStringList qstrList;
    // list of accepted mime types for drop
    qstrList.append("Camera*");
    return qstrList;
}

QMimeData* MyTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const{
    QByteArray ba;
    QDataStream st(&ba, QIODevice::WriteOnly);

    foreach(QTreeWidgetItem * item, items){
    	Camera* c= item->data(0,Qt::UserRole).value<Camera*>();
    	st << c;
    }

    QMimeData* md= new QMimeData();
    md->setData("Camera*",ba);


    return md;
}

Qt::DropActions MyTreeWidget::supportedDropActions () const{
    // returns what actions are supported when dropping
    return (Qt::MoveAction | Qt::CopyAction | Qt::TargetMoveAction);
}

CameraSceneDialog::CameraSceneDialog(Scene* scene, QWidget* parent) :
	QDialog(parent),
	m_scene(scene)
{
	this->setupUi(this);

	treeWidget= new MyTreeWidget(m_scene, groupBox);
    treeWidget->setObjectName(QString::fromUtf8("treeView"));

    horizontalLayout->insertWidget(0,treeWidget);

    qRegisterMetaTypeStreamOperators<QTreeWidgetItem>("Camera*");
    if(m_scene){
    	QList<View*> listViews= m_scene->views();
    	foreach(View* view, listViews){
        	QTreeWidgetItem* viewItem= new QTreeWidgetItem(treeWidget);
    		QVariant varScene, varType;
    		varType.setValue((int)(myTreeWidget::view));
    		varScene.setValue(view);
    		viewItem->setData(0,Qt::UserRoleType, varType);
    		viewItem->setData(0,Qt::UserRole, varScene);
    		viewItem->setText(0,m_scene->getName());
    		viewItem->setFlags((viewItem->flags() | Qt::ItemIsDropEnabled) & (~Qt::ItemIsDragEnabled));

    		QList<Camera*> listCameras= view->cameras();
    		foreach(Camera* camera, listCameras){
				QTreeWidgetItem* item= new QTreeWidgetItem(viewItem);
				QVariant var, varT;
				var.setValue(camera);
				varT.setValue((int)(myTreeWidget::camera));
				item->setData(0, Qt::UserRole, var);
				item->setData(0, Qt::UserRoleType, varT);
				if(camera->isShared()){
					item->setText(0,camera->getName()+" (shared)");
				}
				else{
					item->setText(0,camera->getName());
				}
				item->setFlags((item->flags() | Qt::ItemIsDragEnabled) & ~Qt::ItemIsDropEnabled);
    		}
    	}
    }

    connect(addView_Button, SIGNAL(pressed()),this, SLOT(newDuplicatedView()));
    connect(removeView_Button, SIGNAL(pressed()), this, SLOT(removeView()));
    connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
    		this, SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

void CameraSceneDialog::newDuplicatedView(){
	int n= treeWidget->topLevelItemCount() + 1;
	QTreeWidgetItem* item= new QTreeWidgetItem(treeWidget);
	QVariant var, varType;
	var.setValue((View*)NULL);
	varType.setValue((int)(myTreeWidget::view));
	item->setData(0, Qt::UserRoleType, varType);
	item->setData(0, Qt::UserRole, var);
	item->setText(0, m_scene->getName()+'_'+QString::number(n));
	item->setFlags((item->flags() | Qt::ItemIsDropEnabled) & (~Qt::ItemIsDragEnabled));
}

void CameraSceneDialog::removeView(){
	QTreeWidgetItem* curItem= treeWidget->currentItem();
	if(curItem && curItem->data(0,Qt::UserRoleType).value<int>()== myTreeWidget::view){
		View* curItemView= curItem->data(0, Qt::UserRole).value<View*>();
		if(!curItemView){
			delete curItem;
		}
		else if(m_scene->countViews()>1){
			m_scene->deleteView(curItemView);
			delete curItem;
			if(m_scene->countViews()<=1){
				m_scene->getView(0)->enableSceneCameraGesture(false);
			}
		}
		else{
			QMessageBox err(this);
			err.setText(QString::fromUtf8("Vous ne pouvez supprimer toute les vues de la scène."));
			err.exec();
		}
	}
}

void CameraSceneDialog::currentItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem* prev){
	if(cur && cur->data(0,Qt::UserRoleType).value<int>()== myTreeWidget::view){
		removeView_Button->setEnabled(true);
	}
	else{
		removeView_Button->setEnabled(false);
	}
}
