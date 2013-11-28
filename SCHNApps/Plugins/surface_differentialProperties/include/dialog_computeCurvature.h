#ifndef _DIALOG_COMPUTECURVATURE_H_
#define _DIALOG_COMPUTECURVATURE_H_

#include "ui_dialog_computeCurvature.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class MapHandlerGen;

class Dialog_ComputeCurvature : public QDialog, public Ui::Dialog_ComputeCurvature
{
	Q_OBJECT

public:
	Dialog_ComputeCurvature(SCHNApps* s);

private:
	SCHNApps* m_schnapps;
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
