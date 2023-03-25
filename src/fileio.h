#pragma once
#include <stdbool.h>

//If Created with loadFile.
//Always Destroy with unloadFile.
typedef struct
{
	char* data;
	unsigned long long count;
} File;

//If Created with loadFiles,
//Always Destroy with unloadFiles.
typedef struct
{
	File* files;
	unsigned int count;
} FileList;

//Destroy returned File with unloadFile!
File loadFile(const char* path);


//Destroy returned FileList with unloadFileList!
FileList loadFiles(const char* path, bool recursive);

void unloadFile(File* file);
void unloadFileList(FileList* filelist);