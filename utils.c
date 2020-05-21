#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "publications.h"
#include "data_structures.h"
#include "LinkedList.h"
#include "utils.h"

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

Paper* find_paper_with_id(PublData *data, int64_t target_id) {
    unsigned int hash = data->hash_function(&target_id) % data->hmax;
    struct Node *curr = data->buckets[hash]->head;
    Paper *publication;

    while (curr) {
        publication = (Paper *)curr->data;
        if(publication->id == target_id) {
            return publication;
        }
        curr = curr->next;
    }

    // Nothing found
    return NULL;
}


/* DFS-Style breadcrum cleaing - cleaning strictly the path we've been up on */
void clean_refs_aux_data(PublData *data, Paper *start_publication) {
    int i;

    // Freeing current publications'
    start_publication->ok = 0;
    start_publication->citations = 0;
    start_publication->distance = -1;

    for (i = 0; i < start_publication->num_refs; i++) {
        Paper *curr_publication = find_paper_with_id(data, start_publication->references[i]);

        // If current neighbors is found (added in the HT)
        if (curr_publication) {
            if (curr_publication->ok) {
                // Freeing next's neighbors' aux data
                clean_refs_aux_data(data, curr_publication);
            }
        }  
    }
}

// > 0 --> older influence found
int compare_task1 (PublData *data, Paper *challenger, Paper *titleholder) {
    if (challenger == NULL || titleholder == NULL || challenger->id == titleholder->id) {
        return 0;
    }

    if (challenger->year != titleholder->year) {
        return titleholder->year - challenger->year;
    } else {
        if (!challenger->citations) {
            challenger->citations = get_no_citations(data->citations_ht, challenger->id);
        }
        if (!titleholder->citations) {
            titleholder->citations = get_no_citations(data->citations_ht, titleholder->id);
        }
        if(challenger->citations != titleholder->citations) {
            return challenger->citations - titleholder->citations;
        } else {
            return titleholder->id - challenger->id;
        }
    }
}

/* --------------------- Pentru Taskul 5 ------------------------ */
int no_papers_with_field (Field_HT *field_ht, const char *field, int64_t ids_with_field[NMAX]) {

    unsigned int hash = field_ht->hash_function(field) % field_ht->hmax;
    struct Node *curr = field_ht->buckets[hash].head;
    int i = 0;

    while (curr) {
        field_paper *publication = (field_paper *) curr->data;
        if (!strcmp(publication->field, field)) {
            ids_with_field[i] = publication->id;
            i++;
        }
        curr = curr->next;
    }

    return i;
}

int compare_task5 (PublData *data, Paper *publication1, Paper *publication2) {
    if (!publication1 || !publication2 || publication1->id == publication2->id) {
        return 0;
    }
    publication1->citations = get_no_citations(data->citations_ht, publication1->id);
    publication2->citations = get_no_citations(data->citations_ht, publication2->id);
    

    if (publication1->citations != publication2->citations) {
        return publication1->citations - publication2->citations;
    } else if (publication1->year != publication2->year) {
        return publication1->year - publication2->year;
    } else {
        return publication2->id - publication1->id;
    }
}

void swap (int64_t *a, int64_t *b) {
    int64_t aux = *a;
    *a = *b;
    *b = aux;
}

/* ----------------------------- Taskul 7   -----------------------*/
int is_in_array (char *arr_to_find, char **arr, int length) {
    for (int i = 0; i < length; i++) {
        if (!strcmp(arr[i], arr_to_find)) {
            return 1;
        }
    }
    return 0;
}
