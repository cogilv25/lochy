#ifdef _WIN32
#ifndef LEON_FILEIO_C
#define LEON_FILEIO_C


#include "fileio.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FATAL_ERROR_IF_VAR_NULL(var, message)\
if(var==0)\
	{\
		printf("Fatal Error :%s:%d: %s\n", __FILE__, __LINE__, message);\
		exit(-1);\
	}

void reverse_memcpy(void* dest, const void* src, size_t count)
{
	char* d = dest;
	const char* s = src;
	for(size_t x = 0; x < count; x++)
	{
		d[count-x-1] = s[count-x-1];
	}
}

void file_properties_list_debug_print(file_properties_list* fpl)
{
	printf("\nPrinting file_properties_list!\n");
	printf("----------------------------------------------------------\n");
	printf("Count: %u\n", fpl->count);
	printf("Appended Memory Length: %u\n", fpl->appended_memory_length);
	printf("Allocation Size: %u\n", _msize(fpl));
	printf("----------------------------------------------------------\n");
	for(uint i = 0; i < fpl->count; i++)
	{
		printf("File Properties %u:\n", i);
		printf("File Name: %s\n",fpl->properties[i].path);
		printf("File Size: %llu\n", fpl->properties[i].filesize);
		printf("----------------------------------------------------------\n");
	}
}

file_properties* get_file_properties(const char * path)
{

	uint path_length = strlen(path);
	file_properties* props = malloc(sizeof(file_properties) + path_length + 1);
	props->filesize = 0;
	props->path = (char*)(&path + 1);

	if (path_length > MAX_PATH || path_length < 1)
	{
		printf("Error: Path longer than max path!\n");
		return props;
	}

	WIN32_FIND_DATAA ffd;
	HANDLE search = INVALID_HANDLE_VALUE;
	search = FindFirstFileA(path, &ffd);

	if (search == INVALID_HANDLE_VALUE)
	{
		printf("Error: File does not exist!\n");
		return props;
	}

	props->filesize = ((uint64)ffd.nFileSizeHigh < 32) | ffd.nFileSizeLow;

	return props;
}

file_properties_list* file_properties_list_add_file_priorities(file_properties_list* fpl, uint64 filesize, const char * path, uint path_length)
{
	//Calculate offsets and sizes
	uint old_appended_memory_offset = (2*sizeof(uint)) + ((fpl->count)*sizeof(file_properties));
	uint new_appended_memory_offset = old_appended_memory_offset + sizeof(file_properties);
	uint new_path_append_offset = new_appended_memory_offset + fpl->appended_memory_length;
	uint new_appended_memory_length = fpl->appended_memory_length + path_length;

	//Keep address of old_appended_memory if realloc moves it.
	uchar* old_appended_memory_address = (uchar*)fpl + old_appended_memory_offset;

	//Expand allocated memory block to required size
	uint new_size = new_appended_memory_offset + new_appended_memory_length;
	fpl = realloc(fpl, new_size);

	//If the realloc fails we cannot proceed!
	FATAL_ERROR_IF_VAR_NULL(fpl, "Realloc Failed!")

	//Calculate addresses
	//Must be done after realloc or you will use nonsense old addresses when
	// moving data about!
	uchar* new_appended_memory_address = (uchar*)fpl + new_appended_memory_offset;
	uchar* new_path_append_address = (uchar*)fpl + new_path_append_offset;

	//Move appended memory (paths of files as null-terminated strings)
	// forward 16bytes to accomodate the new file_properties entry. We do this
	// in reverse order as the memory overlaps. 
	reverse_memcpy(new_appended_memory_address, new_appended_memory_address-sizeof(file_properties),
		fpl->appended_memory_length);

	//Update path pointers in existing file_properties entries. They have all
	// been shifted left 16 bytes and if realloc has moved us in memory we have
	// to take that into account as well.
	for (uint i = 0; i < fpl->count; i++)
	{
		fpl->properties[i].path = (char*)new_appended_memory_address +
			(fpl->properties[i].path - (char*)old_appended_memory_address);
	}

	//Now we can append our new path to the end of the appended memory
	memcpy(new_path_append_address, path, path_length);

	//Set the filesize and path pointer in the new file_properties entry.
	file_properties* props = &fpl->properties[fpl->count];
	props->path = (void*)new_path_append_address;
	props->filesize = filesize;

	fpl->appended_memory_length = new_appended_memory_length;
	fpl->count++;
	return fpl;
}


file_properties_list* get_file_properties_in_directory(const char * path)
{
	file_properties_list * list = calloc(2, sizeof(uint));

	uint dir_path_length = strlen(path);
	if (dir_path_length + 3 > MAX_PATH || dir_path_length < 1)
	{
		printf("Error: Path longer than max path!\n");
		return list;
	}

	//Append "/*" to the path
	char searchPath[MAX_PATH];
	memcpy(searchPath, path, dir_path_length);
	memcpy(searchPath + dir_path_length, "/*", 3);

	WIN32_FIND_DATAA ffd;
	HANDLE search = INVALID_HANDLE_VALUE;
	search = FindFirstFileA(searchPath, &ffd);

	if (search == INVALID_HANDLE_VALUE)
	{
		printf("Error: Folder does not exist!\n");
		return list;
	}

	do
	{
		//Ignore directories
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		// + 1 for the '\0'
		uint file_name_length = strlen(ffd.cFileName) + 1;
		// +1 for the '/'
		uint file_path_length = dir_path_length + 1 + file_name_length;

		uint64 filesize = ((uint64)ffd.nFileSizeHigh < 32) | ffd.nFileSizeLow;

		memcpy(searchPath+dir_path_length+1,ffd.cFileName,file_name_length);

		list = file_properties_list_add_file_priorities(list, filesize, searchPath, file_path_length);

	} while (FindNextFileA(search, &ffd));

	if (GetLastError() != ERROR_NO_MORE_FILES)
		printf("Unknown Error encountered while iterating over directory files");
		 
	FindClose(search);

	return list;
}

static file_properties_list* _get_file_properties_recursively(file_properties_list* list, const char * path)
{

	uint dir_path_length = strlen(path);
	if (dir_path_length + 3 > MAX_PATH || dir_path_length < 1)
	{
		printf("Error: Path longer than max path!\n");
		return list;
	}

	//Append "/*" to the path
	char searchPath[MAX_PATH];
	memcpy(searchPath, path, dir_path_length);
	memcpy(searchPath + dir_path_length, "/*", 3);

	WIN32_FIND_DATAA ffd;
	HANDLE search = INVALID_HANDLE_VALUE;
	search = FindFirstFileA(searchPath, &ffd);

	if (search == INVALID_HANDLE_VALUE)
	{
		printf("Error: Folder does not exist!\n");
		return list;
	}

	do
	{
		//Recursively search folders except "." and ".."
		if ((ffd.cFileName[0] == '.') && (ffd.cFileName[1] == '\0' || ffd.cFileName[1] == '.'))
			continue;

		// + 1 for the '\0'
		uint file_name_length = strlen(ffd.cFileName) + 1;
		// +1 for the '/'
		uint file_path_length = dir_path_length + 1 + file_name_length;

		memcpy(searchPath + dir_path_length + 1, ffd.cFileName, file_name_length);

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			list = _get_file_properties_recursively(list, searchPath);
			continue;
		}

		uint64 filesize = ((uint64)ffd.nFileSizeHigh < 32) | ffd.nFileSizeLow;
		list = file_properties_list_add_file_priorities(list, filesize, searchPath, file_path_length);

	} while (FindNextFileA(search, &ffd));

	if (GetLastError() != ERROR_NO_MORE_FILES)
		printf("Unknown Error encountered while iterating over directory files");
		 
	FindClose(search);

	return list;
}

file_properties_list* get_file_properties_recursively(const char * path)
{
	file_properties_list * list = calloc(2, sizeof(uint));
	return _get_file_properties_recursively(list, path);
}


void read_file_contents_to_buffer(const char * path, char* buffer, uint64 filesize)
{

	//Open the file
	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//TODO: Error Message
	if (file == INVALID_HANDLE_VALUE)
	{
		buffer[0] = '\0';
		return;
	}

	//Read the file
	//TODO: Error Message
	DWORD actualBytesRead;
	if (FALSE == ReadFile(file, buffer, filesize, &actualBytesRead, 0))
	{
		CloseHandle(file);
		buffer[0] = '\0';
		return;
	}
	CloseHandle(file);
}

string_list* get_all_file_contents(const file_properties_list* path_list)
{
	//Little lasty but we can't guarantee how much padding the compiler
	// will add to string_list...
	uint64 total_size = (sizeof(string_list) - sizeof(char*)) +
		sizeof(char*) * path_list->count;

	for(uint i = 0; i < path_list->count; i++)
		total_size += path_list->properties[i].filesize;

	string_list * list = malloc(total_size);

	list->count = path_list->count;
	char * p = (char*)&list->string_pointers[list->count];
	for(uint i =0; i < path_list->count; i++)
	{
		list->string_pointers[i] = p;
		read_file_contents_to_buffer(path_list->properties[i].path,
			p, path_list->properties[i].filesize);
		p += path_list->properties[i].filesize;
	}

	return list;
}

char* get_file_contents(file_properties* path)
{
	char * file_contents = malloc(sizeof(*file_contents) * path->filesize);
	read_file_contents_to_buffer(path->path, file_contents, path->filesize);
	return file_contents;
}

char* string_list_get_string(string_list* list, uint index)
{
	return list->string_pointers[index];
}

#endif
#endif