#! /bin/sh
# compile C code
gcc -o generateBin generateBin.c
gcc -o backup backup.c
gcc -o restore restore.c
gcc -o checkEqual checkEqual.c

# 1.gernerate .bin files
./generateBin A.bin 7
./generateBin B.bin 8
./generateBin C.bin 9
./generateBin D.bin 10

# 2.generate parity file
./backup P.bin A.bin B.bin C.bin D.bin

# 3.copy A.bin for compare in step5, and remove A.bin
cp A.bin A_copy.bin
rm A.bin

# 4.restore file
./restore restored.bin P.bin A.bin

# 5.check whether restored.bin is equal to A.bin
./checkEqual restored.bin A_copy.bin
