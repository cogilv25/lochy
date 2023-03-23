#pragma once

struct File
{
	char* data;
	unsigned int count;
};

struct FileList
{
	File* files;
	unsigned int count;
	const unsigned int capacity;
};

//unloadFile(File& file) must be called after calling this function!
File loadFile(const char* path);


//unloadFileList(FileList& filelist) must be called after calling this function!
FileList loadFiles(const char* path, bool recursive);

void unloadFile(File& file);
void unloadFileList(FileList& filelist);