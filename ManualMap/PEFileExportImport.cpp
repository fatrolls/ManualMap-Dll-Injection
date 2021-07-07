#include "PEFileExportImport.h"
#include <stdexcept>

/*ExportEntry::ExportEntry(void * ordinalTable = 0, uint32_t ordinalOffset = 0, void * nameTable = 0, uint32_t nameOffset = 0, void * functionTable = 0, uint32_t functionOffset = 0)
{
	//ordinal = (uint16_t*)((uint64_t*)ordinalTable + ordinalOffset);
	name = (char*)((uint64_t*)nameTable + nameOffset);
	functionRVA = (uint32_t*)((uint64_t*)functionTable + functionOffset);
}*/

ExportEntry::ExportEntry(uint32_t* functionEntry, char * nameEntry = 0)
{
	//ordinal = (uint16_t*)(ordinalEntry);
	name = nameEntry;
	functionRVA = functionEntry;
}

void ExportEntry::setFunctionName(char* name)
{
	this->name = name;
}

uint32_t ExportEntry::getFunctionRVA()
{
	if (!functionRVA)
		return 0;

	return *functionRVA;
}

char * ExportEntry::getName()
{
	return name;
}




