#pragma once
#include "PEFileExportImport.h"
#include <vector>
#include <map>

struct cmp_str
{
	bool operator()(char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

typedef std::map<char*, std::vector<ImportEntry*>, cmp_str> ImportsMap;
typedef std::vector<ExportEntry> ExportsMap;



class PEFile {
public:
	PEFile();
	~PEFile();

private:
	bool						setHeaders(void *filePointer);
	void						destroy();
	void						setExports();
	static PEFile*				CreatePEFile(void* filePointer);					//needs to be deallocated
	void						mapFile();											//Do a copy of the file and map it to memory

protected:
	virtual void				setImports() = 0;

public:
	//INTIALIZATION
	static PEFile*				mapViewOfFile(char* path);
	static PEFile*				mapViewOfFile(void* filePointer);
	static PEFile*				loadMappedFile(void* filePointer);
	void						initialize(void* filePointer,bool isMapped);		//Initialize the file

	//UTILLS
	void*						getPointerBasedOnRVA(int offset);
	void*						getAbsAddress(void* adress, int offset);
	void*						getLoadedImageBase();

	virtual short				getArchiteture() = 0;				//Return PE32 for 32 bits or PE32_PLUS for 64 bits else return 0

	//HEADER GATHERING
	DOS_HEADER*					getDosHeader();
	COFF_HEADER*				getCoffHeader();
	STANDART_OPTIONAL_HEADER*	getStandartOptionalHeader();
	SECTION_HEADER*				getSectionHeader();

	//OptionalHeader
	uint64_t					getEntryPointRVA();
	DATA_DIRECTORY*				getSpecificDataDirectory(int index);
	virtual DATA_DIRECTORY*		getBaseDataDirectory() = 0;
	virtual uint32_t			getNumberOfRvaAndSizes() = 0;
	virtual uint64_t			getPreferedImageBase() = 0;
	virtual uint32_t			getSizeOfImage() = 0;
	virtual uint32_t			getSizeOfHeaders() = 0;

	//Tables
	EXPORT_DIRECTORY_TABLE*		getExportDirectoryTable();


	//Custom Maps for easy access -> maybe upgraded later to use lESS memory
	ExportsMap*					getExport();
	ImportsMap*					getImport();

	uint32_t					getExportFunctionRVA(char* name);
	uint32_t					getExportFunctionRVA(uint16_t ordinal);
	char*						getExportName();


public:
	static DOS_HEADER*					getDosHeader(void* filePointer);
	static COFF_HEADER*					getCoffHeader(DOS_HEADER* dosHeader);
	static SECTION_HEADER*				getSectionHeader(STANDART_OPTIONAL_HEADER* optionalHeader, COFF_HEADER* coffHeader);
	static STANDART_OPTIONAL_HEADER*	getStandartOptionalHeader(COFF_HEADER* coffHeader);

protected:
	DOS_HEADER*					dosHeader;
	COFF_HEADER*				coffHeader;
	STANDART_OPTIONAL_HEADER*	standartOptionalHeader;
	SECTION_HEADER*				sectionHeader;

	ExportsMap Exports;
	ImportsMap Imports;
};



