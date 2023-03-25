# lochy
A LOC (Lines of code) / SLOC (Source lines of code) / LLOC (Logical lines of code) counter.

## Introduction
This project basically counts the lines of code in files / directories / projects ( directory tree ). It currently only supports C/C++ as that's what I use it for.
It can display the following statistics:
- Lines of code.
- Source lines of code.
- Logical lines of code.
- Number of non-code lines.
- Number of blank lines.
- Number of characters.
- Number of comments.
- Number of semi-colons.
- Number of for loops.
- Number of while loops.
- Number of if statements.
- Number of switch statements.

## But wait what is SLOC and LLOC!?
### SSLOC (Source Lines Of Code)
SLOC is simply the number of lines of code if all blank lines and comments are removed.

---
### LLOC (Logical Lines Of Code)
LLOC is an attempt to count the number of logical operations performed by the program.
For my purposes:
- Any statement ending in a semi-colon counts as 1 line.
- If statements and switch statements count as 2 lines (TODO: maybe 2 per condition..?).
- For loops count as 4 lines including the 2 semicolons.
- While loops count as 3 lines.

## Features
- [x] Statistics.
  - [x] Count lines of code.
  - [x] Count blank lines.
  - [x] Count line and in-line comments.
  - [x] Count source lines of code.
  - [x] Count for and while loops.
  - [x] Count if and switch statements.
  - [x] Count semi-colons.
  - [x] Count logical lines of code.
---
- [ ] File io.
  - [x] Read single files.
  - [x] Read whole directories.
  - [ ] Read whole projects (sub-directories).
  - [ ] Deal with wildcards.
  - [ ] Write output to a file.
---
- [ ] Output style.
  - [x] Output for all files.
  - [ ] Organise output per file.
  - [ ] Organize output per directory.
  - [ ] Organize output per directory and per file.
  - [ ] Print usage and help when appropriate.
  - [ ] Granular output using flags.