#ifndef _CELL_SELECTOR_H_
#define _CELL_SELECTOR_H_

#include <QObject>
#include <QString>
#include <QList>

#include "Topology/generic/dart.h"
#include "Topology/generic/genericmap.h"
#include "Topology/generic/cellmarker.h"

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

	virtual void select(Dart d, bool emitSignal = true) = 0;
	virtual void unselect(Dart d, bool emitSignal = true) = 0;

	inline void select(const std::vector<Dart>& d)
	{
		for(unsigned int i = 0; i < d.size(); ++i)
			select(d[i], false);
		checkChange();
		if(m_isMutuallyExclusive && !m_mutuallyExclusive.empty())
		{
			foreach(CellSelectorGen* cs, m_mutuallyExclusive)
				cs->checkChange();
		}
	}

	inline void unselect(const std::vector<Dart>& d)
	{
		for(unsigned int i = 0; i < d.size(); ++i)
			unselect(d[i], false);
		checkChange();
	}

	virtual bool isSelected(Dart d) = 0;

	inline void setMutuallyExclusive(bool b) { m_isMutuallyExclusive = b; }
	inline bool isMutuallyExclusive() const { return m_isMutuallyExclusive; }
	inline void setMutuallyExclusiveSet(const QList<CellSelectorGen*>& mex)
	{
		m_mutuallyExclusive.clear();
		foreach(CellSelectorGen* cs, mex)
		{
			if(cs != this)
				m_mutuallyExclusive.append(cs);
		}
	}

	inline void checkChange()
	{
		if(m_selectionChanged)
		{
			emit(selectedCellsChanged());
			m_selectionChanged = false;
		}
	}

signals:
	void selectedCellsChanged();

protected:
	QString m_name;
	std::vector<Dart> m_cells;

	bool m_selectionChanged;

	bool m_isMutuallyExclusive;
	QList<CellSelectorGen*> m_mutuallyExclusive;
};

template <unsigned int ORBIT>
class CellSelector : public CellSelectorGen
{
public:
	CellSelector(GenericMap& map, const QString& name, unsigned int thread = 0) :
		CellSelectorGen(name),
		m_map(map),
		m_cm(map, thread)
	{}

	~CellSelector()
	{}

	inline unsigned int getOrbit() { return ORBIT; }

	inline const CellMarker<ORBIT>& getMarker() { return m_cm; }

	inline void select(Dart d, bool emitSignal = true)
	{
		if(!m_cm.isMarked(d))
		{
			m_cells.push_back(d);
			m_cm.mark(d);
			if(m_isMutuallyExclusive && !m_mutuallyExclusive.empty())
			{
				foreach(CellSelectorGen* cs, m_mutuallyExclusive)
					cs->unselect(d, emitSignal);
			}
			if(emitSignal)
				emit(selectedCellsChanged());
			else
				m_selectionChanged = true;
		}
	}

	inline void unselect(Dart d, bool emitSignal = true)
	{
		if(m_cm.isMarked(d))
		{
			unsigned int v = m_map.getEmbedding<ORBIT>(d);
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
				else
					m_selectionChanged = true;
			}
		}
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
