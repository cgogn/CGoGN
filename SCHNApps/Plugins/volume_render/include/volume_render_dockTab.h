#ifndef _VOLUME_RENDER_DOCK_TAB_H_
#define _VOLUME_RENDER_DOCK_TAB_H_

#include "ui_volume_render.h"

namespace CGoGN
{

  namespace SCHNApps
  {

    class SCHNApps;
    class Volume_Render_Plugin;
    struct MapParameters;


    class Volume_Render_DockTab : public QWidget, public Ui::Volume_Render_TabWidget
    {
      Q_OBJECT

      friend class Volume_Render_Plugin;

    public:
      Volume_Render_DockTab(SCHNApps* s, Surface_Render_Plugin* p);

    private:
      SCHNApps* m_schnapps;
      Volume_Render_Plugin* m_plugin;
      bool b_updatingUI;

    private slots:
      void positionAttributeChanged(int index);
      void colorAttributeChanged(int index);
      void facesScaleFactorChanged(int i);
      void volumesScaleFactorChanged(int i);
      void renderEdgesChanged(bool b);
      void renderFacesChanged(bool b);
      void faceStyleChanged(QAbstractButton* b);

    private:
      void updateMapParameters();
    };

  } // namespace SCHNApps

} // namespace CGoGN

#endif
