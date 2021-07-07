#pragma once
#include "Loader.h"
#include "PeFile.h"
#include "Process.h"
//#include "Loader.h"

/*void traceError(const char * error) {
	printf("%s\n", error);
	Sleep(1000);
}*/

struct LoadedFile {
	void* file;
	int size;
};

class Injector{


public:
	Injector(char* dllPath, Process* prc);
	~Injector();
	virtual int injectDll() = 0;


private:
	void loadDllToMemory(char* dllPath);

protected:
	PEFile* peFile;
	Process* process;
};
