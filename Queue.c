#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "publications.h"
#include "data_structures.h"
#include "LinkedList.h"
#include "Queue.h"

void init_q(struct Queue *q) {
    q->list = malloc(sizeof(struct LinkedList));
    if (q->list == NULL) {
        perror("Not enough memory to initialize the queue!");
        return;
    }

    init_list(q->list);
}

int get_size_q(struct Queue *q) {
    return q->list->size;
}

int is_empty_q(struct Queue *q) {
    if (!get_size_q(q)) {
        return 1;
    }
    return 0;
}

void* front(struct Queue *q) {
	if (q->list->head) {
		return q->list->head->data; 
	}
    return NULL;
}

void dequeue(struct Queue *q) {
    if (get_size_q(q)) {
        struct Node *first = remove_first_node(q->list);
        free(first);
    }
}

void enqueue(struct Queue *q, void *new_data) {
    add_last_node(q->list, new_data);
}

void clear_q(struct Queue *q) {
    struct Node *curr = q->list->head;
    while (curr != NULL) {
        struct Node *aux = curr;
        curr = curr->next;
        free(aux);
    }
}

void purge_q(struct Queue *q) {
    clear_q(q);
    free(q->list);
}

