#include "dialogs/pluginsDialog.h"

#include <QFileDialog>
#include <QMessageBox>

#include "window.h"
#include "plugin.h"

namespace CGoGN
{

namespace SCHNApps
{

PluginsDialog::PluginsDialog(Window* window) :
	QDialog(window),
	m_window(window),
	b_refreshingUI(false)
{
	setupUi(this);
	setModal(false);

	connect(button_registerDirectory, SIGNAL(pressed()), this, SLOT(registerPluginsDirectory()));
	connect(list_plugins, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(togglePlugin(QListWidgetItem*)));

	connect(m_window, SIGNAL(pluginLoaded(Plugin*)), this, SLOT(checkPlugin(Plugin*)));
	connect(m_window, SIGNAL(pluginUnloaded(Plugin*)), this, SLOT(uncheckPlugin(Plugin*)));
}

PluginsDialog::~PluginsDialog()
{}

void PluginsDialog::refreshPluginsList()
{
	b_refreshingUI = true;
	list_plugins->clear();
	const QMap<QString, QString>& availablePlugins = m_window->getAvailablePlugins();
	const PluginHash& loadedPlugins = m_window->getPluginsHash();
	for(QMap<QString, QString>::const_iterator it = availablePlugins.constBegin(); it != availablePlugins.constEnd(); ++it)
	{
		QListWidgetItem* item = new QListWidgetItem(it.key(), list_plugins);
		if(loadedPlugins.contains(it.key()))
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
	}
	b_refreshingUI = false;
}

void PluginsDialog::registerPluginsDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(
		this,
		tr("Select a directory"),
		m_window->getAppPath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!dir.isEmpty())
		m_window->registerPluginsDirectory(dir);
}

void PluginsDialog::togglePlugin(QListWidgetItem* item)
{
	if(!b_refreshingUI)
	{
		if(item->checkState() == Qt::Checked)
		{
			Plugin* p = m_window->loadPlugin(item->text());
			if (!p)
			{
				b_refreshingUI = true;
				item->setCheckState(Qt::Unchecked);
				b_refreshingUI = false;
			}
		}
		else // Unchecked
		{
			Plugin* p = m_window->getPlugin(item->text());
			if (p)
			{
				if(p->isUsed())
				{
					QMessageBox::warning(this, tr("Warning"), "Plugin is currently used");
					b_refreshingUI = true;
					item->setCheckState(Qt::Checked);
					b_refreshingUI = false;
					return;
				}
				else
					m_window->unloadPlugin(item->text());
			}
		}
	}
}

void PluginsDialog::checkPlugin(Plugin* plugin)
{
	b_refreshingUI = true;
	QString name = plugin->getName();
	QList<QListWidgetItem*> items = list_plugins->findItems(name, Qt::MatchExactly);
	if(!items.empty())
		items[0]->setCheckState(Qt::Checked);
	b_refreshingUI = false;
}

void PluginsDialog::uncheckPlugin(Plugin* plugin)
{
	b_refreshingUI = true;
	QString name = plugin->getName();
	QList<QListWidgetItem*> items = list_plugins->findItems(name, Qt::MatchExactly);
	if(!items.empty())
		items[0]->setCheckState(Qt::Unchecked);
	b_refreshingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
