#pragma once
#include "NtdllExports.h" 
#include "Memory.h"
#include "Process.h"


class UserMode : public Memory {
public:
	UserMode(int processID);

	size_t ProcessReadMemory(void* toRead, void* toWrite, int size) override;
	size_t WriteMemory(void* toWrite, void* toRead, int size) override;
	void* AllocateMemoryProcess(void* adress, int size, DWORD allocType, DWORD protection) override;
	int ReadPEB(void* buffer, int bufferSize) override;
	BYTE getArchiteture() override;
	size_t VirtualQueryInformation(void* address, MEMORY_BASIC_INFORMATION* buffer, size_t size) override;
	DWORD CreateNewThread(void* address) override;

	HANDLE getProcessHandle();

private:
	Ntdll ntdll;
	BYTE architeture;
	HANDLE hProcess;
};