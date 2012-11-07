#include "linkViewDialog.h"


#include <QVariant>
#include <QMessageBox>
#include <QPushButton>

#include <plugin.h>

Q_DECLARE_METATYPE(Plugin*);

LinkViewDialog::LinkViewDialog(Window* parent, PluginHash* activePlugins, Scene* scene) : QDialog(parent),
	m_scene(scene),
	m_mode(LINK)
{
	this->setupUi(this);

	connect(this->buttonBox, SIGNAL(accepted()),this, SLOT(button_OK()));

	if(scene)
		groupBox->setTitle(QString::fromUtf8("Lier la vue ")+scene->getName());



	if(!activePlugins || activePlugins->isEmpty()){
		listWidget->setEnabled(false);
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		QLabel* error= new QLabel(groupBox);
		error->setText(QString::fromUtf8("<FONT COLOR=\"red\">"
				"Il n'y a pas de plugin chargé."
				"</FONT>"));
        formLayout->setWidget(3, QFormLayout::SpanningRole, error);
	}
	else{
		listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(listWidget, SIGNAL(customContextMenuRequested (const QPoint &)),
				this, SLOT(customContextMenu(const QPoint &)));
		PluginHash::iterator it;
		for(it= activePlugins->begin(); it!=activePlugins->end(); ++it){
			if((*it)->isWaitingForScene()){
				QVariant var;
				var.setValue(*it);

				QListWidgetItem* item= new QListWidgetItem(listWidget);
				item->setData(Qt::UserRole, var);
				item->setText((*it)->getName());
			}
		}

		if(listWidget->count()<=0){
			listWidget->setEnabled(false);
			buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			QLabel* error= new QLabel(groupBox);
			error->setText(QString::fromUtf8("<FONT COLOR=\"red\">"
					"Aucun plugin n'est en attente de vue."
					"</FONT>"));
	        formLayout->setWidget(3, QFormLayout::SpanningRole, error);
		}
	}

}


LinkViewDialog::LinkViewDialog(Window* parent, QList<Plugin*> dependingPlugin, Scene* scene) : QDialog(parent),
		m_scene(scene),
		m_mode(UNLINK)
{
	std::cout << "UNLINK MODE" << std::endl;
	this->setupUi(this);

	connect(this->buttonBox, SIGNAL(accepted()),this, SLOT(button_OK()));

	label->setText(QString::fromUtf8("Choisir de séparer la vue du plugin: "));

	if(scene)
		groupBox->setTitle(QString::fromUtf8("Délier la vue ")+scene->getName());


	if(dependingPlugin.isEmpty()){
		listWidget->setEnabled(false);
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		QLabel* error= new QLabel(groupBox);
		error->setText(QString::fromUtf8("<FONT COLOR=\"red\">"
				"Aucun plugin n'opère sur cette vue."
				"</FONT>"));
        formLayout->setWidget(3, QFormLayout::SpanningRole, error);
	}
	else{
		foreach(Plugin* p, dependingPlugin){
			QVariant var;
			var.setValue(p);

			QListWidgetItem* item= new QListWidgetItem(listWidget);
			item->setData(Qt::UserRole, var);
			item->setText(p->getName());
		}

		if(listWidget->count()<=0){
			listWidget->setEnabled(false);
			buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			QLabel* error= new QLabel(groupBox);
			error->setText(QString::fromUtf8("<FONT COLOR=\"red\">"
					"Aucun plugin n'opère sur cette vue."
					"</FONT>"));
	        formLayout->setWidget(3, QFormLayout::SpanningRole, error);
		}
	}

}

void LinkViewDialog::button_OK(){
	if(m_mode==LINK){
		Window* parent=((Window*)this->parent());
		QListWidgetItem* cur_item= listWidget->currentItem();
		if(cur_item && m_scene){
			Plugin* plugin= cur_item->data(Qt::UserRole).value<Plugin*>();

			if(plugin && plugin->isWaitingForScene()){
				std::cout << "BEGIIINNN" << std::endl;
				plugin->recieveScene(m_scene);
				std::cout << "ENDDDD" << std::endl;

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
				return;
			}
		}
		else{
			QMessageBox err(parent->focusWidget());
			err.setText(QString::fromUtf8("ERREUR: Aucun plugin ne semble avoir été sélectionné."));
			err.exec();
			return;
		}
	}
	else if(m_mode==UNLINK){
		Window* parent=((Window*)this->parent());
		QListWidgetItem* cur_item= listWidget->currentItem();
		if(cur_item && m_scene){
			Plugin* plugin= cur_item->data(Qt::UserRole).value<Plugin*>();

			if(plugin){
				m_scene->suppressLinkWith(plugin);

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
				return;
			}
		}
	}
}

void LinkViewDialog::customContextMenu(const QPoint & pos){
	QPoint globalPos= listWidget->mapToGlobal(pos);

	QListWidgetItem* item= listWidget->itemAt(pos);

	if(item){
		item->setSelected(true);

		QMenu myMenu("Plus...", this);
		QAction pluginInfo("Informations sur le plugin", this);
		myMenu.addAction(&pluginInfo);
		connect(&pluginInfo, SIGNAL(triggered()), this, SLOT(showPluginInfo()));

		myMenu.exec(globalPos);
	}
}

void LinkViewDialog::showPluginInfo(){
	QListWidgetItem* item= listWidget->currentItem();
	if(item){
		Plugin* plugin= item->data(Qt::UserRole).value<Plugin*>();
		System::Info::showPluginInfo(plugin->getPluginFilePath());
	}
}
