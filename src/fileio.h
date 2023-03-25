#pragma once

//If Created with loadFile.
//Always Destroy with unloadFile.
struct File
{
	char* data;
	unsigned long long count;
};

//If Created with loadFiles,
//Always Destroy with unloadFiles.
struct FileList
{
	File* files;
	unsigned int count;
};

//Destroy returned File with unloadFile!
File loadFile(const char* path);


//Destroy returned FileList with unloadFileList!
FileList loadFiles(const char* path, bool recursive);

void unloadFile(File& file);
void unloadFileList(FileList& filelist);