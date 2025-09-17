
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include "./os.h"

#define VERBOSE false

const char *file_to_char(const char *file_path)
{
    long buffer_size = 0;
    char *buffer;
    FILE *file;
    
    file = fopen(file_path, "r");
    if (file == NULL) {
	fprintf(stderr, "[FAILED] : File could not be loaded. \n");
	fclose(file);
	return NULL;
    }
    if (fseek(file, 0, SEEK_END) < 0) goto BLANK;
    buffer_size = ftell(file);
    buffer = (char *)malloc(buffer_size + 1);
    if (buffer == NULL) {
	fprintf(stderr, "[FAILED] : Buffer could not be read. \n");
	return NULL;
    }
    
    if (fseek(file, 0, SEEK_SET) < 0) goto BLANK;
    fread(buffer, 1, buffer_size, file);
    if (ferror(file)) {
	fprintf(stderr, "[FAILED] : File could not be read. \n");
	return NULL;
    }
    fclose(file);
#if VERBOSE == true
    printf("[INFO] : File have been loaded and translated to char array sucessfully. \n");
#endif
    const char *result = buffer;
    return result;
    
BLANK:
#if VERBOSE == true
    fprintf(stderr, "[INFO] : File is blank. \n");
#endif
    fclose(file);
    
    buffer = (char *)malloc(sizeof(char));
    free(buffer);
    return NULL;
}

char *load_file(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (!file) {
	fprintf(stderr, "[FAILED] : File could not be recognized or does not exist.\n");
	return NULL;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *data = malloc(size + 1);
    if (!data) { fprintf(stderr, "[FAILED] : Failed to allocate memory to file data.\n"); fclose(file); return NULL; }
    fread(data, 1, size, file);
    data[size] =  '\0';
    fclose(file);
    return data;
}
