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

void init_info(Info *publication) {
    /*
    * Initialising INFO element when added
    * 
    * Allocate char arrays with MAX_LEN size.
    * If size of input exceeds allocated size, we will reallocate.
    * 
    * All values left uninitialised.
    */

    // Title
    publication->title = malloc(MAX_LEN * sizeof(char));
    DIE(publication->title == NULL, "publication->title");

    // Venue
    publication->venue = malloc(MAX_LEN * sizeof(char));
    DIE(publication->venue == NULL, "publication->venue");    

    // Authors
    publication->authors = malloc(MAX_AUTHORS * sizeof(Author *));
    DIE(publication->authors == NULL, "publication->authors");

    for (int i = 0; i < MAX_AUTHORS; i++) {
        Author *author = publication->authors[i];
        
        author = malloc(sizeof(Author));
        DIE(author, "author");

        // Name
        author->name = malloc(MAX_LEN * sizeof(char));
        DIE(author->name, "author->name");

        // Institution
        author->org = malloc(MAX_LEN * sizeof(char));
        DIE(author->org, "author->org");
    }

    // Fields    
    publication->fields = malloc(MAX_FIELDS * sizeof(char *));
    DIE(publication->fields == NULL, "publication->fields");

    for (int i = 0; i < MAX_FIELDS; i++) {
        publication->fields[i] = malloc(MAX_LEN * sizeof(char));
        DIE(publication->fields[i] == NULL, "publication->fields[i]");
    }

    // References
    publication->references = malloc(MAX_REFERENCES * sizeof(int64_t));
    DIE(publication->references == NULL, "publication->references");
}

PublData* init_publ_data(void) {
    PublData *data = malloc(sizeof(PublData));
    DIE(data == NULL, "malloc - data");

    // Initialising hashtable
    data->buckets = malloc(hmax * sizeof(Info));    
    DIE(data->buckets == NULL, "data->buckets malloc");

    data->hmax = hmax;
    data->hash_function = hash_function_int;
    data->compare_function = compare_function;
    return data;
}

void destroy_hashtable(PublData *data) {
    for (int i = 0; i < data->hmax; i++) {
        free(&data->buckets[i]);
    }
    
    free(data);
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
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs) {

    unsigned int hash = data->hash_function(id) % data->hmax;

    for (int i = 0; i < data->hmax; i++) {
        int index = (hash + i) % data->hmax;
        Info bucket = data->buckets[index];
        if (bucket == ) {
            
            break;
        }
    }
    return 0;
    // Put-ul
    Info *publication = &data->buckets[hash];

    // Basic info
    publication->title = title;
    publication->venue = venue;
    publication->year = year;
    publication->num_authors = num_authors;

    // Authors
    for (int i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i]; 
        author->name = author_names[i];
        author->id = author_ids[i];
        author->org = institutions[i];
    }

    // Fields
    publication->num_fields = num_fields;
    for (int i = 0; i < publication->num_fields; i++) {
        publication->fields[i] = fields[i];
    }

    publication->id = id;
    publication->num_refs = num_refs;

    publication->references = references;
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
