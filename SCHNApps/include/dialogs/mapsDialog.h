#ifndef _MAPSDIALOG_H_
#define _MAPSDIALOG_H_

#include "ui_mapsDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

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

	void clearInfo();

public slots:
	void removeMap();
	void selectedMapChanged();
	void toggleVBO(QListWidgetItem* item);
	void addMapToList(MapHandlerGen* m);
	void removeMapFromList(MapHandlerGen* m);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
