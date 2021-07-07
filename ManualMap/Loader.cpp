#include "Loader.h"


int getSizeOfFile(FILE * file)
{
	int init_postion = ftell(file);
	int size = 0;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, init_postion, SEEK_SET);
	return size;
}

//Read file , return size and file as argument
void* loadFile(const char* file_path, int &size) {
	int dll_size = 0;
	void* file_pointer = 0;


	FILE* dll_file = fopen(file_path, "rb");
	if (dll_file == NULL) {
		return 0;
	}
	dll_size = getSizeOfFile(dll_file);

	if (dll_size == 0) {
		fclose(dll_file);
		return 0;
	}

	fseek(dll_file, 0, SEEK_SET);
	file_pointer = malloc(dll_size);
	if (fread(file_pointer, 1, dll_size, dll_file) != dll_size) {
		fclose(dll_file);
		return 0;
	}
	size = dll_size;

	fclose(dll_file);
	return file_pointer;
}

//Deallocation needed
char* GetFileNameFromPath(const char* path, int size) {
	//Path as to have a "\" as delimeter
	char* name = 0;
	for (int i = size - 1; i >= 0; i--) {
		if (path[i] == 7 || path[i] == 92) {	//'\'
			int size_name = size - i + 1;
			name = (char*)calloc(size_name, sizeof(char));
			memcpy(name, path + i + 1, size - i);
			return name;
		}
	}
	return  NULL;
}

bool stripExecutableFromPath(char * path)
{
	int len = strlen(path);
	for (int i = len; i >= 0; --i) {
		if (path[i] == PATH_DELIMITER)
			return true;
		path[i] = 0;
	}
	return false;
}

bool appendFileToPath(char * path, int path_size, char * file)
{
	int f = strlen(file);
	int i = strlen(path);
	for (int j = 0; j < f; i++, j++) {
		if (i > path_size)
			return false;
		path[i] = file[j];
	}
	return true;
}
