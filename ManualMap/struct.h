#pragma once
#include <stdint.h>
#include <Windows.h>

//DOS_HEADER
typedef struct{
	uint16_t signature;
	uint16_t e_cblp;
	uint16_t e_cp;
	uint16_t e_crlc;
	uint16_t e_cparhdr;
	uint16_t e_minalloc;
	uint16_t e_maxalloc;
	uint16_t e_ss;
	uint16_t e_sp;
	uint16_t e_csum;
	uint16_t e_ip;
	uint16_t e_cs;
	uint16_t e_lfarlc;
	uint16_t e_ovno;
	uint16_t e_res[4];
	uint16_t e_oemid;
	uint16_t e_oeminfo;
	uint16_t e_res2[10];
	uint32_t e_lfanew;					// Offset to PE Header
}DOS_HEADER;

//COFF_HEADER
typedef struct{
	uint32_t signature;
	uint16_t machine;					//Architeture
	uint16_t numberOfSections;
	uint32_t timeDateStamp;
	uint32_t pointerToSymbolTable;
	uint32_t numberOfSymbols;
	uint16_t sizeOfOptionalHeader;
	uint16_t characteristics;
}COFF_HEADER;

typedef struct{
	uint32_t virtualAddress;
	uint32_t size;
}DATA_DIRECTORY;

//OPTIONAL HEADERS
typedef struct{
    uint16_t magic;				//decimal number 267 for 32 bit, 523 for 64 bit, and 263 for a ROM image. 
	uint8_t majorLinkerVersion;
	uint8_t minorLinkerVersion;
	uint32_t sizeOfCode;
	uint32_t sizeOfInitializedData;
	uint32_t sizeOfUninitializedData;
	uint32_t addressOfEntryPoint;		
	uint32_t baseOfCode;
}STANDART_OPTIONAL_HEADER;


typedef struct{	
    STANDART_OPTIONAL_HEADER standart;
	uint32_t baseOfData;
	uint32_t imageBase;
	uint32_t sectionAlignment;
	uint32_t fileAlignment;
	uint16_t majorOSVersion;
	uint16_t minorOSVersion;
	uint16_t majorImageVersion;
	uint16_t minorImageVersion;
	uint16_t majorSubsystemVersion;
	uint16_t minorSubsystemVersion;
	uint32_t win32VersionValue;
	uint32_t sizeOfImage;
	uint32_t sizeOfHeaders;
	uint32_t checksum;
	uint16_t subsystem;
	uint16_t dllCharacteristics;
	uint32_t sizeOfStackReserve;
	uint32_t sizeOfStackCommit;
	uint32_t sizeOfHeapReserve;
	uint32_t sizeOfHeapCommit;
	uint32_t loaderFlags;
	uint32_t numberOfRvaAndSizes;			//size of data_directory_array
	DATA_DIRECTORY DataDirectory[0];			//Can have any number of elements, matching the number in NumberOfRvaAndSizes.
}OPTIONAL_HEADER_32;




typedef struct{
	STANDART_OPTIONAL_HEADER standart;
	uint64_t imageBase;
	uint32_t sectionAlignment;
	uint32_t fileAlignment;
	uint16_t majorOSVersion;
	uint16_t minorOSVersion;
	uint16_t majorImageVersion;
	uint16_t minorImageVersion;
	uint16_t majorSubsystemVersion;
	uint16_t minorSubsystemVersion;
	uint32_t win32VersionValue;
	uint32_t sizeOfImage;
	uint32_t sizeOfHeaders;
	uint32_t checksum;
	uint16_t subsystem;
	uint16_t dllCharacteristics;
	uint64_t sizeOfStackReserve;
	uint64_t sizeOfStackCommit;
	uint64_t sizeOfHeapReserve;
	uint64_t sizeOfHeapCommit;
	uint32_t loaderFlags;
	uint32_t numberOfRvaAndSizes;	
	DATA_DIRECTORY DataDirectory[0];
}OPTIONAL_HEADER_64;


typedef struct{
	char name[8];
	uint32_t virtualSize;
	uint32_t virtualAddress;
	uint32_t sizeOfRawData;
	uint32_t pointerToRawData;
	uint32_t pointerToRelocations;
	uint32_t pointerToLineNumbers;
	uint16_t numberOfRelocations;
	uint16_t numberOfLineNumbers;
	uint32_t Characteristics;
}SECTION_HEADER;


//to apply fix real address needs to be calculated by adding ImageBase + virtualAddress + offset
typedef struct {
	uint32_t virtualAddress;	//RVA
	uint32_t size;
	uint16_t* typeOffset;		//high 4 bits for type, remaining 12 for offset
}BASE_RELOCATION;

#define RELOC_ENTRY_TYPE(word) (word >> 12)
#define RELOC_ENTRY_OFFSET(word) (word & 0xFFF)


//IMPORT TABLES


typedef struct {
	uint32_t importLookupTable;	//Relative address to a lookup table
	uint32_t timeStamp;
	uint32_t fowarderChain;
	uint32_t name;				//Relative adress to the name of DLL
	uint32_t importAdressTable;	//Relative address to adress table
}IMPORT_DIRECTORY_TABLE;	//From DataDirectory


template <typename T>
struct IMPORT_LOOKUP_TABLE {
	union {
		T flag;		//1st bit masked as 0x80000000 for 32bits, 0x8000000000000000 for 64bits
		T ordinal;	//Ordinal to function	31 bits, 63 bits
		T hintName;	//Relative address to HintName structure
	}Entry;
};

#define LOOKUP_IS_BY_ORDINAL_32(lookup) (lookup & 0x80000000)
#define LOOKUP_IS_BY_ORDINAL_64(lookup) (lookup & 0x8000000000000000)

#define LOOKUP_VALUE_32(lookup) (lookup & 0x7FFFFFFF)
#define LOOKUP_VALUE_64(lookup) (lookup & 0x7FFFFFFFFFFFFFF)


//ADDRESS TABLE needs to be overwitten by the loader

template <typename T>
struct IMPORT_ADDRESS_TABLE {
	T virtualAddress; //absolute address
};

typedef struct {
	uint16_t Hint; //Index into the Export Name Pointer Table. A match is attempted first with this value. If it fails, a binary search should performed on the DLL Export Name Pointer Table.
	char Name[];   //Name of the import, must be matched with the public name of the dll
					//padding possible
}HINT_NAME_TABLE;



//EXPORT

typedef struct {
	uint32_t exportFlags;
	uint32_t timStamp;
	uint16_t majorVersion;
	uint16_t minorVersion;
	uint32_t nameRVA;					//Name of DLL
	uint32_t ordinalBase;				//Start Ordinal
	uint32_t numberOfFunctionEntrys;	//Number of entries of export adress table
	uint32_t numberOfNamePointers;		//Number of name pointer in name table and number in ordinal table
	uint32_t exportAdressTableRVA;		//exportAdressTable
	uint32_t namePointerRVA;			//NamePointer
	uint32_t ordinalTableRVA;			//OrdinalTable
}EXPORT_DIRECTORY_TABLE;
