#ifndef _MAPSDIALOG_H_
#define _MAPSDIALOG_H_

#include "ui_mapsDialog.h"

class Window;
class MapHandlerGen;

class MapsDialog : public QDialog, Ui::MapsDialog
{
	Q_OBJECT

public:
	MapsDialog(Window* window);
	~MapsDialog();

private:
	Window* m_window;

public slots:
	void cb_removeMap();
	void cb_selectedMapChanged();
	void cb_addMapToList(MapHandlerGen* m);
	void cb_removeMapFromList(MapHandlerGen* m);
};

#endif
