#include <stdio.h>
#include <stdlib.h>

#define MiB (1024 * 1024)

void generate_file(const char *filename, size_t size_in_mib);

int main(int argc, char *argv[]) {
    generate_file(argv[1], atoi(argv[2]));

    return 0;
}

void generate_file(const char *filename, size_t size_in_mib) {
    FILE *file = fopen(filename, "wb"); // open file
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    size_t total_size = size_in_mib * MiB;
    char buffer[4096]; // 4 KiB buffer
    size_t bytes_written = 0;

    // open random
    FILE *urandom = fopen("/dev/urandom", "rb");
    if (!urandom) {
        perror("Failed to open /dev/urandom");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (bytes_written < total_size) {
        size_t to_write = sizeof(buffer);
        if (total_size - bytes_written < to_write) {
            to_write = total_size - bytes_written;
        }

        fread(buffer, 1, to_write, urandom);
        fwrite(buffer, 1, to_write, file);
        bytes_written += to_write;
    }

    fclose(file);
    fclose(urandom);
    printf("File '%s' generated successfully with size %zu MiB.\n", filename, size_in_mib);
}