#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "publications.h"

// Made by Radu
#define MAX_REFERENCES 200
#define MAX_LEN 200
#define MAX_AUTHORS 100
#define MAX_FIELDS 50
#define HMAX 112000

struct author {
    char *name;
    int64_t id;
    char *org;
};

struct publications_data {
    char *title;
    char *venue;
    int year;
    Author **authors;
    int num_authors;
    char **fields;
    int num_fields;
    int64_t id;
    int64_t *references;
    int num_refs;
};

typedef struct Hashtable {
    PublData *buckets;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void *, void *);
} Hashtable;

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

void init_ht(struct Hashtable *ht, int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*)) {
    ht->buckets = (PublData*) malloc(hmax * sizeof(PublData));

    if (ht->buckets == NULL) {
        fprintf(stderr, "No buckets found!\n");
        exit(-1);
    }

    for (int i = 0; i < hmax; i++) {
        ht->buckets[i] = malloc(sizeof(PublData));
    }

    ht->hmax = hmax;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;
}

PublData* init_publ_data(void) {
    PublData *data = malloc(sizeof(PublData));
    DIE(data == NULL, "calloc - data");

    /*
    * Allocate char arrays with MAX_LEN size.
    * If size of input exceeds allocated size, we will reallocate.
    * 
    * All values left uninitialised.
    */

    // Title
    data->title = malloc(MAX_LEN * sizeof(char));
    DIE(data->title == NULL, "data->title");
    // Venue
    data->venue = malloc(MAX_LEN * sizeof(char));
    DIE(data->venue == NULL, "data->venue");
    

    // Authors
    data->authors = malloc(MAX_AUTHORS * sizeof(Author *));
    DIE(data->authors == NULL, "data->authors");

    for (int i = 0; i < MAX_AUTHORS; i++) {
        data->authors[i] = malloc(sizeof(Author));
        DIE(data->authors[i], "data->authors[i]");

        // Name
        data->authors[i]->name = malloc(MAX_LEN * sizeof(char));
        DIE(data->authors[i]->name, "data->authors[i]->name");

        // Institution
        data->authors[i]->org = malloc(MAX_LEN * sizeof(char));
        DIE(data->authors[i]->org, "data->authors[i]->org");
    }

    // Fields    
    data->fields = malloc(MAX_FIELDS * sizeof(char *));
    DIE(data->fields == NULL, "data->fields");

    for (int i = 0; i < MAX_FIELDS; i++) {
        data->fields[i] = malloc(MAX_LEN * sizeof(char));
        DIE(data->fields[i] == NULL, "data->fields[i]");
    }

    // References
    data->references = malloc(MAX_REFERENCES * sizeof(int64_t));
    DIE(data->references == NULL, "data->references");

    return data;
}

void destroy_publ_data(PublData* data) {
    free(data->title);
    free(data->venue);
    
    for (int i = 0; i < data->num_authors; i++) {
        free(data->authors[i]->name);
        free(data->authors[i]->org);
        free(data->authors[i]);
    }
    free(data->authors);
    
    for (int i = 0; i < data->num_fields; i++) {
        free(data->fields[i]);
    }
    free(data->fields);

    free(data->references);
}

void destroy_hashtable(Hashtable *ht) {
    for (int i = 0; i < ht->hmax; i++) {
        free(&ht->buckets[i]);
    }
    
    free(ht);
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs) {

    int hash = ht->
        
    // Put-ul
    data->title = title;
    data->venue = venue;
    data->year = year;
    data->num_authors = num_authors;

    for (int i = 0; i < data->num_authors; i++) {
        data->authors[i]->name = author_names[i];
        data->authors[i]->id = author_ids[i];
        data->authors[i]->org = institutions[i];
    }

    data->num_fields = num_fields;
    for (int i = 0; i < data->num_fields; i++) {
        data->fields[i] = fields[i];
    }

    data->id = id;
    data->num_refs = num_refs;

    data->references = references;
}

char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    /* TODO: implement get_oldest_influence */

    return NULL;
}

float get_venue_impact_factor(PublData* data, const char* venue) {
    /* TODO: implement get_venue_impact_factor */

    return 0.f;
}

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    /* TODO: implement get_number_of_influenced_papers */

    return -1;
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    /* TODO: implement get_erdos_distance */

    return -1;
}

char** get_most_cited_papers_by_field(PublData* data, const char* field,
    int* num_papers) {
    /* TODO: implement get_most_cited_papers_by_field */

    return NULL;
}

int get_number_of_papers_between_dates(PublData* data, const int early_date,
    const int late_date) {
    /* TODO: implement get_number_of_papers_between_dates */

    return 0;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    /* TODO: implement get_number_of_authors_with_field */

    return 0;
}

int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {
    /* TODO: implement get_histogram_of_citations */

    *num_years = 0;

    return NULL;
}

char** get_reading_order(PublData* data, const int64_t id_paper,
    const int distance, int* num_papers) {
    /* TODO: implement get_reading_order */

    *num_papers = 0;

    return NULL;
}

char* find_best_coordinator(PublData* data, const int64_t id_author) {
    /* TODO: implement find_best_coordinator */

    return NULL;
}
