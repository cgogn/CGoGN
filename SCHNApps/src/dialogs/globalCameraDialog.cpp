#include "globalCameraDialog.h"

namespace Qt{
	int UserRoleType= 33;
	int UserRole2= 34;
}

#include <QMetaType>
#include <QMimeData>
#include <QMessageBox>

#include "scene.h"
#include "window.h"

Q_DECLARE_METATYPE(Camera*)
Q_DECLARE_METATYPE(View*)
Q_DECLARE_METATYPE(Scene*)
Q_DECLARE_METATYPE(CarryCamera)

QDataStream &operator<<(QDataStream &out, CarryCamera &c){
	return out << (quintptr)(c.camera) << (quintptr)(c.view);
}

QDataStream &operator>>(QDataStream &in, CarryCamera &c){
	quintptr cptr, vptr;
	in >> reinterpret_cast<quintptr&>(cptr);
	in >> reinterpret_cast<quintptr&>(vptr);

	c.camera= (Camera*)cptr;
	c.view= (View*)vptr;

	return in;
}





MyCameraMenu::MyCameraMenu(const QString & title, QWidget * parent, int mode) :
		QMenu(title, parent),
		m_result(none)
{
	if((mode & moving)>0){
		m_moveAction= addAction(QString::fromUtf8("Donner la caméra"));
		m_moveAction->setIcon(QIcon(":icons/icons/give.png"));
		connect(m_moveAction, SIGNAL(triggered()), this, SLOT(moveCamera()));
	}
	if((mode & sharing)>0){
		m_shareAction= addAction(QString::fromUtf8("Partager la caméra"));
		m_shareAction->setIcon(QIcon(":icons/icons/share.png"));
		connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareCamera()));
	}

}





MyDragListWidget::MyDragListWidget(QWidget* parent) : QListWidget(parent)
{
	setDragEnabled(true);

    qRegisterMetaTypeStreamOperators<QListWidgetItem>("CarryCamera");
}



QMimeData* MyDragListWidget::mimeData(const QList<QListWidgetItem *> items) const{
    QByteArray ba;
    QDataStream st(&ba, QIODevice::WriteOnly);

    foreach(QListWidgetItem * item, items){
    	Camera* c= item->data(Qt::UserRole).value<Camera*>();
    	View* v= item->data(Qt::UserRole2).value<View*>();
    	std::cout << "_-_- W view " << v->getName().toStdString() << std::endl;
    	CarryCamera cc(c,v);
    	st << cc;
    }

    QMimeData* md= new QMimeData();
    md->setData("CarryCamera",ba);


    return md;
}




MyDropTreeWidget::MyDropTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
	setAcceptDrops(true);
	setDropIndicatorShown(true);

	QTreeWidgetItem* root= invisibleRootItem();
	root->setFlags(root->flags() & ~Qt::ItemIsDropEnabled);

    qRegisterMetaTypeStreamOperators<QTreeWidgetItem>("CarryCamera");

}


bool MyDropTreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action){
	CarryCamera cc;
	View* origin_view;
	Camera* c;

	if(parent->data(0,Qt::UserRoleType).value<int>()==myDragDropWidgets::view){
		QByteArray ba= data->data("CarryCamera");
		QDataStream st(&ba, QIODevice::ReadOnly);
		while(!st.atEnd()){
			st >> cc;
			c= cc.camera;
			origin_view= cc.view;


			if(c && origin_view){
				Scene* origin_scene= origin_view->getScene();
				View* dest_view= parent->data(0,Qt::UserRole).value<View*>();
				Scene* dest_scene= dest_view->getScene();

				if(origin_view==dest_view) return false;

				MyCameraMenu::ResultType result;
				if(origin_scene==dest_scene){
					MyCameraMenu menu(QString::fromUtf8("Gestion caméra"), this, MyCameraMenu::moving);
					menu.exec(QCursor::pos());
					result= menu.result();
				}
				else{

					MyCameraMenu menu(QString::fromUtf8("Gestion caméra"), this);
					menu.exec(QCursor::pos());
					result= menu.result();
				}
				switch (result) {
					case MyCameraMenu::move:
						if(origin_view->countCameras()<=1 && origin_scene->countViews()<=1){
							QMessageBox err(this);
							err.setText(QString::fromUtf8("Vous ne pouvez pas retirer toutes les caméras"
									"de l'unique vue d'une scène."));
							err.exec();

							return false;
						}
						else{
							origin_view->takeCamera(c);
							dest_view->insertCamera(index,c);
						}

						if(origin_view->countCameras()<=0){
							View* tmp_view;
							QTreeWidgetItemIterator it(this);
							while(*it){
								if((*it)->data(0,Qt::UserRoleType).value<int>()==myDragDropWidgets::view){
									tmp_view= (*it)->data(0,Qt::UserRole).value<View*>();
									if(tmp_view==origin_view){
										delete *it;
										break;
									}
								}

								++it;
							}
							origin_scene->deleteView(origin_view);
						}

						break;
					case MyCameraMenu::share:
						dest_view->shareCamera(c, index);

						break;
					default:
						return false;

						break;
				}

				this->currentItemChanged(this->currentItem(),this->currentItem());
			}


		}

		return true;
	}
	else{
		return false;
	}
}

QStringList MyDropTreeWidget::mimeTypes() const{
    QStringList qstrList;
    // list of accepted mime types for drop
    qstrList.append("CarryCamera");
    return qstrList;
}

Qt::DropActions MyDropTreeWidget::supportedDropActions () const{
    // returns what actions are supported when dropping
    return (Qt::CopyAction | Qt::TargetMoveAction);
}




GlobalCameraDialog::GlobalCameraDialog(Window* window) : QDialog(window)
{
	this->setupUi(this);

	treeWidget= new MyDropTreeWidget(groupBox);
    treeWidget->setObjectName(QString::fromUtf8("treeWidget"));

    horizontalLayout->insertWidget(0,treeWidget);

    listWidget= new MyDragListWidget(groupBox);
    listWidget->setObjectName(QString::fromUtf8("listWidget"));

    verticalLayout_3->insertWidget(1,listWidget);

    if(window){
		QList<Scene*> listScenes= window->scenes();
		foreach(Scene* scene, listScenes){
			QTreeWidgetItem* sceneItem= new QTreeWidgetItem(treeWidget);
			QVariant varScene, varType;
			varType.setValue((int)(myDragDropWidgets::scene));
			varScene.setValue(scene);
			sceneItem->setData(0,Qt::UserRoleType, varType);
			sceneItem->setData(0,Qt::UserRole, varScene);
			sceneItem->setText(0,scene->getName());
			sceneItem->setFlags((sceneItem->flags() & ~Qt::ItemIsDropEnabled) & (~Qt::ItemIsDragEnabled));

			QList<View*> listView= scene->views();
			foreach(View* view, listView){
	        	QTreeWidgetItem* viewItem= new QTreeWidgetItem(sceneItem);
	    		QVariant varScene, varType;
	    		varType.setValue((int)(myDragDropWidgets::view));
	    		varScene.setValue(view);
	    		viewItem->setData(0,Qt::UserRoleType, varType);
	    		viewItem->setData(0,Qt::UserRole, varScene);
	    		viewItem->setText(0,view->getName());
	    		viewItem->setFlags((viewItem->flags() | Qt::ItemIsDropEnabled) & (~Qt::ItemIsDragEnabled));
			}
		}

		connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
				this, SLOT(treeCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    }
}

void GlobalCameraDialog::treeCurrentItemChanged(QTreeWidgetItem* cur, QTreeWidgetItem* prev){
	if(listWidget){
		listWidget->clear();

		int type= cur->data(0,Qt::UserRoleType).value<int>();
		switch (type) {
			case myDragDropWidgets::scene :
			{
				Scene* scene= cur->data(0,Qt::UserRole).value<Scene*>();
				if(scene){
					QList<View*> listView= scene->views();
					foreach(View* view, listView){
						QListWidgetItem* viewItem= new QListWidgetItem(listWidget);
						QVariant varScene, varType;
						varType.setValue((int)(myDragDropWidgets::view));
						varScene.setValue(view);
						viewItem->setData(Qt::UserRoleType, varType);
						viewItem->setData(Qt::UserRole, varScene);
						viewItem->setText(QString("---")+view->getName()+"---");
						viewItem->setFlags((viewItem->flags() & ~Qt::ItemIsDragEnabled) & (~Qt::ItemIsDropEnabled));

						QList<Camera*> cameraList= view->cameras();
						foreach(Camera* camera, cameraList){
							QListWidgetItem* cameraItem= new QListWidgetItem(listWidget);
							QVariant varScene, varType, varView;
							varType.setValue((int)(myDragDropWidgets::camera));
							varScene.setValue(camera);
							varView.setValue(view);
							cameraItem->setData(Qt::UserRoleType, varType);
							cameraItem->setData(Qt::UserRole, varScene);
							cameraItem->setData(Qt::UserRole2, varView);
							if(camera->isShared()){
								cameraItem->setText(QString("\t")+camera->getName()+" (shared)");
							}
							else{
								cameraItem->setText(QString("\t")+camera->getName());
							}
							cameraItem->setFlags((cameraItem->flags() | Qt::ItemIsDragEnabled) & (~Qt::ItemIsDropEnabled));
						}
					}
					label->setText(QString::fromUtf8("Caméras de la scène \"")+scene->getName()+"\":");
				}

				break;
			}
			case myDragDropWidgets::view :
			{
				View* view= cur->data(0,Qt::UserRole).value<View*>();
				if(view){
					QList<Camera*> cameraList= view->cameras();
					foreach(Camera* camera, cameraList){
						QListWidgetItem* cameraItem= new QListWidgetItem(listWidget);
						QVariant varScene, varType, varView;
						varType.setValue((int)(myDragDropWidgets::camera));
						varScene.setValue(camera);
						varView.setValue(view);
						cameraItem->setData(Qt::UserRoleType, varType);
						cameraItem->setData(Qt::UserRole, varScene);
						cameraItem->setData(Qt::UserRole2, varView);
						if(camera->isShared()){
							cameraItem->setText(camera->getName()+" (shared)");
						}
						else{
							cameraItem->setText(camera->getName());
						}
						cameraItem->setFlags((cameraItem->flags() | Qt::ItemIsDragEnabled) & (~Qt::ItemIsDropEnabled));
					}
					label->setText(QString::fromUtf8("Caméras de la vue \"")+view->getName()+
							QString::fromUtf8("\" (scène \"")+view->getScene()->getName()+"\"):");
				}

				break;
			}
			default:
			{
				break;
			}
		}
	}
}


