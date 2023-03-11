/*
 * Scheduler.cpp
 *
*  Created on: 18 may. 2015 yr.
  *      Author: Dan
 */

#include <cstring>

#include "Scheduler.h"
#include "Timer.h"

namespace Timework
{

Scheduler::Scheduler()
{
	// TODO Auto-generated constructor stub

	m_pTimer = nullptr;
	m_pPE = nullptr;
	m_pPT = nullptr;
	m_pCalendar = nullptr;
}

Scheduler::~Scheduler()
{
	// TODO Auto-generated destructor stub
}

bool Scheduler::SetCalendar(Calendar* pCalendar)
{
	m_pCalendar = pCalendar;
	m_pCalendar->SetEpoch(EPOCH1970_YEAR, EPOCH1970_MONTH, EPOCH1970_DAY);

	return true;
}

bool Scheduler::SetCalendar(Calendar* pCalendar, int yearEpoch, int monthEpoch, int dayEpoch)
{
	m_pCalendar = pCalendar;
	m_pCalendar->SetEpoch(yearEpoch, monthEpoch, dayEpoch);

	return true;
}

bool Scheduler::InitTable(IContext* pContext, TIME timeCurrent, int demandRecsN)
{
	if (m_tableEvents.InitContext(pContext, demandRecsN))
	{
		return InitFromTable(timeCurrent);
	}

	return false;
}

bool Scheduler::EventExist(int ID)
{
	return m_tableEvents.FindEvent(ID) != D_RET_ERROR;
}

int Scheduler::SetEvent(SCHEDULER_EVENT_SET* pSet, TIME timeCurrent)
{
	FindAndDeleteTimer(pSet->ID);

	int index = m_tableEvents.SetEvent(pSet);

	if (index != D_RET_ERROR)
	{
		if (InitEvent(index, timeCurrent))
		{
			return pSet->ID;
		}
	}

	return D_RET_ERROR;
}

bool Scheduler::DeleteEvent(int ID)
{
	FindAndDeleteTimer(ID);

	return m_tableEvents.Delete(ID);
}

bool Scheduler::FindAndDeleteTimer(int eventID)
{
	int index = m_tableEvents.FindEvent(eventID);

	if (index == D_RET_ERROR)
	{
		return true;
	}

	EVENT_REC rec;
	if (!m_tableEvents.GetRecord(&rec, index))
	{
		return false;
	}

	return m_pTimer->DeleteTimer(rec.timerID);
}

bool Scheduler::InitFromTable(TIME timeCurrent)
{
	int index = m_tableEvents.First();

	while (index != D_RET_ERROR)
	{
		if (!InitEvent(index, timeCurrent))
		{
			return false;
		}

		index = m_tableEvents.Next();
	}

	return true;
}

bool Scheduler::GetNextTime(int ID, TIME& time)
{
	return m_tableEvents.GetNextTime(ID, time);
}

bool Scheduler::CorrectTime(TIME timeOld, TIME timeNew)
{
	EVENT_REC rec;
	int index = m_tableEvents.First();

	while (index != D_RET_ERROR)
	{
		if (!m_tableEvents.GetRecord(&rec, index))
		{
			return D_RET_ERROR;
		}

		m_pTimer->DeleteTimer(rec.timerID);

		if (!InitEvent(index, timeNew, timeNew  < timeOld))
		{
			return false;
		}

		index = m_tableEvents.Next();
	}

	return true;
}

bool Scheduler::ProcessTimer(TIMER_TICK* pTick)
{
	int index = m_tableEvents.FindByTimer(pTick->ID);

	if (index != D_RET_ERROR)
	{
		EVENT_REC rec;

		if (m_tableEvents.GetRecord(&rec, index))
		{
			if (!ProcessEvent(&rec, pTick->timeCurrent, false) ||
					CheckElapsed(index, &rec))
			{
				m_tableEvents.DeleteRecord(index);

				return true;
			}

			SetTimer(&rec, pTick->timeCurrent);
			m_tableEvents.SetRecord(&rec, index);

			return true;
		}
	}

	return false;
}

bool Scheduler::InitEvent(int index, TIME timeCurrent, bool recharge)
{
	EVENT_REC rec;

	if (!m_tableEvents.GetRecord(&rec, index))
	{
		return false;
	}

	if (rec.eventN == 0 || recharge)
	{
		// First initialization

		rec.timeNext = rec.set.IsUseStartTime() ? rec.set.timeStart : timeCurrent;
		GetNextEvent(&rec.set.filter, &rec.timeNext, true);
	}

	// Lost events handling
	while (rec.timeNext <= timeCurrent)
	{
		if (rec.set.IsProcessLost())
		{
			if (!ProcessEvent(&rec, timeCurrent, rec.timeNext < timeCurrent) ||
					CheckElapsed(index, &rec))
			{
				m_tableEvents.DeleteRecord(index);

				return true;
			}
		}
		else
		{
			if (rec.set.IsIgnoreLost())
			{
				rec.eventN++;
			}

			rec.lostN++;

			GetNextEvent(&rec.set.filter, &rec.timeNext, false);
		}
	}

	return SetTimer(&rec, timeCurrent) && m_tableEvents.SetRecord(&rec, index);
}

bool Scheduler::SetTimer(EVENT_REC* pRec, TIME timeCurrent)
{
	TIMER_SET set;
	set.fpPT = m_pPT;
	set.timeStart = pRec->timeNext;
	set.timePeriod = 1;
	set.timeBind = 0;
	set.signalsN = 1;
	set.SetUseStartTime();
	set.SetAbsStartTime();
	set.SetProcessLost();

	pRec->timerID = m_pTimer->SetTimer(&set, timeCurrent);

	return pRec->timerID != D_RET_ERROR;
}

bool Scheduler::ProcessEvent(EVENT_REC* pRec, TIME timeCurrent, bool lost)
{
	SCHEDULER_EVENT event;

	event.ID = pRec->set.ID;
	event.time = pRec->timeNext;
	event.timeCurrent = timeCurrent;
	event.eventN = pRec->eventN;
	event.lostN = pRec->lostN;
	event.lost = lost;

	m_pPE(&event);

	pRec->eventN++;
	pRec->lostN = 0;

	return GetNextEvent(&pRec->set.filter, &pRec->timeNext, false);
}

bool Scheduler::GetNextEvent(const SCHEDULER_EVENT_FILTER* pFilter, TIME* pTime, bool normalize)
{
	// Returns false if filter is expired

	TF_FILTER filter;
	TF_HEADER header;
	TF_RANGE ranges[CALENDAR_TIME::PAR_N];

	// Transforms SCHEDULER_EVENT_FILTER to TF_FILTER
	int index = 0;
	for (int ii = CALENDAR_TIME::PAR_N - 1; ii >= 0 ; ii--)
	{
		if (!(&pFilter->year + ii)->IsStepUndefined())
		{
			header.btIncl[CALENDAR_TIME::PAR_N - ii - 1] = 1;
			ranges[index] = *(&pFilter->year + ii);
			index++;
		}
	}

	filter.pHeader = &header;
	filter.pRanges = ranges;

	CALENDAR_TIME time;
	m_pCalendar->SecToDateTimeEpoch(*pTime, &time);

	TimeFilter timefilter;
	timefilter.SetCalendar(m_pCalendar);

	if (timefilter.FilterNext(&time, &filter, normalize))
	{
		// Transforms CALENDAR_TIME to TIME
		*pTime = m_pCalendar->DateTimeToSecEpoch(&time);

		return true;
	}

	return false;
}

bool Scheduler::CheckElapsed(int index, EVENT_REC* pRec)
{
	return !pRec->set.IsInfifnite() && pRec->eventN >= pRec->set.eventsN;
}

} // namespace Timework
