#pragma once

#include <stdlib.h>

struct __vector {
    void *data;
    size_t unitSize;
    size_t length;
    size_t capacity;
};

typedef struct __vector vector;

void vector_initialize(vector *, size_t typeSize);
void vector_pushBack(vector *, void *data);
void *vector_get(vector *, size_t index);
void *vector_getBack(vector *);
void *vector_getData(vector *);
size_t vector_length(vector *);
void vector_destruct(vector *);