#include "plugin_interaction.h"

#include "schnapps.h"
#include "Utils/GLSLShader.h"

namespace CGoGN
{

namespace SCHNApps
{

PluginInteraction::PluginInteraction()
{}

PluginInteraction::~PluginInteraction()
{}

/*********************************************************
 * MANAGE LINKED VIEWS
 *********************************************************/

void PluginInteraction::linkView(View* view)
{
	if(view && !l_views.contains(view))
	{
		l_views.push_back(view);
		viewLinked(view);
	}
}

void PluginInteraction::unlinkView(View* view)
{
	if(l_views.removeOne(view))
		viewUnlinked(view);
}

/*********************************************************
 * MANAGE SHADERS
 *********************************************************/

void PluginInteraction::registerShader(Utils::GLSLShader* shader)
{
	if(shader && !l_shaders.contains(shader))
		l_shaders.push_back(shader);
}

void PluginInteraction::unregisterShader(Utils::GLSLShader* shader)
{
	l_shaders.removeOne(shader);
}

void PluginInteraction::registerShader(const std::vector<Utils::GLSLShader*>& shaders)
{
	for (auto shader: shaders)
		if(shader && !l_shaders.contains(shader))
			l_shaders.push_back(shader);
}

void PluginInteraction::unregisterShader(const std::vector<Utils::GLSLShader*>& shaders)
{
	for (auto shader: shaders)
		l_shaders.removeOne(shader);
}

} // namespace SCHNApps

} // namespace CGoGN
