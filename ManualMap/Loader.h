#pragma once
#include <Windows.h>
#include <cstdio>

#define PATH_DELIMITER '\\'

int getSizeOfFile(FILE * file);

//Read file , return size and file as argument
void* loadFile(const char* file_path, int &size);

//Deallocation needed
char* GetFileNameFromPath(const char* path, int size);

/*Removes the executable name from the end of the path
Example:   C:\Users\ConsoleApplication2.exe -> C:\Users\
*/
bool stripExecutableFromPath(char* path);

bool appendFileToPath(char* path, int path_size, char* file);