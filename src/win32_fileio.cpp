#ifdef _WIN32
#include "fileio.h"
#include <windows.h>
#include <iostream>
#pragma comment(lib, "User32.lib")
#define uint unsigned int

//Gets the file path length or the max win32 path length, whichever is smaller.
uint getFilePathLength(const char* path)
{
	uint pathLength = 0;
	while (pathLength < MAX_PATH && path[pathLength] != '\0')
		++pathLength;

	//Count the null-terminator and return
	return pathLength + 1;
}

File loadFile(const char* path)
{
	//TODO: Error Message
	if (getFilePathLength(path) > MAX_PATH)
		return File{0,0};

	WIN32_FIND_DATAA ffd;
	HANDLE handle = INVALID_HANDLE_VALUE;
	handle = FindFirstFileA(path, &ffd);

	//TODO: Error Message
	if (handle == INVALID_HANDLE_VALUE)
		return File{0,0};
	FindClose(handle);

	//TODO: Error Message
	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return File{0,0};

	//Create our File using the filesize information collected
	UINT64 filesize = ((UINT64)ffd.nFileSizeHigh < 32) | ffd.nFileSizeLow;
	File file{(char*)malloc(filesize), filesize};

	//Open the file
	handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//TODO: Error Message
	if (handle == INVALID_HANDLE_VALUE)
	{
		free(file.data);
		return File{0,0};
	}

	//Read the file
	//TODO: Error Message
	DWORD actualBytesRead;
	if (FALSE == ReadFile(handle, file.data, file.count, &actualBytesRead, 0))
	{
		free(file.data);
		CloseHandle(handle);
		return File{0,0};
	}

	file.count = actualBytesRead;

	CloseHandle(handle);
	return file;
}

FileList loadFiles(const char* path, bool recursive)
{
	FileList list{(File*)malloc(sizeof(File)*4),0,4};

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

bool openDirectory(const char* path)
{
	char dirPath[MAX_PATH];
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD error = 0;

	UINT pathLength = 0;
	while (pathLength < MAX_PATH && path[pathLength] != '\0')
		++pathLength;
	++pathLength;
	if (pathLength + 2 > MAX_PATH)
	{
		std::cout << "\nError: Path is too long!\n";
		return -1;
	}

	memcpy(dirPath, path, pathLength);
	memcpy(dirPath + pathLength - 1, "/*", 3);

	hFind = FindFirstFileA(dirPath, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		std::cout << "\nError: Directory doesn't exist...\n";
		return -1;
	}
	FindClose(hFind);
}

#endif