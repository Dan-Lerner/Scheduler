/*
 * DTimeFilter.h
 *
 *  Created on: 27 may. 2015 yr.
 *      Author: Dan
 */

#ifndef DTIMEFILTER_H_
#define DTIMEFILTER_H_

#include <string>

#include "Macro.h"
#include "Time.h"
#include "Calendar.h"

namespace Timework
{

// The base structure for discrete time period or value
struct TF_RANGE
{
	// Constants
	static const int NO_LIMIT	= -1;
	static const int UNDEFINED	= -1;

	TF_RANGE()
	{
		SetUndefined();
	}

	void SetUndefined()
	{
		start = UNDEFINED;
		end = UNDEFINED;
		step = UNDEFINED;
	}

	// True if value within the range
	bool Belong(int value) const;

	// Calculates the next value in range. If there is no one, returns false
	bool Next(int* value) const;

	// Shifts the value to nearest value in range
	// If the value already corresponds one of range's value then it remains unchanged
	bool Normalize(int* value) const;

	// Sets the range to unlimited
	void AnyTime();

	// Sets the range as not used
    void NotUsed();

    // Sets to single value
    void Value(int value);

    // Sets the period
    void Span(int min, int max, int step);

    // Gets range states
    bool IsStartUndefined() const
    { return start == UNDEFINED;	}
    bool IsStepUndefined() const
    { return step == UNDEFINED;		}
    bool IsEndNotUndefined() const
    { return end == UNDEFINED;		}

    // Overloads
    TF_RANGE operator=(const TF_RANGE& range);

	// Fields
	int start;
	int end;
	int step;
};

struct TF_HEADER
{
	TF_HEADER()
	{
		for (int ii = 0; ii < CALENDAR_TIME::PAR_N; ii++)
		{
			btIncl[ii] = 0x00;
		}
	}

	// Included ranges
	unsigned char btIncl[CALENDAR_TIME::PAR_N];
};

// This struct defines the main algorithm for events generation
struct TF_FILTER
{
	TF_HEADER* pHeader;
	TF_RANGE* pRanges;
};

// This class calculates the next date-time signal in event sequence
class TimeFilter
{

// Constants
private:

	static const int UNDEFINED = -1;

public:

	TimeFilter();
	virtual ~TimeFilter();

public:

	// Associates calendar class for calculations
	bool SetCalendar(Calendar* pCalendar);

	// Generates the next date and time corresponding to algorithm specified in pFilter
	// pTime - current time or last generated time
	// if normalize is set to true, then pTime will be just normalized to closest value in sequence
	bool FilterNext(CALENDAR_TIME* pTime, const TF_FILTER* pFilter, bool normalize);

// For internal use
private:

	// Convert fields of time struct to filter format struct
	void TimeToFilter(const CALENDAR_TIME* pSrc, CALENDAR_TIME* pDst, const TF_HEADER* pHeader);

	// Convert fields of filter format struct to time struct
	void FilterToTime(CALENDAR_TIME* pDst, CALENDAR_TIME* pSrc, const TF_HEADER* pHeader);

	// Normalizes value
	bool Normalize(const TF_HEADER* pHeader, const TF_RANGE* pRanges, bool* pChanged);

	// Moves to next
	bool Next(const TF_HEADER* pHeader, const TF_RANGE* pRanges);

	// This set of methods recursively calculates the next date-time
	bool NextRecursive(const TF_HEADER* pHeader, const TF_RANGE* pRanges,
			int depth, int inclOffset, bool normalize = false, bool setMinValue = false);
	int NextValue(int currentValue, const TF_RANGE* pIncl, unsigned char inclN,
			int minVal, int maxVal, bool normalize, bool *pOverflow);
	int NextIncl(int currentValue, TF_RANGE* pRange, unsigned char rangesN,
			bool *pOverflow, bool normalize);

// Service stuff
private:

	void CastRanges(TF_RANGE* pRange, unsigned char rangesN, int min, int max);
	int NextDefined(const TF_HEADER* pHeader, int depth);
	int FirstOffset(const TF_HEADER* pHeader, int depth);
	int NextOffset(const TF_HEADER* pHeader, int offset, int depth);
	int PrevOffset(const TF_HEADER* pHeader, int offset, int depth);
	void DisableRange(TF_RANGE* pRange);

private:

	// Calendar used for calculations
	Calendar* m_pCalendar;

	// Used for recursive procedure
	CALENDAR_TIME m_find;
};

} // namespace Timework

#endif /* DTIMEFILTER_H_ */
