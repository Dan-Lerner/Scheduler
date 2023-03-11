/*
 * DTimerTable.h
 *
 *  Created on: 15 may. 2015 yr.
 *      Author: dan
 */

#ifndef DTIMERTABLE_H_
#define DTIMERTABLE_H_

#include "Macro.h"
#include "TimerStr.h"

namespace Timework
{

// Record for timer table
struct TIMER_REC
{
	static const int DELETED = -1;	// ID for deleted record

	// Setters
	void SetDeleted()
	{ ID = DELETED; }

	// Getters
	bool IsDeleted() const
	{ return ID == DELETED; }

	int ID;				// Unique timer ID
	TIMER_SET set;		// Timer settings
	TIMEPREC timeNext;	// Time of next signal
	int signalsN;		// Current amount of signals fired
	int lostN;			// Amount of lost signals
};


// TimerTable
// Simple class for storing and managing timer records

// This class is for temporary use. Later it should be refactored using Layers concept
class TimerTable
{
	// Byte-filler for free space
	static const unsigned char FILLER = 0xFF;

public:

	TimerTable();
	virtual ~TimerTable();

// Memory for timer table records must be allocated outside
// For the timers table use RAM
public:

	// Calculates the amount of memory (in bytes)
	// required for timer table with demandRecsN timers
	int GetBinSize(int demandRecsN);

	// Sets the external allocated memory for timer table
	bool SetMem(void* pMem, int size);

public:

	// Adds a new record to table
	TIMER_REC* Add(const TIMER_SET* pRec);

	// Updates record with specified ID
	TIMER_REC* Update(int ID, const TIMER_SET* pSet);

	// Deletes record with specified ID
	bool Delete(int ID);

	// Marks record as deleted
	bool DeleteRec(TIMER_REC* pRec);

	// Returns a new unique ID
	int GetNewID();

	// Current amount of timers
	int GetTimersCount()
	{ return m_RecsN; }

	// Finds the record that first generate signal after timeMin
	TIMER_REC* GetMinNextTime(TIMEPREC &timeMin);

	// Find routine
	TIMER_REC* FindTimer(int ID);

	// Iterations
	TIMER_REC* First();
	TIMER_REC* Next();

// For internal use
protected:

	// Finds first free place in table
	TIMER_REC* FindFreePlace();

private:

	// Pointer to table
	TIMER_REC* m_pRecs;

	// Max and current count of records in table
	int m_MaxRecsN;
	int m_RecsN;

	// For search
	TIMER_REC* m_pFound;
	int m_RecsRemain;
};

} // namespace Timework

#endif /* DTIMERTABLE_H_ */
