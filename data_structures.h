#ifndef DATA_STRUCTURES_H_
#define DATA_STRUCTURES_H_

#include <stddef.h>

/* LinkedList */

struct Node {
    void *data;
    struct Node *next;
};

struct LinkedList {
    struct Node *head;
    struct Node *tail;
    int size;
};

void init_list(struct LinkedList *list);

void add_nth_node(struct LinkedList *list, int n, void *new_data);

struct Node* remove_nth_node(struct LinkedList *list, int n);

int get_size(struct LinkedList *list);

void free_list(struct LinkedList **list);

// void print_int_linkedlist(struct LinkedList *list);

// void print_string_linkedlist(struct LinkedList *list);

/* Queue */

struct Queue {
    struct LinkedList *list;
};

void init_q(struct Queue *q);

int get_size_q(struct Queue *q);

int is_empty_q(struct Queue *q);

void* front(struct Queue *q);

void dequeue(struct Queue *q);

void enqueue(struct Queue *q, void *new_data);

void clear_q(struct Queue *q);

void purge_q(struct Queue *q);

#endif /* DATA_STRUCTURES_H_ */
