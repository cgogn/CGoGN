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
	m_view(view)
{
	this->setupUi(this);
	this->setWindowTitle(m_view->getName() + QString(" : plugins"));

	connect(pluginList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedPluginsChanged()));

	connect(m_window, SIGNAL(pluginAdded(Plugin*)), this, SLOT(cb_addPluginToList(Plugin*)));
	connect(m_window, SIGNAL(pluginRemoved(Plugin*)), this, SLOT(cb_removePluginFromList(Plugin*)));

	QList<Plugin*> plugins = m_window->getPluginsList();
	foreach(Plugin* p, plugins)
	{
		if(p->getProvidesRendering())
			pluginList->addItem(p->getName());
	}
}

PluginsViewDialog::~PluginsViewDialog()
{}

void PluginsViewDialog::cb_selectedPluginsChanged()
{
	for(int i = 0; i < pluginList->count(); ++i)
	{
		QString pluginName = pluginList->item(i)->text();
		Plugin* plugin = m_window->getPlugin(pluginName);
		if(pluginList->item(i)->isSelected() && !m_view->isLinkedToPlugin(plugin))
		{
			assert(!plugin->isLinkedToView(m_view));
			m_view->linkPlugin(plugin);
			plugin->linkView(m_view);
		}
		else if(!pluginList->item(i)->isSelected() && m_view->isLinkedToPlugin(plugin))
		{
			assert(plugin->isLinkedToView(m_view));
			m_view->unlinkPlugin(plugin);
			plugin->unlinkView(m_view);
		}
	}
	m_view->updateGL();
}

void PluginsViewDialog::cb_addPluginToList(Plugin* p)
{
	if(p->getProvidesRendering())
		pluginList->addItem(p->getName());
}

void PluginsViewDialog::cb_removePluginFromList(Plugin* p)
{
	if(p->getProvidesRendering())
	{
		for(int i = 0; i < pluginList->count(); ++i)
		{
			if(pluginList->item(i)->text() == p->getName())
			{
				delete pluginList->item(i);
				return;
			}
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
