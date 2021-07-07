#pragma once
#include "PeFile.h"


class PEFile32 : public PEFile {

public:
	PEFile32();
	~PEFile32();

	void			 setImports() override;
	short			 getArchiteture() override;

	DATA_DIRECTORY * getBaseDataDirectory() override;
	uint32_t		 getNumberOfRvaAndSizes() override;
	uint64_t		 getPreferedImageBase() override;
	uint32_t		 getSizeOfImage() override;
	uint32_t		 getSizeOfHeaders() override;

private:
	void destroyImports();
};