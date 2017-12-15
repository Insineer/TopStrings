#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "useful/vector.h"

void GenerateTest(const char *outputFile) {
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
            char *password = malloc(length * sizeof(*password));
            memcpy(password, str, length);
            vector_pushBack(&template, &password);
        }
    }

    //
    // Create buffer
    //

    size_t bufferSize = 1024 * 1024 * 500; // 500 MB

    char *buffer = malloc(bufferSize);

    if (!buffer) {
        printf("GenerateTest Error: unable to create a buffer");
        return;
    }

    char *bufferEnd = buffer + bufferSize;

    //
    // Fill buffer
    //

    srand((unsigned) time(NULL));
    const double RAND_MULTIPLIER = (double) vector_length(&template) / RAND_MAX;

    for (char *p = buffer; p < bufferEnd; ) {
        char *newString = * (char **) vector_get(&template, (size_t) (rand() * RAND_MULTIPLIER));
        size_t newStringLen = strlen(newString);

        if (bufferEnd - p - 1 > newStringLen) {
            strncpy(p, newString, newStringLen);
        }
        p += newStringLen;
    }

    // Set zero at the end of buffer
    *(bufferEnd - 1) = 0;

    // Write buffer to file

    FILE *fd = fopen(outputFile, "wb");
    fwrite(buffer, bufferSize, sizeof(char), fd);
    fclose(fd);

    //
    // Free resources
    //

    // Free buffer
    free(buffer);

    // Remove template
    for (size_t i = 0; i < vector_length(&template); i++) {
        char *s = * (char **) vector_get(&template, i);
        free(s);
    }
    vector_destruct(&template);
}