#include <stdio.h>
#include <stdlib.h>

#include "publications.h"
#include "data_structures.h"

/* LinkedList */
void init_list(struct LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void add_nth_node(struct LinkedList *list, int n, void *new_data) {
    if (n < 0 || list == NULL) {
    	exit(-1);
    }

	int i;
    struct Node* nth_node;
    nth_node = malloc(sizeof(struct Node));
    nth_node->data = new_data;
    if (n == 0) {
    	
    	nth_node->next = list->head;
		list->head = nth_node;        
		list->size++;
	}

	if (n > 0) {
		if (n >= list->size) {
			n = list->size;
		}
		struct Node *curr;
		curr = list->head;

		for(i = 0; i < n - 1; i++) {
    		curr = curr->next;
    	}
    	nth_node->next = curr->next;
    	curr->next = nth_node;
		list->size++;
	}
}

struct Node* remove_nth_node(struct LinkedList *list, int n) {
	if (n < 0 || list == NULL || list->head == NULL) {
    	fprintf(stderr, "Error!");
    	exit(-1);
    }

	int i;

    if (n == 0) {
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

    } else if (n > 0) {
		struct Node *prev, *curr;
		prev = list->head;

		if (n > list->size - 1) {
			n = list->size - 1;
		}

		for(i = 0; i < n - 1; i++) {
    		prev = prev->next;
    	}
    	curr = prev->next;

    	prev->next = curr->next;
    	list->size--;
    	return curr;
    }

    return 0;
}

int get_size(struct LinkedList *list) {
    return list->size;
}

void free_list(struct LinkedList **pp_list) {
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


/* Queue */
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
    return q->list->head->data;
}

void dequeue(struct Queue *q) {
    if (get_size_q(q)) {
        struct Node *first = remove_nth_node(q->list, 0);
        free(first);
    }
}

void enqueue(struct Queue *q, void *new_data) {
    add_nth_node(q->list, q->list->size, new_data);
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

/* Hashtable */
unsigned int hash_function_int(void *a) {
    /*
     * Credits: https://stackoverflow.com/a/12996028/7883884
     */
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

int compare_function_ints(void *a, void *b) {
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a == int_b) {
        return 0;
    } else if (int_a < int_b) {
        return -1;
    } else {
        return 1;
    }
}