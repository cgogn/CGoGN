#ifndef _SUBDIVIDESURFACE_DIALOG_H_
#define _SUBDIVIDESURFACE_DIALOG_H_

#include "ui_subdivideSurfaceDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class MapHandlerGen;

class SubdivideSurfaceDialog : public QDialog, public Ui::SubdivideSurfaceDialog
{
	Q_OBJECT

public:
	SubdivideSurfaceDialog(Window* w);

private:
	Window* m_window;
	MapHandlerGen* m_selectedMap;

public slots:
	void selectedMapChanged();
	void addMapToList(MapHandlerGen* m);
	void removeMapFromList(MapHandlerGen* m);
	void addAttributeToList(unsigned int orbit, const QString& name);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
