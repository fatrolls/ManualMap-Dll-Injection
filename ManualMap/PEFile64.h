#pragma once
#include "PeFile.h"


class PEFile64 : public PEFile {

public:
	PEFile64();
	~PEFile64();

	void				setImports() override;
	short				getArchiteture() override;

	DATA_DIRECTORY*		getBaseDataDirectory() override;
	uint32_t			getNumberOfRvaAndSizes() override;
	uint64_t			getPreferedImageBase() override;
	uint32_t			getSizeOfImage() override;
	uint32_t			getSizeOfHeaders() override;

private:
	void destroyImports();
};