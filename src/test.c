#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "useful/vector.h"

void GenerateTest(const char *outputFile, size_t memoryBufferSize) {
    printf("Generate Test\n");

    //
    // Read template
    //

    vector template;
    vector_initialize(&template, sizeof(char *));
    FILE *template_fd = fopen("template.txt", "r");
    if (!template_fd) {
        printf("GenerateTest Error: unable to open template.txt");
        return;
    }
    char str[100];

    while (fgets(str, 100, template_fd)) {
        size_t length = strlen(str);
        if (length > 1) { // exclude empty strings "\n"
            char *password = malloc((length + 1) * sizeof(*password));
            memcpy(password, str, length + 1);
            vector_pushBack(&template, &password);
        }
    }

    //
    // Create buffer
    //

    char *buffer = malloc(memoryBufferSize);

    if (!buffer) {
        printf("GenerateTest Error: unable to create a buffer");
        return;
    }

    char *bufferEnd = buffer + memoryBufferSize;

    //
    // Fill buffer
    //

    srand((unsigned) time(NULL));
    const double RAND_MULTIPLIER = (double) vector_length(&template) / RAND_MAX;

    char *buffer_p = buffer;
    while (buffer_p < bufferEnd) {
        char *newString = * (char **) vector_get(&template, (size_t) (rand() * RAND_MULTIPLIER));
        size_t newStringLen = strlen(newString);

        if (bufferEnd - buffer_p - 1 > newStringLen) {
            strncpy(buffer_p, newString, newStringLen);
            buffer_p += newStringLen;
        } else {
            break;
        }
    }

    // Set zero at the end of buffer
    *(bufferEnd - 1) = 0;

    // Write buffer to file

    FILE *fd = fopen(outputFile, "wb");
    fwrite(buffer, buffer_p - buffer, sizeof(char), fd);
    fclose(fd);

    //
    // Free resources
    //

    fclose(template_fd);

    // Free buffer
    free(buffer);

    // Remove template
    for (size_t i = 0; i < vector_length(&template); i++) {
        char *s = * (char **) vector_get(&template, i);
        free(s);
    }
    vector_destruct(&template);
}