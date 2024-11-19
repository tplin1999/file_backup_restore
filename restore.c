#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

typedef struct {
    int file_count;
    char **filenames;
    long *file_sizes;
    long header_end_pos;
} PARITY_HEADER;

PARITY_HEADER read_header(const char *parityFile);
void restore_file(const char *output, const char *parityFile, const char *missingFile);
void free_header(PARITY_HEADER *header);
void *safe_malloc(size_t size);
FILE *safe_fopen(const char *filename, const char *mode);

int main(int argc, char *argv[]) {

    char *restored_file = argv[1];
    char *parity_file = argv[2];
    char *missing_file = argv[3];

    restore_file(restored_file, parity_file, missing_file);
    printf("File '%s' generated successfully.\n", argv[1]);
    return 0;
}

// read header
PARITY_HEADER read_header(const char *parity_file_name){
    PARITY_HEADER header;

    // open parity file
    FILE *p_fp = safe_fopen(parity_file_name, "rb");

    // read file count
    fread(&header.file_count, sizeof(int), 1, p_fp);

    // original files names and files size
    header.filenames = safe_malloc(sizeof(char*) * header.file_count);
    header.file_sizes = safe_malloc(sizeof(long) * header.file_count);

    for (int i = 0; i < header.file_count; i++) {
        // read file name length
        size_t filename_len;
        fread(&filename_len, sizeof(size_t), 1, p_fp);

        // read file name
        header.filenames[i] = safe_malloc(sizeof(char) * filename_len);
        fread(header.filenames[i], sizeof(char), filename_len, p_fp);

        // read file size
        fread(&header.file_sizes[i], sizeof(long), 1, p_fp);
    }

    // return next time reading position
    header.header_end_pos = ftell(p_fp);

    fclose(p_fp);
    return header;
}

// restore file
void restore_file(const char *restore_file_name, const char *parity_file_name, const char *missingFile) {
    // read header
    PARITY_HEADER header = read_header(parity_file_name);
    int file_count = header.file_count;
    char **filenames = header.filenames;
    long *file_sizes = header.file_sizes;
    long header_end_pos = header.header_end_pos;

    // construct file fps of input and p
    FILE **in_fps = safe_malloc(sizeof(FILE*) * file_count);
    size_t missing_file_size = 0;

    // open parity file
    FILE *p_fp = safe_fopen(parity_file_name, "rb");
    // move pointer to end of header
    fseek(p_fp, header_end_pos, SEEK_SET);

    // open restore file
    FILE *restore_fp = safe_fopen(restore_file_name, "wb");

    // add file pointers into in_fps
    size_t file_index = 0;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(filenames[i], missingFile) == 0) {
            missing_file_size = file_sizes[i];  // record missing file size
        } else {
            in_fps[file_index] = safe_fopen(header.filenames[i], "rb");
            file_index ++;
        }
    }
    in_fps[file_count - 1] = p_fp; // Add parity file as the last input

    // create buffer of reading files
    unsigned char **buffer = safe_malloc(sizeof(unsigned char*) * file_count);
    for (size_t i = 0; i < file_count; i++)
    {
        buffer[i] = safe_malloc(sizeof(char) * BUFFER_SIZE);
    }
    // restored files content
    unsigned char *restored_buffer = safe_malloc(sizeof(char) * BUFFER_SIZE);

    // bytes already write into restore file
    size_t bytesWritten = 0;
    // start 1.read data 2.XOR 3.write data
    while (1) {
        // max bytes read of in_fps
        size_t max_bytes = 0, bytes_read = 0;

        // read input files data
        for (int i = 0; i < file_count; i++) {
            bytes_read = fread(buffer[i], 1, BUFFER_SIZE, in_fps[i]);
            // renew max bytes read
            if (bytes_read > max_bytes) {
                max_bytes = bytes_read;
            }
            // if reach end of file, reset buffer
            if (bytes_read == 0) {
                memset(buffer[i], 0, BUFFER_SIZE);
            }
        }
        
        // if all file EOF -> break
        if (max_bytes == 0) break; 

        // using XOR to restore data
        for (size_t i = 0; i < max_bytes; i++) {
            restored_buffer[i] = 0;
            for (int j = 0; j < file_count; j++) {
                restored_buffer[i] ^= buffer[j][i];
            }
        }

        // make sure bytesWritten == missing_file_size
        if(bytesWritten + max_bytes > missing_file_size){
            max_bytes = missing_file_size - bytesWritten;
            fwrite(restored_buffer, 1, max_bytes, restore_fp);
            break;
        }

        // write data into restore file
        fwrite(restored_buffer, 1, max_bytes, restore_fp);
        bytesWritten += max_bytes;
    }

    for (int i = 0; i < file_count; i++) {
        fclose(in_fps[i]);
        free(buffer[i]);
    }
    free(in_fps);
    free(buffer);
    free(restored_buffer);
    free_header(&header);
    fclose(restore_fp);
}

// free header
void free_header(PARITY_HEADER *header) {
    for (int i = 0; i < header->file_count; i++) {
        free(header->filenames[i]);
    }
    free(header->filenames);
    free(header->file_sizes);
}

// safe way to malloc
void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// safe way to open file
FILE *safe_fopen(const char *filename, const char *mode) {
    FILE *fp = fopen(filename, mode);
    if (!fp) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        perror("Details");
        exit(EXIT_FAILURE);
    }
    return fp;
}