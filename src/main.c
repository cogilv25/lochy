#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "fileio.h"

//Operations
#define FILES_COUNT 1
#define DIRECTORIES_COUNT 2

//Modifiers
#define RECURSIVELY 1

//Valid Characters Before a 'Statement' (for, while, if, switch)
#define VALID_POST_STATEMENT_CHARACTER(x) x == '\n' || x == '\t' \
|| x == ' ' || x == '('

//Additional Valid Characters Before a 'Statement'
#define VALID_PRE_STATEMENT_CHARACTER(x) VALID_POST_STATEMENT_CHARACTER(x) \
|| x == ')' || x == ';' || x == '{' || x == '}'

typedef struct
{
	unsigned int characters;
	unsigned int lines;
	unsigned int lineComments;
	unsigned int inlineComments;
	unsigned int blankLines;
	unsigned int semicolons;
	unsigned int nonCodeLines;
	unsigned int forLoops;
	unsigned int whileLoops;
	unsigned int ifStatements;
	unsigned int switchStatements;
} Measurements;

static inline bool validPostStatementCharacter(char c)
{
	return (c == '\n' || c == '\t' || c == ' ' || c == '(');
}

static inline bool validPreStatementCharacter(char c)
{
	return validPostStatementCharacter(c) ||
	c == ')' || c == ';' || c == '{' || c == '}';
}

void getLocStats(Measurements* m, const char* string, uint64 length)
{
	bool isNonCodeLine = true;
	bool isBlankLine = true;
	bool isLineComment = false;
	bool isInlineComment = false;

	for (unsigned int i = 0; i < length; ++i)
	{
		switch (string[i])
		{
		case'\0':
			return;
		case';':
			isBlankLine = false;
			if (!(isLineComment || isInlineComment))
				++m->semicolons;
			break;
		case'/':
			isBlankLine = false;
			if (!(isInlineComment || isLineComment))
				if (i + 1 < length)
					if (string[i + 1] == '/')
						isLineComment = true;
					if (string[i + 1] == '*')
						isInlineComment = true;
			break;
		case'*':
			isBlankLine = false;
			if (isInlineComment && i < length)
				if (string[i + 1] == '/')
				{
					++m->inlineComments;
					isInlineComment = false;
				}
			break;
		case'\n':
			++m->lines;

			if (isNonCodeLine)
				++m->nonCodeLines;

			if (isBlankLine)
				++m->blankLines;

			if (isLineComment)
				++m->lineComments;

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

			++m->forLoops;
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

			++m->whileLoops;
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

			++m->ifStatements;
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

			++m->switchStatements;
			break;

		default:
			isBlankLine = false;
			if (!(isInlineComment || isLineComment))
				isNonCodeLine = false;
			break;
		}

		++m->characters;

	}
	//Account for the last line that has no newline character
	++m->lines;

	if (isNonCodeLine)
		++m->nonCodeLines;

	if (isBlankLine)
		++m->blankLines;

	if (isLineComment)
		++m->lineComments;
}

void printNumber(unsigned long long number, int maxIntLen)
{
	unsigned int spaces = maxIntLen - ((unsigned int)log10(number) + 1);
	printf("%llu",number);
	for (int i = 0; i < spaces; ++i)
		printf(" ");
}

void printSpaces(unsigned int n)
{
	for (int i = 0; i < n; ++i)
		printf(" ");
}

void printMeasurements(Measurements m)
{
	int maxIntLen = (unsigned int)log10(m.characters) + 1;

	int rowSpace = 80 - (20 + 24 + maxIntLen * 2 + 4);
	if (rowSpace < 0)
	{
		printf("\nError: Files are too large to display!\n");
		return;
	}
	unsigned int lines = m.lines;
	unsigned int comments = m.inlineComments + m.lineComments;
	unsigned int loops = m.forLoops + m.whileLoops;
	unsigned int conditionals = m.ifStatements + m.switchStatements;
	unsigned int sloc = lines - m.nonCodeLines;
	unsigned int lloc = m.forLoops * 2 + m.semicolons
		+ m.whileLoops * 3 + conditionals;

	printf("\n");
	printf("   +------------------------------------------------------------------------+   \n");
	printf("   |                                                                        |   \n");
	printf("   |                       Lochy The Monster Counter                        |   \n");
	printf("   |                                                                        |   \n");
	printf("   +------------------------------------------------------------------------+   \n");

	printf("   | Characters   : "); 
	printNumber(m.characters, maxIntLen);
	printSpaces(rowSpace);
	printf("Lines Of Code         : ");
	printNumber(lines, maxIntLen);
	printf("|   \n");

	printf("   +------------------------------------------------------------------------+   \n");

	printf("   | Comments     : ");
	printNumber(comments, maxIntLen);
	printSpaces(rowSpace);
	printf("Source Lines Of Code  : ");
	printNumber(sloc, maxIntLen);
	printf("|   \n");

	printf("   +------------------------------------------------------------------------+   \n");

	printf("   | Semi-colons  : ");
	printNumber(m.semicolons, maxIntLen);
	printSpaces(rowSpace);
	printf("Logical Lines Of Code : ");
	printNumber(lloc, maxIntLen);
	printf("|   \n");

	printf("   +------------------------------------------------------------------------+   \n");

	printf("   | Loops        : ");
	printNumber(loops, maxIntLen);
	printSpaces(rowSpace);
	printf("Non-code Lines        : ");
	printNumber(m.nonCodeLines, maxIntLen);
	printf("|   \n");

	printf("   +------------------------------------------------------------------------+   \n");
	
	printf("   | Conditionals : ");
	printNumber(conditionals, maxIntLen);
	printSpaces(rowSpace);
	printf("Blank Lines           : ");
	printNumber(m.blankLines, maxIntLen);
	printf("|   \n");


	printf("   +------------------------------------------------------------------------+   \n");
}

int main(int argc, char** argv)
{
	int * flagsPerArguments = calloc(argc-1,sizeof(int));
	int nFlags = 0;
	int* dataArgumentIndexes = calloc(argc-1,sizeof(int));
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
	char* flags = malloc(nFlags);
	{
		int count = 0;
		for (int i = 0; i < argc - 1; ++i)
			for (int j = 0; j < flagsPerArguments[i]; ++j)
			{
				flags[count] = argv[i][j + 1];
				++count;
			}
	}
	free(flagsPerArguments);

	//Extract data into string list
	char** data = malloc(nData*sizeof(char*));
	for (int i = 0; i < nData; ++i)
		data[i] = argv[dataArgumentIndexes[i]];
	free(dataArgumentIndexes);


	int operation = FILES_COUNT;
	int modifiers = 0;
	bool operationSet = false;

	for (int i = 0; i < nFlags; ++i)
			switch (flags[i])
			{
				case 'd':
					if (operationSet)
					{
						printf("\nError: Multiple operations set!\n");
						goto end;
					}
					operation = DIRECTORIES_COUNT;
					operationSet = true;
					break;
				case 'r':
					modifiers |= 1;
			}
	Measurements m = {0};
	if (operation == FILES_COUNT)
	{
		for (int i = 0; i < nData; ++i)
		{
			file_properties* props = get_file_properties(data[i]);
			char * file_contents = get_file_contents(props);

			if (props->filesize > 0)
				getLocStats(&m, file_contents, props->filesize);
			else
				printf("Error: failed to load file (%s)\n", data[i]);

			free(props);
			free(file_contents);
		}
		printMeasurements(m);
	}
	else if (operation == DIRECTORIES_COUNT)
	{
		file_properties_list* list;
		if(modifiers & 1)
			list = get_file_properties_recursively(data[0]);
		else
			list = get_file_properties_in_directory(data[0]);
		string_list* file_contents = get_all_file_contents(list);

		for (int i = 0; i < file_contents->count; ++i)
			getLocStats(&m, file_contents->string_pointers[i], list->properties[i].filesize);

		free(list);
		free(file_contents);

		printMeasurements(m);
	}


end:

	free(flags);
	free(data);
	return 0;
}