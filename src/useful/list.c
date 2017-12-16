#include "list.h"

#include <stdio.h>

void list_initialize(list *l, size_t typeSize) {
    if (!l || !typeSize) return;

    l->size = 0;
    l->unitSize = typeSize;
    l->head = NULL;
    l->tail = NULL;
}

void list_pushBack(list *l, void *data) {
    if (!l || !data) return;

    l->size++;

    struct __list_elem *newElem = malloc(sizeof(struct __list_elem) + l->unitSize);
    if (!newElem) {
        printf("list_pushBack error: unable to allocate memory");
        return;
    }
    newElem->list = l;
    newElem->data = newElem + 1; // data is part of buffer after __list_elem
    memcpy(newElem->data, data, l->unitSize);
    newElem->next = NULL;

    if (l->tail) {
        l->tail->next = newElem;
        l->tail = newElem;
    } else {
        l->head = newElem;
        l->tail = newElem;
    }
}

void list_pushFront(list *l, void *data) {
    if (!l || !data) return;

    l->size++;

    struct __list_elem *newElem = malloc(sizeof(struct __list_elem) + l->unitSize);
    if (!newElem) {
        printf("list_pushBack error: unable to allocate memory");
        return;
    }
    newElem->list = l;
    newElem->data = newElem + 1; // data is part of buffer after __list_elem
    memcpy(newElem->data, data, l->unitSize);
    newElem->next = NULL;

    if (l->head) {
        newElem->next = l->head;
        l->head = newElem;
    } else {
        l->head = newElem;
        l->tail = newElem;
    }
}

void list_insertAfter(list_elem *le, void *data) {
    if (!le || !data) return;
    list *l = le->list;
    l->size++;

    struct __list_elem *newElem = malloc(sizeof(struct __list_elem) + l->unitSize);
    if (!newElem) {
        printf("list_insertAfter error: unable to allocate memory");
        return;
    }
    newElem->list = l;
    newElem->data = newElem + 1; // data is part of buffer after __list_elem
    memcpy(newElem->data, data, l->unitSize);

    newElem->next = le->next;
    le->next = newElem;

    if (l->tail == le) {
        l->tail = newElem;
    }
}

void list_removeFront(list *l) {
    if (!l) return;

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

size_t list_getSize(list *l) {
    return l->size;
}

void list_destruct(list *l) {
    list_elem *p = l->head;
    while (p) {
        list_elem *t = p;
        p = p->next;
        free(t);
    }
    list_initialize(l, l->unitSize);
}