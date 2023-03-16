#include <filesystem>
#include <iostream>
#include <fstream>

//Acronym (Print Debug Messages)
//#define PDM

//Acronym (Fake command line arguments)
//#define FCLA

#define FILES_COUNT 1

unsigned int countLOC(char* string, unsigned int length)
{
	//There is no newline character on the last line of a file.
	int count = 1;
	for (int i = 0; i < length; ++i)
	{
		if (string[i] == '\n')
			++count;
	}
	return count;
}

int main(int argc, char** argv)
{
#ifdef FCLA
	// Fake command line arguments
	char** rAV = argv;
	int rAC = argc;
	argc = 3;
	argv = new char* [3];
	argv[0] = rAV[0];
	argv[1] = new char[]{ '-', 'f', 0 };
	argv[2] = new char[]{ 's', 'r', 'c', '/', 'm', 'a', 'i', 'n', '.', 'c', 'p', 'p', 0 };
#endif

#ifdef PDM
	std::cout << "Arguments List:\n[\"";
	if (argc > 0)
		std::cout << argv[1];
	for (int i = 2; i < argc; ++i)
		std::cout << "\", \"" << argv[i];
	std::cout << "\"]\n";
#endif

	int * flagsPerArguments = new int[argc-1]{};
	int nFlags = 0;
	int* dataArgumentIndexes = new int[argc - 1]{};
	int nData = 0;

	//Find flags and arguments
	for(int i = 1; i < argc; ++i)
		if (argv[i][0] == '-')
		{
			for (int j = 1; argv[i][j] != 0; j++)
			{
				++flagsPerArguments[i];
				++nFlags;
			}
		}
		else
		{
			dataArgumentIndexes[nData] = i;
			++nData;
		}

	//Compact flags into single char list and counter (nFlags)
	char* flags = new char[nFlags];
	{
		int count = 0;
		for (int i = 0; i < argc - 1; ++i)
			for (int j = 0; j < flagsPerArguments[i]; ++j)
			{
				flags[count] = argv[i][j + 1];
				++count;
			}
	}
	delete[] flagsPerArguments;

	//Extract data into string list
	char** data = new char* [nData];
	for (int i = 0; i < nData; ++i)
		data[i] = argv[dataArgumentIndexes[i]];
	delete[] dataArgumentIndexes;

#ifdef PDM
	std::cout << "\nFlags List:\n[";
	if (nFlags > 0)
		std::cout << flags[0];
	for (int i = 1; i < nFlags; ++i)
		std::cout << ", " << flags[i];
	std::cout << "]\n";
	std::cout << "\nData list:\n";
	for (int i = 0; i < nData; ++i)
		std::cout << data[i] << '\n';
#endif
	int operation = 0;
	bool operationSet = false;

	if (nData != 1)
	{
		std::cout << "\nError: Only one file is supported at the moment!\n";
		goto end;
	}

	for (int i = 0; i < nFlags; ++i)
			switch (flags[i])
			{
				case 'f':
					if (operationSet)
					{
						std::cout << "\nError: Multiple operations set!\n";
						goto end;
					}
					operation = FILES_COUNT;
					operationSet = true;
					break;
			}

	if (operation == FILES_COUNT)
	{
		std::ifstream fs(data[0], std::ios::in);
		if (!fs)
		{
			std::cout << "Error: Invalid file path specified!\n";
			fs.close();
			goto end;
		}

#ifdef PDM
		std::cout << "\n" << data[0] << " file loaded...\n";
#endif

		fs.seekg(0,fs.end);
		unsigned int length = fs.tellg();
		fs.seekg(0, fs.beg);

		char* fileContents = new char[length];
		fs.read(fileContents, length);
		fs.close();
		
		std::cout << "\nLOC: " << countLOC(fileContents, length);

		delete[] fileContents;
	}


end:

#ifdef FCLA
	//Cleanup fake command line arguments
	delete[] argv[1];
	delete[] argv[2];
	delete[] argv;
	argv = rAV;
	argc = rAC;
#endif

	delete[] flags;
	delete[] data;
	return 0;
}