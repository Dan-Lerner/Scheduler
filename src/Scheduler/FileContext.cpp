/*
 * FileContext.cpp
 *
 *  Created on: Mar 9, 2023
 *      Author: dan
 */

#include <iostream>
#include <fstream>
#include <string.h>

#include "FileContext.h"

namespace Timework
{

using namespace std;

FileContext::FileContext(const char* file)
{
	// TODO Auto-generated constructor stub

	::strcpy(m_file, file);
}

FileContext::~FileContext()
{
	// TODO Auto-generated destructor stub

	if (stream.is_open())
	{
		Close();
	}
}

bool FileContext::IsExist()
{
	ifstream file(m_file);

	return file.is_open();
}

bool FileContext::Create(int size)
{
	stream.open(m_file, ios::in | ios::out | ios::binary | ios::trunc);
	stream.seekg(size - 1);
	char byte = 0x00;
	stream.write(&byte, 1);
	stream.flush();

	return stream.is_open();
}

int FileContext::Open()
{
	stream.open(m_file, ios::in | ios::out | ios::binary);

	return stream.is_open();
}

int FileContext::Size()
{
	ifstream file(m_file);
	int size = 0;
	file.seekg(0, std::ios::end);
	size = file.tellg();
    file.close();

    return size;
}

bool FileContext::Close()
{
	stream.close();

	return true;
}

bool FileContext::Delete()
{
	return 0 == remove(m_file);
}

bool FileContext::Read(void* pData, int offset, int size)
{
	if (stream.is_open())
	{
		stream.seekg(offset, std::ios::beg);
		stream.read(static_cast<char*>(pData), size);

		return true;
	}

	return false;
}

bool FileContext::Write(const void* pData, int offset, int size)
{
	if (stream.is_open())
	{
		stream.seekg(offset, std::ios::beg);
		stream.write(static_cast<const char*>(pData), size);
		stream.flush();

		return true;
	}

	return false;
}

} /* namespace Timework */

