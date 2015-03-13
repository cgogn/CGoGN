#include "controlDock_pluginTab.h"

#include <QFileDialog>

#include "schnapps.h"
#include "plugin.h"
#include "plugin_interaction.h"
#include "plugin_processing.h"
#include "view.h"

namespace CGoGN
{

namespace SCHNApps
{

ControlDock_PluginTab::ControlDock_PluginTab(SCHNApps* s) :
	m_schnapps(s),
	b_updatingUI(false)
{
	setupUi(this);

	connect(button_addPluginDirectory, SIGNAL(clicked()), this, SLOT(addPluginDirectoryClicked()));
	connect(button_enablePlugins, SIGNAL(clicked()), this, SLOT(enableSelectedPluginsClicked()));
	connect(button_disablePlugins, SIGNAL(clicked()), this, SLOT(disableSelectedPluginsClicked()));
//	connect(list_pluginsEnabled, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(pluginCheckStateChanged(QListWidgetItem*)));

	connect(m_schnapps, SIGNAL(pluginAvailableAdded(QString)), this, SLOT(pluginAvailableAdded(QString)));
	connect(m_schnapps, SIGNAL(pluginEnabled(Plugin*)), this, SLOT(pluginEnabled(Plugin*)));
	connect(m_schnapps, SIGNAL(pluginDisabled(Plugin*)), this, SLOT(pluginDisabled(Plugin*)));
//	connect(m_schnapps, SIGNAL(selectedViewChanged(View*,View*)), this, SLOT(selectedViewChanged(View*,View*)));
}





void ControlDock_PluginTab::addPluginDirectoryClicked()
{
	if(!b_updatingUI)
	{
		QString dir = QFileDialog::getExistingDirectory(
			this,
			tr("Select a directory"),
			m_schnapps->getAppPath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
		);

		if (!dir.isEmpty())
			m_schnapps->registerPluginsDirectory(dir);
	}
}

void ControlDock_PluginTab::enableSelectedPluginsClicked()
{
	if(!b_updatingUI)
	{
		QList<QListWidgetItem*> items = list_pluginsAvailable->selectedItems();
		foreach(QListWidgetItem* item, items)
			m_schnapps->enablePlugin(item->text());
	}
}

void ControlDock_PluginTab::disableSelectedPluginsClicked()
{
	if(!b_updatingUI)
	{
		QList<QListWidgetItem*> items = list_pluginsEnabled->selectedItems();
		foreach(QListWidgetItem* item, items)
			m_schnapps->disablePlugin(item->text());
	}
}

//void ControlDock_PluginTab::pluginCheckStateChanged(QListWidgetItem *item)
//{
//	if(!b_updatingUI)
//	{
//		View* selectedView = m_schnapps->getSelectedView();
//		PluginInteraction* p = dynamic_cast<PluginInteraction*>(m_schnapps->getPlugin(item->text()));
//		if(p)
//		{
//			if(item->checkState() == Qt::Checked)
//				selectedView->linkPlugin(p);
//			else
//				selectedView->unlinkPlugin(p);
//		}
//	}
//}





void ControlDock_PluginTab::pluginAvailableAdded(QString name)
{
	b_updatingUI = true;
	list_pluginsAvailable->addItem(name);
	b_updatingUI = false;
}

void ControlDock_PluginTab::pluginEnabled(Plugin *plugin)
{
	b_updatingUI = true;
	const QString& pluginName = plugin->getName();
	QList<QListWidgetItem*> av = list_pluginsAvailable->findItems(pluginName, Qt::MatchExactly);
	if (!av.empty())
		delete av[0];
	list_pluginsEnabled->addItem(pluginName);
	b_updatingUI = false;
}

void ControlDock_PluginTab::pluginDisabled(Plugin *plugin)
{
	b_updatingUI = true;
	const QString& pluginName = plugin->getName();
	QList<QListWidgetItem*> av = list_pluginsEnabled->findItems(pluginName, Qt::MatchExactly);
	if(!av.empty())
		delete av[0];
	list_pluginsAvailable->addItem(pluginName);
	b_updatingUI = false;
}

//void ControlDock_PluginTab::selectedViewChanged(View* prev, View* cur)
//{
//	if(prev)
//	{
//		foreach(PluginInteraction* pi, prev->getLinkedPlugins())
//		{
//			QList<QListWidgetItem*> prevItems = list_pluginsEnabled->findItems(pi->getName(), Qt::MatchExactly);
//			if(!prevItems.empty())
//			{
//				b_updatingUI = true;
//				prevItems[0]->setCheckState(Qt::Unchecked);
//				b_updatingUI = false;
//			}
//		}

//		disconnect(prev, SIGNAL(pluginLinked(PluginInteraction*)), this, SLOT(selectedViewPluginLinked(PluginInteraction*)));
//		disconnect(prev, SIGNAL(pluginUnlinked(PluginInteraction*)), this, SLOT(selectedViewPluginUnlinked(PluginInteraction*)));
//	}
//	if(cur)
//	{
//		foreach(PluginInteraction* pi, cur->getLinkedPlugins())
//		{
//			QList<QListWidgetItem*> curItems = list_pluginsEnabled->findItems(pi->getName(), Qt::MatchExactly);
//			if(!curItems.empty())
//			{
//				b_updatingUI = true;
//				curItems[0]->setCheckState(Qt::Checked);
//				b_updatingUI = false;
//			}
//		}

//		connect(cur, SIGNAL(pluginLinked(PluginInteraction*)), this, SLOT(selectedViewPluginLinked(PluginInteraction*)));
//		connect(cur, SIGNAL(pluginUnlinked(PluginInteraction*)), this, SLOT(selectedViewPluginUnlinked(PluginInteraction*)));
//	}
//}





//void ControlDock_PluginTab::selectedViewPluginLinked(PluginInteraction* plugin)
//{
//	QList<QListWidgetItem*> curItems = list_pluginsEnabled->findItems(plugin->getName(), Qt::MatchExactly);
//	if(!curItems.empty())
//	{
//		b_updatingUI = true;
//		curItems[0]->setCheckState(Qt::Checked);
//		b_updatingUI = false;
//	}
//}

//void ControlDock_PluginTab::selectedViewPluginUnlinked(PluginInteraction* plugin)
//{
//	QList<QListWidgetItem*> prevItems = list_pluginsEnabled->findItems(plugin->getName(), Qt::MatchExactly);
//	if(!prevItems.empty())
//	{
//		b_updatingUI = true;
//		prevItems[0]->setCheckState(Qt::Unchecked);
//		b_updatingUI = false;
//	}
//}

} // namespace SCHNApps

} // namespace CGoGN
