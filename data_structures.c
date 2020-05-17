#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "publications.h"
#include "data_structures.h"

void init_list(struct LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/**
 * TOOD: optimise with tail
 */
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

/**
 * TODO: optimise with tail
 */
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
        struct Node *first = remove_nth_node(q->list, 0);
        free(first);
    }
}

/**
 * TODO: optimise with tail
 */
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

void init_cit_ht(struct Citations_HT *ht) {
    if (ht == NULL) {
        return;
    }    

    // Initializing citations HT
    ht->hmax = HMAX;
    ht->hash_function = hash_function_int;
    ht->compare_function = compare_function_ints;

    // Initializing buckets
    ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
    DIE(ht->buckets == NULL, "Citations_HT: ht->buckets");
    
    int i;
    for (i = 0; i < ht->hmax; i++) {
        init_list(&ht->buckets[i]);

    }
}

void add_citation(struct Citations_HT *ht, int64_t cited_paper_id) {
    if (ht == NULL) {
        return;
    }

    unsigned int hash = ht->hash_function(&cited_paper_id) % ht->hmax;
    struct LinkedList *bucket = &ht->buckets[hash];

    // Iterate through the bucket until keymatch
    struct Node *it = bucket->head;
    while (it != NULL) {
        struct cited_paper *inside_data = (struct cited_paper *)it->data;
        // Key match
        if (ht->compare_function(inside_data->id, &cited_paper_id) == 0) {
            // Already cited => updating count
            inside_data->citations++;
            return;
        }
        it = it->next;
    }

    // First citation => new element in bucket
    struct cited_paper *new_paper = malloc(sizeof(cited_paper));
    DIE(new_paper == NULL, "add_citations -> new_paper malloc");

    // Allocating memory for key
    new_paper->id = malloc(sizeof(cited_paper_id));
    DIE(new_paper->id == NULL, "paper->id");
    
    memcpy(new_paper->id, &cited_paper_id, sizeof(cited_paper_id)); // copying key
    new_paper->citations = FIRST_CITATION;

    // Add/chain => bascially appending to the current bucket
    add_nth_node(bucket, bucket->size, new_paper);
}

int get_no_citations(Citations_HT *ht, int64_t paper_id) {
    if (ht == NULL) {
        return -1;
    }
    
    unsigned int hash = ht->hash_function(&paper_id) % ht->hmax;
    struct LinkedList *bucket = &ht->buckets[hash];

    struct Node *it = bucket->head;
    while (it != NULL) {
        cited_paper *inside_data = (cited_paper *)it->data;
        // Key match
        if (ht->compare_function(inside_data->id, &paper_id) == 0) {
            return inside_data->citations;
        }
    it = it->next;
    }

    // Nothing found
    return -1;
}

void free_cit_ht(Citations_HT *ht) {
    if (ht == NULL) {
        return;
    }

    int i;
    for (i = 0; i < ht->hmax; i++) {
        struct LinkedList *bucket = &ht->buckets[i];
        struct Node *it = bucket->head;
        
        struct Node *prev;
        while (it != NULL) {
            prev = it;
            it = it->next;

            cited_paper *inside_data = (cited_paper *)prev->data;
            free(inside_data->id);
            free(inside_data);
            free(prev);
        }
    }

    free(ht->buckets);
    free(ht);
}