#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "publications.h"

#define HMAX 112500

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
    Info **buckets;
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

void init_info(Info *publication, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, int64_t id, const int64_t* references,
    const int num_refs) {
    // Title
    publication->title = calloc(strlen(title) + 1, sizeof(char));
    DIE(publication->title == NULL, "publication->title");

    // Venue
    publication->venue = calloc(strlen(venue) + 1, sizeof(char));
    DIE(publication->venue == NULL, "publication->venue");    

    // Authors
    publication->authors = calloc(num_authors, sizeof(Author *));
    DIE(publication->authors == NULL, "publication->authors");


    for (int i = 0; i < num_authors; i++) {
        publication->authors[i] = calloc(1, sizeof(Author));
        DIE(publication->authors[i] == NULL, "publications->authors[i]");

        Author *author = publication->authors[i];

        // Name
        author->name = calloc(strlen(author_names[i]) + 1 , sizeof(char));
        DIE(author->name == NULL, "author->name");

        // Institution
        author->org = calloc(strlen(institutions[i]) + 1, sizeof(char));
        DIE(author->org == NULL, "author->org");
    }

    // Fields    
    publication->fields = calloc(num_fields, sizeof(char *));
    DIE(publication->fields == NULL, "publication->fields");

    for (int i = 0; i < num_fields; i++) {
        publication->fields[i] = calloc(strlen(fields[i]) + 1, sizeof(char));
        DIE(publication->fields[i] == NULL, "publication->fields[i]");
    }

    // References
    publication->references = calloc(num_refs, sizeof(int64_t));
    DIE(publication->references == NULL, "publication->references");
}

PublData* init_publ_data(void) {
    PublData *data = calloc(1, sizeof(PublData));
    DIE(data == NULL, "malloc - data");

    // Initialising hashtable
    data->buckets = calloc(HMAX, sizeof(Info *));
    DIE(data->buckets == NULL, "data->buckets malloc");

    for (int i = 0; i < HMAX; i++) {
        data->buckets[i] = NULL;
    }

    data->hmax = HMAX;
    data->hash_function = hash_function_int;
    data->compare_function = compare_function_ints;
    return data;
}

void destroy_hashtable(PublData *ht) {
  if (ht == NULL) {
    return;
  }

  for (int i = 0; i < ht->hmax; i++) {
      free(ht->buckets[i]);
  }

  free(ht->buckets);
  free(ht);
}

void destroy_info(Info *publication) {
    // Title
    free(publication->title);
    free(publication->venue);
    
    // Authors
    for (int i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i]; 
        free(author->name);
        free(author->org);
        free(author);
    }
    free(publication->authors);

    // Fields
    for (int i = 0; i < publication->num_fields; i++) {
        free(publication->fields[i]);
    }
    free(publication->fields);

    // References
    free(publication->references);
}

void destroy_publ_data(PublData* data) {
    for (int i = 0; i < data->hmax; i++) {
        if (data->buckets[i]) {
            destroy_info(data->buckets[i]);
        }
    }

    destroy_hashtable(data);
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, int64_t id, const int64_t* references,
    const int num_refs) {

    unsigned int hash = data->hash_function(&id) % data->hmax;
    printf("hash = %ld\n", hash);
    int index;

    for (int i = 0; i < data->hmax; i++) {
        index = (hash + i) % data->hmax;
        Info *bucket = data->buckets[index];
        if (!bucket) {
            break;
        }
    }

    // Put-ul
    data->buckets[index] = calloc(1, sizeof(Info));
    Info *publication = data->buckets[index];
    init_info(publication, title, venue, year, author_names, author_ids, institutions, num_authors, fields, num_fields, id, references, num_refs);

    // Basic info
    memcpy(publication->title, title, (strlen(title) + 1) * sizeof(char));
    memcpy(publication->venue, venue, (strlen(venue) + 1) * sizeof(char));
    publication->year = year;

    publication->num_authors = num_authors;

    // Authors
    for (int i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i];
        memcpy(author->name, author_names[i], (strlen(author_names[i]) + 1) * sizeof(char));
        author->id = author_ids[i];
        memcpy(author->org, institutions[i], (strlen(institutions[i]) + 1) * sizeof(char));
    }

    // Fields
    publication->num_fields = num_fields;
    for (int i = 0; i < publication->num_fields; i++) {
        memcpy(publication->fields[i], fields[i], (strlen(fields[i]) + 1) * sizeof(char));
    }

    publication->id = id;
    publication->num_refs = num_refs;

    for (int i = 0; i < num_refs; i++) {
        publication->references[i] = references[i];
    }
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

void print_entry(PublData *data, int hash) {
    printf("Title = %s\n", data->buckets[hash]->title);
    printf("Year = %d\n", data->buckets[hash]->year);
    printf("First author name = %s\n", data->buckets[hash]->authors[0]->name);
    printf("Second author name = %s\n", data->buckets[hash]->authors[1]->name);
    printf("Second author ID = %lld\n", data->buckets[hash]->authors[1]->id);
    printf("Third reference = %lld\n", data->buckets[hash]->references[2]);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
}