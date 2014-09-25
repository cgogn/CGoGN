#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <QtPlugin>
#include <QAction>

#include "types.h"

namespace CGoGN
{

namespace SCHNApps
{

class Plugin : public QObject
{
	Q_OBJECT

	friend class SCHNApps;

public:
	Plugin();
	virtual ~Plugin();

public slots:
	QString getName();
	QString getFilePath();
	SCHNApps* getSCHNApps();

private:
	void setName(const QString& name);
	void setFilePath(const QString& f);
	void setSCHNApps(SCHNApps* s);

	virtual bool enable() = 0;
	virtual void disable() = 0;

protected:
	QString m_name;
	QString m_filePath;
	SCHNApps* m_schnapps;
};

} // namespace SCHNApps

} // namespace CGoGN

Q_DECLARE_INTERFACE(CGoGN::SCHNApps::Plugin, "CGoGN.SCHNapps.Plugin")

#endif
