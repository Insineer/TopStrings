#include "top_strings.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "vector.h"
#include "list.h"

struct work {
    char *buffer;
    vector *vect;
    const char *outputFile;
};

struct work_pool {
    list lst; // struct work
    bool stop;
    pthread_mutex_t mutex;
};

int comp(const void *elem1, const void *elem2) {
    return strcmp(* (char **) elem1, * (char **) elem2);
}

int sortStrings(char *strings[], size_t num, const char *outputFile) {
    qsort(strings, num, sizeof(*strings), comp);

    // Write result in file
    FILE *fd = fopen(outputFile, "w");
    if (!fd) {
        printf("sortStrings error: unable to open file \"%s\"", outputFile);
        return 1;
    }
    for (size_t i = 0; i < num; i++) {
        fputs(strings[i], fd);
    }
    fclose(fd);
    return 0;
}


void threadFunc(struct work_pool *workPool) {
    bool stop = false;
    bool wait = false;
    while (!stop) {
        pthread_mutex_lock(&workPool->mutex);
        stop = workPool->stop;
        struct work *nextWork = (struct work *) list_getFront(&workPool->lst);
        if (nextWork) {
            list_removeFront(&workPool->lst);
            sortStrings((char **) vector_getData(nextWork->vect),
                        vector_length(nextWork->vect),
                        nextWork->outputFile);
        } else {
            wait = true;
        }
        pthread_mutex_unlock(&workPool->mutex);
        if (wait)
            usleep(1000);
    }
}


void FindTopStrings(const char *inputFile, const char *outputFile, int topSize) {
    //
    // Create buffer
    //

    size_t bufferSize = 100; //1024 * 1024 * 100; // 100 MB

    char *buffer = malloc(bufferSize);
    vector strings;
    vector_initialize(&strings, sizeof(char *));

    if (!buffer) {
        printf("FindTopStrings Error: unable to create a buffer");
        return;
    }

    char *bufferEnd = buffer + bufferSize;

    FILE *inputFileFD = fopen(inputFile, "r");
    if (!inputFileFD) {
        printf("FindTopStrings Error: unable to open inputFile \"%s\"", inputFile);
        return;
    }

    int tFileNum = 0;

    char newString[256];
    size_t newStringLen = 0;

    while (!feof(inputFileFD) || newStringLen) { // if newStringLen != 0 then we need to write it in the next file
        char *buffer_p = buffer;

        while (buffer_p < bufferEnd) {
            if (!newStringLen) {
                if (!fgets(newString, 256, inputFileFD))
                    break;
                newStringLen = strlen(newString);
            }

            if (bufferEnd - buffer_p - 1 > newStringLen) {
                strncpy(buffer_p, newString, newStringLen + 1);
                vector_pushBack(&strings, &buffer_p);
                buffer_p += newStringLen + 1;
                newStringLen = 0;
            } else {
                break;
            }
        }

        char tOutputFileName[256];
        sprintf(tOutputFileName, "sortResult%d.txt", tFileNum);

        sortStrings((char **) vector_getData(&strings),
                              vector_length(&strings),
                              tOutputFileName);

        vector_destruct(&strings);
        tFileNum++;
    }
}
