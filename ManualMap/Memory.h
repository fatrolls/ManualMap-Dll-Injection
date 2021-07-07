#include <Windows.h>


class Memory{
public:
	//If process is 32 bits, the MEMORY_BASIC_INFORMATION will be copied to the buffer
	size_t				VirtualQueryMemory(void* address, MEMORY_BASIC_INFORMATION64* buffer);

	//Read memory and can exclude inaccessible memory to avoid error in RPM
	//All the inacessible memory will not be written to the buffer, and will be skipped
	//Returns the ammount of memory read
	size_t				ReadMemory(void* toRead, void* toWrite, int size, bool excludeInacessibleMemory);
	virtual size_t      WriteMemory(void* toWrite, void* toRead, int size) = 0;
	virtual void*       AllocateMemoryProcess(void* adress, int size, DWORD allocType, DWORD protection) = 0;
	virtual int         ReadPEB(void* buffer, int bufferSize) = 0;
	virtual BYTE        getArchiteture() = 0;
	virtual	DWORD		CreateNewThread(void* address) = 0;

protected:
	virtual size_t      ProcessReadMemory(void* toRead, void* toWrite, int size) = 0;
	virtual size_t      VirtualQueryInformation(void* address, MEMORY_BASIC_INFORMATION* buffer, size_t size) = 0;
};