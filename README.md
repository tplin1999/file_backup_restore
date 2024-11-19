# file backup restore code


1.	Generate four binary files (A.bin, B.bin, C.bin, D.bin) with sizes of 7, 8, 9, and 10 MiB respectively. The content of these files must be randomized.

2.	Develop a program in C/C++, named “backup”, to create a parity file (P.bin) from A.bin, B.bin, C.bin, and D.bin.

3.	Delete any one of the files generated in step 1.

4.	Write a program in C/C++, named “restore”, to recover the deleted file using P.bin and the remaining three files from step 1. Please design the program arguments/options to take the input and output files.
