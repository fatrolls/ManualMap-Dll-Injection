#pragma once
#include "PEB.h"
#include <iostream>
#include <unordered_map>
#include "Tracer.h"
#include <string>
#include "PEFile.h"


#define PROCESS_32 1
#define PROCESS_64 2

struct Module {
	Module(uint64_t DllBase, std::string FullDllName, std::string BaseDllName, uint64_t EntryPoint, uint32_t SizeOfImage, PEFile* peFile);
	~Module();
	uint64_t DllBase;
	std::string FullDllName;
	std::string BaseDllName;
	uint64_t EntryPoint;
	uint32_t SizeOfImage;
	PEFile* peFile;
};
 

class Process{
public:
	Process();
	~Process();

	//AttachType
	//1 - UserMode 
	void Attach(int processID, short attachType);
	int getPID();
	std::unordered_map<std::string,Module*>* getModules();
	Module* getModuleByName(char* moduleName);
	void printAllModules();
	void addModuleIfAbsent(Module* mod);
	

	size_t ReadMemory(void* toRead, void* toWrite, int size, bool excludeInacessibleMemory);
	size_t VirtualQueryMemory(void* address, MEMORY_BASIC_INFORMATION64* buffer);
	size_t WriteMemory(void* toRead, void* toWrite, int size);
	void*  AllocateMemoryProcess(void* adress, int size, DWORD allocType, DWORD protection);
	DWORD  CreateNewThread(void* address);
	BYTE getArchiteture();

private:
	template<typename T>
	void extractAllPebDataTables();

	template<typename T>
	std::string ReadMemoryUnicodeString(PEBStruct::UNICODE_STRING<T> inProcessString);

	void readModule(uint64_t DllBase, std::string FullDllName, std::string BaseDllName, uint64_t EntryPoint, uint32_t SizeOfImage);

private:
	std::unordered_map<std::string,Module*> moduleList; //DO NOT USE This is never refreshed and can be empty at start
	Memory* MemoryAccess;
	PEB* peb;
	bool isAttached;
	int processID;
};
