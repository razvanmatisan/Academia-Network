#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "publications.h"
#include "data_structures.h"
#include "LinkedList.h"
#include "Queue.h"
#include "utils.h"

#define CURR_YEAR 2020
#define MAX_YEAR 5000
#define INITIAL_HISTOGRAM_SIZE 1

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
    publication->distance = -1;
    publication->citations = 0;
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
    
    // Initializing MINI-hashtables :))
    data->citations_ht = calloc(1, sizeof(Citations_HT));
    DIE(data->citations_ht == NULL, "data->citations_ht calloc");
    init_cit_ht(data->citations_ht);

    data->venue_ht = calloc(1, sizeof(Venue_HT));
    DIE(data->venue_ht == NULL, "data->venue_ht calloc");
    init_venue_ht(data->venue_ht);

    data->field_ht = calloc(1, sizeof(Field_HT));
    DIE(data->field_ht == NULL, "data->field_ht calloc");
    init_field_ht(data->field_ht);

    data->authors_ht = calloc(1, sizeof(Authors_HT));
    DIE(data->authors_ht == NULL, "data->authors_ht calloc");
    init_authors_ht(data->authors_ht);

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

    // Freeing info buckets
    int i;
    for (i = 0; i < data->hmax; i++) {
        free_info_list(&data->buckets[i]);
    }
    free(data->buckets);

    // Freeing MINI-hashtables :))
    free_cit_ht(data->citations_ht);
    free_venue_ht(data->venue_ht);
    free_field_ht(data->field_ht);
    free_author_ht(data->authors_ht);

    // Freeing PublData as a whole
    free(data);
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, int64_t id, const int64_t* references,
    const int num_refs) {
    unsigned int hash = data->hash_function(&id) % data->hmax;
    int i;

    if (data == NULL) {
        return;
    }

    // Initializing data
    Info *publication = calloc(1, sizeof(Info));
    init_info(publication, title, venue, year, author_names, author_ids, institutions, num_authors, fields, num_fields, id, references, num_refs);

    // Baisc info
    memcpy(publication->title, title, (strlen(title) + 1) * sizeof(char));

    memcpy(publication->venue, venue, (strlen(venue) + 1) * sizeof(char));
    add_venue(data->venue_ht, publication->venue, id);

    publication->year = year;

    // No. authors
    publication->num_authors = num_authors;

    for (i = 0; i < publication->num_authors; i++) {
        Author *author = publication->authors[i];

        // Author info
        memcpy(author->name, author_names[i], (strlen(author_names[i]) + 1) * sizeof(char));

        author->id = author_ids[i];
        add_author(data->authors_ht, author_ids[i], id, year);

        memcpy(author->org, institutions[i], (strlen(institutions[i]) + 1) * sizeof(char));
    }

    // Fields
    publication->num_fields = num_fields;
    for (i = 0; i < publication->num_fields; i++) {
        memcpy(publication->fields[i], fields[i], (strlen(fields[i]) + 1) * sizeof(char));
        add_field(data->field_ht, publication->fields[i], id);
    }

    publication->id = id;
    publication->num_refs = num_refs;

    for (i = 0; i < num_refs; i++) {
        publication->references[i] = references[i];
        add_citation(data->citations_ht, references[i]);
    }

    // Package & Send
    add_last_node(data->buckets[hash], publication);
}




/* DONE */
char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    // Initializing variables
    int i;
    unsigned int hash;
    struct Node *curr;
    Info *publication, *vertex;
    Info *oldest_influence = NULL;

    struct Queue *q = malloc(sizeof(struct Queue));
    init_q(q);
    
    /* 
     * Enqueing the given paper
     * Marking it as visited
     */

    Info *starting_paper = find_paper_with_id(data, id_paper);
    enqueue(q, starting_paper);
    starting_paper->ok = 1;

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
    free_aux_data(data, starting_paper);
    purge_q(q);
    free(q);

    if (oldest_influence) {
        return oldest_influence->title;
    }

    return "None";
}

/* DONE */
float get_venue_impact_factor(PublData* data, const char* venue) {
    /* TODO: implement get_venue_impact_factor */

    long x = 0;
    int cnt = 0;

    char *copy_venue = calloc(strlen(venue) + 1, sizeof(char));
    DIE(copy_venue == NULL, "copy_venue calloc");

    memcpy(copy_venue, venue, (strlen(venue) + 1) * sizeof(char));

    unsigned int hash = data->venue_ht->hash_function(copy_venue) % data->venue_ht->hmax;
    struct Node *curr = data->venue_ht->buckets[hash].head;

    while (curr) {
        venue_paper *publication = (venue_paper *) curr->data;

        if (!strcmp(publication->venue, venue)) {
            int cits = get_no_citations(data->citations_ht, publication->id);
            x += cits;
            cnt++;
        }

        curr = curr->next;
    }

    free(copy_venue);

    if (cnt) {
        return (float) x / cnt; 
    }

    return 0.f;
}

/* TODO: implement get_number_of_influenced_papers */
int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {

    int i;
    unsigned int hash;
    struct Node *curr;
    Info *publication, *vertex;

    int cnt = 0;
    int curr_distance = 0;

    struct Queue *q = malloc(sizeof(struct Queue));
    init_q(q);

    /* 
     * Enqueing the given paper
     * Marking it as visited
     */
    Info *starting_paper = find_paper_with_id(data, id_paper);
    enqueue(q, starting_paper);
    starting_paper->ok = 1;
    starting_paper->distance = curr_distance;

    while(curr_distance <= distance || !is_empty_q(q)) {
        vertex = (Info *)front(q);
        dequeue(q);

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
                        publication->distance = curr_distance + 1;
                        cnt++;
                    }
                    break;
                }
                curr = curr->next;
            }
        }
        vertex = (Info *) front(q);
        if (vertex == NULL) {
            break;
        }
        curr_distance = vertex->distance;
    }

    purge_q(q);
    free(q);
    free_aux_data(data, starting_paper);

    return cnt;
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    /* TODO: implement get_erdos_distance */

    return -1;
}

/* ------------------------ Taskul 5 -------------------------- */

char** get_most_cited_papers_by_field(PublData* data, const char* field,
    int* num_papers) {
    /* TODO: implement get_most_cited_papers_by_field */

    int64_t ids_with_field[NMAX];
    int no_ids = no_papers_with_field(data->field_ht, field, ids_with_field);

    if (!no_ids) {
        return NULL;
    }

    if (no_ids < *(int *)num_papers) {
        *num_papers = no_ids;
    }

    int num = *(int *)num_papers;

    char **titles = calloc(num, sizeof(char *));
    for (int i = 0; i < num; i++) {
        titles[i] = calloc(LEN_TITLE, sizeof(char));
    }

    /* Sorting */
    for (int i = 0; i < no_ids - 1; i++) {
        Info *publication1 = find_paper_with_id(data, ids_with_field[i]);
        for (int j = i + 1; j < no_ids; j++) {
            Info *publication2 = find_paper_with_id(data, ids_with_field[j]);
            if (compare_task5(data, publication1, publication2) < 0) {
                swap(&ids_with_field[i], &ids_with_field[j]);
            }
        }
    }

    for (int i = 0; i < num; i++) {
        Info *publication = find_paper_with_id(data, ids_with_field[i]);
        memcpy(titles[i], publication->title, (strlen(publication->title) + 1) * sizeof(char));
    }

    return titles;
}


/* ---------------------------- Taskul 6 ------------------------- */
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

/* ------------------  Taskul 7  ---------------------------------*/

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    /* TODO: implement get_number_of_authors_with_field */
    int cnt = 0;

    int64_t ids_with_field[NMAX];
    int no_ids = no_papers_with_field(data->field_ht, field, ids_with_field);

    char **author_names = calloc(MAX_AUTHORS, sizeof(char *));
    DIE(author_names == NULL, "author_names malloc");

    for (int i = 0; i < MAX_AUTHORS; i++) {
        author_names[i] = calloc(LEN_NAME, sizeof(char));
        DIE(author_names[i] == NULL, "author_names[i] malloc");
    }

    for (int i = 0; i < no_ids; i++) {
        Info *publication = find_paper_with_id(data, ids_with_field[i]);
        for (int j = 0; j < publication->num_authors; j++) {
            Author *author = publication->authors[j];
            if (!strcmp(author->org, institution) && !is_in_array(author->name, author_names, cnt + 1)) {
                memcpy(author_names[cnt], author->name, (strlen(author->name) + 1) * sizeof(char));
                cnt++;
            }
        }
    }

    for (int i = 0; i < MAX_AUTHORS; i++) {
        free(author_names[i]);
    }
    free(author_names);

    return cnt;
}

/* TODO: implement get_histogram_of_citations */
int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {
    // Initializing variables
    int *histogram = calloc(INITIAL_HISTOGRAM_SIZE, sizeof(int)); // realloc la nevoie cu smart memsert
    DIE(histogram == NULL, "histogram calloc");
    
    *num_years = INITIAL_HISTOGRAM_SIZE;
    int min_year = MAX_YEAR;

    Authors_HT *ht = data->authors_ht;
    unsigned int hash = ht->hash_function(&id_author) % ht->hmax;
    struct Node *curr = ht->buckets[hash].head;

    while (curr) {
        authors_paper *publication = (authors_paper *) curr->data;
        if(ht->compare_function(publication->author_id, &id_author) == 0) {
            if (publication->paper_year < min_year) {
                // Updating minimum year 
                min_year = publication->paper_year;
                // Updating num_years (histogram size)
                int prev_size = *num_years;
                *num_years = CURR_YEAR - min_year + 1;

                // Resizing histogram
                histogram = realloc(histogram, *num_years * sizeof(int));
                DIE(histogram == NULL, "histogram realloc");
                
                // Memsetting new counts
                memset(histogram + prev_size, 0, (*num_years - prev_size) * sizeof(int)); 
            }
            // Adding current paper's citations in its bin
            int bin = CURR_YEAR - publication->paper_year;
            int no_citations = get_no_citations(data->citations_ht, publication->paper_id);
            histogram[bin] += no_citations;
        }
        curr = curr->next;
    }

    return histogram;
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

