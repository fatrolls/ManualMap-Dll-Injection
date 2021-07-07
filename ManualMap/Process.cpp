#include "Process.h"


Process::Process()
{
	isAttached = 0;
}

Process::~Process(){
	for (auto x : moduleList)
		delete x.second;
	
}

void Process::Attach(int processID, short attachType)
{
	this->processID = processID;
	if (attachType == 1) {
		MemoryAccess = new UserMode(processID);
		BYTE arch = getArchiteture();

		if (arch == PROCESS_32)
			peb = new _PEB<uint32_t>(MemoryAccess);
		else if (arch == PROCESS_64)
			peb = new _PEB<uint64_t>(MemoryAccess);
		else
			throw std::exception("Invalid architeture");
		
	}
	else {
		throw std::exception("attachType value not recognized");
	}
	isAttached = true;
}

int Process::getPID()
{
	return processID;
}



std::unordered_map<std::string,Module*>*  Process::getModules()
{
	int architeture = getArchiteture();
	if (moduleList.empty()) {
		if (architeture == PROCESS_32)
			extractAllPebDataTables<uint32_t>();
		else if (architeture == PROCESS_64)
			extractAllPebDataTables<uint64_t>();
		else
			throw std::exception("Not 32bit nor 64");
	}
	return &moduleList;
}

Module* Process::getModuleByName(char* moduleName)
{
	getModules();
	if (!moduleList[moduleName])
		throw std::exception("Error finding module");
	return moduleList[moduleName];
}

void Process::printAllModules()
{
	getModules();
	int something = moduleList.size();
	for (auto x : moduleList) {
		Module *m = x.second;
		printf("Module: %s\n", x.first.data());
		printf("	DllName Base: %s\n", m->BaseDllName.data());
		printf("	Base Address: %#llx\n", m->DllBase);
		printf("	Module Path: %s\n", m->FullDllName.data());
		printf("	SizeOfImage: %d\n", m->SizeOfImage);
		printf("	EntryPoint: %#llx\n", m->EntryPoint);
		printf("	Name Based on Export: %s\n\n", m->peFile->getExportName());
	}
}

void Process::addModuleIfAbsent(Module * mod)
{
	const char* name = mod->peFile->getExportName();
	if (!name) {
		name = mod->BaseDllName.data();
	}
	if (moduleList.find(name) == moduleList.end()) {
		moduleList[name] = mod;
		return;
	}

	throw std::exception("Module with same name already loaded");
}

size_t Process::ReadMemory(void * toRead, void * toWrite, int size, bool excludeInacessibleMemory)
{
	return MemoryAccess->ReadMemory(toRead, toWrite, size, excludeInacessibleMemory);
}

size_t Process::VirtualQueryMemory(void * address, MEMORY_BASIC_INFORMATION64 * buffer)
{
	return MemoryAccess->VirtualQueryMemory(address, buffer);
}

size_t Process::WriteMemory(void * toRead, void * toWrite, int size)
{
	return MemoryAccess->WriteMemory(toRead, toWrite, size);
}

void * Process::AllocateMemoryProcess(void * adress, int size, DWORD allocType, DWORD protection)
{
	return MemoryAccess->AllocateMemoryProcess(adress, size, allocType, protection);
}

DWORD Process::CreateNewThread(void * address)
{
	MemoryAccess->CreateNewThread(address);
}

BYTE Process::getArchiteture()
{
	return MemoryAccess->getArchiteture();
}

void Process::readModule(uint64_t DllBase, std::string FullDllName, std::string BaseDllName, uint64_t EntryPoint, uint32_t SizeOfImage)
{
	void* buffer = malloc(SizeOfImage);
	if(!buffer){
		throw std::exception("Error Allocating memory to new module");
	}

	int bytes = ReadMemory((void*)DllBase, buffer, SizeOfImage,1);
	if (!bytes) {
		free(buffer);
		throw std::exception("Error reading PEFile from process");
	}
	//system("pause");

	PEFile *peFile = PEFile::loadMappedFile(buffer);
	//printf("TEST\n");
	Module *test = new Module(DllBase, FullDllName.data(), BaseDllName.data(), EntryPoint, SizeOfImage, peFile);
	addModuleIfAbsent(test);
}

template<typename T>
std::string Process::ReadMemoryUnicodeString(PEBStruct::UNICODE_STRING<T> inProcessString)
{
	int length = inProcessString.Length;
	//wchar_t* buffer = (wchar_t*)malloc(length);
	std::wstring buffer(length+1,0);

	if (ReadMemory((void*)inProcessString.Buffer, (void*)buffer.data(), length,0) != length) {
		return std::string("");
	}

	std::string result(buffer.begin(), buffer.end());
	//printf("%s\n", result.data());
	return result;

}

template<typename T>
void Process::extractAllPebDataTables()
{
	PEBStruct::PEB_LDR_DATA<T> ldr = { 0 };
	if (sizeof(ldr) != ReadMemory((void*)peb->getLdrPointer(), &ldr, sizeof(ldr),0))
		throw std::exception("Fail to read LDR DATA");
	
	T first_link = (T)peb.Ldr + ((T)(&ldr.InLoadOrderModuleList) - (T)(&ldr));
	T forward_link = ldr.InLoadOrderModuleList.Flink;
	//printf("ldr.InLoadOrderModuleList = %#llx\n", first_link);
	

	do {
		PEBStruct::LDR_DATA_TABLE_ENTRY<T> entry = { 0 };
		if (ReadMemory((void*)forward_link, &entry, sizeof(entry),0) != sizeof(entry)) {
			debugInformation("Fail to Read a Module on PEB");
			continue;
		}

		//printf("ForwardList: %#llx		FirstList: %#llx\n", forward_link, first_link);
		//Sleep(1500);
		if (!entry.DllBase)
			continue;
		std::string FullName = ReadMemoryUnicodeString(entry.FullDllName);
		std::string BaseName = ReadMemoryUnicodeString(entry.BaseDllName);

		if (FullName.empty() || BaseName.empty())
			throw std::exception("Error reading module name");

		debugInformation(BaseName.data());
		readModule(entry.DllBase, FullName, BaseName, entry.EntryPoint, entry.SizeOfImage);
		forward_link = entry.InLoadOrderLinks.Flink;
	} while (forward_link && first_link != forward_link);

}

Module::Module(uint64_t DllBase, std::string FullDllName, std::string BaseDllName, uint64_t EntryPoint, uint32_t SizeOfImage, PEFile* peFile) :
	DllBase(DllBase), FullDllName(FullDllName), BaseDllName(BaseDllName), EntryPoint(EntryPoint), SizeOfImage(SizeOfImage), peFile(peFile){
}

Module::~Module()
{
	delete peFile;
}



