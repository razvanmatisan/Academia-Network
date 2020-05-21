#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "publications.h"
#include "data_structures.h"
#include "LinkedList.h"

void init_list(struct LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void add_last_node(struct LinkedList *list, void *new_data) {
    if (list == NULL) {
    	exit(-1);
    }

    // Initializing new_node
    struct Node *new_node = malloc(sizeof(struct Node));
    DIE(new_node == NULL, "add_last_node new_node malloc");

	//new_node->data = malloc(sizeof(new_data));
    new_node->data = new_data;
    new_node->next = NULL;

    if (list->size == 0) {
        // Updating head
        list->head = new_node;
    } else {
        // Updating link to new tail
        list->tail->next = new_node;
    }

    // Updating tail
    list->tail = new_node;

    // Increasing list size
    list->size++;
}

struct Node* remove_first_node(struct LinkedList *list) {
	if (list == NULL || list->head == NULL) {
    	fprintf(stderr, "Error!");
    	exit(-1);
    }

    if (list->head->next == NULL) {
        struct Node *curr = list->head;
        list->head = NULL;
        list->size--;
        return curr;
    } else {
        struct Node *first = list->head;
        list->head = first->next;
        list->size--;
        return first;
    }

    return NULL;
}

int get_size(struct LinkedList *list) {
    return list->size;
}

void free_info_list(struct LinkedList **pp_list) {
	struct Node *curr = (*pp_list)->head;
	struct Node *copy_node;
	while (curr != NULL) {
		copy_node = curr;
		curr = curr->next;
        destroy_info(copy_node->data);
		free(copy_node);
	}
	free(*pp_list);
}
