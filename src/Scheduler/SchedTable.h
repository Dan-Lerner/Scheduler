/*
 * DShedTable.h
 *
 *  Created on: 15 may. 2015 yr.
 *      Author: Dan
 */

#ifndef DSHEDTABLE_H_
#define DSHEDTABLE_H_

#include "Macro.h"
#include "SchedStr.h"

#define SCHED_USER_ID_BOUNDARY	0xFFFF		// Max value for ID, assigned from outside

namespace Timework
{

// Record for scheduler table
struct EVENT_REC
{
	EVENT_REC()
	{
		timeNext = 0;
		timerID = 0;
		eventN = 0;
		lostN = 0;
	}

	static const int DELETED = -1;	// ID for deleted record

	// Setters
	void SetDeleted()
	{ set.ID = DELETED; }

	// Getters
	bool IsDeleted() const
	{ return set.ID == DELETED; }

	// Overloads
	EVENT_REC operator=(const EVENT_REC& rec);

	SCHEDULER_EVENT_SET set;		// Event settings
	TIME timeNext;					// Next event time
	int timerID;					// ID of attached timer
	int eventN;						// Number of current event
	int lostN;						// Lost events amount
	int align[3];					// Just for align
};

class IContext;

// SchedTable
// Simple class for storing and managing scheduler records

class SchedTable
{
	// Byte-filler for free space
	static const unsigned char FILLER = 0xFF;

public:

	SchedTable();
	virtual ~SchedTable();

// For the schedule table use FRAM/NOR/EEPROM
public:

	// Initialization context for table
	bool InitContext(IContext* pContext, int demandRecsN);

public:
	// Adds a new record to table
	int SetEvent(SCHEDULER_EVENT_SET* pSet);

	// Adds a new record
	int AddEvent(SCHEDULER_EVENT_SET* pSet);

	// Updates an existing record
	bool UpdateEvent(const SCHEDULER_EVENT_SET* pSet, int index);

	// Reads the record placed at index
	bool GetRecord(EVENT_REC* pRec, int index);

	// Writes the record placed at index
	bool SetRecord(const EVENT_REC* pRec, int index);

	// Deletes record with specified ID
	bool Delete(int ID);

	// Deletes record placed at index
	bool DeleteRecord(int index);

	// Deletes all records in table
	bool ClearTable();

	// Finds the record that will generate event first after timeMin
	int GetMinNextTime(TIME &timeMin);

	// Gets time of next event generation for record with specified ID
	bool GetNextTime(int ID, TIME& time);

	// Finds record with specified ID
	int FindEvent(int ID);

	// Finds record by timer ID
	int FindByTimer(int timerID);

	// Iterations
	int First();
	int Next();

public:

	// Calculates binary offset by index
	int IndexToPos(int index);

	// Calculates index by binary offset
	int PosToIndex(int pos);

// For internal usage
protected:

	// Opens existing Table
	bool OpenTable(IContext* pContext);

	// Creates and initializes new table
	bool CreateTable(IContext* pContext, int demandRecsN);

	// Calculates the amount of memory (in bytes) required for a scheduler table with demandRecsN records
	int GetBinSize(int demandRecsN)
	{ return demandRecsN * sizeof(EVENT_REC); }

	// Finds all non-deleted records in table;
	int GetValidRecordsCount(int RecsN);

	// Checks if record deleted
	bool IsRecordDeleted(int index);

	// Finds first free place in table
	int FindFreePlace();

	// Finds free ID in the table
	int GetAutoID();

private:

	// Context for table read/write operations
	IContext* m_pContext;

	// Max and current count of records in table
	int m_MaxRecsN;
	int m_RecsN;

	// For search
	int m_Found;
	int m_RecsRemain;
};

} //namespace Timework

#endif /* DSHEDTABLE_H_ */
