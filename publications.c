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
    int ok;
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

void init_info(Info *publication) {
    /*
    * Initialising INFO element when added
    * 
    * Allocate char arrays with MAX_LEN size.
    * If size of input exceeds allocated size, we will reallocate.
    * 
    * All values MEMSETED
    */

    // Title
    publication->title = malloc(MAX_LEN * sizeof(char));
    DIE(publication->title == NULL, "publication->title");
    memset(publication->title, '\0', MAX_LEN * sizeof(char));

    // Venue
    publication->venue = malloc(MAX_LEN * sizeof(char));
    DIE(publication->venue == NULL, "publication->venue");    
    memset(publication->venue, '\0', MAX_LEN * sizeof(char));

    // Authors
    publication->authors = malloc(MAX_AUTHORS * sizeof(Author *));
    DIE(publication->authors == NULL, "publication->authors");

    for (int i = 0; i < MAX_AUTHORS; i++) {
        publication->authors[i] = malloc(sizeof(Author));
        DIE(publication->authors[i] == NULL, "publications->authors[i]");
        memset(publication->authors[i], '\0', sizeof(Author));

        Author *author = publication->authors[i];

        // Name
        author->name = malloc(MAX_LEN * sizeof(char));
        DIE(author->name == NULL, "author->name");
        memset(author->name, '\0', MAX_LEN * sizeof(char));

        // Institution
        author->org = malloc(MAX_LEN * sizeof(char));
        DIE(author->org == NULL, "author->org");
        memset(author->org, '\0', MAX_LEN * sizeof(char));
    }

    // Fields    
    publication->fields = malloc(MAX_FIELDS * sizeof(char *));
    DIE(publication->fields == NULL, "publication->fields");

    for (int i = 0; i < MAX_FIELDS; i++) {
        publication->fields[i] = malloc(MAX_LEN * sizeof(char));
        DIE(publication->fields[i] == NULL, "publication->fields[i]");
        memset(publication->fields[i], '\0', MAX_LEN * sizeof(char));
    }

    // References
    publication->references = malloc(MAX_REFERENCES * sizeof(int64_t));
    DIE(publication->references == NULL, "publication->references");
    memset(publication->references, '\0', MAX_REFERENCES * sizeof(int64_t));
}

PublData* init_publ_data(void) {
    PublData *data = malloc(sizeof(PublData));
    DIE(data == NULL, "malloc - data");
    memset(data, '\0', sizeof(PublData));

    // Initialising hashtable
    data->buckets = malloc(HMAX * sizeof(Info *));
    DIE(data->buckets == NULL, "data->buckets malloc");

    for (int i = 0; i < HMAX; i++) {
        data->buckets[i] = malloc(sizeof(Info));
        memset(data->buckets[i], '\0', sizeof(Info));
        data->buckets[i]->ok = 0;
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
      if(ht->buckets[i]->ok) destroy_info(ht->buckets[i]);
      free(ht->buckets[i]);
  }

  free(ht->buckets);
  free(ht);
}

void destroy_info(Info *publication) {
    // Title
    free(publication->title);
    // free(publication->venue);
    
    // Authors
    for (int i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i]; 
        // free(author->name);
        // free(author->org);
        // free(author);
    }
    // free(publication->authors);

    // Fields
    for (int i = 0; i < publication->num_fields; i++) {
        // free(publication->fields[i]);
    }
    // free(publication->fields);

    // References
    // free(publication->references);
}

void destroy_publ_data(PublData* data) {
    for (int i = 0; i < data->hmax; i++) {
        if (data->buckets[i]->ok) {
            // destroy_info(data->buckets[i]);
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

    int index;

    for (int i = 0; i < data->hmax; i++) {
        index = (hash + i) % data->hmax;
        Info *bucket = data->buckets[index];
        if (!bucket->ok) {
            bucket->ok = 1;
            break;
        }
    }

    // Put-ul
    Info *publication = data->buckets[index];
    init_info(publication);

    // Basic info
    memcpy(publication->title, title, (strlen(title) + 1) * sizeof(char));
    memcpy(publication->venue, venue, (strlen(venue) + 1) * sizeof(char));
    memcpy(publication->year, year, sizeof(const int));

    memcpy(publication->num_authors, num_authors, sizeof(const int));

    // Authors
    for (int i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i];
        memcpy(author->name, author_names[i], (strlen(author_names[i]) + 1) * sizeof(char));
        memcpy(author->id, author_ids[i], sizeof(const int64_t));
        memcpy(author->org, institutions[i], (strlen(institutions[i]) + 1) * sizeof(char));
    }

    // Fields
    publication->num_fields = num_fields;
    for (int i = 0; i < publication->num_fields; i++) {
        memcpy(publication->fields[i], fields[i], (strlen(fields[i]) + 1) * sizeof(char));
        publication->fields[i] = fields[i];
    }

    memcpy(publication->id, id, sizeof(const int64_t));
    memcpy(publication->num_refs, num_refs, sizeof(const int));

    for (int i = 0; i < num_refs; i++) {
        memcpy(publication->references[i], references[i], sizeof(const int64_t));
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
