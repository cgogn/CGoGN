#include "volume_render_dockTab.h"

#include "volume_render.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Volume_Render_DockTab::Volume_Render_DockTab(SCHNApps* s, Surface_Render_Plugin* p) :
  m_schnapps(s),
  m_plugin(p),
  b_updatingUI(false)
{
	setupUi(this);
}

void Volume_Render_DockTab::positionAttributeChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changePositionAttribute(view, map, map->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText()), true);
	}
}

void Volume_Render_DockTab::colorAttributeChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeColorAttribute(view, map, map->getAttribute<PFP2::VEC3, VERTEX>(combo_colorAttribute->currentText()), true);
	}
}

void Volume_Render_DockTab::facesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeFacesScaleFactor(view, map, i, true);
	}
}

void Volume_Render_DockTab::volumesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeVolumesScaleFactor(view, map, i, true);
	}
}

void Volume_Render_DockTab::renderEdgesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeRenderEdges(view, map, b, true);
	}
}

void Volume_Render_DockTab::renderFacesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = m_currentParams->selectedMap;
		if(map)
			m_plugin->changeRenderFaces(view, map, b, true);
	}
}

void Volume_Render_DockTab::faceStyleChanged(QAbstractButton* b)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			if(radio_flatShading->isChecked())
				m_plugin->h_viewParameterSet[view][map].faceStyle = MapParameters::FLAT;
			else if(radio_smoothShading->isChecked())
				m_plugin->h_viewParameterSet[view][map].faceStyle = MapParameters::SMOOTH;
			view->updateGL();
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
