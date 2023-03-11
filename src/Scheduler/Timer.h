/*
 * DTimer.h
 *
 *  Created on: 27 may. 2015 yr.
 *      Author: dan
 */

#ifndef DTIMER_H_
#define DTIMER_H_

#include "TimerTable.h"
#include "TimerStr.h"

namespace Timework
{

// !!! For multithreading it is necessary to wrap methods with mutexes

// This class provides basic Timers functionality
class Timer
{
public:

	Timer();
	virtual ~Timer();

// Memory for Timers table records must be allocated outside
// For the timers table use RAM
public:

	// Calculates the amount of memory (in bytes) required for a timer table with demandRecsN timers
	int GetTableBinSize(int demandTimersN);

	// Sets the external allocated memory for timer table
	bool SetTableMem(void* pMem, int size);

	// Current amount of timers
	int GetTimersCount();

public:

	// This method checks the state of all the timers records
	// It is necessary to call this method periodically to check if any timer has fired
	// with frequency that corresponds to required accuracy of events generation
	// For instance, method must be called 1000 times per second if 1 millisecond accuracy is required
	bool CheckTimers(TIMEPREC timeCurrent);

	// Sets a new timer
	// Return value is ID of new timer
	int SetTimer(const TIMER_SET* pSet, TIMEPREC timeCurrent);

	// Recharges specified timer
	bool RechargeTimer(int ID, const TIMER_SET* pSet, TIMEPREC timeCurrent);

	// This method must be called after RTC correction
	bool CorrectTime(TIMEPREC timeOld, TIMEPREC timeNew);

	// Deletes timer with specified ID
	bool DeleteTimer(int ID);

	// Calculates the time of next signal for specified timer
	bool GetNextTime(int ID, TIMEPREC* pTimeNext);

// For internal use
protected:

	// Binds one of signals in sequence to specified time
	TIMEPREC BindTime(TIMEPREC timeBind, TIMEPREC timeInterval, TIMEPREC timeToBind);

	// Timer initialization
	bool InitTimer(TIMER_REC* pRec, TIMEPREC timeCurrent);

	// Handles of timer
	bool ProcessTimer(TIMER_REC* pRec, TIMEPREC timeCurrent);

	// Handles of timer signal
	bool ProcessTick(TIMER_REC* pRec, TIMEPREC timeCurrent, bool lost = false);

	// Checks if all timer signals have expired
	bool CheckElapsed(TIMER_REC* pRec);

private:

	// Embedded object for table managing
	TimerTable m_tableTimer;
};

} // namespace Timework

#endif /* DTIMER_H_ */
