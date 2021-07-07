#include "PEFile64.h"
#include "Tracer.h"
#include "constants.h"

PEFile64::PEFile64()
{

}

PEFile64::~PEFile64()
{
	destroyImports();
}

void PEFile64::setImports()
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
	for (int i = 0; directoryTable[i].name; i++) {
		std::vector<ImportEntry*> &entry = Imports.insert(std::make_pair((char*)getPointerBasedOnRVA(directoryTable[i].name), std::vector<ImportEntry*>())).first->second;

		IMPORT_LOOKUP_TABLE<uint64_t>* lookup = (IMPORT_LOOKUP_TABLE<uint64_t>*)getPointerBasedOnRVA(directoryTable[i].importLookupTable);
		IMPORT_ADDRESS_TABLE<uint64_t>* iat = (IMPORT_ADDRESS_TABLE<uint64_t>*)getPointerBasedOnRVA(directoryTable[i].importAdressTable);
;
		for (int j = 0; lookup[j].Entry.flag; j++) {
			entry.push_back(new _ImportEntry<uint32_t>(&(lookup[j]), &(iat[j]), getLoadedImageBase()));
		}
	}
}

short PEFile64::getArchiteture()
{
	return PE32_PLUS;
}

DATA_DIRECTORY * PEFile64::getBaseDataDirectory()
{
	return ((OPTIONAL_HEADER_64*)standartOptionalHeader)->DataDirectory;
}

uint32_t PEFile64::getNumberOfRvaAndSizes()
{
	return ((OPTIONAL_HEADER_64*)standartOptionalHeader)->numberOfRvaAndSizes;
}

uint64_t PEFile64::getPreferedImageBase()
{
	return ((OPTIONAL_HEADER_64*)standartOptionalHeader)->imageBase;
}

uint32_t PEFile64::getSizeOfImage()
{
	return ((OPTIONAL_HEADER_64*)standartOptionalHeader)->sizeOfImage;
}

uint32_t PEFile64::getSizeOfHeaders()
{
	return ((OPTIONAL_HEADER_64*)standartOptionalHeader)->sizeOfHeaders;
}

void PEFile64::destroyImports()
{
	for (auto entry : Imports) {
		for (auto vecEntry : entry.second) {
			delete vecEntry;
		}
	}
}