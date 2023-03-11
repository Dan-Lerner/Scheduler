/*
 * DTimerStr.h
 *
 *  Created on: 27 apr. 2015 yr.
 *      Author: dan
 */

#ifndef DTIMERSTR_H_
#define DTIMERSTR_H_

#include "Time.h"

namespace Timework
{

// Data for signal processing
// It is used in timer signal handler
struct TIMER_TICK
{
	int ID;					// Unique timer ID
	TIMEPREC time;			// Signal time (i.e. time the signal should have fired)
	TIMEPREC timeCurrent;	// Current time
	int signalN;			// Number of signal
	int lostN;				// Lost signals amount
	bool lost;				// true, if lost signal is processing
};

// Signals handler
typedef bool (*fpProcessTimer)(TIMER_TICK* tick);

// Timer settings
struct TIMER_SET
{
	static const int INFINITE = -1;	// For signalsN - number of signals not limited

	// Flags
	static const unsigned short FLAG_USE_START_TIME	= 0x0001;	// Starts signals generation after timeStart
	static const unsigned short FLAG_ABS_START_TIME	= 0x0002;	// timeStart is absolute, otherwise it counted from the moment of applying
	static const unsigned short FLAG_BIND			= 0x0004;	// Bind signals to timeBind;
	static const unsigned short FLAG_IGNORE_LOST	= 0x0008;	// Ignore lost signals (lost signals accumulated in lostN)
	static const unsigned short FLAG_PROCESS_LOST	= 0x0010;	// Handle lost signals
	static const unsigned short FLAG_FIXED_PERIOD	= 0x0020;	// timePeriod is always counted after the last signal (don't work with FLAG_BIND)

	// Setters
	bool SetUseStartTime(bool flag = true)
	{ return flag ? flags |= FLAG_USE_START_TIME : flags &= ~FLAG_USE_START_TIME; }
	bool SetAbsStartTime(bool flag = true)
	{ return flag ? flags |= FLAG_ABS_START_TIME : flags &= ~FLAG_ABS_START_TIME; }
	bool SetBind(bool flag = true)
	{ return flag ? flags |= FLAG_BIND : flags &= ~FLAG_BIND; }
	bool SetIgnoreLost(bool flag = true)
	{ return flag ? flags |= FLAG_IGNORE_LOST : flags &= ~FLAG_IGNORE_LOST; }
	bool SetProcessLost(bool flag = true)
	{ return flag ? flags |= FLAG_PROCESS_LOST : flags &= ~FLAG_PROCESS_LOST; }
	bool SetFixedPeriod(bool flag = true)
	{ return flag ? flags |= FLAG_FIXED_PERIOD : flags &= ~FLAG_FIXED_PERIOD; }

	// Getters
	bool IsUseStartTime() const
	{ return flags & FLAG_USE_START_TIME; }
	bool IsAbsStartTime() const
	{ return flags & FLAG_ABS_START_TIME; }
	bool IsBind() const
	{ return flags & FLAG_BIND; }
	bool IsIgnoreLost() const
	{ return flags & FLAG_IGNORE_LOST; }
	bool IsProcessLost() const
	{ return flags & FLAG_PROCESS_LOST; }
	bool IsFixedPeriod() const
	{ return flags & FLAG_FIXED_PERIOD; }

	bool IsInfinite() const
	{ return signalsN == INFINITE; }

	// Overloads
	TIMER_SET operator=(const TIMER_SET& set)
	{
		if (this == &set)
		{
			return *this;
		}

		fpPT = set.fpPT;
		timeStart = set.timeStart;
		timePeriod = set.timePeriod;
		timeBind = set.timeBind;
		signalsN = set.signalsN;
		flags = set.flags;

	 	return *this;
	}

	fpProcessTimer fpPT;	// Pointer to signals handler
	TIMEPREC timeStart;		// Timer starts generating signals after timeStart
	TIMEPREC timePeriod;	// Period between signals. If period is expired then signal is counted as lost (even if nSignalsN = 1)
	TIMEPREC timeBind;		// Bind time. One of of signals must hit this time
	int signalsN;			// Max amount of signals to generate, is set to INFINITE - not limited
	unsigned int flags;		// Flags
};

//  !FLAG_FIXED_PERIOD
//
//           Check       Check
//             |           |
// Signal   -        -        -
//           -------- -------- -------
//          | Period | Period |
//
//  FLAG_FIXED_PERIOD
//
//             Check    Check
//               |        |
// Signal   -             -        -
//           ------------- -------- -------
//               | Period | Period |

// If check period exceeded timePeriod
//
//           Check                     Check
//             |                         |
// Signal   -        -        -       -
//           -------- -------- ------- -------
//                  Lost     Lost
//
// If FLAG_PROCESS_LOST is set, signals dispatcher is invoked for each lost signal
// Else if FLAG_IGNOR_LOST is set, lost signals will be ignored
// Otherwise, the signals generation will continue from the signalsN where they left off

} // namespace Timework

#endif /* DTIMERSTR_H_ */
