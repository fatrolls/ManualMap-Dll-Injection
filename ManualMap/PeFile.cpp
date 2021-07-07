#include "PeFile.h"
#include "PEFile32.h"
#include "PEFile64.h"
#include "Tracer.h"
#include "constants.h"
#include "Loader.h"


PEFile::PEFile()
{
	dosHeader = nullptr;
	coffHeader = nullptr;
	standartOptionalHeader = nullptr;
	sectionHeader = nullptr;
}

PEFile::~PEFile()
{
	destroy();
}

bool PEFile::setHeaders(void * filePointer)
{
	if (!filePointer) {
		throw std::exception("No file especifed");
	}
	dosHeader = getDosHeader(filePointer);
	coffHeader = getCoffHeader(dosHeader);
	standartOptionalHeader = getStandartOptionalHeader(coffHeader);
	sectionHeader = getSectionHeader(standartOptionalHeader,coffHeader);

	return 1;
}

void PEFile::setExports()
{
	if (!Exports.empty())
		Exports.clear();

	EXPORT_DIRECTORY_TABLE* exports = getExportDirectoryTable();
	if (!exports)
		return;

	uint16_t* ordinalArray = (uint16_t*)getPointerBasedOnRVA(exports->ordinalTableRVA);
	uint32_t* namesArray = (uint32_t*)getPointerBasedOnRVA(exports->namePointerRVA);
	uint32_t* functionsExport = (uint32_t*)getPointerBasedOnRVA(exports->exportAdressTableRVA);
	for (int i = 0; i < exports->numberOfFunctionEntrys; i++) {
		Exports.push_back(ExportEntry((uint32_t*)&(functionsExport[i]), 0));
	}
	for (int i = 0; i < exports->numberOfNamePointers; i++) {

		char* name = (char*)getPointerBasedOnRVA(namesArray[i]);
		uint32_t ordinal = ordinalArray[i];
		ExportEntry &entry = Exports[ordinal];
		entry.setFunctionName(name);
	}
}

void PEFile::destroy()
{
	free(dosHeader);
	Exports.clear();
	for (auto x : Imports) {
		for (auto y : x.second) {
			delete y;
		}
	}
	Imports.clear();
}

PEFile * PEFile::CreatePEFile(void * filePointer)
{
	STANDART_OPTIONAL_HEADER* soh = getStandartOptionalHeader(getCoffHeader(getDosHeader(filePointer)));
	int arch = soh->magic;
	PEFile *file = 0;
	if (arch == PE32)
		file = new PEFile32();
	else if (arch == PE32_PLUS)
		file = new PEFile64();
	else
		throw std::exception("No file corresponds to 32 or 64 bits");

	return file;
}

PEFile * PEFile::mapViewOfFile(char * path)
{
	int size = 0;
	void* file = loadFile(path, size);
	if (!size)
		throw std::exception("Error loading DLL");
	return mapViewOfFile(file);
}

PEFile * PEFile::mapViewOfFile(void * filePointer)
{
	PEFile* peFile = CreatePEFile(filePointer);
	peFile->initialize(filePointer,0);
	return peFile;
}

PEFile* PEFile::loadMappedFile(void * filePointer)
{
	PEFile* peFile = CreatePEFile(filePointer);
	peFile->initialize(filePointer, 1);
	return peFile;
}

void PEFile::initialize(void* filePointer, bool isMapped)
{
	destroy();
	setHeaders(filePointer);
	if (!isMapped)
		return mapFile();
	setExports();
	setImports();
}

void * PEFile::getPointerBasedOnRVA(int offset)
{
	return getAbsAddress(getLoadedImageBase(),offset);
}

void * PEFile::getAbsAddress(void * adress, int offset)
{
	return (void*)((uint64_t)adress + offset);
}

void* PEFile::getLoadedImageBase()
{
	return (void*)dosHeader;
}

short PEFile::getArchiteture()
{
	short arch = 0;

	arch = standartOptionalHeader->magic;

	if (arch == PE32_PLUS || arch == PE32)
		return arch;

	return 0;
}

DOS_HEADER * PEFile::getDosHeader()
{
	return dosHeader;
}

COFF_HEADER * PEFile::getCoffHeader()
{
	return coffHeader;
}

STANDART_OPTIONAL_HEADER * PEFile::getStandartOptionalHeader()
{
	return standartOptionalHeader;
}

SECTION_HEADER * PEFile::getSectionHeader()
{
	return sectionHeader;
}

uint64_t PEFile::getEntryPointRVA()
{
	return standartOptionalHeader->addressOfEntryPoint;
}

DATA_DIRECTORY* PEFile::getSpecificDataDirectory(int index)
{
	if (index < getNumberOfRvaAndSizes())
		return &(getBaseDataDirectory()[index]);
	else
		throw std::exception("Error getting data_directory");
}

ExportsMap* PEFile::getExport()
{
	return &Exports;
}

ImportsMap * PEFile::getImport()
{
	return &Imports;
}


void PEFile::mapFile()
{
	void *bufferImageBase = malloc(getSizeOfImage());
	if (!bufferImageBase) {
		throw std::invalid_argument("Error alocating memory at my process");
	}

	memcpy(bufferImageBase, (void*)dosHeader, getSizeOfHeaders());

	for (int i = 0; i < coffHeader->numberOfSections; i++) {
		SECTION_HEADER * hdr = (SECTION_HEADER*)getAbsAddress((void*)sectionHeader, i * sizeof(SECTION_HEADER));
		memcpy(getAbsAddress(bufferImageBase, hdr->virtualAddress), getAbsAddress((void*)dosHeader, hdr->pointerToRawData), hdr->sizeOfRawData);
	}
	initialize(bufferImageBase, 1);
}

DOS_HEADER* PEFile::getDosHeader(void * filePointer)
{
	DOS_HEADER* dos = (DOS_HEADER*)filePointer;
	if (dos->signature == DOS_SIGNATURE)
		return dos;
	throw std::invalid_argument("Not a DOS Header");
}

COFF_HEADER* PEFile::getCoffHeader(DOS_HEADER * dosHeader)
{
	COFF_HEADER* coff = (COFF_HEADER*)((uint64_t)dosHeader + (uint64_t)dosHeader->e_lfanew);
	if (coff->signature == COFF_SIGNATURE)
		return coff;
	printf("coff->signature = %d", coff->signature);
	throw std::invalid_argument("Not a COFF Header");
}

SECTION_HEADER* PEFile::getSectionHeader(STANDART_OPTIONAL_HEADER * optionalHeader, COFF_HEADER * coffHeader)
{
	SECTION_HEADER* sh = (SECTION_HEADER*)((uint64_t)optionalHeader + coffHeader->sizeOfOptionalHeader);
	return sh;
}

STANDART_OPTIONAL_HEADER* PEFile::getStandartOptionalHeader(COFF_HEADER * coffHeader)
{
	STANDART_OPTIONAL_HEADER * oh = (STANDART_OPTIONAL_HEADER*)((uint64_t)coffHeader + sizeof(COFF_HEADER));
	if (oh->magic == PE32 || oh->magic == PE32_PLUS)
		return oh;
	throw std::invalid_argument("Not an image file x86 or x86_64");
}

EXPORT_DIRECTORY_TABLE* PEFile::getExportDirectoryTable()
{
	uint32_t RVA = getSpecificDataDirectory(DIRECTORY_EXPORT_TABLE)->virtualAddress;
	if (!RVA) {
		debugInformation("Image doesn't have exports");
		return nullptr;
	}
	return (EXPORT_DIRECTORY_TABLE*)getPointerBasedOnRVA(RVA);
}

uint32_t PEFile::getExportFunctionRVA(char * name)
{
	for (ExportEntry entry : Exports) {
		char* exportName = entry.getName();
		if(!name)
			continue;

		if (!strcmp(name, exportName))
			return entry.getFunctionRVA();
	}
	throw std::exception("Fail to find function by name");
}

uint32_t PEFile::getExportFunctionRVA(uint16_t ordinal)
{
	EXPORT_DIRECTORY_TABLE* table = getExportDirectoryTable();
	if(table){
		uint32_t index = ordinal - table->ordinalBase;
		ExportsMap *map = getExport();
		if(index>=0 && index<map->size())
			return map->operator[](ordinal - table->ordinalBase).getFunctionRVA();
	}
	throw std::exception("Fail to find function by ordinal");
}

char * PEFile::getExportName()
{
	EXPORT_DIRECTORY_TABLE* exp = getExportDirectoryTable();
	if (!exp)
		return NULL;
	return (char*)getPointerBasedOnRVA(exp->nameRVA);
}


