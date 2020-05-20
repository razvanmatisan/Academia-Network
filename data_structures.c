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

/**
 * TODO: optimise with tail
 */
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

int compare_function_strings(void *a, void *b) {
    char *str_a = (char *)a;
    char *str_b = (char *)b;

    return strcmp(str_a, str_b);
}

unsigned int hash_function_string(void *a) {
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char*) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
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
    add_last_node(bucket, new_paper);
}

int get_no_citations(Citations_HT *ht, int64_t paper_id) {
    if (ht == NULL) {
        return -1;
    }
    
    unsigned int hash = ht->hash_function(&paper_id) % ht->hmax;
    struct Node *it = ht->buckets[hash].head;

    //struct Node *it = bucket->head;
    while (it != NULL) {
        cited_paper *inside_data = (cited_paper *)it->data;
        // Key match
            // printf("%d %d\n", inside_data->id, paper_id);
        if (compare_function_ints(inside_data->id, &paper_id) == 0) {
            return inside_data->citations;
        }
        it = it->next;
    }

    // Nothing found
    return 0;
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

/* Init + Add + Free pt Hashtable-urile pt taskurile 2, 5, 7 */

void init_venue_ht(Venue_HT *ht) {
    if (ht == NULL) {
        return;
    }    

    // Initializing citations HT
    ht->hmax = HMAX;
    ht->hash_function = hash_function_string;
    ht->compare_function = compare_function_strings;

    // Initializing buckets
    ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
    DIE(ht->buckets == NULL, "Venue_HT: ht->buckets");
    
    int i;
    for (i = 0; i < ht->hmax; i++) {
        init_list(&ht->buckets[i]);
    }
}


void add_venue(Venue_HT *ht, char *venue, int64_t id) {
    if (ht == NULL) {
        return;
    }

    unsigned int hash = ht->hash_function(venue) % ht->hmax;
    struct LinkedList *bucket = &ht->buckets[hash];

    venue_paper *new_paper = (venue_paper *)calloc(1, sizeof(venue_paper));
    DIE(new_paper == NULL, "add_venue -> new_paper malloc");

    // Allocating memory for key
    //venue[strlen(venue)] = '\0';
	new_paper->venue = calloc(strlen(venue) + 1, sizeof(char));
    DIE(new_paper->venue == NULL, "new_paper->venue");

    memcpy(new_paper->venue, venue, (strlen(venue) + 1) * sizeof(char)); // copying key
	new_paper->id = id;

    // Add/chain => bascially appending to the current bucket
    add_last_node(bucket, new_paper);
}

void free_venue_ht(Venue_HT *ht) {
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

            venue_paper *inside_data = (venue_paper *)prev->data;
            free(inside_data->venue);
            free(inside_data);
            free(prev);
        }
    }

    free(ht->buckets);
    free(ht);
}

void init_field_ht(struct Field_HT *ht) {
    if (ht == NULL) {
        return;
    }    

    // Initializing citations HT
    ht->hmax = HMAX;
    ht->hash_function = hash_function_string;
    ht->compare_function = compare_function_strings;

    // Initializing buckets
    ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
    DIE(ht->buckets == NULL, "Field_HT: ht->buckets");
    
    int i;
    for (i = 0; i < ht->hmax; i++) {
        init_list(&ht->buckets[i]);

    }
}

void add_field(Field_HT *ht, char *field, int64_t id) {
    if (ht == NULL) {
        return;
    }

    unsigned int hash = ht->hash_function(field) % ht->hmax;
    struct LinkedList *bucket = &ht->buckets[hash];



    field_paper *new_paper = (field_paper *)malloc(sizeof(field_paper));
    DIE(new_paper == NULL, "add_field -> new_paper malloc");

    // Allocating memory for key
    new_paper->field = (char *)calloc(strlen(field) + 1, sizeof(char));
    DIE(new_paper->field == NULL, "new_paper->field");
    
	// long size = sizeof(field);
    memcpy(new_paper->field, field, (strlen(field) + 1) * sizeof(char)); // copying key
    new_paper->id = id;

    // Add/chain => bascially appending to the current bucket
    add_last_node(bucket, new_paper);
}

void free_field_ht (Field_HT *ht) {
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

            field_paper *inside_data = (field_paper *)prev->data;
            free(inside_data->field);
            free(inside_data);
            free(prev);
        }
    }

    free(ht->buckets);
    free(ht);
}
