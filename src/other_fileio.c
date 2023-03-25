#ifndef _WIN32
#include "fileio.h"
#include <stdlib.h>
#include <stdio.h>

//TODO: test on linux, etc
File loadFile(const char* path)
{
	FILE *filePointer = fopen(path,"rb");
	if (filePointer == 0)
		return File{0,0};

	fseek(filePointer, 0L, SEEK_END);
	unsigned int filesize = ftell(filePointer);
	rewind(filePointer);

	File file{ (char*)malloc(filesize) };

	if (file.data == 0)
	{
		printf("\nInternal Error: Malloc failed!\n");
		return file;
	}

	file.count = fread(file.data, filesize, 1, filePointer);

	fclose(filePointer);
}

void unloadFile(File& file)
{
	free(file.data);
}

#endif