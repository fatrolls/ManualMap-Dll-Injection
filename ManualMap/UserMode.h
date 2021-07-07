#pragma once
#include "NtdllExports.h" 
#include "Memory.h"
#include "Process.h"


class UserMode : public Memory {
public:
	UserMode(int processID);

	SIZE_T ProcessReadMemory(void* toRead, void* toWrite, int size) override;
	SIZE_T WriteMemory(void* toWrite, void* toRead, int size) override;
	void* AllocateMemoryProcess(void* adress, int size, DWORD allocType, DWORD protection) override;
	int ReadPEB(void* buffer, int bufferSize) override;
	BYTE getArchiteture() override;
	SIZE_T VirtualQueryInformation(void* address, MEMORY_BASIC_INFORMATION* buffer, SIZE_T size) override;
	DWORD CreateNewThread(void* address) override;

	HANDLE getProcessHandle();

private:
	Ntdll ntdll;
	BYTE architeture;
	HANDLE hProcess;
};