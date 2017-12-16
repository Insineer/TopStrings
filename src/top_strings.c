#include "top_strings.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "useful/vector.h"
#include "useful/list.h"

const int MAX_STRING_LEN = 500;

struct work {
    char *buffer;
    int bufferNum;
    vector *vect;
    const char *outputFile;
};

struct work_pool {
    list lst; // struct work
    bool *bufferStatuses;
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


void *threadFunc(void *workPool_v) {
    struct work_pool *workPool = workPool_v;
    bool stop = false;
    bool wait = false;
    while (!stop) {
        pthread_mutex_lock(&workPool->mutex);
        stop = workPool->stop;

        struct work *myWork_p = (struct work *) list_getFront(&workPool->lst);

        if (myWork_p) {
            struct work myWork;
            memcpy(&myWork, myWork_p, sizeof(myWork));
            list_removeFront(&workPool->lst);

            pthread_mutex_unlock(&workPool->mutex);

            sortStrings((char **) vector_getData(myWork.vect),
                        vector_length(myWork.vect),
                        myWork.outputFile);

            // free buffer and vector
            vector_destruct(myWork.vect);

            pthread_mutex_lock(&workPool->mutex);
            workPool->bufferStatuses[myWork.bufferNum] = false;
            pthread_mutex_unlock(&workPool->mutex);
        } else {
            pthread_mutex_unlock(&workPool->mutex);
            wait = true;
        }

        if (wait)
            usleep(1000);
    }
}

void findTopStringsInSortedFiles(vector *filesVect, const char *outputFile, int topSize);

void FindTopStrings(const char *inputFile, const char *outputFile, int topSize, size_t maxRAM, int numOfThreads) {
    int numOfBuffers = numOfThreads + 1;
    size_t bufferSize = maxRAM / numOfBuffers;

    FILE *inputFileFD = fopen(inputFile, "r");
    if (!inputFileFD) {
        printf("FindTopStrings Error: unable to open inputFile \"%s\"", inputFile);
        return;
    }

    char *buffer = malloc(bufferSize * numOfBuffers);        // copy of file chunks
    bool bufferStatuses[numOfBuffers];                       // false if buffer free
    vector vectors[numOfBuffers];                            // vector of new lines begins
    for (int i = 0; i < numOfBuffers; i++) {
        vector_initialize(vectors + i, sizeof(char *));
        bufferStatuses[i] = false;
    }

    if (!buffer) {
        printf("FindTopStrings Error: unable to create a buffer");
        return;
    }

    // Start threads
    pthread_t threads[numOfThreads];
    struct work_pool works;
    pthread_mutex_init(&works.mutex, NULL);
    list_initialize(&works.lst, sizeof(struct work));
    works.bufferStatuses = bufferStatuses;
    works.stop = false;
    for (int i = 0; i < numOfThreads; i++)
        pthread_create(threads + i, NULL, threadFunc, &works);

    vector fileNames;
    vector_initialize(&fileNames, sizeof(char *));

    int tFileNum = 0;
    int bufferNum = 0;

    char newString[MAX_STRING_LEN];
    size_t newStringLen = 0;

    while (!feof(inputFileFD) || newStringLen) { // if newStringLen != 0 then we need to write it in the next file
        // Fill buffer number bufferNum
        char *buffer_p = buffer + bufferNum * bufferSize;
        char *bufferEnd = buffer_p + bufferSize;

        while (buffer_p < bufferEnd) {
            if (!newStringLen) {
                if (!fgets(newString, MAX_STRING_LEN, inputFileFD))
                    break;
                newStringLen = strlen(newString);
            }

            if (bufferEnd - buffer_p - 1 > newStringLen) {
                strncpy(buffer_p, newString, newStringLen + 1);
                vector_pushBack(&vectors[bufferNum], &buffer_p);
                buffer_p += newStringLen + 1;
                newStringLen = 0;
            } else {
                break;
            }
        }

        // buffer bufferNum ready
        // push work

        char *p = malloc(MAX_STRING_LEN * sizeof(char));
        vector_pushBack(&fileNames, &p);
        char *tOutputFileName = * (char **) vector_getBack(&fileNames);
        sprintf(tOutputFileName, "sortResult%d.txt", tFileNum);

        struct work newWork;
        newWork.buffer = buffer + bufferNum * bufferSize;
        newWork.vect = &vectors[bufferNum];
        newWork.outputFile = tOutputFileName;
        newWork.bufferNum = bufferNum;

        pthread_mutex_lock(&works.mutex);
        //printf("Added work with buffer %d\n", bufferNum);
        list_pushBack(&works.lst, &newWork);
        works.bufferStatuses[bufferNum] = true;
        pthread_mutex_unlock(&works.mutex);

        tFileNum++;

        // Wait empty buffer
        bufferNum = -1;
        while (bufferNum < 0) {
            pthread_mutex_lock(&works.mutex);
            for (int i = 0; i < numOfBuffers; i++) {
                if (!works.bufferStatuses[i]) {
                    bufferNum = i;
                    break;
                }
            }
            pthread_mutex_unlock(&works.mutex);
            if (bufferNum < 0)
                usleep(1000);
        }
    }

    pthread_mutex_lock(&works.mutex);
    works.stop = true;
    pthread_mutex_unlock(&works.mutex);

    fclose(inputFileFD);

    for (size_t i = 0; i < numOfThreads; i++)
        pthread_join(threads[i], NULL);

    free(buffer);

    findTopStringsInSortedFiles(&fileNames, outputFile, topSize);
    for (size_t i = 0; i < vector_length(&fileNames); i++)
        free(* (char **) vector_get(&fileNames, i));
    vector_destruct(&fileNames);
}

void findTopStringsInSortedFiles(vector *filesVect, const char *outputFile, int topSize) {
    struct topMember {
        char *str;
        int num;
    };

    FILE *files[filesVect->length];
    char *str[filesVect->length];     // one file - one buffer for cur string
    for (size_t i = 0; i < filesVect->length; i++) {
        files[i] = fopen(* (char **) vector_get(filesVect, i), "r");
        if (!files[i]) {
            printf("findTopStringsInSortedFiles Error: unable to open %d file\n", i);
            return;
        }
        str[i] = malloc(MAX_STRING_LEN * sizeof(char));
    }

    list top;
    list_initialize(&top, sizeof(struct topMember));

    bool allFilesEnd = false;

    for (int i = 0; i < filesVect->length; i++) {
        fgets(str[i], MAX_STRING_LEN, files[i]);
    }

    do {
        allFilesEnd = true;
        // Find first string
        char *firstStr = NULL;
        for (int i = 1; i < filesVect->length; i++) {
            if (!str[i]) continue;
            if (!firstStr) firstStr = str[i];
            int res = strncmp(firstStr, str[i], MAX_STRING_LEN);
            if (res > 0) {
                firstStr = str[i];
            }
        }

        char firstStrBuf[MAX_STRING_LEN];
        int count = 0;
        strncpy(firstStrBuf, firstStr, MAX_STRING_LEN);

        // Count total amount
        for (int i = 0; i < filesVect->length; i++) {
            while (str[i] && !strncmp(firstStrBuf, str[i], MAX_STRING_LEN)) {
                count++;
                if (!fgets(str[i], MAX_STRING_LEN, files[i])) {
                    free(str[i]);
                    str[i] = NULL;
                }
            }
            if (str[i]) allFilesEnd = false;
        }

        // Insert in top-list

        list_elem *p = top.head;
        list_elem *place = NULL;
        if (!p) {
            char *s = malloc(sizeof(*s) * MAX_STRING_LEN);
            struct topMember *st = malloc(sizeof(*st));
            strncpy(s, firstStrBuf, MAX_STRING_LEN);
            st->str = s;
            st->num = count;
            list_pushBack(&top, &st);
        } else {
            do {
                if ((*(struct topMember **)p->data)->num < count) {
                    place = p;
                    p = p->next;
                } else {
                    break;
                }
            } while (p);
            if (place) {
                char *s = malloc(sizeof(*s) * MAX_STRING_LEN);
                struct topMember *st = malloc(sizeof(*st));
                strncpy(s, firstStrBuf, MAX_STRING_LEN);
                st->str = s;
                st->num = count;
                list_insertAfter(place, &st);
            } else {
                if (list_getSize(&top) < topSize) {
                    char *s = malloc(sizeof(*s) * MAX_STRING_LEN);
                    struct topMember *st = malloc(sizeof(*st));
                    strncpy(s, firstStrBuf, MAX_STRING_LEN);
                    st->str = s;
                    st->num = count;
                    list_pushFront(&top, &st);
                }
            }
        }

        if (list_getSize(&top) > topSize) {
            struct topMember *tm = * (struct topMember **) list_getFront(&top);
            free(tm->str);
            free(tm);
            list_removeFront(&top);
        }
    } while (!allFilesEnd);

    for (size_t i = 0; i < filesVect->length; i++) {
        fclose(files[i]);
        free(str[i]);
    }

    FILE *outputFD = fopen(outputFile, "w");

    list_elem *p = top.head;
    for (int i = 0; i < list_getSize(&top); i++) {
        struct topMember *tm = * (struct topMember **) p->data;
        fprintf(outputFD, "%d: %s", tm->num, tm->str);
        free(tm->str);
        free(tm);
        p = p->next;
    }

    list_destruct(&top);

    fclose(outputFD);
}
