#pragma once
#include "struct.h"
#include "Tracer.h"

class ExportEntry {
public:

	//If is there any export without one of the entrys needed, just pass 0 or nothing
	//ExportEntry(void* ordinalTable, uint32_t ordinalOffset, void* nameTable, uint32_t nameOffset, void* functionTable, uint32_t functionOffset);
	ExportEntry(uint32_t* functionEntry, char * nameEntry);

	void	 setFunctionName(char* name);

	uint32_t getFunctionRVA();
	char*	 getName();				//if 0, export doesn't have a name
	//int32_t	 getOrdinal();		//if less then 0 export doesn't have a ordinal

private:
	uint32_t* functionRVA;
	char*	  name;
	//uint16_t* ordinal;
};

class ImportEntry {
public:
	virtual uint64_t			getImportAbsoluteAdress() = 0;
	virtual void				setImportAbsoluteAdress(uint64_t address) = 0;

	virtual bool				isByOrdinal() = 0;
	virtual uint32_t			getOrdinal() = 0;
	virtual char*				getFunctionName() = 0;
};

template <typename T>
class _ImportEntry : public ImportEntry {

public:
	_ImportEntry(void* lookupTableEntry, void* ImportAdressTableEntry, void* fileBaseAddress){
		LookupEntry = (IMPORT_LOOKUP_TABLE<T>*)(lookupTableEntry);
		IATEntry = (IMPORT_ADDRESS_TABLE<T>*)(ImportAdressTableEntry);
		this->fileBaseAddress = fileBaseAddress;
		HintName = getHintNameTable();
	}

	uint64_t	getImportAbsoluteAdress() override {return IATEntry->virtualAddress;}

	void		setImportAbsoluteAdress(uint64_t address) override {IATEntry->virtualAddress = address;}

	//LOOKUP
	bool		isByOrdinal() override {
		if (sizeof(T) == sizeof(uint32_t))
			return LOOKUP_IS_BY_ORDINAL_32(LookupEntry->Entry.flag);
		else if (sizeof(T) == sizeof(uint64_t))
			return LOOKUP_IS_BY_ORDINAL_64(LookupEntry->Entry.flag);

		throw std::exception("Error Import data is not 4 bytes or 8 bytes in size");
	}

	uint32_t	getOrdinal() override {
		if (!HintName) {
			return getLookupValue();
		}
		else {
			return (uint32_t)HintName->Hint;
		}
	}
	char*		getFunctionName() override {
		if (!HintName)
			throw std::exception("Import not by name");
		return HintName->Name;
	}

private:
	HINT_NAME_TABLE* getHintNameTable() {
		if (isByOrdinal())
			return nullptr;

		return (HINT_NAME_TABLE *)((uint64_t)getLookupValue() + (uint64_t)fileBaseAddress);
	}

	T getLookupValue() {
		if (sizeof(T) == sizeof(uint32_t))
			return LOOKUP_VALUE_32(LookupEntry->Entry.flag);
		else if (sizeof(T) == sizeof(uint64_t))
			return LOOKUP_VALUE_64(LookupEntry->Entry.flag);

		throw std::exception("Error Import data is not 4 bytes or 8 bytes in size");
	}

private:
	IMPORT_LOOKUP_TABLE<T>*		LookupEntry;
	IMPORT_ADDRESS_TABLE<T>*	IATEntry;
	HINT_NAME_TABLE*			HintName;
	void*						fileBaseAddress;
};