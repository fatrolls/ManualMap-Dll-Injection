#pragma once
#include "PEBStruct.h"
#include "Memory.h"

class PEB {
public:
	virtual void* getPEB() = 0;
	virtual uint64_t getLdrPointer() = 0;
	virtual uint64_t getApiSetMap() = 0;
};

template <typename T>
class _PEB : public PEB{
public:
	_PEB(Memory* reader) { this->reader = reader; reader->ReadPEB(&peb,sizeof(peb)); }

	void* getPEB() override { return (void*)peb; }
	uint64_t getLdrPointer() override { return (uint64_t)peb.Ldr; }
	uint64_t getApiSetMap() override { return (uint64_t)peb.ApiSetMap; }

private:
	PEBStruct::_PEB<T> peb;
	Memory* reader;
};
