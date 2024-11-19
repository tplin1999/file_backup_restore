#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

void get_parity_file(const char *parity_file_name, const char **input_file_name, size_t file_count);
size_t get_file_size(const char *filename);
void write_parity_header(FILE *p_fp, const char **input_file_name, size_t file_count);
void free_resources(FILE **files, unsigned char **buffers, size_t file_count);
void *safe_malloc(size_t size);
FILE *safe_fopen(const char *filename, const char *mode);

int main(int argc, char *argv[]) {
    char *parity_file = argv[1];
    const char **input_file = (const char **) &argv[2];
    size_t input_file_count = argc - 2;

    get_parity_file(parity_file, input_file, input_file_count);
    printf("File '%s' generated successfully.\n", parity_file);
    return 0;
}

// get parity file(Using XOR to generate file)
void get_parity_file(const char *parity_file_name, const char **input_file_name, size_t file_count) {
    // open parity file
    FILE *p_fp = safe_fopen(parity_file_name, "wb");

    // create file pointers array for input file
    FILE **in_fps = safe_malloc(sizeof(FILE*) * file_count);
    // open input files
    for (size_t i = 0; i < file_count; i++) {
        in_fps[i] = safe_fopen(input_file_name[i], "rb");
    }

    // write parity header
    write_parity_header(p_fp, input_file_name, file_count);

    // create buffer for reading files
    //(buffer size: file_count * BUFFER_SIZE)
    unsigned char **buffer = safe_malloc(sizeof(unsigned char*) * file_count);
    for (size_t i = 0; i < file_count; i++)
    {
        buffer[i] = safe_malloc(sizeof(char) * BUFFER_SIZE);
    }
    // create buffer for parity file
    unsigned char *parity_buffer = safe_malloc(sizeof(char) * BUFFER_SIZE);

    // start 1.read data 2.XOR 3.write data
    while (1) {
        // max bytes read of in_fps
        size_t max_bytes = 0, bytes_read = 0;

        // for each input files
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

        // all files reach end of file -> break 
        if (max_bytes == 0) break;

        // using XOR to calculate parity
        for (size_t i = 0; i < max_bytes; i++) {
            parity_buffer[i] = 0;
            for (int j = 0; j < file_count; j++) {
                parity_buffer[i] ^= buffer[j][i];
            }
        }

        // wrting parity buffer into parity file
        fwrite(parity_buffer, 1, max_bytes, p_fp);
    }

    // close file and free memory
    free_resources(in_fps, buffer, file_count);
    free(parity_buffer);
    fclose(p_fp); 
}

// get file size
size_t get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb"); // open file
    if (file == NULL) { // if open file failed
        perror("Error opening file");
        exit(EXIT_FAILURE);  
    }
    
    fseek(file, 0, SEEK_END);   // move file pointer to the end of file
    size_t size = ftell(file);  // get file size
    fclose(file);               // close file

    return size;
}

// write parity header
void write_parity_header(FILE *p_fp, const char **input_file_name, size_t file_count){
    // 1. file count
    fwrite(&file_count, sizeof(int), 1, p_fp);

    for (size_t i = 0; i < file_count; i++)
    {
        // 2. files name length
        size_t filename_len = strlen(input_file_name[i]) + 1;
        fwrite(&filename_len, sizeof(size_t), 1, p_fp);
        // 3. input files name
        fwrite(input_file_name[i], sizeof(char), filename_len, p_fp);
        // 4. input files size
        size_t fileSize = get_file_size(input_file_name[i]);
        fwrite(&fileSize, sizeof(long), 1, p_fp);
    }
}

// free allocated resources
void free_resources(FILE **files, unsigned char **buffers, size_t file_count) {
    // close and free files pointers
    for (size_t i = 0; i < file_count; i++) {
        if (files[i]) {
            fclose(files[i]);
        }
    }
    free(files);
    // free buffers
    for (size_t i = 0; i < file_count; i++) {
        free(buffers[i]);
    }
    free(buffers);
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

// safe way to open
FILE *safe_fopen(const char *filename, const char *mode) {
    FILE *fp = fopen(filename, mode);
    if (!fp) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        perror("Details");
        exit(EXIT_FAILURE);
    }
    return fp;
}