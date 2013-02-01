#include "dialogs/pluginsViewDialog.h"

#include "window.h"
#include "view.h"
#include "plugin.h"

#include <QMessageBox>
#include <QMouseEvent>

namespace CGoGN
{

namespace SCHNApps
{

PluginsViewDialog::PluginsViewDialog(Window* window, View* view) :
	QDialog(view),
	m_window(window),
	m_view(view),
	b_refreshingUI(false)
{
	this->setupUi(this);
	this->setWindowTitle(m_view->getName() + QString(" : plugins"));

	connect(pluginList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedPluginsChanged()));

	connect(m_window, SIGNAL(pluginLoaded(Plugin*)), this, SLOT(addPluginToList(Plugin*)));
	connect(m_window, SIGNAL(pluginUnloaded(Plugin*)), this, SLOT(removePluginFromList(Plugin*)));

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(selectPlugin(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(deselectPlugin(View*, Plugin*)));

	QList<Plugin*> plugins = m_window->getPluginsList();
	foreach(Plugin* p, plugins)
	{
		if(p->getProvidesRendering())
			pluginList->addItem(p->getName());
	}
}

PluginsViewDialog::~PluginsViewDialog()
{}

void PluginsViewDialog::selectedPluginsChanged()
{
	if(!b_refreshingUI)
	{
		for(int i = 0; i < pluginList->count(); ++i)
		{
			QString pluginName = pluginList->item(i)->text();
			Plugin* plugin = m_window->getPlugin(pluginName);
			if(pluginList->item(i)->isSelected() && !m_view->isLinkedToPlugin(plugin))
				m_window->linkViewAndPlugin(m_view, plugin);

			else if(!pluginList->item(i)->isSelected() && m_view->isLinkedToPlugin(plugin))
				m_window->unlinkViewAndPlugin(m_view, plugin);
		}
	}
}

void PluginsViewDialog::selectPlugin(View* view, Plugin* plugin)
{
	if(view == m_view)
	{
		QList<QListWidgetItem*> items = pluginList->findItems(plugin->getName(), Qt::MatchExactly);
		if(!items.empty())
		{
			b_refreshingUI = true;
			items[0]->setSelected(true);
			b_refreshingUI = false;
		}
	}
}

void PluginsViewDialog::deselectPlugin(View* view, Plugin* plugin)
{
	if(view == m_view)
	{
		QList<QListWidgetItem*> items = pluginList->findItems(plugin->getName(), Qt::MatchExactly);
		if(!items.empty())
		{
			b_refreshingUI = true;
			items[0]->setSelected(false);
			b_refreshingUI = false;
		}
	}
}

void PluginsViewDialog::addPluginToList(Plugin* p)
{
	if(p->getProvidesRendering())
		pluginList->addItem(p->getName());
}

void PluginsViewDialog::removePluginFromList(Plugin* p)
{
	if(p->getProvidesRendering())
	{
		QList<QListWidgetItem*> items = pluginList->findItems(p->getName(), Qt::MatchExactly);
		if(!items.empty())
			delete items[0];
	}
}

} // namespace SCHNApps

} // namespace CGoGN
