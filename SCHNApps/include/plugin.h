#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <QtPlugin>
#include <QAction>

#include <GL/glew.h>

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
	Plugin() : m_schnapps(NULL)
	{
		glewInit();
	}

	virtual ~Plugin() {}

	const QString& getName() const { return m_name; }

public slots:
	QString getName() { return m_name; }
	const QString& getFilePath() { return m_filePath; }
	SCHNApps* getSCHNApps() { return m_schnapps; }

private:
	void setName(const QString& name) { m_name = name; }
	void setFilePath(const QString& f) { m_filePath = f; }
	void setSCHNApps(SCHNApps* s) { m_schnapps = s; }

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
