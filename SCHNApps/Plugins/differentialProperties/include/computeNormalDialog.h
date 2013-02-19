#ifndef _COMPUTENORMAL_DIALOG_H_
#define _COMPUTENORMAL_DIALOG_H_

#include "ui_computeNormalDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class MapHandlerGen;

class ComputeNormalDialog : public QDialog, public Ui::ComputeNormalDialog
{
	Q_OBJECT

public:
	ComputeNormalDialog(Window* w);

private:
	Window* m_window;
	MapHandlerGen* m_selectedMap;

public slots:
	void selectedMapChanged();
	void addMapToList(MapHandlerGen* m);
	void removeMapFromList(MapHandlerGen* m);
	void addAttributeToList(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
