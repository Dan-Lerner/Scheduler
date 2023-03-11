/*
 * FileContext.h
 *
 *  Created on: Mar 9, 2023
 *      Author: dan
 */

#ifndef SCHEDULER_FILECONTEXT_H_
#define SCHEDULER_FILECONTEXT_H_

#include <fstream>
#include "IContext.h"

namespace Timework
{

// Example of context for Scheduler table

class FileContext : public IContext
{

public:

	FileContext(const char* file);
	virtual ~FileContext();

// Implementation of IContext
public:

	virtual bool IsExist();
	virtual bool Create(int size);
	virtual int Open();
	virtual int Size();
	virtual bool Close();
	virtual bool Delete();
	virtual bool Read(void* pData, int offset, int size);
	virtual bool Write(const void* pData, int offset, int size);

private:

	// Filename
	char m_file[255];

	// Stream object
	std::fstream stream;
};

} /* namespace Timework */

#endif /* SCHEDULER_FILECONTEXT_H_ */
