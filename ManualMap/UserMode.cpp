#include "UserMode.h"

UserMode::UserMode(int processID)
{
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, processID);
	if (!hProcess) {
		throw std::exception("Cannot get handle to process");
	}
	debugInformation("UserMode Process sucessfully Opened");

	BOOL is64 = 0;
	if(!IsWow64Process(hProcess, &is64))
		throw std::exception("Error getting process type");

	if (!is64) {
		architeture = PROCESS_64;
		debugInformation("Process is 64bit");
	}
	else {
		architeture = PROCESS_32;
		debugInformation("Process is 32bit");
	}
	
}




size_t UserMode::ProcessReadMemory(void * toRead, void * toWrite, int size)
{
	size_t readBytes = 0;
	DWORD oldProtect = 0;
	if (!ReadProcessMemory(hProcess, toRead, toWrite, size, &readBytes)) {
		return 0;
	}
	return readBytes;
}

size_t UserMode::WriteMemory(void * toWrite, void * toRead, int size)
{
	size_t readBytes = 0;
	WriteProcessMemory(hProcess, toWrite, toRead, size, &readBytes);
	return readBytes;
}

void * UserMode::AllocateMemoryProcess(void * adress, int size, DWORD allocType, DWORD protection)
{
	return VirtualAllocEx(hProcess, adress, size, allocType, protection);
}

int UserMode::ReadPEB(void* buffer, int bufferSize)
{
	debugInformation("Extracting PEB");
	if (architeture == PROCESS_32) {
		uint64_t pebBase = 0;
		ULONG size = 0;
		if (NTSTATUS ret = ntdll.NtQueryInformationProcess(hProcess, PROCESSINFOCLASS::ProcessWow64Information, &pebBase, sizeof(pebBase), &size)) {
			printf("NTSTATUS : %#llx SizeOfPbi: %d\n", ret, pebBase);
			throw std::exception("Could not get PEB");
		}
		
		debugInformation("PROCESS_BASIC_INFORMATION32 succesfully retrieved");

		size_t size1 = 0;
		size1 = ReadMemory((void*)pebBase, buffer, bufferSize,0);
		if (size1!=bufferSize) {
			//printf("Error code: %d\n", GetLastError());
			throw std::exception("Could not get PBI");
		}
		return size1;
	}
	
	if (architeture == PROCESS_64) {
		PEBStruct::PROCESS_BASIC_INFORMATION<uint64_t> pbi;
		ULONG size = 0;
		if (ntdll.NtQueryInformationProcess(hProcess, PROCESSINFOCLASS::ProcessBasicInformation, &pbi, sizeof(pbi), &size))
			throw std::exception("Could not get PBI");

		debugInformation("PROCESS_BASIC_INFORMATION64 succesfully retrieved");
		printf("PEBBase: %#llx		Reserved1: %#llx\n", pbi.PebBaseAddress, pbi.Reserved1);

		size_t size1 = 0;
		size1 = ReadMemory((void*)pbi.PebBaseAddress, buffer, bufferSize,0);
		if (size1 != bufferSize) {
			//printf("Error code: %d\n", GetLastError());
			throw std::exception("Fail To read PEB");
		}
		return size1;

	}

	throw std::exception("No architeture");
}

BYTE UserMode::getArchiteture()
{
	return architeture;
}

size_t UserMode::VirtualQueryInformation(void * address, MEMORY_BASIC_INFORMATION * buffer, size_t size)
{
	return VirtualQueryEx(hProcess, address, buffer, size);
}

DWORD UserMode::CreateNewThread(void* address)
{
	DWORD threadID = 0;
	if(CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)address, NULL, NULL, &threadID))
		return threadID;
	return NULL;
}

HANDLE UserMode::getProcessHandle()
{
	return hProcess;
}
