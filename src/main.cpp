#include <iostream>
#include <fstream>
#include <cmath>

//Acronym (Print Debug Messages)
//#define PDM

//Acronym (Fake command line arguments)
//#define FCLA

//Operations
#define FILES_COUNT 1
#define DIRECTORIES_COUNT 2

struct Measurements
{
	unsigned int characters = 0;
	unsigned int lines = 0;
	unsigned int lineComments = 0;
	unsigned int inlineComments = 0;
	unsigned int blankLines = 0;
	unsigned int semicolons = 0;
	unsigned int nonCodeLines = 0;
};

Measurements performMeasurements(char* string, unsigned int length)
{
	Measurements m;
	
	bool isNonCodeLine = true;
	for (unsigned int i = 0; i < length; ++i)
	{
		//Count comments
		if(string[i] == '/')
		{
			//Line Comments
			if (string[i + 1] == '/')
			{
				bool isComment = true;
				int j = i + 2;
				for (; j < length; ++j)
					if (string[j] == '\n')
						break;
				m.characters+=j-i;
				i = j;
				++m.lineComments;
			}
			//In-line Comments
			else if(string[i+1] == '*')
			{
				int j = i + 2;
				for(;j < length; ++j)
				{
					if(string[j] == '*' && string[j+1] == '/') /* Some Stuff */
						break;
					if(string[j] == '\n')
					{
						++m.lines;
						for(int j = i - 1; j > 0; --j)
						{
							if(string[j] == '\n')
							{
								++m.blankLines;
								break;
							}
							if(string[j]!=' ' && string[j]!='\t')
								break;
						}
					}
				}
				m.characters+=j-i;
				i=j;
				++m.inlineComments;
			}

		}

		//Count newline characters
		if (string[i] == '\n')
		{
			++m.lines;
			if(isNonCodeLine)
				++m.nonCodeLines;
			isNonCodeLine = true;
			//Count blank lines
			for(int j = i - 1; j > 0; --j)
			{
				if(string[j] == '\n')
				{
					++m.blankLines;
					break;
				}
				if(string[j]!=' ' && string[j]!='\t')
					break;
			}
		}

		//Set isNonCodeLine flag
		if(string[i]!= ' ' && string[i]!='\t' && string[i]!='\n')
			isNonCodeLine = false;

		//Count Semicolons
		if(string[i] == ';')
			++m.semicolons;

		//Count all characters
		++m.characters;
	}
	//Account for the last line that has no newline character
	++m.lines;
	return m;
}

void printMeasurements(Measurements m)
{
	int maxIntLen = (unsigned int)log10(m.characters) + 1;

	auto printNumber = [maxIntLen](unsigned int number)
	{
		unsigned int spaces = maxIntLen - ((unsigned int)log10(number)+1);
		std::cout << number;
		for (int i = 0; i < spaces; ++i)
			std::cout << ' ';
	};

	auto printSpaces = [](int n)
	{
		for (int i = 0; i < n; ++i)
			std::cout << ' ';
	};

	int rowSpace = 80 - (20 + 24 + maxIntLen * 2 + 4);
	int lines = m.lines;
	int comments = m.inlineComments + m.lineComments;
	int sloc = lines - m.nonCodeLines;

	std::cout << '\n';
	std::cout << "   +------------------------------------------------------------------------+   \n";
	std::cout << "   |                                                                        |   \n";
	std::cout << "   |                       Lochy The Monster Counter                        |   \n";
	std::cout << "   |                                                                        |   \n";
	std::cout << "   +------------------------------------------------------------------------+   \n";

	std::cout << "   | Characters   : "; 
	printNumber(m.characters);
	printSpaces(rowSpace);
	std::cout << "Lines Of Code         : ";
	printNumber(lines);
	std::cout << "|   \n";

	std::cout << "   +------------------------------------------------------------------------+   \n";

	std::cout << "   | Comments     : ";
	printNumber(comments);
	printSpaces(rowSpace);
	std::cout << "Source Lines Of Code  : ";
	printNumber(sloc);
	std::cout << "|   \n";

	std::cout << "   +------------------------------------------------------------------------+   \n";

	std::cout << "   | Semi-colons  : ";
	printNumber(m.semicolons);
	printSpaces(rowSpace);
	std::cout << "Logical Lines Of Code : ";
	printNumber(0);
	std::cout << "|   \n";

	std::cout << "   +------------------------------------------------------------------------+   \n";

	std::cout << "   | Loops        : ";
	printNumber(0);
	printSpaces(rowSpace);
	std::cout << "Non-code Lines        : ";
	printNumber(m.nonCodeLines);
	std::cout << "|   \n";

	std::cout << "   +------------------------------------------------------------------------+   \n";
	
	std::cout << "   | Conditionals : ";
	printNumber(0);
	printSpaces(rowSpace);
	std::cout << "Blank Lines           : ";
	printNumber(m.blankLines);
	std::cout << "|   \n";


	std::cout << "   +------------------------------------------------------------------------+   \n";
}

template <class T>
void printCommaSeperatedList(T* items, unsigned int count)
{
	std::cout << "[";
	
	if (count > 0)
		std::cout << items[0];

	for (int i = 1; i < count; ++i)
		std::cout << ", " << items[i];

	std::cout << ']';
}

template <class T>
void printCommaSeperatedListWithQuotes(T* items, unsigned int count)
{
	std::cout << "[";

	if (count > 0)
		std::cout << '"' << items[0];

	for (int i = 1; i < count; ++i)
		std::cout << "\", \"" << items[i] << '"';

	std::cout << ']';
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
	std::cout << "Arguments List:\n";
	printCommaSeperatedListWithQuotes(argv + 1, argc - 1);
	std::cout << "\n";
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
	std::cout << "\nFlags List:\n";
	printCommaSeperatedList(flags, nFlags);
	std::cout << "\n";
	std::cout << "\nData list:\n";
	printCommaSeperatedList(data, nFlags);
	std::cout << "\n";
#endif
	int operation = FILES_COUNT;
	bool operationSet = false;

	for (int i = 0; i < nFlags; ++i)
			switch (flags[i])
			{
				case 'd':
					if (operationSet)
					{
						std::cout << "\nError: Multiple operations set!\n";
						goto end;
					}
					operation = DIRECTORIES_COUNT;
					operationSet = true;
					break;
			}

	if (operation == FILES_COUNT)
	{
		Measurements m;
		unsigned int* pM = reinterpret_cast<unsigned int*>(&m);
		for (int i = 0; i < nData; ++i)
		{
			std::ifstream fs(data[i], std::ios::in);
			if (!fs)
			{
				std::cout << "Error: Invalid file path specified!\n";
				fs.close();
				goto end;
			}

#ifdef PDM
			std::cout << "\n" << data[i] << " file loaded successfully...\n\n";
#endif

			fs.seekg(0, fs.end);
			unsigned int length = fs.tellg();
			fs.seekg(0, fs.beg);

			char* fileContents = new char[length];
			fs.read(fileContents, length);
			fs.close();

			Measurements temp = performMeasurements(fileContents, length);

			unsigned int* pTemp = reinterpret_cast<unsigned int*>(&temp);
			unsigned int nInts = sizeof(Measurements) / 4;
			for (int i = 0; i < nInts; ++i)
				pM[i] += pTemp[i];


			delete[] fileContents;
		}
		printMeasurements(m);
	}
	else if (operation == DIRECTORIES_COUNT)
		std::cout << "\nError: Directories not currently supported!\n";


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