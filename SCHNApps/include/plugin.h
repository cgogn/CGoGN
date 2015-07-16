#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <QtPlugin>
#include <QAction>
#include <GL/glew.h>


#include "types.h"
#include "schnapps.h"

#include "dll.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNAPPS_API Plugin : public QObject
{
	Q_OBJECT

	friend class SCHNApps;

public:
	Plugin();
	virtual ~Plugin();

public slots:
	/// [PYTHON] get object name
	QString getName();

	/// [PYTHON] get file path to the shared library of plugin
	QString getFilePath();

	/// get schnapps object
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

//	void pythonRecording(const QString& slotName, const QString& returned = "",  const QString& param1 = "", const QString& param2 = "", const QString& param3 = "", const QString& param4 = "");

	//template <typename T1, typename T2, typename T3, typename T4>
	//inline void pythonRecording(QString slotName, QString returned = "", T1 param1 = "", T2 param2 = "", T3 param3 = "", T4 param4 = "");

	template <typename T1>
	inline void pythonRecording(QString slotName, QString returned, T1 param1);

	template <typename T1, typename T2>
	inline void pythonRecording(QString slotName, QString returned, T1 param1, T2 param2);

	template <typename T1, typename T2, typename T3>
	inline void pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3);

	template <typename T1, typename T2, typename T3, typename T4>
	inline void pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4);

	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	inline void pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5);

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	inline void pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6);


	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
	inline void pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9, T10 param10, T11 param11);


};

template <typename T>
inline T pyR_stringify(T v)
{
	return v;
}

template <>
inline QString pyR_stringify(QString v)
{
	// test if v containe non ascii char
	bool needDecode = false;
	int i = 0;
	while (!needDecode && (i < v.size()))
	{
		if (v[i].unicode()>127)
			needDecode = true;
		++i;
	}

	if (needDecode)
		return "\"" + v + "\".decode('latin-1')";
	else
		return "\"" + v + "\"";
}


template <typename T1>
void Plugin::pythonRecording(QString slotName, QString returned, T1 param1)
{
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
	{
		if (!returned.isEmpty())
		{
			m_schnapps->pythonVarDeclare(returned);
			*rec << returned << " = ";
		}
		*rec << this->getName() << "." << slotName << "(" << pyR_stringify(param1) << ");" << endl;
	}
}

template <typename T1, typename T2>
void Plugin::pythonRecording(QString slotName, QString returned, T1 param1, T2 param2)
{
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
	{
		if (!returned.isEmpty())
		{
			m_schnapps->pythonVarDeclare(returned);
			*rec << returned << " = ";
		}
		*rec << this->getName() << "." << slotName << "(" << pyR_stringify(param1) << ", " << pyR_stringify(param2) << ");" << endl;
	}
}

template <typename T1, typename T2, typename T3>
void Plugin::pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3)
{
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
	{
		if (!returned.isEmpty())
		{
			m_schnapps->pythonVarDeclare(returned);
			*rec << returned << " = ";
		}
		*rec << this->getName() << "." << slotName << "(" << pyR_stringify(param1) << ", " << pyR_stringify(param2) << ", " << pyR_stringify(param3) << ");" << endl;
	}
}


template <typename T1, typename T2, typename T3, typename T4>
void Plugin::pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4)
{
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
	{
		if (!returned.isEmpty())
		{
			m_schnapps->pythonVarDeclare(returned);
			*rec << returned << " = ";
		}
		*rec << this->getName() << "." << slotName << "(" << pyR_stringify(param1) << ", " << pyR_stringify(param2) << ", " << pyR_stringify(param3) << ", " << pyR_stringify(param4) << ");" << endl;
	}
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
void Plugin::pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5)
{
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
	{
		if (!returned.isEmpty())
		{
			m_schnapps->pythonVarDeclare(returned);
			*rec << returned << " = ";
		}
		*rec << this->getName() << "." << slotName << "(" << pyR_stringify(param1) << ", " << pyR_stringify(param2) << ", ";
		*rec << pyR_stringify(param3) << ", " << pyR_stringify(param4) << ", " << pyR_stringify(param5) << ");" << endl;
	}
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void Plugin::pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6)
{
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
	{
		if (!returned.isEmpty())
		{
			m_schnapps->pythonVarDeclare(returned);
			*rec << returned << " = ";
		}
		*rec << this->getName() << "." << slotName << "(" << pyR_stringify(param1) << ", " << pyR_stringify(param2) << ", ";
		*rec << pyR_stringify(param3) << ", " << pyR_stringify(param4) << ", " << pyR_stringify(param5) << ", " << pyR_stringify(param6)  << ");" << endl;
	}
}



template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
void Plugin::pythonRecording(QString slotName, QString returned, T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9, T10 param10, T11 param11)
{
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
	{
		if (!returned.isEmpty())
		{
			m_schnapps->pythonVarDeclare(returned);
			*rec << returned << " = ";
		}
		*rec << this->getName() << "." << slotName << "(" << pyR_stringify(param1) << ", " << pyR_stringify(param2) << ", ";
		*rec << pyR_stringify(param3) << ", " << pyR_stringify(param4) << ", " << pyR_stringify(param5) << ", ";
		*rec << pyR_stringify(param6) << ", " << pyR_stringify(param7) << ", " << pyR_stringify(param8) << ", ";
		*rec << pyR_stringify(param9) << ", " << pyR_stringify(param10) << ", " << pyR_stringify(param11) << ");" << endl;
	}
}
	



} // namespace SCHNApps

} // namespace CGoGN

Q_DECLARE_INTERFACE(CGoGN::SCHNApps::Plugin, "CGoGN.SCHNapps.Plugin")

#endif
