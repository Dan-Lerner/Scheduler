/*
 * TimeFilter.cpp
 *
 *  Created on: 27 may. 2015 yr.
 *      Author: Dan
 */

#include "TimeFilter.h"

namespace Timework
{

// TF_RANGE
////////////////////////////////////////////////////////////////////////////////////////////////////

bool TF_RANGE::Belong(int value) const
{
	return (value >= start) && (value < end) && ((value - start) % step == 0);
}

bool TF_RANGE::Next(int* value) const
{
	*value = (*value < start) ? start : start + ((*value - start) / step) * step + step;

	if (*value < end)
	{
		// No overflow
		return false;
	}

	*value = start;

	// Overflow
	return true;
}

bool TF_RANGE::Normalize(int* value) const
{
	return Belong(*value) ? false : Next(value);
}

void TF_RANGE::AnyTime()
{
	start = UNDEFINED;
	end = UNDEFINED;
	step = 1;
}

void TF_RANGE::NotUsed()
{
	start = UNDEFINED;
	end = UNDEFINED;
	step = UNDEFINED;
}

void TF_RANGE::Value(int value)
{
	start = value;
	end = value + 1;
	step = 1;
}

void TF_RANGE::Span(int min, int max, int step)
{
	start = min != NO_LIMIT ? min : NO_LIMIT;
	end = max != NO_LIMIT ? max + 1 : NO_LIMIT;
	TF_RANGE::step = step;
}

TF_RANGE TF_RANGE::operator=(const TF_RANGE& range)
{
	if (this == &range)
	{
		return *this;
	}

	start = range.start;
	end = range.end;
	step = range.step;

 	return *this;
}

// TimeFilter
////////////////////////////////////////////////////////////////////////////////////////////////////

TimeFilter::TimeFilter()
{
	// TODO Auto-generated constructor stub

	m_pCalendar = nullptr;
}

TimeFilter::~TimeFilter()
{
	// TODO Auto-generated destructor stub
}

bool TimeFilter::SetCalendar(Calendar* pCalendar)
{
	m_pCalendar = pCalendar;

	return true;
}

bool TimeFilter::FilterNext(CALENDAR_TIME* pTime, const TF_FILTER* pFilter, bool normalize)
{
	CALENDAR_TIME find;

	// Convert *pTime to filter format
	TimeToFilter(pTime, &find, pFilter->pHeader);

	bool updated = false;
	bool changed = false;

	m_find = find;

	updated = normalize ?
			Normalize(pFilter->pHeader, pFilter->pRanges, &changed) :
			Next(pFilter->pHeader, pFilter->pRanges);

	if (updated)
	{
		CALENDAR_TIME normalized;

		// Converts result back to time format
		FilterToTime(&normalized, &m_find, pFilter->pHeader);

		// Because significant digits may be lost during normalization
		while (normalize && updated && (normalized < *pTime))
		{
			updated = Next(pFilter->pHeader, pFilter->pRanges);
			FilterToTime(&normalized, &m_find, pFilter->pHeader);
		}

		*pTime = normalized;
	}

	return updated;
}

void TimeFilter::TimeToFilter(const CALENDAR_TIME* pSrc, CALENDAR_TIME* pDst, const TF_HEADER* pHeader)
{
	int min;
	int max;
	int value;
	bool defined[CALENDAR_TIME::PAR_N];
	bool wasDefined = false;

	for (int ii = CALENDAR_TIME::YEAR; ii >= CALENDAR_TIME::SEC; ii--)
	{
		defined[ii] = pHeader->btIncl[ii] != 0;

		if (!wasDefined && defined[ii])
		{
			wasDefined = true;
		}

		defined[ii] = defined[ii] || !wasDefined;

		switch(ii)
		{
			case CALENDAR_TIME::YEAR:

				pDst->timePar[ii] = pSrc->timePar[ii];
				continue;

			case CALENDAR_TIME::MONTH:

				value = pSrc->timePar[ii];
				break;

			case CALENDAR_TIME::WEEK:

				defined[ii] = pHeader->btIncl[ii] != 0;

				if (defined[CALENDAR_TIME::MONTH])
				{
					value = m_pCalendar->WeekOfMonth(pSrc->timePar[CALENDAR_TIME::YEAR],
							pSrc->timePar[CALENDAR_TIME::MONTH],
							pSrc->timePar[CALENDAR_TIME::DAY]);
					break;
				}

				value = m_pCalendar->WeekOfYear(pSrc->timePar[CALENDAR_TIME::YEAR],
						pSrc->timePar[CALENDAR_TIME::MONTH],
						pSrc->timePar[CALENDAR_TIME::DAY]);

				break;

			case CALENDAR_TIME::DAY:

				if (defined[CALENDAR_TIME::WEEK])
				{
					value = m_pCalendar->WeekDay(pSrc->timePar[CALENDAR_TIME::YEAR],
							pSrc->timePar[CALENDAR_TIME::MONTH],
							pSrc->timePar[CALENDAR_TIME::DAY]);
					break;
				}

				if (defined[CALENDAR_TIME::MONTH])
				{
					value = pSrc->timePar[CALENDAR_TIME::DAY];
					break;
				}

				value = m_pCalendar->DayOfYear(pSrc->timePar[CALENDAR_TIME::YEAR],
						pSrc->timePar[CALENDAR_TIME::MONTH],
						pSrc->timePar[CALENDAR_TIME::DAY]);

				break;

			default:

				m_pCalendar->GetValueRange(min, max, ii, ii + 1, pDst);

				if (ii == CALENDAR_TIME::HOUR)
				{
					value = ((value - 1) * (max - min)) + pSrc->timePar[ii]; // because days starts from 1
					break;
				}

				value = (value * (max - min)) + pSrc->timePar[ii];

				break;
		}

		if (defined[ii])
		{
			pDst->timePar[ii] = value;
			value = (ii == CALENDAR_TIME::DAY) ? 1 : 0;
			continue;
		}

		pDst->timePar[ii] = UNDEFINED;

		if (ii == CALENDAR_TIME::DAY && defined[CALENDAR_TIME::WEEK])
		{
			// week started not on Monday

			m_pCalendar->GetWeekRange(min,
					max,
					pSrc->timePar[CALENDAR_TIME::YEAR],
					pDst->timePar[CALENDAR_TIME::MONTH],
					pDst->timePar[CALENDAR_TIME::WEEK]);
			value -= min;
		}
	}
}

void TimeFilter::FilterToTime(CALENDAR_TIME* pDst, CALENDAR_TIME* pSrc, const TF_HEADER* pHeader)
{
	int min;
	int max;
	int value = 0;
	bool defined[CALENDAR_TIME::PAR_N];

	for (int ii = CALENDAR_TIME::SEC; ii < CALENDAR_TIME::PAR_N; ii++)
	{
		defined[ii] = pSrc->timePar[ii] != UNDEFINED;
	}

	for (int ii = CALENDAR_TIME::SEC; ii < CALENDAR_TIME::PAR_N; ii++)
	{
		switch(ii)
		{
			case CALENDAR_TIME::YEAR:
			case CALENDAR_TIME::MONTH:
			case CALENDAR_TIME::WEEK:

				pDst->timePar[ii] = pSrc->timePar[ii];
				continue;

			case CALENDAR_TIME::DAY:

				value += (defined[ii] ? pSrc->timePar[ii] : 1);

				if (defined[CALENDAR_TIME::WEEK])
				{
					if (!defined[ii])
					{
						// week started not on Monday

						m_pCalendar->GetWeekRange(min,
								max,
								pSrc->timePar[CALENDAR_TIME::YEAR],
								pSrc->timePar[CALENDAR_TIME::MONTH],
								pSrc->timePar[CALENDAR_TIME::WEEK]);

						value += min - 1;
					}

					value = defined[CALENDAR_TIME::MONTH] ?
							m_pCalendar->DayOfWeekToDayOfMonth(pSrc->timePar[CALENDAR_TIME::YEAR],
									pSrc->timePar[CALENDAR_TIME::MONTH],
									pSrc->timePar[CALENDAR_TIME::WEEK],
									value) :
							m_pCalendar->DayOfWeekToDayOfYear(pSrc->timePar[CALENDAR_TIME::YEAR],
									pSrc->timePar[CALENDAR_TIME::WEEK],
									value);
				}

				if (defined[CALENDAR_TIME::MONTH])
				{
					pDst->timePar[ii] = value;
				}
				else
				{
					pDst->timePar[ii] = m_pCalendar->DayOfYearToDayAndMonth(pSrc->timePar[CALENDAR_TIME::YEAR],
							pSrc->timePar[CALENDAR_TIME::MONTH],
							value);
				}

				defined[CALENDAR_TIME::MONTH] = true;

				break;

			default:

				m_pCalendar->GetValueRange(min, max, ii, ii + 1, &m_find);
				value += (defined[ii] ? pSrc->timePar[ii] : 0);
				pDst->timePar[ii] = value % (max - min);
				value = value / (max - min);
				break;
		}
	}
}

bool TimeFilter::Next(const TF_HEADER* pHeader, const TF_RANGE* pRanges)
{
	bool changed = false;

	// First we need to normalize start value
	if (!Normalize(pHeader, pRanges, &changed))
	{
		return false;
	}

	if (changed)
	{
		// If time changed while normalization then there is no need to do something else
		return true;
	}

	int depth = NextDefined(pHeader, -1);

	if (depth == D_RET_ERROR)
	{
		return false;
	}

	int inclOffset = FirstOffset(pHeader, depth);

	// Recursive search for next value according to specified algorithm
	return NextRecursive(pHeader, pRanges, depth, inclOffset);
}

bool TimeFilter::Normalize(const TF_HEADER* pHeader, const TF_RANGE* pRanges, bool* changed)
{
	// Increases value to next value in sequence
	// If the value is already equal to one in the sequence, do nothing

	int inclOffset = FirstOffset(pHeader, CALENDAR_TIME::PAR_N);
	CALENDAR_TIME findMem = m_find;
	*changed = false;
	bool defined = false;

	for (int ii = CALENDAR_TIME::YEAR; ii >= CALENDAR_TIME::SEC; ii--)
	{
		if (pHeader->btIncl[ii] == 0)
		{
			if (defined || ii == CALENDAR_TIME::WEEK)
			{
				// week should't be used in this case
				continue;
			}
		}
		else
		{
			defined = true;
		}

		inclOffset = PrevOffset(pHeader, inclOffset, ii);

		if (!NextRecursive(pHeader, pRanges, ii, inclOffset, true, *changed))
		{
			return false;
		}

		if (!*changed && findMem != m_find)
		{
			// if current range has changed then all remaining ranges must be reset
			*changed = true;
		}
	}

	return true;
}

bool TimeFilter::NextRecursive(const TF_HEADER* pHeader, const TF_RANGE* pRanges, int depth, int inclOffset,
		bool normalize, bool setMinValue)
{
	// This recursive method steps to next date-time in sequence

	int minValue;
	int maxValue;
	bool overflow;
	int nextRange = NextDefined(pHeader, depth);

	if (nextRange == D_RET_ERROR)
	{
		// No ranges defined up to year

		nextRange = depth + 1;

		if (nextRange == CALENDAR_TIME::WEEK)
		{
			// Week should't be used in this case
			nextRange++;
		}
	}

	const TF_RANGE* pInclRange = pRanges + inclOffset;

	m_pCalendar->GetValueRange(minValue, maxValue, depth, nextRange, &m_find);

	m_find.timePar[depth] = NextValue(setMinValue ? minValue : m_find.timePar[depth],
			pInclRange, pHeader->btIncl[depth], minValue, maxValue, normalize, &overflow);

	int nextInclOffset = NextOffset(pHeader, inclOffset, depth);

	// "while" need here in case of possible overflow (or "underfloor")
	// Value may initially be out of range
	// For instance, while weekday&&week&&month filter is used and first or last week of month is incomplete
	// In this case next iteration will fix it
	while (depth < CALENDAR_TIME::YEAR && overflow)
	{
		if (!NextRecursive(pHeader, pRanges, nextRange, nextInclOffset))
		{
			return false;
		}

		m_pCalendar->GetValueRange(minValue, maxValue, depth, nextRange, &m_find);

		m_find.timePar[depth] = NextValue(minValue, pInclRange, pHeader->btIncl[depth],
				minValue, maxValue, true, &overflow);
	}

	return !overflow;
}

int TimeFilter::NextValue(int currentValue, const TF_RANGE* pIncl, unsigned char inclN,
		int minValue, int maxValue, bool normalize, bool *pOverflow)
{
	unsigned char includeN = (inclN != 0) ? inclN : 1;
	TF_RANGE* pInclude = static_cast<TF_RANGE*>(::alloca(sizeof(TF_RANGE) * includeN));

	if (inclN == 0)
	{
		pInclude->SetUndefined();
	}
	else
	{
		for (unsigned char ii = 0; ii < includeN; ii++)
		{
			pInclude[ii] = pIncl[ii];
		}
	}

	CastRanges(pInclude, includeN, minValue, maxValue);
	int value = NextIncl(currentValue, pInclude, includeN, pOverflow, normalize);

	return *pOverflow ? currentValue : value;
}

int TimeFilter::NextIncl(int currentValue, TF_RANGE* pRange, unsigned char rangesN, bool *pOverflow, bool normalize)
{
	int value;
	int minValue = currentValue;
	bool foundRange = false;

	for (int ii = 0; ii < rangesN; ii++)
	{
		if (pRange[ii].step == UNDEFINED)
		{
			*pOverflow = true;
			continue;
		}

		value = currentValue;
		*pOverflow = normalize ? pRange[ii].Normalize(&value) : pRange[ii].Next(&value);

		if (!*pOverflow)
		{
			if (!foundRange)
			{
				minValue = value;
				foundRange = true;
			}
			else
			{
				minValue = (value < minValue) ? value : minValue;
			}
		}
	}

	*pOverflow = !foundRange;

	return minValue;
}

void TimeFilter::CastRanges(TF_RANGE* pRange, unsigned char rangesN, int min, int max)
{
	int minVal;

	for (int ii = 0; ii < rangesN; ii++)
	{
		if ((pRange[ii].end == TF_RANGE::NO_LIMIT) || (pRange[ii].end > max))
		{
			// no need to normalize
			pRange[ii].end = max;
		}

		if (pRange[ii].step < 1)
		{
			pRange[ii].step = 1;
		}

		if (pRange[ii].start == TF_RANGE::NO_LIMIT)
		{
			pRange[ii].start = min;
		}
		else if (min > pRange[ii].start)
		{
			minVal = min;

			if (!pRange[ii].Normalize(&minVal)) // returns false if there is no overflow
			{
				pRange[ii].start = minVal;
			}
			else
			{
				DisableRange(pRange + ii);
				continue;
			}
		}

		if (pRange[ii].end <= pRange[ii].start)
		{
			DisableRange(pRange + ii);
		}
	}
}

int TimeFilter::NextDefined(const TF_HEADER* pHeader, int depth)
{
	for (int ii = depth + 1; ii < CALENDAR_TIME::PAR_N; ii++)
	{
		if (pHeader->btIncl[ii] != 0)
		{
			return ii;
		}
	}

	return D_RET_ERROR;
}

int TimeFilter::FirstOffset(const TF_HEADER* pHeader, int depth)
{
	int inclOffset = 0;

	for (int ii = CALENDAR_TIME::SEC; ii < depth; ii++)
	{
		inclOffset += pHeader->btIncl[ii];
	}

	return inclOffset;
}

int TimeFilter::NextOffset(const TF_HEADER* pHeader, int offset, int depth)
{
	return offset + pHeader->btIncl[depth];
}

int TimeFilter::PrevOffset(const TF_HEADER* pHeader, int offset, int depth)
{
	return offset - pHeader->btIncl[depth];
}

void TimeFilter::DisableRange(TF_RANGE* pRange)
{
	pRange->SetUndefined();
}

} // namespace Timework
