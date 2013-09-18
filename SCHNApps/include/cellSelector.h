#ifndef _CELL_SELECTOR_H_
#define _CELL_SELECTOR_H_

#include <QString>

#include "Topology/generic/dart.h"
#include "Topology/generic/genericmap.h"
#include "Topology/generic/cellmarker.h"

#include <QObject>

namespace CGoGN
{

namespace SCHNApps
{

class CellSelectorGen : public QObject
{
	Q_OBJECT

public:
	static unsigned int selectorCount;

	CellSelectorGen(const QString& name);
	virtual ~CellSelectorGen()
	{}

	inline const QString& getName() { return m_name; }

	inline const std::vector<Dart>& getSelectedCells() { return m_cells; }

	virtual unsigned int getOrbit() = 0;

	virtual void select(Dart d, bool emitSignal) = 0;
	virtual void unselect(Dart d, bool emitSignal) = 0;

	virtual void select(const std::vector<Dart>& d) = 0;
	virtual void unselect(const std::vector<Dart>& d) = 0;

	virtual bool isSelected(Dart d) = 0;

signals:
	void selectedCellsChanged();

protected:
	QString m_name;
	std::vector<Dart> m_cells;
};

template <unsigned int ORBIT>
class CellSelector : public CellSelectorGen
{
public:
	CellSelector(GenericMap& m, const QString& name, unsigned int thread = 0) :
		CellSelectorGen(name),
		m_map(m),
		m_cm(m, thread)
	{}

	~CellSelector()
	{}

	inline unsigned int getOrbit() { return ORBIT; }

	inline const CellMarker<ORBIT>& getMarker() { return m_cm; }

	inline void select(Dart d, bool emitSignal = true)
	{
		unsigned int v = m_map.getEmbedding<ORBIT>(d);
		if(!m_cm.isMarked(v))
		{
			m_cells.push_back(d);
			m_cm.mark(v);
			if(emitSignal)
				emit(selectedCellsChanged());
		}
	}

	inline void unselect(Dart d, bool emitSignal = true)
	{
		unsigned int v = m_map.getEmbedding<ORBIT>(d);
		if(m_cm.isMarked(v))
		{
			bool found = false;
			unsigned int i;
			for(i = 0; i < m_cells.size() && !found; ++i)
			{
				if(m_map.getEmbedding<ORBIT>(m_cells[i]) == v)
					found = true ;
			}
			if(found)
			{
				m_cm.unmark(m_cells[i-1]);
				m_cells[i-1] = m_cells.back();
				m_cells.pop_back();
				if(emitSignal)
					emit(selectedCellsChanged());
			}
		}
	}

	inline void select(const std::vector<Dart>& d)
	{
		for(unsigned int i = 0; i < d.size(); ++i)
			select(d[i], false);
		emit(selectedCellsChanged());
	}

	inline void unselect(const std::vector<Dart>& d)
	{
		for(unsigned int i = 0; i < d.size(); ++i)
			unselect(d[i], false);
		emit(selectedCellsChanged());
	}

	inline bool isSelected(Dart d)
	{
		return m_cm.isMarked(d);
	}

private:
	GenericMap& m_map;
	CellMarker<ORBIT> m_cm;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
