#ifndef _MAPSVIEWDIALOG_H_
#define _MAPSVIEWDIALOG_H_

#include "ui_mapsViewDialog.h"

class Window;
class View;
class MapHandler;

class MapsViewDialog : public QDialog, Ui::MapsViewDialog
{
	Q_OBJECT

public:
	MapsViewDialog(Window* window, View* view);
	~MapsViewDialog();

private:
	Window* m_window;
	View* m_view;

	void selectCurrentMaps();

public slots:
	void cb_selectedMapsChanged();
	void cb_addMapToList(MapHandler* m);
	void cb_removeMapFromList(MapHandler* m);
};

#endif
