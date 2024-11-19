#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

# define BUFFER_SIZE 1024

int check_files_equal(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (!f1 || !f2) {
        perror("Error opening files for comparison");
        return 0;
    }

    unsigned char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    size_t bytes1, bytes2;
    while (1) {
        bytes1 = fread(buffer1, 1, BUFFER_SIZE, f1);
        bytes2 = fread(buffer2, 1, BUFFER_SIZE, f2);

        if (bytes1 != bytes2) {
            fclose(f1);
            fclose(f2);
            return 0;
        }

        if (bytes1 == 0) break;

        if (memcmp(buffer1, buffer2, bytes1) != 0) {
            fclose(f1);
            fclose(f2);
            return 0;
        }
    }

    fclose(f1);
    fclose(f2);
    return 1;
}

int main(int argc, char *argv[]) {

    if (check_files_equal(argv[1], argv[2])) {
        printf("'%s' matches '%s'.\n", argv[1], argv[2]);
    } else {
        printf("'%s' does not matches '%s'.\n", argv[1], argv[2]);
    }
    
    return 0;
}