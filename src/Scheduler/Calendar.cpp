/*
 * Calendar.cpp
 *
 *  Created on: 12 may 2015 yr.
 *      Author: dan
 */


#include <malloc.h>
#include <string.h>

#include "Calendar.h"

namespace Timework
{

// CALENDAR_TIME
////////////////////////////////////////////////////////////////////////////////////////////////////

CALENDAR_TIME CALENDAR_TIME::operator=(const CALENDAR_TIME& time)
{
	if (this == &time)
	{
		return *this;
	}

 	::memcpy(timePar, &time.timePar, sizeof(CALENDAR_TIME));

 	return *this;
}

bool CALENDAR_TIME::operator==(const CALENDAR_TIME& time)
{
	return ::memcmp(timePar, time.timePar, sizeof(CALENDAR_TIME)) == 0;
}

bool CALENDAR_TIME::operator!=(const CALENDAR_TIME& time)
{
	return ::memcmp(timePar, time.timePar, sizeof(CALENDAR_TIME)) != 0;
}

bool CALENDAR_TIME::operator<(const CALENDAR_TIME& time)
{
	for (int ii = CALENDAR_TIME::YEAR; ii >= CALENDAR_TIME::SEC; ii--)
	{
		if (ii != CALENDAR_TIME::WEEK)
		{
			if (timePar[ii] < time.timePar[ii])
			{
				return true;
			}

			if (timePar[ii] > time.timePar[ii])
			{
				return false;
			}
		}
	}

	return false;
}

bool CALENDAR_TIME::operator>(const CALENDAR_TIME& time)
{
	for (int ii = CALENDAR_TIME::YEAR; ii >= CALENDAR_TIME::SEC; ii--)
	{
		if (ii != CALENDAR_TIME::WEEK)
		{
			if (timePar[ii] > time.timePar[ii])
			{
				return true;
			}

			if (timePar[ii] < time.timePar[ii])
			{
				return false;
			}
		}
	}

	return false;
}

bool CALENDAR_TIME::operator<=(const CALENDAR_TIME& time)
{
	for (int ii = CALENDAR_TIME::YEAR; ii >= CALENDAR_TIME::SEC; ii--)
	{
		if (ii != CALENDAR_TIME::WEEK)
		{
			if (this->timePar[ii] < time.timePar[ii])
			{
				return true;
			}

			if (timePar[ii] > time.timePar[ii])
			{
				return false;
			}
		}
	}

	return true;
}

bool CALENDAR_TIME::operator>=(const CALENDAR_TIME& time)
{
	for (int ii = CALENDAR_TIME::YEAR; ii >= CALENDAR_TIME::SEC; ii--)
	{
		if (ii != CALENDAR_TIME::WEEK)
		{
			if (timePar[ii] > time.timePar[ii])
			{
				return true;
			}
			if (timePar[ii] < time.timePar[ii])
			{
				return false;
			}
		}
	}

	return true;
}

// Calendar
////////////////////////////////////////////////////////////////////////////////////////////////////

char Calendar::m_DayInMonth[MONTH_IN_YEAR + 1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
char Calendar::m_MonthIndex[MONTH_IN_YEAR + 1] = { 0, 6, 2, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
short Calendar::m_DaysInMonths[MONTH_IN_YEAR + 1] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

Calendar::Calendar()
{
	// TODO Auto-generated constructor stub

	// Default Epoch counted from 01.01.0000
	m_startEpoch = 0;
}

Calendar::Calendar(int yearEpoch, int monthEpoch, int dayEpoch)
{
	// TODO Auto-generated constructor stub

	SetEpoch(yearEpoch, monthEpoch, dayEpoch);
}

Calendar::~Calendar()
{
	// TODO Auto-generated destructor stub
}

void Calendar::SetEpoch(int yearEpoch, int monthEpoch, int dayEpoch)
{
	CALENDAR_TIME start = { { 0, 0, 0, dayEpoch, -1, monthEpoch, yearEpoch } };
	m_startEpoch = DateTimeToSec(&start);
}

bool Calendar::GetValueRange(int& minVal, int& maxVal, int valueID, int inID, int year,
		char month, char week)
{
	CALENDAR_TIME time;
	time.timePar[CALENDAR_TIME::YEAR] = year;
	time.timePar[CALENDAR_TIME::MONTH] = month;
	time.timePar[CALENDAR_TIME::WEEK] = week;

	return GetValueRange(minVal, maxVal, valueID, inID, &time);
}

bool Calendar::GetValueRange(int& minVal, int& maxVal, int valueID, int inID, const CALENDAR_TIME* parCal)
{
	int min;
	int max;

	minVal = 0;

	switch (valueID)
	{
		case CALENDAR_TIME::SEC:

			switch (inID)
			{
				case CALENDAR_TIME::MIN:

					maxVal = minVal + SEC_IN_MIN;
					return true;

				case CALENDAR_TIME::HOUR:

					maxVal = minVal + SEC_IN_HOUR;
					return true;

				case CALENDAR_TIME::DAY:

					maxVal = minVal + SEC_IN_DAY;
					return true;

				case CALENDAR_TIME::WEEK:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::WEEK, parCal);
					break;

				case CALENDAR_TIME::MONTH:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::MONTH, parCal);
					break;

				case CALENDAR_TIME::YEAR:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::YEAR, parCal);
					break;

				default:

					return false;
			}

			maxVal = minVal + (max - min) * SEC_IN_DAY;
			return true;

		case CALENDAR_TIME::MIN:

			switch (inID)
			{
				case CALENDAR_TIME::HOUR:

					maxVal = minVal + MIN_IN_HOUR;
					return true;

				case CALENDAR_TIME::DAY:

					maxVal = minVal + MIN_IN_DAY;
					return true;

				case CALENDAR_TIME::WEEK:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::WEEK, parCal);
					break;

				case CALENDAR_TIME::MONTH:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::MONTH, parCal);
					break;

				case CALENDAR_TIME::YEAR:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::YEAR, parCal);
					break;

				default:

					return false;
			}

			maxVal = minVal + (max - min) * MIN_IN_DAY;

			return true;

		case CALENDAR_TIME::HOUR:

			switch (inID)
			{
				case CALENDAR_TIME::DAY:

					maxVal = minVal + HOUR_IN_DAY;
					return true;

				case CALENDAR_TIME::WEEK:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::WEEK, parCal);
					break;

				case CALENDAR_TIME::MONTH:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::MONTH, parCal);
					break;

				case CALENDAR_TIME::YEAR:

					GetValueRange(min, max, CALENDAR_TIME::DAY, CALENDAR_TIME::YEAR, parCal);
					break;

				default:

					return false;
			}

			maxVal = minVal + (max - min) * HOUR_IN_DAY;
			return true;

		case CALENDAR_TIME::DAY:

			switch (inID)
			{
				case CALENDAR_TIME::WEEK:

					GetWeekRange(minVal, maxVal, parCal->timePar[CALENDAR_TIME::YEAR], parCal->timePar[CALENDAR_TIME::MONTH],
							parCal->timePar[CALENDAR_TIME::WEEK]);
					return true;

				case CALENDAR_TIME::MONTH:

					minVal = 1;
					maxVal = DaysInMonth(parCal->timePar[CALENDAR_TIME::YEAR], parCal->timePar[CALENDAR_TIME::MONTH]) + 1;
					return true;

				case CALENDAR_TIME::YEAR:

					minVal = 1;
					maxVal = DaysInYear(parCal->timePar[CALENDAR_TIME::YEAR]) + 1;
					return true;

				default:

					return false;
			}

			break;

		case CALENDAR_TIME::WEEK:

			minVal = 1;

			switch (inID)
			{

				case CALENDAR_TIME::MONTH:

					maxVal = WeeksInMonth(parCal->timePar[CALENDAR_TIME::YEAR], parCal->timePar[CALENDAR_TIME::MONTH]) + 1;
					return true;

				case CALENDAR_TIME::YEAR:

					maxVal = WeeksInYear(parCal->timePar[CALENDAR_TIME::YEAR]) + 1;
					return true;

				default:

					return false;
			}

			break;

		case CALENDAR_TIME::MONTH:

			minVal = 1;

			switch (inID)
			{
				case CALENDAR_TIME::YEAR:

					maxVal = minVal + MONTH_IN_YEAR;
					return true;

				default:

					return false;
			}

			break;

		case CALENDAR_TIME::YEAR:

			maxVal = LAST_YEAR;
			break;

		default:

			return false;
	}

	return false;
}

unsigned long long Calendar::DateTimeToSec(const CALENDAR_TIME* pTime)
{
	unsigned int year = pTime->timePar[CALENDAR_TIME::YEAR] - 1;

	unsigned int days = year * 365 + year / 4 - year / 100 + year / 400 +
			DayOfYear(pTime->timePar[CALENDAR_TIME::YEAR], pTime->timePar[CALENDAR_TIME::MONTH],
					pTime->timePar[CALENDAR_TIME::DAY]) - 1;

	return static_cast<unsigned long long>(days) * SEC_IN_DAY +
			pTime->timePar[CALENDAR_TIME::HOUR] * SEC_IN_HOUR +
			pTime->timePar[CALENDAR_TIME::MIN] * SEC_IN_MIN +
			pTime->timePar[CALENDAR_TIME::SEC];
}

bool Calendar::SecToDateTime(unsigned long long sec, CALENDAR_TIME* pTime)
{
	// Number of whole days

    pTime->timePar[CALENDAR_TIME::DAY] = sec / SEC_IN_DAY;

    // Time calculation by number of remaining seconds

    pTime->timePar[CALENDAR_TIME::SEC] = static_cast<int>(sec - pTime->timePar[CALENDAR_TIME::DAY] * SEC_IN_DAY);
    pTime->timePar[CALENDAR_TIME::HOUR] = pTime->timePar[CALENDAR_TIME::SEC] / SEC_IN_HOUR;
    pTime->timePar[CALENDAR_TIME::SEC] -= pTime->timePar[CALENDAR_TIME::HOUR] * SEC_IN_HOUR;
    pTime->timePar[CALENDAR_TIME::MIN] = pTime->timePar[CALENDAR_TIME::SEC] / SEC_IN_MIN;
    pTime->timePar[CALENDAR_TIME::SEC] -= pTime->timePar[CALENDAR_TIME::MIN] * SEC_IN_MIN;

    // Date calculation by number of days

    int nYears400 = pTime->timePar[CALENDAR_TIME::DAY] / DAY_IN_400YEAR;
    pTime->timePar[CALENDAR_TIME::DAY] -= nYears400 * DAY_IN_400YEAR;
    int nYears100 = pTime->timePar[CALENDAR_TIME::DAY] / DAY_IN_100YEAR;

    if (nYears100 == 4)
    {
    	nYears100 = 3;
    }

    pTime->timePar[CALENDAR_TIME::DAY] -= nYears100 * DAY_IN_100YEAR;
    int nYears4 = pTime->timePar[CALENDAR_TIME::DAY] / DAY_IN_4YEAR;
    pTime->timePar[CALENDAR_TIME::DAY] -= nYears4 * DAY_IN_4YEAR;
    int nYears1 = pTime->timePar[CALENDAR_TIME::DAY] / DAY_IN_YEAR;

    if (nYears1 == 4)
    {
    	nYears1 = 3;
    }

    pTime->timePar[CALENDAR_TIME::DAY] = pTime->timePar[CALENDAR_TIME::DAY] - nYears1 * DAY_IN_YEAR + 1;	// Day of year
    pTime->timePar[CALENDAR_TIME::YEAR] = nYears400 * 400 + nYears100 * 100 + nYears4 * 4 + nYears1 + 1;
    pTime->timePar[CALENDAR_TIME::DAY] = DayOfYearToDayAndMonth(pTime->timePar[CALENDAR_TIME::YEAR],
    		pTime->timePar[CALENDAR_TIME::MONTH], pTime->timePar[CALENDAR_TIME::DAY]);

    return true;
}

unsigned long long Calendar::DateTimeToSecEpoch(const CALENDAR_TIME* pTime)
{
	return DateTimeToSec(pTime) - m_startEpoch;
}

bool Calendar::SecToDateTimeEpoch(unsigned long long time, CALENDAR_TIME* pTime)
{
	return SecToDateTime(m_startEpoch + time, pTime);
}

bool Calendar::IsLeapYear(int year)
{
	return ((year % 4) == 0 && (year % 100) != 0) || ((year % 400) == 0);
}

int Calendar::GetWeekRange(int& minVal, int& maxVal, int year, int month, int week)
{
	if (year < FIRST_YEAR || month > MONTH_IN_YEAR)
	{
		return D_RET_ERROR;
	}

	int first = (month != Calendar::UNDEFINED) ?
			FirstWeekDayInMonth(year, month) :
			FirstWeekDayInYear(year);

	int days = (month != Calendar::UNDEFINED) ?
			DaysInMonth(year, month) :
			DaysInYear(year);

	minVal = (week != 1) ? 1 : first;
	first--;

	maxVal = 1 + ((days - ((week * DAY_IN_WEEK) - first) < 0) ?
			(days - (DAY_IN_WEEK - first)) % DAY_IN_WEEK :
			DAY_IN_WEEK);

	return true;
}

int Calendar::DaysInMonth(int year, int month)
{
	if (month <= MONTH_IN_YEAR)
	{
		return m_DayInMonth[month] + ((month == CALENDAR_TIME::FEB) && IsLeapYear(year) ? LEAP_ADD : 0);
	}

	return D_RET_ERROR;
}

int Calendar::DaysInYear(int year)
{
	if (year < FIRST_YEAR)
	{
		return D_RET_ERROR;
	}

	return DAY_IN_YEAR + (IsLeapYear(year) ? LEAP_ADD : 0);
}

int Calendar::WeeksInMonth(int year, int month)
{
	if (year < FIRST_YEAR || month > MONTH_IN_YEAR)
	{
		return D_RET_ERROR;
	}

	return WeeksInPeriod(DaysInMonth(year, month), FirstWeekDayInMonth(year, month));
}

int Calendar::WeeksInYear(int year)
{
	if (year < FIRST_YEAR)
	{
		return D_RET_ERROR;
	}

	return WeeksInPeriod(DaysInYear(year), FirstWeekDayInYear(year));
}

int Calendar::WeeksInPeriod(int days, int firstDay)
{
	int period = days - (DAY_IN_WEEK - firstDay + 1);

	return 1 + period / DAY_IN_WEEK + ((period % DAY_IN_WEEK) > 0 ? 1 : 0);
}

int Calendar::DayOfYear(int year, int month, int day)
{
	if (year < FIRST_YEAR || month > MONTH_IN_YEAR)
	{
		return D_RET_ERROR;
	}

	return ((month != CALENDAR_TIME::JAN) ?
			m_DaysInMonths[month - 1] : 0) + (((month > CALENDAR_TIME::FEB) && IsLeapYear(year)) ?
			LEAP_ADD : 0) + day;
}

int Calendar::DayOfYearToDayAndMonth(int year, int& month, int day)
{
	int add = IsLeapYear(year) ? LEAP_ADD : 0;
	int dayWithLeap;

	for (int ii = 1; ii <= MONTH_IN_YEAR; ii++)
	{
		dayWithLeap = m_DaysInMonths[ii];

		if (add && ii > CALENDAR_TIME::JAN)
		{
			dayWithLeap += add;
		}

		if (day <= dayWithLeap)
		{
			month = ii;
			dayWithLeap = day;

			if (ii > CALENDAR_TIME::JAN)
			{
				dayWithLeap -= m_DaysInMonths[ii - 1];

				if (add && ii > CALENDAR_TIME::FEB)
				{
					dayWithLeap -= add;
				}
			}

			return dayWithLeap;
		}
	}

	return D_RET_ERROR;
}

int Calendar::WeekDay(int year, int month, int day)
{
	if (year < FIRST_YEAR || month > MONTH_IN_YEAR)
	{
		return D_RET_ERROR;
	}

	int yearIndex = year % 100;
	int rest = yearIndex % 12;
	yearIndex = yearIndex / 12 + rest + rest / 4;
	int centuryIndex = (year < 2000) ? 1 : 0;
	int leapYearIndex = (month <= CALENDAR_TIME::FEB) && IsLeapYear(year) ? -1 : 0;

	return (yearIndex + m_MonthIndex[month] + (day - 1) +
			centuryIndex + leapYearIndex) % DAY_IN_WEEK + 1;
}

int Calendar::WeekOfMonth(int year, int month, int day)
{
	return WeeksInPeriod(day, FirstWeekDayInMonth(year, month));
}

int Calendar::WeekOfYear(int year, int month, int day)
{
	if (year < FIRST_YEAR || month > MONTH_IN_YEAR)
	{
		return D_RET_ERROR;
	}

	return WeeksInPeriod(DayOfYear(year, month, day), FirstWeekDayInYear(year));
}

int Calendar::FirstWeekDayInYear(int year)
{
	return WeekDay(year, 1, 1);
}

int Calendar::FirstWeekDayInMonth(int year, int month)
{
	return WeekDay(year, month, 1);
}

int Calendar::DayOfWeekToDayOfMonth(int year, int month, int week, int day)
{
	return (week - 1) * DAY_IN_WEEK - FirstWeekDayInMonth(year, month) + day + 1;
}

int Calendar::DayOfWeekToDayOfYear(int year, int week, int day)
{
	return (week - 1) * DAY_IN_WEEK - FirstWeekDayInYear(year) + day + 1;
}

} // namespace Timework
