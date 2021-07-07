#include "ManualInjection.h"
#include "constants.h"
#define MAX_32_INT 2147483647


ManualInjection::ManualInjection(char * dllPath, Process * prc): Injector(dllPath, prc)
{
}

int ManualInjection::injectDll()
{
	void* base = process->AllocateMemoryProcess((void*)peFile->getPreferedImageBase(), peFile->getSizeOfImage(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	fixRelocations(base);
	fixDependencies();
	copyDllToTargetProcess(base);
	uint64_t RVA = peFile->getEntryPointRVA();
	if (RVA) {
		injectRemoteThread((void*)((uint64_t)base + RVA));
	}

	addManualMapDll(base,peFile->getExportName(),NULL, (uint64_t)base + RVA);
	return 1;
}

void ManualInjection::injectRemoteThread(void * entryPoint)
{
	if(process->CreateNewThread(entryPoint));
	else { throw std::exception("Fail to create remote thread"); }
}

void ManualInjection::copyDllToTargetProcess(void * address)
{
	process->WriteMemory(address, peFile->getLoadedImageBase(), peFile->getSizeOfImage());
}

void ManualInjection::fixDependencies()
{
	ImportsMap* imports = peFile->getImport();
	for (ImportsMap::iterator itr = imports->begin(); itr != imports->end(); ++itr) {
		char * importDll = itr->first;

		Module * mod = 0;

		try {mod = process->getModuleByName(importDll);}	
		catch (std::exception &e) {
			debugInformation(std::string("Manually Loading = ").append(importDll).data());
			mapDependencie(importDll);
			try { mod = process->getModuleByName(importDll); }
			catch (std::exception &e) {
				throw std::exception("Fail to load Module");
			}
		}

		for (ImportEntry *entry : itr->second) {
			uint32_t functionRVA = 0;
			if (entry->isByOrdinal()) {
				functionRVA = mod->peFile->getExportFunctionRVA(entry->getOrdinal());
			}
			else {
				ExportEntry &exportEntry = mod->peFile->getExport()->at(entry->getOrdinal());
				if (strcmp(exportEntry.getName(), entry->getFunctionName()))
					functionRVA = exportEntry.getFunctionRVA();
				else
					functionRVA = mod->peFile->getExportFunctionRVA(entry->getFunctionName());
				
			}

			entry->setImportAbsoluteAdress(functionRVA + (uint64_t)mod->DllBase);
		}
	}

}

void ManualInjection::fixRelocations(void * imageBase)
{
	if (peFile->getPreferedImageBase() == (uint64_t)imageBase) {
		return;
	}

	if (peFile->getNumberOfRvaAndSizes() < BASE_RELOCATION_TABLE) {
		throw std::exception("Error: numberOfRvaAndSizes < 5");
	}

	if (peFile->getCoffHeader()->characteristics & 0x0001) {
		throw std::exception("Error: Relocation stripped");
	}
	int relocTableSize = peFile->getBaseDataDirectory()[BASE_RELOCATION_TABLE].size;

	if (!relocTableSize) {
		return;
	}

	void* loadedImageBase = peFile->getLoadedImageBase();
	BASE_RELOCATION* relocBase = (BASE_RELOCATION*)peFile->getAbsAddress(loadedImageBase, peFile->getBaseDataDirectory()[BASE_RELOCATION_TABLE].virtualAddress);
	int64_t delta = (int64_t)imageBase - peFile->getPreferedImageBase();

	if (abs(delta) >= MAX_32_INT && peFile->getArchiteture() == PE32) {
		throw std::exception("Relocation delta too big for 32 bit");
	}

	int byteCounter = 0;
	while (byteCounter < relocTableSize) {

		int sizeArrWords = (relocBase->size - 8) / sizeof(uint16_t);
		uint64_t* valueToSwapRVA = (uint64_t*)peFile->getAbsAddress(loadedImageBase, relocBase->virtualAddress);
		uint16_t* typeOffset = (uint16_t*)peFile->getAbsAddress(relocBase, 8);

		for (int i = 0; i < sizeArrWords; i++) {
			uint16_t offset = RELOC_ENTRY_OFFSET(typeOffset[i]);
			uint8_t type = RELOC_ENTRY_TYPE(typeOffset[i]);

			if (type == IMAGE_REL_BASED_DIR64) {
				uint64_t* result = (uint64_t*)peFile->getAbsAddress(valueToSwapRVA, offset);
				*result += delta;
			}
			else if (type == IMAGE_REL_BASED_HIGHLOW) {
				uint32_t* result = (uint32_t*)peFile->getAbsAddress(valueToSwapRVA, offset);
				*result += delta;
			}
			else if (type == IMAGE_REL_BASED_ABSOLUTE) {
				continue;
			}
			else {
				throw std::exception("Not supported relocation base");
			}

		}
		byteCounter += relocBase->size;
		relocBase = (BASE_RELOCATION*)peFile->getAbsAddress(relocBase, relocBase->size);
	}
}

void ManualInjection::mapDependencie(char * name)
{
	HMODULE hModule = GetModuleHandleA(NULL);
	char path[MAX_PATH] = { 0 };
	if (!hModule)
		throw std::exception("Fail to get Module");
	GetModuleFileNameA(hModule, path, MAX_PATH);
	stripExecutableFromPath(path);
	appendFileToPath(path, MAX_PATH, name);

	char systemPath[MAX_PATH] = { 0 };
	if (!GetSystemDirectoryA(systemPath, MAX_PATH))
		return;
	systemPath[strlen(systemPath)] = '\\';
	appendFileToPath(systemPath, MAX_PATH, name);


	//Try to load using main path
	try {
		ManualInjection injector(path, process);
		if (!injector.injectDll())
			throw std::exception("Fail to load Dll Dependencie");
		return;
	}catch (std::exception &e) {
		debugInformation("Dependencie not present in mainPath");
	}

	//Try to load using system32 path
	try {
		ManualInjection injector(systemPath, process);
		if (!injector.injectDll())
			throw std::exception("Fail to load Dll Dependencie");
		return;
	}
	catch (std::exception &e) {
		throw std::exception(std::string("Dll Not Found: ").append(name).data());
	}

}

void ManualInjection::addManualMapDll(void *address, char* name, char* fullName, uint64_t entryPoint)
{
	Module *mod = new Module((uint64_t)address, name, fullName, entryPoint, peFile->getSizeOfImage(), peFile);
	process->addModuleIfAbsent(mod);
}
