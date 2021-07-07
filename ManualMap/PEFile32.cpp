#include "PEFile32.h"
#include "Tracer.h"
#include "constants.h"

PEFile32::PEFile32()
{

}

PEFile32::~PEFile32()
{
	destroyImports();
}

void PEFile32::setImports()
{
	if (!Imports.empty()) {
		destroyImports();
		Imports.clear();
	}

	uint32_t RVA = getSpecificDataDirectory(DIRECTORY_IMPORT_TABLE)->virtualAddress;
	if (!RVA) {
		debugInformation("Image doesn't have imports");
		return;
	}
	IMPORT_DIRECTORY_TABLE* directoryTable = (IMPORT_DIRECTORY_TABLE*)(getPointerBasedOnRVA(RVA));

	BYTE* dir = (BYTE*)directoryTable;

	for (int i = 0; directoryTable[i].name; i++) {
		std::vector<ImportEntry*> &entry  = Imports.insert(std::make_pair((char*)getPointerBasedOnRVA(directoryTable[i].name), std::vector<ImportEntry*>())).first->second;

		IMPORT_LOOKUP_TABLE<uint32_t>* lookup = (IMPORT_LOOKUP_TABLE<uint32_t>*)getPointerBasedOnRVA(directoryTable[i].importLookupTable);
		IMPORT_ADDRESS_TABLE<uint32_t>* iat = (IMPORT_ADDRESS_TABLE<uint32_t>*)getPointerBasedOnRVA(directoryTable[i].importAdressTable);


		for (int j = 0; (uint32_t)(lookup[j].Entry.ordinal) != 0; j++) {
			entry.push_back(new _ImportEntry<uint32_t>(&(lookup[j]), &(iat[j]), getLoadedImageBase()));
		}
	}
}

short PEFile32::getArchiteture()
{
	return PE32;
}

DATA_DIRECTORY * PEFile32::getBaseDataDirectory()
{
	return ((OPTIONAL_HEADER_32*)standartOptionalHeader)->DataDirectory;
}

uint32_t PEFile32::getNumberOfRvaAndSizes()
{
	return ((OPTIONAL_HEADER_32*)standartOptionalHeader)->numberOfRvaAndSizes;
}

uint64_t PEFile32::getPreferedImageBase()
{
	return ((OPTIONAL_HEADER_32*)standartOptionalHeader)->imageBase;
}

uint32_t PEFile32::getSizeOfImage()
{
	return ((OPTIONAL_HEADER_32*)standartOptionalHeader)->sizeOfImage;
}

uint32_t PEFile32::getSizeOfHeaders()
{
	return ((OPTIONAL_HEADER_32*)standartOptionalHeader)->sizeOfHeaders;
}

void PEFile32::destroyImports()
{
	for (auto entry : Imports) {
		for (auto vecEntry : entry.second) {
			delete vecEntry;
		}
	}
}
