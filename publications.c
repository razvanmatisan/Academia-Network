#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "publications.h"
#include "data_structures.h"

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

    int ok; // "Visited" mark
};

struct publications_data {
    struct LinkedList **buckets;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void *, void *);

    Citations_HT *citations_ht;
};

void init_info(Info *publication, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, int64_t id, const int64_t* references,
    const int num_refs) {
    int i;

    // Title
    publication->title = calloc(strlen(title) + 1, sizeof(char));
    DIE(publication->title == NULL, "publication->title");

    // Venue
    publication->venue = calloc(strlen(venue) + 1, sizeof(char));
    DIE(publication->venue == NULL, "publication->venue");    

    // Authors
    publication->authors = calloc(num_authors, sizeof(Author *));
    DIE(publication->authors == NULL, "publication->authors");

    for (i = 0; i < num_authors; i++) {
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

    for (i = 0; i < num_fields; i++) {
        publication->fields[i] = calloc(strlen(fields[i]) + 1, sizeof(char));
        DIE(publication->fields[i] == NULL, "publication->fields[i]");
    }

    // References
    publication->references = calloc(num_refs, sizeof(int64_t));
    DIE(publication->references == NULL, "publication->references");

    // Auxiliary fields
    publication->ok = 0;
}

PublData* init_publ_data(void) {
    PublData *data = calloc(1, sizeof(PublData));
    DIE(data == NULL, "malloc - data");
    int i;

    // Initialising data hashtable
    data->buckets = calloc(HMAX, sizeof(struct LinkedList *));
    DIE(data->buckets == NULL, "data->buckets malloc");

    data->hmax = HMAX;
    data->hash_function = hash_function_int;
    data->compare_function = compare_function_ints;

    for (i = 0; i < data->hmax; i++) {
        data->buckets[i] = calloc(1, sizeof(struct LinkedList));
        DIE(data->buckets[i] == NULL, "data->buckets[i] malloc");

        init_list(data->buckets[i]);
    }
    
    // Initializing citations hashtable
    data->citations_ht = calloc(1, sizeof(Citations_HT));
    DIE(data->citations_ht == NULL, "data->citations_ht calloc");
    init_cit_ht(data->citations_ht);    

    return data;
}

void destroy_info(Info *publication) {
    int i;
    
    // Title
    free(publication->title);
    free(publication->venue);
    
    // Authors
    for (i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i]; 
        free(author->name);
        free(author->org);
        free(author);
    }
    free(publication->authors);

    // Fields
    for (i = 0; i < publication->num_fields; i++) {
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

    // Freeing citations ht
    free_cit_ht(data->citations_ht);

    // Freeing info buckets
    int i;
    for (i = 0; i < data->hmax; i++) {
        free_list(&data->buckets[i]);
    }
    free(data->buckets);


    // Freeing PublData as a whole
    free(data);
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, int64_t id, const int64_t* references,
    const int num_refs) {
    if (data == NULL) {
        return;
    }

    unsigned int hash = data->hash_function(&id) % data->hmax;
    int i;

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
    for (i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i];

        memcpy(author->name, author_names[i], (strlen(author_names[i]) + 1) * sizeof(char));
        author->id = author_ids[i];
        memcpy(author->org, institutions[i], (strlen(institutions[i]) + 1) * sizeof(char));
    }

    // Fields
    publication->num_fields = num_fields;
    for (i = 0; i < publication->num_fields; i++) {
        memcpy(publication->fields[i], fields[i], (strlen(fields[i]) + 1) * sizeof(char));
    }

    publication->id = id;
    publication->num_refs = num_refs;

    for (i = 0; i < num_refs; i++) {
        publication->references[i] = references[i];
        add_citation(data->citations_ht, references[i]);
    }

    // Package & Send
    add_nth_node(data->buckets[hash], data->buckets[hash]->size, publication);
}

void free_ok_data(PublData *data) {
    int i;

    for (i = 0; i < data->hmax; i++) {
        struct Node *curr = data->buckets[i]->head;
        while (curr) {
            Info *publication = (Info *) curr->data;
            publication->ok = 0;
            curr = curr->next;
        }
    }
}

/* > 0 --> older influence found */
int compare_task1 (PublData *data, Info *challenger, Info *titleholder) {
    if (challenger == NULL || titleholder == NULL || challenger->id == titleholder->id) {
        return 0;
    }

    if (challenger->year != titleholder->year) {
        return titleholder->year - challenger->year;
    } else {
        int challenger_citations = get_no_citations(data->citations_ht, challenger->id);
        int titleholder_citations = get_no_citations(data->citations_ht, titleholder->id);
        if(challenger_citations != titleholder_citations) {
            return challenger_citations - titleholder_citations;
        } else {
            return titleholder->id - challenger->id;
        }
    }
}

/* TODO: update to chaining */
char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    // Initializing variables
    int i;
    struct Node *curr;
    Info *publication, *vertex;
    Info *oldest_influence = NULL;

    struct Queue *q = malloc(sizeof(struct Queue));
    init_q(q);
    
    /* 
     * Enqueing the given paper
     * Marking it as visited
     */
    unsigned int hash = data->hash_function(&id_paper) % data->hmax;
    curr = data->buckets[hash]->head;

    while (curr) {
        publication = (Info *) curr->data;
        if (publication->id == id_paper) {
            enqueue(q, publication);
            publication->ok = 1;
            break;
        }
        curr = curr->next;
    }

    // BFS-style search
    while (!is_empty_q(q)) {
        vertex = (Info *)front(q);

        // Checking if the vertex is an older influence
        if(!oldest_influence && vertex->id != id_paper) {
            // First influence found
            oldest_influence = vertex;
        } else if(compare_task1(data, vertex, oldest_influence) > 0) {
            // Older influence found
            oldest_influence = vertex;
        }

        // Searching for further references through the vertex's references
        for (i = 0; i < vertex->num_refs; i++) {
            hash = data->hash_function(&vertex->references[i]) % data->hmax;
            curr = data->buckets[hash]->head;

            while (curr) {
                publication = (Info *) curr->data;
                if (publication->id == vertex->references[i]) {
                    if (!publication->ok) {
                        // Unvisited reference found
                        enqueue(q, publication);
                        publication->ok = 1;
                    }
                    break;
                }
                curr = curr->next;
            }
        }

        // Done with current vertex
        dequeue(q);
    }

    // Freeing allocated memory
    free_ok_data(data);
    purge_q(q);
    free(q);

    if (oldest_influence) {
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

/* DONE */
int get_number_of_papers_between_dates(PublData* data, const int early_date,
    const int late_date) {
    int i;
    int cnt = 0;

    /*
     * Iterating through the whole hashtable looking for papers between
     * the given dates
     */

    for (i = 0; i < data->hmax; i++) {
        struct Node *curr = data->buckets[i]->head;
        while (curr) {
            Info *publication = curr->data;
            // Paper published between the given dates     
            if (publication->year >= early_date && publication->year <= late_date) {
                cnt++;
            }
            // Going further down the LinkedList
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