#ifndef _COMPUTECURVATURE_DIALOG_H_
#define _COMPUTECURVATURE_DIALOG_H_

#include "ui_computeCurvatureDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class MapHandlerGen;

class ComputeCurvatureDialog : public QDialog, public Ui::ComputeCurvatureDialog
{
	Q_OBJECT

public:
	ComputeCurvatureDialog(Window* w);

private:
	Window* m_window;

public slots:
	void selectedMapChanged();
	void addMapToList(MapHandlerGen* m);
	void removeMapFromList(MapHandlerGen* m);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
