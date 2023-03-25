#ifdef _WIN32
#include "fileio.h"
#include <windows.h>
#include <iostream>
#pragma comment(lib, "User32.lib")
#define uint unsigned int

//Gets the file path length or the max win32 path length, whichever is smaller.
static uint getFilePathLength(const char* path)
{
	uint pathLength = 0;
	while (pathLength < MAX_PATH && path[pathLength] != '\0')
		++pathLength;

	//Count the null-terminator and return
	return pathLength + 1;
}

static DWORD getFileContents(const char * path, char * data, UINT64 filesize)
{
	//Open the file
	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//TODO: Error Message
	if (file == INVALID_HANDLE_VALUE)
		return 0;

	//Read the file
	//TODO: Error Message
	DWORD actualBytesRead;
	if (FALSE == ReadFile(file, data, filesize, &actualBytesRead, 0))
	{
		CloseHandle(file);
		return 0;
	}
	CloseHandle(file);

	return actualBytesRead;
}

File loadFile(const char* path)
{
	//TODO: Error Message
	if (getFilePathLength(path) > MAX_PATH)
		return File{0,0};

	WIN32_FIND_DATAA ffd;
	HANDLE search = INVALID_HANDLE_VALUE;
	search = FindFirstFileA(path, &ffd);

	//TODO: Error Message
	if (search == INVALID_HANDLE_VALUE)
		return File{0,0};
	FindClose(search);

	//TODO: Error Message
	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return File{0,0};

	//Create our File using the filesize information collected
	UINT64 filesize = ((UINT64)ffd.nFileSizeHigh < 32) | ffd.nFileSizeLow;
	File file{(char*)malloc(filesize)};

	file.count = getFileContents(path, file.data, filesize);
	
	return file;
}

static FileList loadFilesRecursively(const char* path)
{
	return FileList{0,0};
}

//Avoid too many malloc calls version
FileList loadFiles(const char* path, bool recursive)
{
	struct Node
	{
		Node* next;
		char* path;
		UINT64 filesize;
	};
	//DO NOT ADD DUD FILES TO THE FILELIST THAT YOU RETURN
	if (recursive)
		return loadFilesRecursively(path);

	//TODO: Error Message
	uint pathLength = getFilePathLength(path);
	if (pathLength + 3 > MAX_PATH || pathLength == 0)
		return FileList{ 0,0 };

	//Append "/*" to the path
	char searchPath[MAX_PATH];
	memcpy(searchPath, path, pathLength);
	memcpy(searchPath + pathLength - 1, "/*", 3);

	WIN32_FIND_DATAA ffd;
	HANDLE search = INVALID_HANDLE_VALUE;
	search = FindFirstFileA(searchPath, &ffd);

	//TODO: Error Message
	if (search == INVALID_HANDLE_VALUE)
		return FileList{ 0,0 };

	//Iterate over directory to get file paths and nFiles
	Node head;
	head.next = (Node*)malloc(sizeof(Node));
	Node* tail = head.next;
	tail->next = 0;
	int nFiles = 0;
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		tail->path = (char*)malloc(MAX_PATH);
		
		//does not copy \0
		memcpy(tail->path, searchPath, pathLength);
		//does copy \0
		strcpy_s(tail->path + pathLength ,MAX_PATH - pathLength, ffd.cFileName);

		tail->filesize = ((UINT64)ffd.nFileSizeHigh < 32) | ffd.nFileSizeLow;

		tail->next = (Node*)malloc(sizeof(Node));
		tail->next->next = 0;
		tail = tail->next;
		++nFiles;

	} while (FindNextFileA(search, &ffd));

	if (GetLastError() != ERROR_NO_MORE_FILES)
		printf("Unknown Error encountered while iterating over directory files");
		 
	FindClose(search);

	FileList list{(File*)malloc(sizeof(File)*nFiles),0};
	tail = head.next;
	do
	{
		list.files[list.count].data = (char*)malloc(tail->filesize);
		list.files[list.count].count =
			getFileContents(tail->path, list.files[list.count].data, tail->filesize);
		if (list.files[list.count].count > 0)
			++list.count;
		else
		{
			free(list.files[list.count].data);
		}
		Node* prev = tail;
		tail = tail->next;
		free(prev->path);
		free(prev);
	} while (tail->next != 0);

	free(tail);

	return list;
}

void unloadFile(File& file)
{
	free(file.data);
}

void unloadFileList(FileList& fileList)
{
	for (int i = 0; i < fileList.count; ++i)
		free(fileList.files[i].data);
	free(fileList.files);
}

#endif