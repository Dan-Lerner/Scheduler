/*
 * Timer.cpp
 *
 *  Created on: 27 may. 2015 yr.
 *      Author: dan
 */

#include "Timer.h"

namespace Timework
{

Timer::Timer()
{
	// TODO Auto-generated constructor stub

}

Timer::~Timer()
{
	// TODO Auto-generated destructor stub
}

int Timer::GetTableBinSize(int demandTimersN)
{
	return m_tableTimer.GetBinSize(demandTimersN);
}

bool Timer::SetTableMem(void* pMem, int size)
{
	return m_tableTimer.SetMem(pMem, size);
}

int Timer::GetTimersCount()
{
	return m_tableTimer.GetTimersCount();
}

bool Timer::CheckTimers(TIMEPREC timeCurrent)
{
	TIMEPREC timeMin;
	TIMER_REC* pRec;

	while ((pRec = m_tableTimer.GetMinNextTime(timeMin)) != nullptr && timeMin <= timeCurrent)
	{
		if (!ProcessTimer(pRec, timeCurrent))
		{
			return false;
		}
	}

	return true;
}

int Timer::SetTimer(const TIMER_SET* pSet, TIMEPREC timeCurrent)
{
	TIMER_REC* pRec = m_tableTimer.Add(pSet);

	if (pRec != nullptr)
	{
		if (InitTimer(pRec, timeCurrent))
		{
			return pRec->ID;
		}

		m_tableTimer.DeleteRec(pRec);
	}

	return D_RET_ERROR;
}

bool Timer::RechargeTimer(int ID, const TIMER_SET* pSet, TIMEPREC timeCurrent)
{
	TIMER_REC* pRec = m_tableTimer.Update(ID, pSet);

	if (pRec != nullptr)
	{
		if (InitTimer(pRec, timeCurrent))
		{
			return true;
		}

		m_tableTimer.DeleteRec(pRec);
	}

	return false;
}

bool Timer::CorrectTime(TIMEPREC timeOld, TIMEPREC timeNew)
{
	TIMER_REC* pRec = m_tableTimer.First();

	while (pRec)
	{
		if (!pRec->IsDeleted() && !pRec->set.IsAbsStartTime())
		{
			pRec->timeNext += (timeNew - timeOld);

			if (pRec->set.IsBind())
			{
				pRec->timeNext = BindTime(pRec->set.timeBind, pRec->set.timePeriod, timeNew);
			}
		}

		pRec = m_tableTimer.Next();
	}

	return true;
}

bool Timer::DeleteTimer(int ID)
{
	return m_tableTimer.Delete(ID);
}

bool Timer::GetNextTime(int ID, TIMEPREC* pTimeNext)
{
	TIMER_REC* pRec = m_tableTimer.FindTimer(ID);

	if (pRec != nullptr)
	{
		*pTimeNext = pRec->timeNext;

		return true;
	}

	return false;
}

TIMEPREC Timer::BindTime(TIMEPREC timeBind, TIMEPREC timeInterval, TIMEPREC timeToBind)
{
	char sign = timeToBind > timeBind ? 1 : -1;
	TIMEPREC timeDiff = sign > 0 ? timeToBind - timeBind : timeBind - timeToBind;
	TIMEPREC timeReduce = (timeDiff / timeInterval) * timeInterval;

	return timeBind + sign * timeReduce + ((sign > 0) &&
			(timeReduce < timeDiff) ?
			timeInterval :
			0);
}

bool Timer::InitTimer(TIMER_REC* pRec, TIMEPREC timeCurrent)
{
	if (pRec->set.IsUseStartTime())
	{
		pRec->timeNext = pRec->set.IsAbsStartTime() ?
				pRec->set.timeStart :
				timeCurrent + pRec->set.timeStart;
	}
	else
	{
		pRec->timeNext = timeCurrent + pRec->set.timePeriod;
	}

	if (pRec->set.IsBind())
	{
		pRec->timeNext = BindTime(pRec->set.timeBind, pRec->set.timePeriod, pRec->timeNext);
	}

	pRec->signalsN = 0;
	pRec->lostN = 0;

	return true;
}

bool Timer::ProcessTimer(TIMER_REC* pRec, TIMEPREC timeCurrent)
{
	int lostN = pRec->set.timePeriod != 0 ? (timeCurrent - pRec->timeNext) / pRec->set.timePeriod : 0;

	if ((lostN == 0) || pRec->set.IsProcessLost())
	{
		ProcessTick(pRec, timeCurrent, lostN > 0);
		CheckElapsed(pRec);

		return true;
	}

	if (pRec->set.IsIgnoreLost())
	{
		pRec->signalsN += lostN;

		if (CheckElapsed(pRec))
		{
			return true;
		}
	}

	pRec->timeNext += pRec->set.timePeriod * lostN;
	pRec->lostN += lostN;

	return true;
}

bool Timer::ProcessTick(TIMER_REC* pRec, TIMEPREC timeCurrent, bool lost)
{
	TIMER_TICK tick;

	tick.ID = pRec->ID;
	tick.time = pRec->timeNext;
	tick.timeCurrent = timeCurrent;
	tick.signalN = pRec->signalsN;
	tick.lostN = pRec->lostN;
	tick.lost = lost;

	pRec->set.fpPT(&tick);

	pRec->timeNext = (!lost && !pRec->set.IsBind() && pRec->set.IsFixedPeriod() ?
			timeCurrent :
			pRec->timeNext) + pRec->set.timePeriod;
	pRec->signalsN++;
	pRec->lostN = 0;

	return true;
}

bool Timer::CheckElapsed(TIMER_REC* pRec)
{
	if (!pRec->set.IsInfinite() && pRec->signalsN >= pRec->set.signalsN)
	{
		// Amount of signals exceeds its limit
		m_tableTimer.DeleteRec(pRec);

		return true;
	}

	return false;
}

} // namespace Timework
