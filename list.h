#pragma once

#include <stdlib.h>
#include <string.h>

struct __list;

struct __list_elem {
    struct __list *list;
    void *data;
    struct __list_elem *next;
};

struct __list {
    struct __list_elem *head;
    struct __list_elem *tail;
    size_t size;
    size_t unitSize;
};

typedef struct __list list;

void list_initialize(list *l, size_t typeSize) {
    l->size = 0;
    l->unitSize = typeSize;
    l->head = NULL;
    l->tail = NULL;
}

void list_pushBack(list *l, void *data) {
    l->size++;

    struct __list_elem *newElem = malloc(sizeof(struct __list_elem) + l->unitSize);
    newElem->list = l;
    memcpy(newElem + sizeof(struct __list_elem), data, l->unitSize);
    newElem->data = newElem + sizeof(struct __list_elem);
    newElem->next = NULL;

    if (l->tail) {
        l->tail->next = newElem;
        l->tail = newElem;
    } else {
        l->head = newElem;
        l->tail = newElem;
    }
}

void list_removeFront(list *l) {
    if (l->head) {
        l->size--;
        struct __list_elem *elemToRemove = l->head;
        if (l->head->next) {
            l->head = l->head->next;
        } else {
            l->tail = l->head = NULL;
        }
        free(elemToRemove);
    }
}

void *list_getFront(list *l) {
    if (!l->head)
        return NULL;
    return l->head->data;
}