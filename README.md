# file backup restore code
> this program can backup several files, and restore file when one file is broken.

1.	Generate four binary files (A.bin, B.bin, C.bin, D.bin) with sizes of 7, 8, 9, and 10 MiB respectively. Four binary files are for testing.
![image info](./fig/generateBin.bmp)

2.	A program in C/C++, named “backup”. Create a parity file (P.bin) from A.bin, B.bin, C.bin, and D.bin.
![image info](./fig/backup.bmp)

3.	Delete any one of the files generated in step 1.
![image info](./fig/remove.bmp)

4.	A program in C/C++, named “restore”, to recover the deleted file using P.bin and the remaining three files from step 1.
![image info](./fig/restored.bmp)
