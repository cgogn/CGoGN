#ifndef _DIALOG_COMPUTENORMAL_H_
#define _DIALOG_COMPUTENORMAL_H_

#include "ui_dialog_computeNormal.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class MapHandlerGen;

class Dialog_ComputeNormal : public QDialog, public Ui::Dialog_ComputeNormal
{
	Q_OBJECT

public:
	Dialog_ComputeNormal(SCHNApps* s);

private:
	SCHNApps* m_schnapps;
	MapHandlerGen* m_selectedMap;

public slots:
	void selectedMapChanged();
	void addMapToList(MapHandlerGen* map);
	void removeMapFromList(MapHandlerGen* map);
	void addAttributeToList(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
