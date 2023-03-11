/*
 * IContext.h
 *
 *  Created on: Mar 9, 2023
 *      Author: dan
 */

#ifndef SCHEDULER_ICONTEXT_H_
#define SCHEDULER_ICONTEXT_H_

namespace Timework
{

// Interface for ScedTable store
class IContext {

public:

	virtual ~IContext() = default;

	// Must check if resource (file) already exists
	virtual bool IsExist() = 0;

	// Must create new resource (file)
	virtual bool Create(int size) = 0;

	// Must open new resource (file)
	virtual int Open() = 0;

	// Must return binary size size of resource
	// or D_RET_ERROR if error occurs
	virtual int Size() = 0;

	// Must close new resource (file)
	virtual bool Close() = 0;

	// Must delete new resource (file)
	virtual bool Delete() = 0;

	// Must implement system Read/Write operations
	virtual bool Read(void* pData, int offset, int size) = 0;
	virtual bool Write(const void* pData, int offset, int size) = 0;
};

} /* namespace Timework */

#endif /* SCHEDULER_ICONTEXT_H_ */
