#ifndef _MAPSVIEWDIALOG_H_
#define _MAPSVIEWDIALOG_H_

#include "ui_mapsViewDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class View;
class MapHandlerGen;

class MapsViewDialog : public QDialog, Ui::MapsViewDialog
{
	Q_OBJECT

public:
	MapsViewDialog(Window* window, View* view);
	~MapsViewDialog();

private:
	Window* m_window;
	View* m_view;
	bool b_refreshingUI;

public slots:
	void selectedMapsChanged();
	void selectMap(View* view, MapHandlerGen* map);
	void deselectMap(View* view, MapHandlerGen* map);
	void addMapToList(MapHandlerGen* m);
	void removeMapFromList(MapHandlerGen* m);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
