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

struct info {
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

struct publications_data {
    Info *buckets;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void *, void *);
};

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

void init_info(PublData *data) {
    /*
    * Initialising INFO array (buckets)
    * 
    * Allocate char arrays with MAX_LEN size.
    * If size of input exceeds allocated size, we will reallocate.
    * 
    * All values left uninitialised.
    */

    // Title
    data->buckets->title = malloc(MAX_LEN * sizeof(char));
    DIE(data->buckets->title == NULL, "data->buckets->title");
    // Venue
    data->buckets->venue = malloc(MAX_LEN * sizeof(char));
    DIE(data->buckets->venue == NULL, "data->buckets->venue");
    

    // Authors
    data->buckets->authors = malloc(MAX_AUTHORS * sizeof(Author *));
    DIE(data->buckets->authors == NULL, "data->buckets->authors");

    for (int i = 0; i < MAX_AUTHORS; i++) {
        data->buckets->authors[i] = malloc(sizeof(Author));
        DIE(data->buckets->authors[i], "data->buckets->authors[i]");

        // Name
        data->buckets->authors[i]->name = malloc(MAX_LEN * sizeof(char));
        DIE(data->buckets->authors[i]->name, "data->buckets->authors[i]->name");

        // Institution
        data->buckets->authors[i]->org = malloc(MAX_LEN * sizeof(char));
        DIE(data->buckets->authors[i]->org, "data->buckets->authors[i]->org");
    }

    // Fields    
    data->buckets->fields = malloc(MAX_FIELDS * sizeof(char *));
    DIE(data->buckets->fields == NULL, "data->buckets->fields");

    for (int i = 0; i < MAX_FIELDS; i++) {
        data->buckets->fields[i] = malloc(MAX_LEN * sizeof(char));
        DIE(data->buckets->fields[i] == NULL, "data->buckets->fields[i]");
    }

    // References
    data->buckets->references = malloc(MAX_REFERENCES * sizeof(int64_t));
    DIE(data->buckets->references == NULL, "data->buckets->references");
}

PublData* init_publ_data(void) {
    PublData *data = malloc(sizeof(PublData));
    DIE(data == NULL, "malloc - data");

    // // Initialising hashtable
    // init_ht(data, HMAX, hash_function_int, compare_function_ints);

    // Initialising INFO array
    init_info(data);
    return data;
}

void destroy_hashtable(PublData *ht) {
    for (int i = 0; i < ht->hmax; i++) {
        free(&ht->buckets[i]);
    }
    
    free(ht);
}

void destroy_info(PublData *data) {
    // Title
    free(data->buckets->title);
    free(data->buckets->venue);
    
    // Authors
    for (int i = 0; i < data->buckets->num_authors; i++) {
        free(data->buckets->authors[i]->name);
        free(data->buckets->authors[i]->org);
        free(data->buckets->authors[i]);
    }
    free(data->buckets->authors);
    
    // Fields
    for (int i = 0; i < data->buckets->num_fields; i++) {
        free(data->buckets->fields[i]);
    }
    free(data->buckets->fields);

    // References
    free(data->buckets->references);
}

void destroy_publ_data(PublData* data) {
    destroy_hashtable(data);
    destroy_info(data);
}
void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, int64_t id, const int64_t* references,
    const int num_refs) {

    unsigned int hash = data->hash_function(&id) % data->hmax;

    int index;

    for (int i = 0; i < data->hmax; i++) {
        index = (hash + i) % data->hmax;
        Info bucket = data->buckets[index];
        if (bucket.title == NULL) {
            break;
        }
    }

    // Put-ul
    Info publication = data->buckets[index];

    // Basic info
    publication.title = title;
    publication.venue = venue;
    publication.year = year;
    publication.num_authors = num_authors;

    // Authors
    for (int i = 0; i < publication.num_authors; i++) {
        Author *author = publication.authors[i]; 
        author->name = author_names[i];
        author->id = author_ids[i];
        author->org = institutions[i];
    }

    // Fields
    publication.num_fields = num_fields;
    for (int i = 0; i < publication.num_fields; i++) {
        publication.fields[i] = fields[i];
    }

    publication.id = id;
    publication.num_refs = num_refs;

    publication.references = references;

    data->buckets[index] = publication;
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
