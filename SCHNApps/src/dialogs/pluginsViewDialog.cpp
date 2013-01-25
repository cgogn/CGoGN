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

	connect(pluginList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedPluginsChanged()));

	connect(m_window, SIGNAL(pluginLoaded(Plugin*)), this, SLOT(addPluginToList(Plugin*)));
	connect(m_window, SIGNAL(pluginUnloaded(Plugin*)), this, SLOT(removePluginFromList(Plugin*)));

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
	for(int i = 0; i < pluginList->count(); ++i)
	{
		QString pluginName = pluginList->item(i)->text();
		Plugin* plugin = m_window->getPlugin(pluginName);
		if(pluginList->item(i)->isSelected() && !m_view->isLinkedToPlugin(plugin))
			m_window->linkViewAndPlugin(m_view, plugin);

		else if(!pluginList->item(i)->isSelected() && m_view->isLinkedToPlugin(plugin))
			m_window->unlinkViewAndPlugin(m_view, plugin);
	}
	m_view->updateGL();
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
