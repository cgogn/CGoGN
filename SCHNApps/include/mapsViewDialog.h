#ifndef _MAPSVIEWDIALOG_H_
#define _MAPSVIEWDIALOG_H_

#include "ui_mapsViewDialog.h"

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

public slots:
	void cb_selectedMapsChanged();
	void cb_addMapToList(MapHandlerGen* m);
	void cb_removeMapFromList(MapHandlerGen* m);
};

#endif
