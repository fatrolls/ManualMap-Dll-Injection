#pragma once
#include "Injector.h"
#include <Math.h>

class ManualInjection : public Injector {

public:
	ManualInjection(char* dllPath, Process* prc);
	int injectDll();

private:
	void injectRemoteThread(void* entryPoint);
	void copyDllToTargetProcess(void* address);
	void fixDependencies();
	void fixRelocations(void* imageBase);
	void mapDependencie(char* name);
	void addManualMapDll(void *address, char* name, char* fullName, uint64_t entryPoint);
};