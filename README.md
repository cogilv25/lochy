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
- Number os switch statements.

## But wait what is SLOC and LLOC!?
### SSLOC (Source Lines Of Code)
SLOC is simply the number of lines of code if all blank lines and comments are removed.
### LLOC (Logical Lines Of Code)
LLOC is an attempt to count the number of logical operations performed by the program.
For my purposes:
- Any statement ending in a semi-colon counts as 1 line.
- If statements and switch statements count as 2 lines (TODO: maybe 2 per condition..?).
- For loops count as 4 lines including the 2 semicolons.
- While loops count as 3 lines.

## Features
- [x] Statistics.
  - [x] Simple LOC counting.
  - [x] Blank line counting.
  - [x] Comment line counting.
  - [x] SLOC counting.
  - [x] Loop counting.
  - [x] Conditional counting.
  - [x] LLOC counting.
---
- [ ] File io.
  - [x] Read single files.
  - [ ] Read whole directories.
  - [ ] Read whole projects (sub-directories).
  - [ ] Write output to a file.
---
- [ ] Output style.
  - [x] Print total output for all files.
  - [ ] Print output organized per file.
  - [ ] Print output organizes per directory.
  - [ ] Print output organized per directory and per file.
  - [ ] Print simple output.
  - [ ] Print limited output.