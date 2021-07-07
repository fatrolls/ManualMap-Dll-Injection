#include "Injector.h"


Injector::Injector(char* dllPath, Process* prc)
{
	loadDllToMemory(dllPath);
	process = prc;
}

Injector::~Injector() {
}


void Injector::loadDllToMemory(char* dllPath) {
	peFile = PEFile::mapViewOfFile(dllPath);
}
