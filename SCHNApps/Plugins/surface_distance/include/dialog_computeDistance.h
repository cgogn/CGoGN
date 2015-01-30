#ifndef _DIALOG_COMPUTEDISTANCE_H_
#define _DIALOG_COMPUTEDISTANCE_H_

#include "ui_dialog_computeDistance.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class MapHandlerGen;

class Dialog_ComputeDistance : public QDialog, public Ui::Dialog_ComputeDistance
{
	Q_OBJECT

public:
	Dialog_ComputeDistance(SCHNApps* s);

private:
	SCHNApps* m_schnapps;
	MapHandlerGen* m_selectedMap1;
	MapHandlerGen* m_selectedMap2;

public slots:
	void selectedMap1Changed();
	void selectedMap2Changed();
	void addMapToList(MapHandlerGen* map);
	void removeMapFromList(MapHandlerGen* map);
	void addAttributeToList1(unsigned int orbit, const QString& nameAttr);
	void addAttributeToList2(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
