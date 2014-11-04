#include "dialogList.h"
#include "Utils/cgognStream.h"


namespace CGoGN
{

namespace SCHNApps
{

ListPopUp::ListPopUp(const QString& name, QWidget* parent):
	QDialog(parent)
{
	setWindowTitle(name);
	setWindowFlags(windowFlags() & Qt::WindowCloseButtonHint);
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);
	m_list_items = new QListWidget();
	m_list_items->setSelectionMode(QAbstractItemView::NoSelection);
	m_layout->addWidget(m_list_items);
}

ListPopUp::~ListPopUp()
{
}

QListWidget* ListPopUp::list()
{
	return m_list_items;
}


QListWidgetItem* ListPopUp::addItem(const QString& str, Qt::CheckState checked)
{
	QListWidgetItem* item = new QListWidgetItem(str, m_list_items);
	item->setCheckState(checked);
	return item;
}

QListWidgetItem* ListPopUp::getItem(unsigned int row)
{
	return m_list_items->item(row);
}

unsigned int ListPopUp::nbItems()
{
	return m_list_items->count();
}

QListWidgetItem* ListPopUp::findItem(const QString& str)
{
	QList<QListWidgetItem*> items = m_list_items->findItems(str, Qt::MatchExactly);
	if(!items.empty())
		return items[0];
	return NULL;
}

bool ListPopUp::removeItem(const QString& str)
{
	QList<QListWidgetItem*> items = m_list_items->findItems(str, Qt::MatchExactly);
	if(items.empty())
		return false;
	m_list_items->takeItem(m_list_items->row(items[0]));
	return true;
}

void ListPopUp::check(const QString& str, Qt::CheckState ck)
{
	QList<QListWidgetItem*> items = m_list_items->findItems(str, Qt::MatchExactly);
	if(!items.empty())
		items[0]->setCheckState(ck);
	else
		CGoGNerr<< "ListPopUp::check()"<< str.toStdString()<< " not in list"<< CGoGNendl;
}

 bool ListPopUp::isChecked(const QString& str)
{
	QList<QListWidgetItem*> items = m_list_items->findItems(str, Qt::MatchExactly);
	if(!items.empty())
		return (items[0]->checkState() == Qt::Checked);
	CGoGNerr<< "ListPopUp::isChecked()"<< str.toStdString()<< " not in list"<< CGoGNendl;
	return false;
}


} // namespace SCHNApps

} // namespace CGoGN
