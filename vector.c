#include "vector.h"

#include <string.h>
#include <math.h>

// PRIVATE

double VECTOR_EXPAND_MODIFIER = 1.3;

// Realloc to newCapacity size
void vector_realloc(vector *v, size_t newCapacity) {
    if (!newCapacity) {
        if (v->capacity) {
            free(v->data);
        }
        v->data = NULL;
    } else {
        if (v->capacity)
            v->data = realloc(v->data, newCapacity * v->unitSize);
        else
            v->data = malloc(newCapacity * v->unitSize);
    }

    if (v->length > newCapacity)
        v->length = newCapacity;
    v->capacity = newCapacity;
}

// Expand vector to capacity * VECTOR_EXPAND_MODIFIER
void vector_expand(vector *v) {
    size_t newCapacity = v->capacity;
    if (!v->capacity) {
        newCapacity = 1;
    }
    vector_realloc(v, (size_t) ceil(newCapacity * VECTOR_EXPAND_MODIFIER));
}

// PUBLIC

void vector_initialize(vector *v, size_t typeSize) {
    v->unitSize = typeSize;
    v->capacity = v->length = 0;
    v->data = NULL;
}

void vector_pushBack(vector *v, void *data) {
    if (v->capacity == v->length) {
        vector_expand(v);
    }
    memcpy(v->data + v->unitSize * v->length, data, v->unitSize);
    v->length++;
}

void *vector_get(vector *v, size_t index) {
    if (index < v->length) {
        return v->data + index * v->unitSize;
    }
    return NULL;
}

void *vector_getBack(vector *v) {
    if (v->length) {
        return v->data + (v->length - 1) * v->unitSize;
    }
    return NULL;
}

void *vector_getData(vector *v) {
    if (v->capacity) {
        return v->data;
    }
    return NULL;
}

size_t vector_length(vector *v) {
    return v->length;
}

void vector_destruct(vector *v) {
    if (v->capacity) {
        free(v->data);
    }
    vector_initialize(v, v->unitSize);
}
