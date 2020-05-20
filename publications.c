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
    int citations;
    int distance; // Distance to the origin :)
};

struct publications_data {
    struct LinkedList **buckets;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void *, void *);

    Citations_HT *citations_ht;

    Venue_HT *venue_ht;
    Field_HT *field_ht;
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

    // COPYING DATA

    // Basic info
    memcpy(publication->title, title, (strlen(title) + 1) * sizeof(char));

    memcpy(publication->venue, venue, (strlen(venue) + 1) * sizeof(char));
    add_venue(data->venue_ht, publication->venue, id);

    publication->year = year;


    // Authors
    publication->num_authors = num_authors;

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

Info* find_paper_with_id(PublData *data, int64_t target_id) {
    unsigned int hash = data->hash_function(&target_id) % data->hmax;
    struct Node *curr = data->buckets[hash]->head;
    Info *publication;

    while (curr) {
        publication = (Info *)curr->data;
        if(publication->id == target_id) {
            return publication;
        }
        curr = curr->next;
    }

    // Nothing found
    return NULL;
}


/* DFS-Style breadcrum cleaing - cleaning strictly the path we've been up on */
void free_aux_data(PublData *data, Info *start_publication) {
    int i;

    // Freeing current publications'
    start_publication->ok = 0;
    start_publication->citations = 0;
    start_publication->distance = -1;

    for (i = 0; i < start_publication->num_refs; i++) {
        Info *curr_publication = find_paper_with_id(data, start_publication->references[i]);

        // If current neighbors is found (added in the HT)
        if (curr_publication) {
            if (curr_publication->ok) {
                // Freeing next's neighbors' aux data
                free_aux_data(data, curr_publication);
            }
        }  
    }
}

// > 0 --> older influence found
int compare_task1 (PublData *data, Info *challenger, Info *titleholder) {
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

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    /* TODO: implement get_number_of_influenced_papers */

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

int compare_task5 (PublData *data, Info *publication1, Info *publication2) {
    if (!publication1 || !publication2 || publication1->id == publication2->id) {
        return 0;
    }

    publication1->citations = get_no_citations(data->citations_ht, publication1->id);
    publication2->citations = get_no_citations(data->citations_ht, publication2->id);
    //printf("%d\n", publication1->citations);

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
            if (compare_task5(data, publication2, publication1) > 0) {
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
int is_in_array (char *arr_to_find, char **arr, int length) {
    for (int i = 0; i < length; i++) {
        if (!strcmp(arr[i], arr_to_find)) {
            return 1;
        }
    }
    return 0;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    /* TODO: implement get_number_of_authors_with_field */
    int cnt = 0;

    int64_t ids_with_field[NMAX];
    int no_ids = no_papers_with_field(data->field_ht, field, ids_with_field);

    char **author_names = calloc(NMAX, sizeof(char *));
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