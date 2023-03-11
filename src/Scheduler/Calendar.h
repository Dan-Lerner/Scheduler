/*
 * DCalendar.h
 *
 *  Created on: 12 may 2015 yr.
 *      Author: dan
 */

#ifndef DCALENDAR_H_
#define DCALENDAR_H_

#include "Macro.h"
#include "Time.h"

namespace Timework
{

struct CALENDAR_TIME
{
	// Constants
	enum {	SEC = 0, MIN, HOUR, DAY, WEEK, MONTH, YEAR, PAR_N };
	enum {	JAN = 1, FEB, MAR, APR, MAY, JUNE, JULY, AUG, SEPT, OCT, NOV, DEC };
	enum {	MON = 1, TUE, WED, THU, FRI, SAT, SUN };

	// Date and time
	int timePar[PAR_N];

	CALENDAR_TIME operator=(const CALENDAR_TIME& time);
	bool operator==(const CALENDAR_TIME& time);
	bool operator!=(const CALENDAR_TIME& time);
	bool operator<(const CALENDAR_TIME& time);
	bool operator>(const CALENDAR_TIME& time);
	bool operator<=(const CALENDAR_TIME& time);
	bool operator>=(const CALENDAR_TIME& time);
};

class Calendar
{

// Internal constants
protected:

	static const int MSEC_IN_SEC	= 1000;

	static const int SEC_IN_MIN		= 60;
	static const int MIN_IN_HOUR 	= 60;
	static const int HOUR_IN_DAY	= 24;
	static const int MONTH_IN_YEAR	= 12;
	static const int DAY_IN_WEEK	= 7;

	static const int SEC_IN_HOUR	= SEC_IN_MIN * MIN_IN_HOUR;
	static const int SEC_IN_DAY		= SEC_IN_HOUR * HOUR_IN_DAY;
	static const int MIN_IN_DAY		= MIN_IN_HOUR * HOUR_IN_DAY;

	static const int DAY_IN_YEAR	= 365;
	static const int DAY_IN_4YEAR	= DAY_IN_YEAR * 4 + 1;
	static const int DAY_IN_100YEAR	= DAY_IN_4YEAR * 25 - 1;
	static const int DAY_IN_400YEAR	= DAY_IN_100YEAR * 4 + 1;

	static const int LEAP_ADD		= 1;

	static const int FIRST_YEAR		= 1900;
	static const int LAST_YEAR		= 1000000;

	static char m_DayInMonth[MONTH_IN_YEAR + 1];
	static char m_MonthIndex[MONTH_IN_YEAR + 1];
	static short m_DaysInMonths[MONTH_IN_YEAR + 1];

	static const int UNDEFINED	= -1;

public:

	Calendar();
	Calendar(int yearEpoch, int monthEpoch, int dayEpoch);
	virtual ~Calendar();

public:

	// Epoch setting
	void SetEpoch(int yearEpoch, int monthEpoch, int dayEpoch);

	// Getting a range of any date&time parameter
	bool GetValueRange(int& minVal, int& maxVal, int valueID, int inID, int year, char month, char week);
	bool GetValueRange(int& minVal, int& maxVal, int valueID, int inID, const CALENDAR_TIME* parCal);

public:

	// Translates CALENDAR_TIME to seconds from 01.01.0000
	unsigned long long DateTimeToSec(const CALENDAR_TIME* pTime);

	// Translates seconds estimated from 01.01.0000 to CALENDAR_TIME
	bool SecToDateTime(unsigned long long time, CALENDAR_TIME* pTime);

	// Converters operating relative Epoch start
	unsigned long long DateTimeToSecEpoch(const CALENDAR_TIME* pTime);
	bool SecToDateTimeEpoch(unsigned long long time, CALENDAR_TIME* pTime);

// Other methods for calendar related calculations
public:

	bool IsLeapYear(int year);

	int GetWeekRange(int& minVal, int& maxVal, int year, int month, int week);
	int DaysInMonth(int year, int month);
	int DaysInYear(int year);

	int WeeksInMonth(int year, int month);
	int WeeksInYear(int year);
	int WeeksInPeriod(int days, int firstDay);

	int DayOfYear(int year, int month, int day);
	int DayOfYearToDayAndMonth(int year, int& month, int day);

	int WeekDay(int year, int month, int day);
	int WeekOfMonth(int year, int month, int day);
	int WeekOfYear(int year, int month, int day);

	int FirstWeekDayInYear(int year);
	int FirstWeekDayInMonth(int year, int month);

	int DayOfWeekToDayOfMonth(int year, int month, int week, int day);
	int DayOfWeekToDayOfYear(int year, int week, int day);

private:

	// Start of Epoch counted from 00.00.0000 in seconds
	unsigned long long m_startEpoch;
};

} // namespace Timework

#endif /* DCALENDAR_H_ */
