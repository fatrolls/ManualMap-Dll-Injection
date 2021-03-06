// ManualMapper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "UserMode.h"
#include "Loader.h"
#include "ManualInjection.h"

#define PROCESS_ID 4044
#define DLL_PATH "myDllTest.dll"


void testImports(char* path) {
	try {

		PEFile* peFile = PEFile::mapViewOfFile(path);
		ImportsMap* map = peFile->getImport();
		for (auto dll : *map) {
			printf("Dll: %s\n\n", dll.first);
			for (auto entry : dll.second) {
				char* name = entry->getFunctionName();
				if (!name) { continue; }
				printf("Import: %s\n", name);
			}
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		system("pause");
	}
}

void testExports(char* path) {
	try {
		PEFile* peFile = PEFile::mapViewOfFile(path);
		ExportsMap* map = peFile->getExport();
		for (auto dll : *map) {
			printf("ExportName: %s,    ExportRVA: %x\n", dll.getName(), dll.getFunctionRVA());
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		system("pause");
	}
}


void testInjection(){
	try {
		UserMode proc(PROCESS_ID);
		system("pause");
		ManualInjection Injector((char*)DLL_PATH, &proc);
		system("pause");
		Injector.injectDll();

	}catch (std::exception &e) {
			printf("%s\n", e.what());
			system("pause");
		}
}

int main()
{
	testInjection();
	//UserModeProcess proc(PROCESS_ID);
	//proc.printAllModules();
	printf("\nDONE\n");
	system("PAUSE");
}


