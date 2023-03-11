/*
 * TimerTable.cpp
 *
 *  Created on: 27 may. 2015 yr.
 *      Author: dan
 */

#include <string.h>

#include "TimerTable.h"

namespace Timework
{

TimerTable::TimerTable()
{
	// TODO Auto-generated constructor stub

	m_pRecs = nullptr;
	m_RecsN = 0;
	m_MaxRecsN = 0;
	m_pFound = nullptr;
	m_RecsRemain = D_RET_ERROR;
}

TimerTable::~TimerTable()
{
	// TODO Auto-generated destructor stub
}

int TimerTable::GetBinSize(int demandRecsN)
{
	return demandRecsN * sizeof(TIMER_REC);
}

bool TimerTable::SetMem(void* pMem, int size)
{
	m_pRecs = static_cast<TIMER_REC*>(pMem);
	m_MaxRecsN = size / sizeof(TIMER_REC);

	::memset(pMem, FILLER, size);

	return true;
}

TIMER_REC* TimerTable::Add(const TIMER_SET* pSet)
{
	TIMER_REC* pRec = FindFreePlace();

	if (pRec != nullptr)
	{
		m_RecsN++;
		pRec->set = *pSet;
		pRec->ID = GetNewID();
	}

	return pRec;
}

TIMER_REC* TimerTable::Update(int ID, const TIMER_SET* pSet)
{
	TIMER_REC* pRec = FindTimer(ID);

	if (pRec != nullptr)
	{
		pRec->set = *pSet;

		return pRec;
	}

	return nullptr;
}

bool TimerTable::Delete(int ID)
{
	TIMER_REC* pRec = FindTimer(ID);

	return (pRec != nullptr) ? DeleteRec(pRec) : false;
}

TIMER_REC* TimerTable::FindTimer(int ID)
{
	TIMER_REC* pRec = First();

	while (pRec)
	{
		if (pRec->ID == ID)
		{
			return pRec;
		}

		pRec = Next();
	}

	return nullptr;
}

TIMER_REC* TimerTable::First()
{
	m_pFound = m_pRecs;
	m_RecsRemain = m_RecsN;

	return Next();
}

TIMER_REC* TimerTable::Next()
{
	if (m_RecsRemain)
	{
		TIMER_REC* pEnd = m_pRecs + m_MaxRecsN;

		while (m_pFound < pEnd && m_pFound->IsDeleted())
		{
			m_pFound++;
		}

		if (m_pFound < pEnd)
		{
			m_RecsRemain--;

			return m_pFound++;
		}
	}

	return nullptr;
}

TIMER_REC* TimerTable::FindFreePlace()
{
	TIMER_REC* pEnd = m_pRecs + m_MaxRecsN;

	for (TIMER_REC* pRec = m_pRecs; pRec < pEnd; pRec++)
	{
		if (pRec->IsDeleted())
		{
			return pRec;
		}
	}

	return nullptr;
}

bool TimerTable::DeleteRec(TIMER_REC* pRec)
{
	if (!pRec->IsDeleted())
	{
		pRec->SetDeleted();
		m_RecsN--;
	}

	return true;
}

int TimerTable::GetNewID()
{
	int ID = D_RET_ERROR;
	TIMER_REC* pRec = First();

	while (pRec)
	{
		if (!pRec->IsDeleted() && pRec->ID > ID)
		{
			ID = pRec->ID;
		}

		pRec = Next();
	}

	return ++ID;
}

TIMER_REC* TimerTable::GetMinNextTime(TIMEPREC &timeMin)
{
	timeMin = static_cast<TIMEPREC>(D_RET_ERROR);

	TIMER_REC* pRecMin = nullptr;
	TIMER_REC* pRec = First();

	while (pRec)
	{
		if (pRec->timeNext < timeMin)
		{
			timeMin = pRec->timeNext;
			pRecMin = pRec;
		}

		pRec = Next();
	}

	return pRecMin;
}

} // namespace Timework
