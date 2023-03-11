/*
 * DSheduler.h
 *
 *  Created on: 18 may. 2015 yr.
 *      Author: Dan
 */

#ifndef DSHEDULER_H_
#define DSHEDULER_H_

#include "SchedStr.h"
#include "TimerStr.h"
#include "TimeFilter.h"
#include "SchedTable.h"
#include "IContext.h"

namespace Timework
{

class Timer;
class Calendar;

// Scheduler
// This class provides event generation according to assigned calendar-time algorithms
class Scheduler
{
public:

	// Epoch default values
	static const int EPOCH1970_YEAR = 1970;
	static const int EPOCH1970_MONTH = 1;
	static const int EPOCH1970_DAY = 1;

public:

	Scheduler();
	virtual ~Scheduler();

public:

	// Sets a calendar object
	bool SetCalendar(Calendar* pCalendar);
	bool SetCalendar(Calendar* pCalendar, int yearEpoch, int monthEpoch, int dayEpoch);

	// Table initialization
	bool InitTable(IContext* pContext, TIME timeCurrent, int demandRecsN = 0);

	// Sets an attached Timer for time period operations
	// Also sets an external handler fpProcessTimer for timer signals
	// (it should call ProcessTimer(D_TIMER_TICK* tick) of this object
	bool SetTimer(Timer* pTimer, fpProcessTimer pPT)
	{
		m_pTimer = pTimer;
		m_pPT = pPT;
		return true;
	};

	//	Example
	//
	//	Scheduler sched;
	//	sched.SetTimer(&timerForSched, ProcessTimerForShed);
	//
	//	bool ProcessTimerForSched(D_TIMER_TICK* pTick)
	//	{
	//		return sched.ProcessTimer(pTick);
	//	}

	// Sets a handler for the scheduler events
	bool SetEventDispather(fpProcessEvents pPE) { m_pPE = pPE; return true; };

	// Handler for attached timer signals
	bool ProcessTimer(TIMER_TICK* pTick);

public:

	// Checks if event record exists
	bool EventExist(int ID);

	// Charges new event record with specified settings
	int SetEvent(SCHEDULER_EVENT_SET* pSet, TIME timeCurrent);

	// Deletes event record
	bool DeleteEvent(int ID);

	// Initializes events from the table.
	// Call it at system startup
	bool InitFromTable(TIME timeCurrent);

	// Gets time of next event
	bool GetNextTime(int ID, TIME& time);

	// This method must be called after RTC correction
	bool CorrectTime(TIME timeOld, TIME timeNew);

protected:

	// Deletes Timer with eventID if any
	bool FindAndDeleteTimer(int eventID);

	// Initializing of new event record
	bool InitEvent(int index,TIME timeCurrent, bool recharge = false);

	// Sets timer for next event generation
	bool SetTimer(EVENT_REC* pRec, TIME timeCurrent);

	// Processes fired event
	bool ProcessEvent(EVENT_REC* pRec, TIME timeCurrent, bool lost);

public:

	// Calculates time for the next event
	bool GetNextEvent(const SCHEDULER_EVENT_FILTER* pFilter, TIME* pTime, bool normalize = false);

	// Checks if event is expired
	bool CheckElapsed(int index, EVENT_REC* pRec);

private:

	// Embedded object for scheduler table managing
	SchedTable m_tableEvents;

	// Associated calendar
	Calendar* m_pCalendar;

	// Scheduler events handler
	fpProcessEvents m_pPE;

	// External handler for timer signals
	fpProcessTimer m_pPT;

	// Associated timer
	Timer* m_pTimer;
};

} // namespace Timework


#endif /* DSHEDULER_H_ */
