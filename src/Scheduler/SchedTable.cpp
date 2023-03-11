/*
 * DShedTable.cpp
 *
 *  Created on: 15 may. 2015 yr.
 *      Author: Dan
 */

#include <string.h>

#include "SchedTable.h"
#include "IContext.h"

namespace Timework
{

// EVENT_REC
////////////////////////////////////////////////////////////////////////////////////////////////////

EVENT_REC EVENT_REC::operator=(const EVENT_REC& rec)
{
	if (this == &rec)
	{
		return *this;
	}

 	set = rec.set;
	timeNext = rec.timeNext;
	timerID = rec.timerID;
	eventN = rec.eventN;
	lostN = rec.lostN;

 	return *this;
}

// SchedTable
////////////////////////////////////////////////////////////////////////////////////////////////////

SchedTable::SchedTable()
{
	// TODO Auto-generated constructor stub

	m_pContext = nullptr;
	m_MaxRecsN = 0;
	m_RecsN = 0;
	m_Found = D_RET_ERROR;
	m_RecsRemain = D_RET_ERROR;
}

SchedTable::~SchedTable()
{
	// TODO Auto-generated destructor stub

}

bool SchedTable::InitContext(IContext* pContext, int demandRecsN)
{
	return pContext->IsExist() ?
			OpenTable(pContext) :
			CreateTable(pContext, demandRecsN);
}

bool SchedTable::OpenTable(IContext* pContext)
{
	if (pContext->Open())
	{
		int size = pContext->Size();

		if (size != D_RET_ERROR)
		{
			m_pContext = pContext;
			m_MaxRecsN = size / sizeof(EVENT_REC);
			m_RecsN = GetValidRecordsCount(m_MaxRecsN);

			return true;
		}
	}

	return false;
}

bool SchedTable::CreateTable(IContext* pContext, int demandRecsN)
{
	int size = GetBinSize(demandRecsN);

	// Initialize table with empty records
	if (pContext->Create(size))
	{
		m_pContext = pContext;
		m_MaxRecsN = demandRecsN;

		return ClearTable();
	}

	return false;
}

int SchedTable::SetEvent(SCHEDULER_EVENT_SET* pSet)
{
	int index = FindEvent(pSet->ID);

	if (index == D_RET_ERROR)
	{
		return AddEvent(pSet);
	}

	if (UpdateEvent(pSet, index))
	{
		return index;
	}

	return D_RET_ERROR;
}

int SchedTable::AddEvent(SCHEDULER_EVENT_SET* pSet)
{
	if (pSet->ID == SCHEDULER_EVENT_SET::ID_AUTO)
	{
		pSet->ID = GetAutoID();
	}

	int index = FindFreePlace();

	if (index == D_RET_ERROR)
	{
		return D_RET_ERROR;
	}

	EVENT_REC rec;
	rec.set = *pSet;

	if (!SetRecord(&rec, index))
	{
		return D_RET_ERROR;
	}

	m_RecsN++;

	return index;
}

bool SchedTable::UpdateEvent(const SCHEDULER_EVENT_SET* pSet, int index)
{
	EVENT_REC rec;
	rec.set = *pSet;

	return SetRecord(&rec, index);
}

bool SchedTable::GetRecord(EVENT_REC* pRec, int index)
{
	return m_pContext->Read(pRec, IndexToPos(index), sizeof(EVENT_REC));
}

bool SchedTable::SetRecord(const EVENT_REC* pRec, int index)
{
	return m_pContext->Write(pRec, IndexToPos(index), sizeof(EVENT_REC));
}

bool SchedTable::IsRecordDeleted(int index)
{
	EVENT_REC rec;

	return GetRecord(&rec, index) && rec.IsDeleted();
}

bool SchedTable::Delete(int ID)
{
	int index = FindEvent(ID);

	return index != D_RET_ERROR ? DeleteRecord(index) : false;
}

bool SchedTable::DeleteRecord(int index)
{
	EVENT_REC rec;

	if (!GetRecord(&rec, index))
	{
		return false;
	}

	if (rec.IsDeleted())
	{
		return true;
	}

	rec.SetDeleted();

	if (SetRecord(&rec, index))
	{
		m_RecsN--;

		return true;
	}

	return false;
}

bool SchedTable::ClearTable()
{
	for (int index = 0; index < m_MaxRecsN; index++)
	{
		if (!DeleteRecord(index))
		{
			return false;
		}
	}

	m_RecsN = 0;

	return true;
}

int SchedTable::GetValidRecordsCount(int recsN)
{
	int count = 0;

	for (int index = 0; index < recsN; index++)
	{
		if (!IsRecordDeleted(index))
		{
			count++;
		}
	}

	return count;
}

bool SchedTable::GetNextTime(int ID, TIME& time)
{
	int index = FindEvent(ID);
	EVENT_REC rec;

	if (index != D_RET_ERROR)
	{
		if (!GetRecord(&rec, index))
		{
			return D_RET_ERROR;
		}

		time = rec.timeNext;

		return true;
	}

	return false;
}

int SchedTable::GetMinNextTime(TIME &timeMin)
{
	EVENT_REC rec;
	int indexMin = D_RET_ERROR;
	timeMin = static_cast<TIME>(D_RET_ERROR);
	int index = First();

	while (index != D_RET_ERROR)
	{
		if (!GetRecord(&rec, index))
		{
			return D_RET_ERROR;
		}

		if (rec.timeNext < timeMin)
		{
			timeMin = rec.timeNext;
			indexMin = index;
		}

		index = Next();
	}

	return indexMin;
}

int SchedTable::FindEvent(int ID)
{
	int index = First();
	EVENT_REC rec;

	while (index != D_RET_ERROR)
	{
		if (!GetRecord(&rec, index))
		{
			return D_RET_ERROR;
		}

		if (rec.set.ID == ID)
		{
			return index;
		}

		index = Next();
	}

	return D_RET_ERROR;
}

int SchedTable::FindByTimer(int timerID)
{
	int index = First();
	EVENT_REC rec;

	while (index != D_RET_ERROR)
	{
		if (!GetRecord(&rec, index))
		{
			return D_RET_ERROR;
		}

		if (rec.timerID == timerID)
		{
			return index;
		}

		index = Next();
	}

	return D_RET_ERROR;
}

int SchedTable::First()
{
	m_Found = 0;
	m_RecsRemain = m_RecsN;

	return Next();
}

int SchedTable::Next()
{
	if (m_RecsRemain)
	{
		EVENT_REC rec;

		if (!GetRecord(&rec, m_Found))
		{
			return D_RET_ERROR;
		}

		while (m_Found < m_MaxRecsN && rec.IsDeleted())
		{
			m_Found++;

			if (!GetRecord(&rec, m_Found))
			{
				return D_RET_ERROR;
			}
		}

		if (m_Found < m_MaxRecsN)
		{
			m_RecsRemain--;

			return m_Found++;
		}
	}

	return D_RET_ERROR;
}

int SchedTable::FindFreePlace()
{
	EVENT_REC rec;

	for (int index = 0; index < m_MaxRecsN; index++)
	{
		if (!GetRecord(&rec, index))
		{
			return D_RET_ERROR;
		}

		if (rec.IsDeleted())
		{
			return index;
		}
	}

	return D_RET_ERROR;
}

int SchedTable::GetAutoID()
{
	int ID = SCHED_USER_ID_BOUNDARY;

	while (FindEvent(ID) != D_RET_ERROR)
	{
		ID++;
	}

	return ID;
}

int SchedTable::IndexToPos(int index)
{
	return index * sizeof(EVENT_REC);
}

int SchedTable::PosToIndex(int pos)
{
	return pos / sizeof(EVENT_REC);
}

} //namespace Timework

