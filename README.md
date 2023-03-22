# lochy
A LOC (Lines of code) / SLOC (Source lines of code) / LLOC (Logical lines of code) counter.

## Introduction
This project basically counts the lines of code in a file / directory / project and displays some information about it such as:
- LOC (Lines of code)
- SLOC (Source lines of code)
- LLOC (Logical lines of code)
- Number of comment lines
- Number of blank lines
- Number of loops
- Number of if statements

## But wait what is SLOC and LLOC!?
### SSLOC
SLOC is simply the number of lines of code if all blank lines and comments are removed.
### LLOC
LLOC is an attempt to count the number of logical operations performed by the program.
For my purposes:
- Any statement ending in a semi-colon counts as 1 line.
- If statements and switch statements count as 2 lines (TODO: maybe 2 per condition..?).
- For loops count as 4 lines including the 2 semicolons.
- While loops count as 3 lines.

## Features
- [x] Read single files
- [x] Simple LOC counting
- [ ] Read whole directory
- [ ] Read whole project (sub-directories)
- [x] Blank line counting
- [x] Comment line counting
- [x] SLOC counting
- [x] Loop counting
- [x] Conditional counting
- [x] LLOC counting