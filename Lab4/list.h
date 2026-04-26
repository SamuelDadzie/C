#ifndef LIST_H
#define LIST_H

#include <stddef.h>  /* for size_t */

/* ── Node ─────────────────────────────────────────────────────────────────── */
/* Each node is HEAP allocated (lives as long as we need it).                 */
typedef struct Node {
    int          data;
    struct Node *next;
} Node;

/* ── List ─────────────────────────────────────────────────────────────────── */
/* The List struct itself can be STACK allocated by the caller; only the      */
/* nodes it points to are on the heap.                                        */
typedef struct {
    Node  *head;
    size_t size;
} List;

/* ── API ──────────────────────────────────────────────────────────────────── */

/* Initialise a list (call before using any other function).                  */
void list_init(List *list);

/* Free every node; resets the list to an empty/init state.                  */
void list_destroy(List *list);

/* Insert value at the front – O(1).                                          */
void list_push_front(List *list, int value);

/* Insert value at the back  – O(n).                                          */
void list_push_back(List *list, int value);

/* Remove and return the front value.  Undefined if list is empty.           */
int  list_pop_front(List *list);

/* Remove and return the back value.   Undefined if list is empty.           */
int  list_pop_back(List *list);

/* Return the front value without removing it. Undefined if list is empty.   */
int  list_peek_front(const List *list);

/* Return the back value without removing it.  Undefined if list is empty.   */
int  list_peek_back(const List *list);

/* Return 1 if the list contains value, 0 otherwise.                         */
int  list_contains(const List *list, int value);

/* Return the number of elements in the list.                                 */
size_t list_size(const List *list);

/* Return 1 if the list is empty, 0 otherwise.                               */
int  list_is_empty(const List *list);

/* Print every element, separated by " -> ", followed by "NULL