/*
 * DShedStr.h
 *
 *  Created on: 15 ��� 2015 �.
 *      Author: Dan
 */

#ifndef DSHEDSTR_H_
#define DSHEDSTR_H_

#include "Time.h"
#include "TimeFilter.h"

namespace Timework
{

// Schedule event data
// It is used in event handler
struct SCHEDULER_EVENT
{
	int ID;				// Unique event ID
	TIME time;			// Event time (i.e. time the event should have fired)
	TIME timeCurrent;	// Current time
	int eventN;			// Number of current event
	int lostN;			// Lost events amount
	bool lost;			// true, if lost event is processing
};

// Filter
// This filter sets the algorithm for events generation
struct SCHEDULER_EVENT_FILTER
{
	TF_RANGE year;
	TF_RANGE month;
	TF_RANGE week;
	TF_RANGE day;	// if week is set than day treated as a day of week, else it treated as a day of month (or year)
	TF_RANGE hour;
	TF_RANGE min;
	TF_RANGE sec;

	SCHEDULER_EVENT_FILTER operator=(const SCHEDULER_EVENT_FILTER& filter)
	{
		if (this == &filter)
		{
			return *this;
		}

		year = filter.year;
		month = filter.month;
		week = filter.week;
		day = filter.day;
		hour = filter.hour;
		min = filter.min;
		sec = filter.sec;

	 	return *this;
	}
};

// Event dispatcher
typedef bool (*fpProcessEvents)(SCHEDULER_EVENT* event);

// Schedule settings
struct SCHEDULER_EVENT_SET
{
	static const int ID_AUTO	= -1;	// Value for event ID. If ID_AUTO is set then ID assigned automatically, else it must be set manually
	static const int INFINITE	= -1;	// For eventsN - number of events is not limited

	// Flags
	static const unsigned short FLAG_USE_START_TIME	= 0x0001;	// Starts events generation after timeStart
	static const unsigned short FLAG_IGNORE_LOST	= 0x0008;	// Ignore lost signals (but it still accumulated in lostN)
	static const unsigned short FLAG_PROCESS_LOST	= 0x0010;	// Dispatch lost events

	// Setters
	bool SetUseStartTime(bool flag = true)
	{ return flag ? flags |= FLAG_USE_START_TIME : flags &= ~FLAG_USE_START_TIME; }
	bool SetIgnoreLost(bool flag = true)
	{ return flag ? flags |= FLAG_IGNORE_LOST : flags &= ~FLAG_IGNORE_LOST; }
	bool SetProcessLost(bool flag = true)
	{ return flag ? flags |= FLAG_PROCESS_LOST : flags &= ~FLAG_PROCESS_LOST; }

	// Getters
	bool IsUseStartTime() const
	{ return flags & FLAG_USE_START_TIME; }
	bool IsIgnoreLost() const
	{ return flags & FLAG_IGNORE_LOST; }
	bool IsProcessLost() const
	{ return flags & FLAG_PROCESS_LOST; }

	bool IsInfifnite() const
	{ return eventsN == INFINITE; }

	// Overloads
	SCHEDULER_EVENT_SET operator=(const SCHEDULER_EVENT_SET& set)
	{
		if (this == &set)
		{
			return *this;
		}

		ID = set.ID;
		timeStart = set.timeStart;
		filter = set.filter;
		eventsN = set.eventsN;
		flags = set.flags;

	 	return *this;
	}

	int ID;							// Unique event ID.
	TIME timeStart;					// Scheduler starts to generate events after timeStart
	SCHEDULER_EVENT_FILTER filter;	// Filter
	int eventsN;					// Max amount of events to generate, if set to INFINITE - not limited
	unsigned int flags;				// flags
};

} // namespace Timework

#endif /* DSHEDSTR_H_ */
