#ifndef _DIALOG_LIST_H_
#define _DIALOG_LIST_H_


#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>

#include "dll.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNAPPS_API ListPopUp : public QDialog
{
	Q_OBJECT
	QVBoxLayout* m_layout;
	QListWidget* m_list_items;

public:
	ListPopUp(const QString& name, QWidget* parent = NULL);

	virtual ~ListPopUp();

	virtual void show();

	QListWidget* list();

	QListWidgetItem* addItem(const QString& str, Qt::CheckState checked = Qt::Unchecked);

	QListWidgetItem* getItem(unsigned int row);

	unsigned int nbItems();

	QListWidgetItem* findItem(const QString& str);

	bool removeItem(const QString& str);

	void check(const QString& str, Qt::CheckState ck);

	bool isChecked(const QString& str);

};



} // namespace SCHNApps

} // namespace CGoGN

#endif
