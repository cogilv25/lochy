#include <iostream>
#include <fstream>
#include <cmath>
#include "fileio.h"


//Acronym (Print Debug Messages)
//#define PDM

//Acronym (Fake command line arguments)
//#define FCLA

//Operations
#define FILES_COUNT 1
#define DIRECTORIES_COUNT 2


//Valid Characters Before a 'Statement' (for, while, if, switch)
#define VALID_POST_STATEMENT_CHARACTER(x) x == '\n' || x == '\t' \
|| x == ' ' || x == '('

//Additional Valid Characters Before a 'Statement'
#define VALID_PRE_STATEMENT_CHARACTER(x) VALID_POST_STATEMENT_CHARACTER(x) \
|| x == ')' || x == ';' || x == '{' || x == '}'

struct Measurements
{
	unsigned int characters = 0;
	unsigned int lines = 0;
	unsigned int lineComments = 0;
	unsigned int inlineComments = 0;
	unsigned int blankLines = 0;
	unsigned int semicolons = 0;
	unsigned int nonCodeLines = 0;
	unsigned int forLoops = 0;
	unsigned int whileLoops = 0;
	unsigned int ifStatements = 0;
	unsigned int switchStatements = 0;
};

void getLocStats(Measurements& m, const char* string, unsigned long long length)
{
	bool isNonCodeLine = true;
	bool isBlankLine = true;
	bool isLineComment = false;
	bool isInlineComment = false;

	for (unsigned int i = 0; i < length && string[i] != 0; ++i)
	{
		switch (string[i])
		{
		case';':
			isBlankLine = false;
			if (!(isLineComment || isInlineComment))
				++m.semicolons;
			break;
		case'/':
			isBlankLine = false;
			if (!(isInlineComment || isLineComment))
				if (i + 1 < length)
					if (string[i + 1] == '/')
						isLineComment = true;
					else if (string[i + 1] == '*')
						isInlineComment = true;
			break;
		case'*':
			isBlankLine = false;
			if (isInlineComment && i < length)
				if (string[i + 1] == '/')
				{
					++m.inlineComments;
					isInlineComment = false;
				}
			break;
		case'\n':
			++m.lines;

			if (isNonCodeLine)
				++m.nonCodeLines;

			if (isBlankLine)
				++m.blankLines;

			if (isLineComment)
				++m.lineComments;

			//Reset state flags
			isLineComment = false;
			isBlankLine = true;
			isNonCodeLine = true;
		case' ':
		case'\t':
			break;
			// Ignore carriage returns
		case'\r':
			continue;

			//For Loops
		case'f':
			if (isLineComment || isInlineComment)
			{
				break;
			}
			isBlankLine = false;
			isNonCodeLine = false;
			if (i + 3 > length)
			{
				break;
			}
			if (string[i + 1] != 'o')
			{
				break;
			}
			if (string[i + 2] != 'r')
			{
				break;
			}
			if (i > 0)
				if (!(VALID_PRE_STATEMENT_CHARACTER(string[i - 1])))
					break;

			if (!(VALID_POST_STATEMENT_CHARACTER(string[i+3])))
				break;

			++m.forLoops;
			break;

			//While Loops
		case 'w':
			if (isLineComment || isInlineComment)
				break;

			isBlankLine = false;
			isNonCodeLine = false;
			if (i + 5 > length)
				break;

			if (string[i + 1] != 'h')
				break;
			if (string[i + 2] != 'i')
				break;
			if (string[i + 3] != 'l')
				break;
			if (string[i + 4] != 'e')
				break;

			if (i > 0)
				if (!(VALID_PRE_STATEMENT_CHARACTER(string[i - 1])))
					break;

			if (!(VALID_POST_STATEMENT_CHARACTER(string[i + 5])))
				break;

			++m.whileLoops;
			break;

			//If Statements
		case 'i':
			if (isLineComment || isInlineComment)
				break;

			isBlankLine = false;
			isNonCodeLine = false;
			if (i + 2 > length)
				break;

			if (string[i + 1] != 'f')
				break;

			if (i > 0)
				if (!(VALID_PRE_STATEMENT_CHARACTER(string[i - 1])))
					break;

			if (!(VALID_POST_STATEMENT_CHARACTER(string[i + 2])))
				break;

			++m.ifStatements;
			break;

			//Switch Statements
		case 's':
			if (isLineComment || isInlineComment)
				break;

			isBlankLine = false;
			isNonCodeLine = false;
			if (i + 6 > length)
				break;

			if (string[i + 1] != 'w')
				break;
			if (string[i + 2] != 'i')
				break;
			if (string[i + 3] != 't')
				break;
			if (string[i + 4] != 'c')
				break;
			if (string[i + 5] != 'h')
				break;

			if (i > 0)
				if (!(VALID_PRE_STATEMENT_CHARACTER(string[i - 1])))
					break;

			if (!(VALID_POST_STATEMENT_CHARACTER(string[i + 6])))
				break;

			++m.switchStatements;
			break;

		default:
			isBlankLine = false;
			if (!(isInlineComment || isLineComment))
				isNonCodeLine = false;
			break;
		}

		++m.characters;

	}
	//Account for the last line that has no newline character
	++m.lines;

	if (isNonCodeLine)
		++m.nonCodeLines;

	if (isBlankLine)
		++m.blankLines;

	if (isLineComment)
		++m.lineComments;
}

void printMeasurements(Measurements m)
{
	int maxIntLen = (unsigned int)log10(m.characters) + 1;

	auto printNumber = [maxIntLen](unsigned long long number)
	{
		unsigned int spaces = maxIntLen - ((unsigned int)log10(number)+1);
		std::cout << number;
		for (int i = 0; i < spaces; ++i)
			std::cout << ' ';
	};

	auto printSpaces = [](unsigned int n)
	{
		for (int i = 0; i < n; ++i)
			std::cout << ' ';
	};

	int rowSpace = 80 - (20 + 24 + maxIntLen * 2 + 4);
	if (rowSpace < 0)
	{
		std::cout << "\nError: Files are too large to display!\n";
		return;
	}
	unsigned int lines = m.lines;
	unsigned int comments = m.inlineComments + m.lineComments;
	unsigned int loops = m.forLoops + m.whileLoops;
	unsigned int conditionals = m.ifStatements + m.switchStatements;
	unsigned int sloc = lines - m.nonCodeLines;
	unsigned int lloc = m.forLoops * 2 + m.semicolons
		+ m.whileLoops * 3 + conditionals;

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
	printNumber(lloc);
	std::cout << "|   \n";

	std::cout << "   +------------------------------------------------------------------------+   \n";

	std::cout << "   | Loops        : ";
	printNumber(loops);
	printSpaces(rowSpace);
	std::cout << "Non-code Lines        : ";
	printNumber(m.nonCodeLines);
	std::cout << "|   \n";

	std::cout << "   +------------------------------------------------------------------------+   \n";
	
	std::cout << "   | Conditionals : ";
	printNumber(conditionals);
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

unsigned int someGuy = 9;

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
	for (int i = 1; i < argc; ++i)
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
		for (int i = 0; i < nData; ++i)
		{
			File file = loadFile(data[i]);
			if (file.count > 0)
				getLocStats(m, file.data, file.count);
			else
				std::cout << "Error: failed to load file (" << data[i] << ")\n";
			unloadFile(file);
			/*std::ifstream fs(data[i], std::ios::in);
			if (!fs)
			{
				std::cout << "Error: Invalid file path specified!\n";
				fs.close();
				goto end;
			}

			fs.seekg(0, fs.end);
			unsigned int length = fs.tellg();
			fs.seekg(0, fs.beg);

			char* fileContents = new char[length];
			fs.read(fileContents, length);
			unsigned long long count = fs.gcount();
			fs.close();

			getLocStats(m, fileContents, count);

			delete[] fileContents;*/
		}
		printMeasurements(m);
	}
	else if (operation == DIRECTORIES_COUNT)
	{

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