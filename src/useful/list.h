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
typedef struct __list_elem list_elem;

void list_initialize(list *l, size_t typeSize);
void list_pushBack(list *l, void *data);
void list_pushFront(list *l, void *data);
void list_insertAfter(list_elem *l, void *data);
void list_removeFront(list *l);
void *list_getFront(list *l);
size_t list_getSize(list *l);
void list_destruct(list *l);