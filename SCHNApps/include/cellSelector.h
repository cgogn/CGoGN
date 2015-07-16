#ifndef _CELL_SELECTOR_H_
#define _CELL_SELECTOR_H_

#include <QObject>
#include <QString>
#include <QList>

#include "Topology/generic/dart.h"
#include "Topology/generic/genericmap.h"
#include "Topology/generic/cellmarker.h"
#include "Topology/generic/traversor/traversorCell.h"

#include "slot_debug.h"

#include "dll.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNAPPS_API CellSelectorGen : public QObject
{
	Q_OBJECT

public:
	static unsigned int selectorCount;

	CellSelectorGen(const QString& name);
	virtual ~CellSelectorGen()
	{}

	inline const QString& getName() { return m_name; }

	virtual unsigned int getOrbit() const = 0;

	virtual unsigned int getNbSelectedCells() const = 0;

	virtual void rebuild() = 0;

	inline void checkChange()
	{
		if(m_selectionChanged)
		{
			DEBUG_EMIT("selectedCellsChanged");
			emit(selectedCellsChanged());
			m_selectionChanged = false;
		}
	}

	inline void setMutuallyExclusive(bool b) { m_isMutuallyExclusive = b; }
	inline bool isMutuallyExclusive() const { return m_isMutuallyExclusive; }
	virtual void setMutuallyExclusiveSet(const QList<CellSelectorGen*>& mex) = 0;

	virtual void clearAll() = 0;

signals:
	void selectedCellsChanged();

protected:
	QString m_name;
	bool m_isMutuallyExclusive;
	bool m_selectionChanged;
};

template <typename MAP, unsigned int ORBIT>
class CellSelector : public CellSelectorGen
{
	typedef Cell<ORBIT> CELL;
	typedef CellSelector<MAP, ORBIT> SELECTOR;

public:
	CellSelector(MAP& map, const QString& name) :
		CellSelectorGen(name),
		m_map(map),
		m_cm(map)
	{}

	~CellSelector()
	{}

	inline unsigned int getOrbit() const { return ORBIT; }

	inline const CellMarker<MAP, ORBIT>& getMarker() { return m_cm; }

	inline const std::vector<CELL>& getSelectedCells() { return m_cells; }

	inline unsigned int getNbSelectedCells() const { return (unsigned int)(m_cells.size()); }

	inline void select(CELL c, bool emitSignal = true)
	{
		if(!m_cm.isMarked(c))
		{
			m_cm.mark(c);
			m_cells.push_back(c);
			if(m_isMutuallyExclusive && !m_mutuallyExclusive.empty())
			{
				foreach(SELECTOR* cs, m_mutuallyExclusive)
					cs->unselect(c, emitSignal);
			}
			if(emitSignal)
			{
				DEBUG_EMIT("selectedCellsChanged");
				emit(selectedCellsChanged());
			}
			else
				m_selectionChanged = true;
		}
	}

	inline void select(const std::vector<CELL>& c)
	{
		for(unsigned int i = 0; i < c.size(); ++i)
			select(c[i], false);
		checkChange();
		if(m_isMutuallyExclusive && !m_mutuallyExclusive.empty())
		{
			foreach(SELECTOR* cs, m_mutuallyExclusive)
				cs->checkChange();
		}
	}

	inline void unselect(CELL c, bool emitSignal = true)
	{
		if(m_cm.isMarked(c))
		{
			unsigned int emb = m_map.getEmbedding(c);
			bool found = false;
			unsigned int i;
			for(i = 0; i < m_cells.size() && !found; ++i)
			{
				if(m_map.template getEmbedding<ORBIT>(m_cells[i]) == emb)
					found = true ;
			}
			if(found)
			{
				m_cm.unmark(m_cells[i-1]);
				m_cells[i-1] = m_cells.back();
				m_cells.pop_back();
				if(emitSignal)
				{
					DEBUG_EMIT("selectedCellsChanged");
					emit(selectedCellsChanged());
				}
				else
					m_selectionChanged = true;
			}
		}
	}

	inline void unselect(const std::vector<CELL>& c)
	{
		for(unsigned int i = 0; i < c.size(); ++i)
			unselect(c[i], false);
		checkChange();
	}

	inline bool isSelected(CELL c)
	{
		return m_cm.isMarked(c);
	}

	void rebuild()
	{
		m_cells.clear();
		foreach_cell<ORBIT>(m_map, [&] (CELL c)
		{
			if(m_cm.isMarked(c))
				m_cells.push_back(c);
		});
		emit(selectedCellsChanged());
	}

	inline void setMutuallyExclusiveSet(const QList<CellSelectorGen*>& mex)
	{
		m_mutuallyExclusive.clear();
		foreach(CellSelectorGen* cs, mex)
		{
			if(cs != this)
			{
				SELECTOR* s = dynamic_cast<SELECTOR*>(cs);
				if (s)
					m_mutuallyExclusive.append(s);
			}
		}
	}


	inline void clearAll()
	{
		std::vector<CELL> tempo;
		m_cells.swap(tempo);
		m_cm.unmarkAll();
		m_selectionChanged = true;
		checkChange();
	}

private:
	MAP& m_map;
	CellMarker<MAP, ORBIT> m_cm;

	std::vector<CELL> m_cells;

	QList<SELECTOR*> m_mutuallyExclusive;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
