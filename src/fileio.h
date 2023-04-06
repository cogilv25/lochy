#ifndef LEON_FILEIO_H
#define LEON_FILEIO_H

#define uint unsigned int
#define uchar unsigned char
#define uint64 unsigned long long

typedef struct file_properties
{
	uint64 filesize;
	char* path;
} file_properties;

//Free with free
typedef struct file_properties_list
{
	uint count;
	uint appended_memory_length;
	file_properties properties[1];
} file_properties_list;

//Free with free
typedef struct string_list
{
	uint count;
	char* string_pointers[1];
} string_list;

file_properties_list* get_file_properties_in_directory(const char * path);

file_properties_list* get_file_properties_recursively(const char * path);

file_properties* get_file_properties(const char * path);

string_list* get_all_file_contents(const file_properties_list* property_list);

char* get_file_contents(file_properties* properties);

char* string_list_get_string(string_list*, uint index);

void string_list_free(string_list* list);

void file_properties_list_debug_print(file_properties_list* fpl);

#endif