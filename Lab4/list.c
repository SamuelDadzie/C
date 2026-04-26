// list/list.c
//
// Implementation for linked list.
//
// <Author>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

/* ---------------------------------------------------------------------------
 * Memory allocation notes:
 *   - list_t  → HEAP allocated (via list_alloc / malloc)
 *   - node_t  → HEAP allocated (via getNode / malloc)
 *   - Local loop pointers (curr, prev, …) → STACK allocated
 * --------------------------------------------------------------------------- */

/* Allocate and initialise a new, empty list. */
list_t *list_alloc() {
    list_t *mylist = (list_t *) malloc(sizeof(list_t));
    mylist->head = NULL;
    return mylist;   /* bug fix: original was missing return */
}

/* Free every node in the list, then free the list struct itself. */
void list_free(list_t *l) {
    node_t *curr = l->head;
    while (curr != NULL) {
        node_t *next = curr->next;
        free(curr);
        curr = next;
    }
    free(l);
}

/* Print each element separated by " -> ", ending with "NULL\n". */
void list_print(list_t *l) {
    node_t *curr = l->head;
    while (curr != NULL) {
        printf("%d -> ", curr->value);
        curr = curr->next;
    }
    printf("NULL\n");
}

/* Return a heap-allocated string representation of the list. */
char *listToString(list_t *l) {
    char *buf = (char *) malloc(sizeof(char) * 10024);
    buf[0] = '\0';   /* bug fix: must zero before strcat */
    char tbuf[20];

    node_t *curr = l->head;
    while (curr != NULL) {
        sprintf(tbuf, "%d->", curr->value);
        strcat(buf, tbuf);
        curr = curr->next;
    }
    strcat(buf, "NULL");
    return buf;
}

/* Allocate and return a new heap node with the given value. */
node_t *getNode(elem value) {
    node_t *mynode = (node_t *) malloc(sizeof(node_t));
    mynode->value = value;
    mynode->next  = NULL;
    return mynode;
}

/* Return the number of elements in the list. */
int list_length(list_t *l) {
    int count = 0;
    node_t *curr = l->head;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    return count;
}

/* Add a new element at the front of the list – O(1). */
void list_add_to_front(list_t *l, elem value) {
    node_t *new_node = getNode(value);
    new_node->next = l->head;
    l->head = new_node;
}

/* Add a new element at the back of the list – O(n). */
void list_add_to_back(list_t *l, elem value) {
    node_t *new_node = getNode(value);

    if (l->head == NULL) {
        l->head = new_node;
        return;
    }

    node_t *curr = l->head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
}

/* Add a new element at the given index (1-based).
 * index 1      → insert at front
 * index > len  → append to back
 * index <= 0   → no-op */
void list_add_at_index(list_t *l, elem value, int index) {
    if (index <= 0) return;

    if (index == 1 || l->head == NULL) {
        list_add_to_front(l, value);
        return;
    }

    node_t *new_node = getNode(value);
    node_t *curr = l->head;

    for (int i = 1; i < index - 1 && curr->next != NULL; i++) {
        curr = curr->next;
    }

    new_node->next = curr->next;
    curr->next = new_node;
}

/* Remove and return the element at the front – O(1).
 * Returns -1 if list is empty. */
elem list_remove_from_front(list_t *l) {
    if (l->head == NULL) return -1;

    node_t *old_head = l->head;
    elem value       = old_head->value;
    l->head          = old_head->next;
    free(old_head);
    return value;
}

/* Remove and return the element at the back – O(n).
 * Returns -1 if list is empty. */
elem list_remove_from_back(list_t *l) {
    if (l->head == NULL) return -1;

    if (l->head->next == NULL) {
        elem value = l->head->value;
        free(l->head);
        l->head = NULL;
        return value;
    }

    node_t *curr = l->head;
    while (curr->next->next != NULL) {
        curr = curr->next;
    }

    elem value = curr->next->value;
    free(curr->next);
    curr->next = NULL;
    return value;
}

/* Remove and return the element at the given index (1-based).
 * Returns -1 and does nothing if index <= 0 or out of range. */
elem list_remove_at_index(list_t *l, int index) {
    if (l->head == NULL || index <= 0) return -1;
    if (index == 1) return list_remove_from_front(l);

    node_t *curr = l->head;
    for (int i = 1; i < index - 1 && curr->next != NULL; i++) {
        curr = curr->next;
    }

    if (curr->next == NULL) return -1;   /* index out of range → no-op */

    node_t *to_remove = curr->next;
    elem value        = to_remove->value;
    curr->next        = to_remove->next;
    free(to_remove);
    return value;
}

/* Return true if value exists anywhere in the list. */
bool list_is_in(list_t *l, elem value) {
    node_t *curr = l->head;
    while (curr != NULL) {
        if (curr->value == value) return true;
        curr = curr->next;
    }
    return false;
}

/* Return the element at the given index (1-based).
 * Returns -1 if index < 1 or out of range. */
elem list_get_elem_at(list_t *l, int index) {
    if (index < 1) return -1;

    node_t *curr = l->head;
    for (int i = 1; i < index && curr != NULL; i++) {
        curr = curr->next;
    }
    return (curr != NULL) ? curr->value : -1;
}

/* Return the first index (1-based) of value, or -1 if not found. */
int list_get_index_of(list_t *l, elem value) {
    node_t *curr = l->head;
    int index = 1;
    while (curr != NULL) {
        if (curr->value == value) return index;
        curr = curr->next;
        index++;
    }
    return -1;
}