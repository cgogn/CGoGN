#ifndef _PLUGIN_INTERACTION_H_
#define _PLUGIN_INTERACTION_H_

#include "plugin.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class View;

class SCHNAPPS_API PluginInteraction : public Plugin
{
	Q_OBJECT

	friend class View;

public:
	PluginInteraction();
	virtual ~PluginInteraction();

public slots:
	/**
	 * @brief get thte list of views linked with this plugin
	 * @return the list
	 */
	inline const QList<View*>& getLinkedViews() const { return l_views; }

	/**
	 * @brief [PYTHON] test if this plugin is linked to a view
	 * @param view view object ptr
	 * @return
	 */
	inline bool isLinkedToView(View* view) const { return l_views.contains(view); }

	/**
	 * @brief get list of shaders used in this rendering plugin
	 * @return  list of shaders ptr
	 */
	inline const QList<Utils::GLSLShader*> getShaders() const { return l_shaders; }

private:
	virtual void draw(View* view) = 0;
	virtual void drawMap(View* view, MapHandlerGen* map) = 0;

	virtual void keyPress(View* view, QKeyEvent* event) = 0;
	virtual void keyRelease(View* view, QKeyEvent* event) = 0;
	virtual void mousePress(View* view, QMouseEvent* event) = 0;
	virtual void mouseRelease(View* view, QMouseEvent* event) = 0;
	virtual void mouseMove(View* view, QMouseEvent* event) = 0;
	virtual void wheelEvent(View* view, QWheelEvent* event) = 0;

	virtual void viewLinked(View* view) = 0;
	virtual void viewUnlinked(View* view) = 0;

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	void linkView(View* view);
	void unlinkView(View* view);

	/*********************************************************
	 * MANAGE SHADERS
	 *********************************************************/

protected:
	void registerShader(Utils::GLSLShader* shader);
	void unregisterShader(Utils::GLSLShader* shader);

	void registerShader(const std::vector<Utils::GLSLShader*>& shaders);
	void unregisterShader(const std::vector<Utils::GLSLShader*>& shaders);


protected:
	QList<View*> l_views;
	QList<Utils::GLSLShader*> l_shaders;
};


} // namespace SCHNApps

} // namespace CGoGN

#endif
