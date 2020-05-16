#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "publications.h"
#include "data_structures.h"

#define HMAX 120000

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
    struct LinkedList **buckets;
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
    data->buckets = calloc(HMAX, sizeof(struct LinkedList *));
    DIE(data->buckets == NULL, "data->buckets malloc");

    for (int i = 0; i < HMAX; i++) {
        data->buckets[i] = calloc(1, sizeof(struct LinkedList));
        DIE(data->buckets[i] == NULL, "data->buckets[i] malloc");

        init_list(data->buckets[i]);
    }

    data->hmax = HMAX;
    data->hash_function = hash_function_int;
    data->compare_function = compare_function_ints;
    return data;
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
    free(publication);
}

void destroy_publ_data(PublData* data) {
    if (data == NULL) {
       return;
    }

    for (int i = 0; i < data->hmax; i++) {
        free_list(&data->buckets[i]);
    }

    free(data->buckets);
    free(data);
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, int64_t id, const int64_t* references,
    const int num_refs) {

    unsigned int hash = data->hash_function(&id) % data->hmax;

    int index;

    for (int i = 0; i < data->hmax; i++) {
        
    }  

    // Initializing data
    Info *publication = calloc(1, sizeof(Info));
    init_info(publication, title, venue, year, author_names, author_ids, institutions, num_authors, fields, num_fields, id, references, num_refs);

    // COPYING DATA

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
    publication->ok = 0;
    
    add_nth_node(data->buckets[hash], data->buckets[hash]->size, publication);
}





void free_ok_data(PublData *data) {
    for (int i = 0; i < data->hmax; i++) {
        struct Node *curr = data->buckets[i]->head;
        while (curr) {
            Info *publication = (Info *) curr->data;
            publication->ok = 0;
            curr = curr->next;
        }
    }
}

int nr_refs (PublData *data, int64_t id_paper) {
    int cnt = 0;

    for (int i = 0; i < data->hmax; i++) {
        struct Node *curr = data->buckets[i]->head;
        while (curr) {
            Info *publication = (Info *) curr->data;
            for (int j = 0; j < publication->num_refs; j++) {
                if (publication->references[j] == id_paper) {
                    cnt++;
                    //printf("Yeah\n");
                    break;
                }
            }
            curr = curr->next;
        }
    }

    return cnt;
}

// > 0 --> ok!
int compare_task1 (PublData *data, Info *publication1, Info *publication2) {
    if (publication1 == NULL || publication2 == NULL || publication1->id == publication2->id) {
        return 0;
    }

    if (publication1->year != publication2->year) {
        return publication2->year - publication1->year;
    } else {
        int nr_refs1 = nr_refs(data, publication1->id);
        int nr_refs2 = nr_refs(data, publication2->id);
        if (nr_refs1 != nr_refs2) {
            return nr_refs1 - nr_refs2;
        } else {
            return publication2->id - publication1->id;
        }
    }
}

char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    /* TODO: implement get_oldest_influence */

    Info *oldest_influence;

    struct Queue *q = malloc(sizeof(struct Queue));
    init_q(q);
    unsigned int hash = data->hash_function(&id_paper) % data->hmax;
    
    struct Node *curr = data->buckets[hash]->head;
    
    while (curr) {
        Info *publication = (Info *) curr->data;
        if (publication->id == id_paper) {
            enqueue(q, publication);
            publication->ok = 1;
            oldest_influence = publication;
            break;
        }
        curr = curr->next;
    }

    while (!is_empty_q(q)) {
        Info *vertex = (Info *)front(q);
        dequeue(q);

        if (compare_task1(data, vertex, oldest_influence) > 0) {
            oldest_influence = vertex;
        }

        for (int i = 0; i < vertex->num_refs; i++) {
            unsigned int hash = data->hash_function(&vertex->references[i]) % data->hmax;
    
            struct Node *curr = data->buckets[hash]->head;
            while (curr) {
                Info *publication = (Info *) curr->data;
                if (publication->id == vertex->references[i]) {
                    if (!publication->ok) {
                        enqueue(q, publication);
                        publication->ok = 1;
                    }
                    break;
                }
                curr = curr->next;
            }
        }
    }
    purge_q(q);
    free(q);

    free_ok_data(data);

    if (oldest_influence->id != id_paper) {
        return oldest_influence->title;
    }

    return "None";
}

float get_venue_impact_factor(PublData* data, const char* venue) {
    /* TODO: implement get_venue_impact_factor */

    return 0.f;
}

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    /* TODO: implement get_number_of_influenced_papers */

    // int cnt = 0;

    // unsigned int hash = data->hash_function(&id_paper) % data->hmax;
    
    // struct Node *curr = data->buckets[hash]->head;
    
    // while (curr) {
    //     Info *publication = (Info *) curr->data;
    //     if (publication->id == id_paper) {
    //         enqueue(q, publication);
    //         publication->ok = 1;
    //         oldest_influence = publication;
    //         break;
    //     }
    //     curr = curr->next;
    // }

    // return -1;
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
    int cnt = 0;
    
    for (int i = 0; i < data->hmax; i++) {
        struct Node *curr = data->buckets[i]->head;
        while (curr) {
            Info *publication = curr->data;
            
            if (publication->year >= early_date && publication->year <= late_date) {
                cnt++;
            }

            curr = curr->next;
        }
    }

    return cnt;
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
    struct Node *curr = data->buckets[hash]->head;
    
    while(curr) {
        Info *publication = curr->data;
        printf("Title = %s\n", publication->title);
        printf("Year = %d\n", publication->year);
        printf("First author name = %s\n", publication->authors[0]->name);
        printf("Second author name = %s\n", publication->authors[1]->name);
        printf("Second author ID = %lld\n", publication->authors[1]->id);
        printf("Third reference = %lld\n", publication->references[2]);
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
        curr = curr->next;
    }   
}