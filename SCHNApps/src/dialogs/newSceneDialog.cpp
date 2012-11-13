#include "newSceneDialog.h"

#include <QMessageBox>
#include <QVariant>

#include "system.h"
#include "window.h"
#include "scene.h"
#include "view.h"
#include "camera.h"
#include "plugin.h"


Q_DECLARE_METATYPE(Camera*)
Q_DECLARE_METATYPE(View*)
Q_DECLARE_METATYPE(Scene*)
Q_DECLARE_METATYPE(Plugin*)


NewSceneDialog::NewSceneDialog(Window* parent) : QDialog(parent),
	m_window(parent),
	m_listWidget(NULL)
{
	this->setupUi(this);

	connect(this->buttonBox, SIGNAL(accepted()),this, SLOT(button_OK()));
	connect(radioButton, SIGNAL(clicked()), this, SLOT(new_camera()));
	connect(radioButton_2, SIGNAL(clicked()), this, SLOT(share_camera()));
	connect(check_linkPlugin, SIGNAL(toggled(bool)), SLOT(linkPlugin(bool)));

	QList<Plugin*> listPlugins= m_window->activePlugins();
	for(QList<Plugin*>::iterator it= listPlugins.begin(); it!= listPlugins.end(); ++it){
		if((*it)->isWaitingForScene()){
			check_linkPlugin->setEnabled(true);
			break;
		}
	}
}

NewSceneDialog::~NewSceneDialog(){}

void NewSceneDialog::button_OK(){
	Window* parent=((Window*)this->parent());
	if(!line_name->text().isEmpty()){
		Scene* scene;
		if(radioButton_2->isChecked()){
			Camera* c= m_listWidget->currentItem()->data(Qt::UserRole).value<Camera*>();
			if (!parent->addNewEmptyScene(line_name->text().left(30), scene, true, c)){
				QMessageBox err(parent->focusWidget());
				err.setText(QString::fromUtf8("ERREUR: Une erreur est survenue lors de la création de la vue.\n"
						"Le nom était peut-être mauvais."));
				err.exec();
				System::Error::showError();
				return;
			}

		}
		else{
			if (!parent->addNewEmptyScene(line_name->text().left(30), scene, true)){
				QMessageBox err(parent->focusWidget());
				err.setText(QString::fromUtf8("ERREUR: Une erreur est survenue lors de la création de la vue.\n"
						"Le nom était peut-être mauvais."));
				err.exec();
				System::Error::showError();
				return;
			}
		}

		if(check_linkPlugin->isChecked()){
			QListWidgetItem* cur_item= m_listWidget2->currentItem();
			if(cur_item && scene){
				Plugin* plugin= cur_item->data(Qt::UserRole).value<Plugin*>();

				if(plugin && plugin->isWaitingForScene()){
					plugin->recieveScene(scene);

					this->accept();
					return;
				}
				else{
					QMessageBox err(parent->focusWidget());
					QString message;
					if(!plugin){
						message= QString::fromUtf8("Le plugin spécifié n'existe pas ou est mal chargé.");
					}
					else{
						message= QString::fromUtf8("Le plugin spécifié n'est pas en attente de vue"
								"supplémentaire.");
					}
					err.setText(message);
					err.exec();
					this->accept();
					return;
				}
			}
			else{
				QMessageBox err(parent->focusWidget());
				err.setText(QString::fromUtf8("ERREUR: Aucun plugin ne semble avoir été sélectionné."));
				err.exec();
				this->accept();
				return;
			}
		}
		else{
			this->accept();
		}
	}
	else{
		QMessageBox err(parent->focusWidget());
		err.setText(QString::fromUtf8("ERREUR: un nom (non-existant) doit être spécifié pour la vue"));
		err.exec();
	}
}

void NewSceneDialog::new_camera(){
	if(m_listWidget){
		delete m_listWidget;
		m_listWidget=NULL;
	}
}

void NewSceneDialog::share_camera(){
	if(!m_listWidget){
		m_listWidget= new QListWidget(groupBox_3);
		verticalLayout_3->insertWidget(2,m_listWidget);

		bool currentSet= false;

		Window* parent=((Window*)this->parent());
		QList<Scene*> listScene= parent->scenes();
		foreach(Scene* scene, listScene){
			QListWidgetItem* sceneItem= new QListWidgetItem(m_listWidget);
			QVariant var, varType;
			varType.setValue((int)(myListCameraTypes::scene));
			var.setValue(scene);
			sceneItem->setData(Qt::UserRoleType, varType);
			sceneItem->setData(Qt::UserRole, var);
			sceneItem->setText(QString("-=")+scene->getName()+"=-");
			sceneItem->setFlags(sceneItem->flags() & ~Qt::ItemIsSelectable);

			QList<View*> listView= scene->views();
			foreach(View* view, listView){
				QListWidgetItem* viewItem= new QListWidgetItem(m_listWidget);
				varType.setValue((int)(myListCameraTypes::view));
				var.setValue(view);
				viewItem->setData(Qt::UserRoleType, varType);
				viewItem->setData(Qt::UserRole, var);
				viewItem->setText(QString("  ---")+view->getName()+"---");
				viewItem->setFlags(viewItem->flags() & ~Qt::ItemIsSelectable);

				QList<Camera*> listCamera= view->cameras();
				foreach(Camera* camera, listCamera){
					QListWidgetItem* cameraItem= new QListWidgetItem(m_listWidget);
					varType.setValue((int)(myListCameraTypes::camera));
					var.setValue(camera);
					cameraItem->setData(Qt::UserRoleType, varType);
					cameraItem->setData(Qt::UserRole, var);
					cameraItem->setText(QString("     ")+camera->getName());
					cameraItem->setFlags(cameraItem->flags() | Qt::ItemIsSelectable);
					if(!currentSet){
						m_listWidget->setCurrentItem(cameraItem);
					}
				}
			}
		}

		connect(m_listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
				this, SLOT(cameraListSelectionChange(QListWidgetItem*,QListWidgetItem*)));
	}
}

void NewSceneDialog::cameraListSelectionChange(QListWidgetItem* cur, QListWidgetItem* prev){
	if(cur->data(Qt::UserRoleType).value<int>()!=myListCameraTypes::camera){
		m_listWidget->setCurrentItem(prev);
	}
}

void NewSceneDialog::linkPlugin(bool b){
	if(b){
		m_listWidget2= new QListWidget(groupBox);
		m_listWidget2->setContextMenuPolicy(Qt::CustomContextMenu);

		verticalLayout_2->addWidget(m_listWidget2);

		connect(m_listWidget2, SIGNAL(customContextMenuRequested (const QPoint &)),
				this, SLOT(customContextMenu(const QPoint &)));

		QList<Plugin*> listPlugins= m_window->activePlugins();
		for(QList<Plugin*>::iterator it= listPlugins.begin(); it!= listPlugins.end(); ++it){
			if((*it)->isWaitingForScene()){
				QVariant var;
				var.setValue(*it);

				QListWidgetItem* item= new QListWidgetItem(m_listWidget2);
				item->setData(Qt::UserRole, var);
				item->setText((*it)->getName());
			}
		}
		if(m_listWidget2->count()<=0){
			check_linkPlugin->setChecked(false);
			check_linkPlugin->setEnabled(false);
			m_listWidget2= NULL;
		}
	}
	else{
		if(m_listWidget2){
			delete m_listWidget2;
		}
	}
}

void NewSceneDialog::customContextMenu(const QPoint & pos){
	QPoint globalPos= m_listWidget2->mapToGlobal(pos);

	QListWidgetItem* item= m_listWidget2->itemAt(pos);

	if(item){
		item->setSelected(true);

		QMenu myMenu("Plus...", this);
		QAction pluginInfo("Informations sur le plugin", this);
		myMenu.addAction(&pluginInfo);
		connect(&pluginInfo, SIGNAL(triggered()), this, SLOT(showPluginInfo()));

		myMenu.exec(globalPos);
	}
}

void NewSceneDialog::showPluginInfo(){
	QListWidgetItem* item= m_listWidget2->currentItem();
	if(item){
		Plugin* plugin= item->data(Qt::UserRole).value<Plugin*>();
		System::Info::showPluginInfo(plugin->getPluginFilePath());
	}
}
