#include "Memory.h"
#include <cstdint>
#include <stdio.h>
#include <exception>

size_t Memory::VirtualQueryMemory(void * address, MEMORY_BASIC_INFORMATION64* buffer)
{
	size_t result = VirtualQueryInformation(address, (MEMORY_BASIC_INFORMATION*)buffer, sizeof(MEMORY_BASIC_INFORMATION64));
	if (result == sizeof(MEMORY_BASIC_INFORMATION64)) {
		return result;
	}
	if (result == sizeof(MEMORY_BASIC_INFORMATION32)) {
		MEMORY_BASIC_INFORMATION32 temp;
		memcpy(&temp, buffer, sizeof(MEMORY_BASIC_INFORMATION32));
		buffer->RegionSize = temp.RegionSize;
		buffer->AllocationProtect = temp.AllocationProtect;
		buffer->AllocationBase = (PVOID)temp.AllocationBase;
		buffer->BaseAddress = (PVOID)temp.BaseAddress;
		buffer->Protect = temp.Protect;
		buffer->State = temp.State;
		buffer->Type = temp.Type;
		return result;
	}
	return 0;
}

size_t Memory::ReadMemory(void * toRead, void * toWrite, int size, bool excludeInacessibleMemory)
{
	int readBytes = ProcessReadMemory(toRead, toWrite, size);
	if (size > readBytes && excludeInacessibleMemory) {
		MEMORY_BASIC_INFORMATION mib = { 0 };
		size_t currOffset = 0;
		size_t bytesRead = 0;

		uintptr_t bufferOffsetPointer = (uint64_t)toWrite;
		uintptr_t targetOffsetPointer = (uint64_t)toRead;

		for (currOffset = 0; currOffset < size; currOffset += mib.RegionSize) {
			bufferOffsetPointer += mib.RegionSize;
			targetOffsetPointer += mib.RegionSize;

			if (!VirtualQueryMemory((void*)targetOffsetPointer, &mib)) {
				printf("Error: %d\n", GetLastError());
				throw std::exception("Error Querying memory");
			}
			//printf("Size: %x, BaseAdress: %x, BaseAllocation: %x, RegionSize: %x, targetOffsetPointer: %x\n\n",size,mib.BaseAddress,mib.AllocationBase,mib.RegionSize, targetOffsetPointer);

			if (mib.State != MEM_COMMIT || mib.Protect == PAGE_NOACCESS)
				continue;

			if (currOffset + mib.RegionSize > size) {
				bytesRead += ReadMemory((void*)targetOffsetPointer, (void*)bufferOffsetPointer, size - currOffset, 0);
				return bytesRead;
			}
			bytesRead += ReadMemory((void*)targetOffsetPointer, (void*)bufferOffsetPointer, mib.RegionSize, 0);

		}

		return bytesRead;
	}
	else
		return readBytes;
}
