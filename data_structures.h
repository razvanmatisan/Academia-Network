#ifndef DATA_STRUCTURES_H_
#define DATA_STRUCTURES_H_

#include <stddef.h>

#define HMAX 10000
#define FIRST_CITATION 1
#define LEN_TITLE 200 

/* LinkedList */
struct Node {
    void *data;
    struct Node *next;
};

struct LinkedList {
    struct Node *head;
    struct Node *tail;
    int size;
};

void init_list(struct LinkedList *list);

void add_last_node(struct LinkedList *list, void *new_data);

struct Node* remove_first_node(struct LinkedList *list);

int get_size(struct LinkedList *list);

void free_info_list(struct LinkedList **list);

/* Queue */
struct Queue {
    struct LinkedList *list;
};

void init_q(struct Queue *q);

int get_size_q(struct Queue *q);

int is_empty_q(struct Queue *q);

void* front(struct Queue *q);

void dequeue(struct Queue *q);

void enqueue(struct Queue *q, void *new_data);

void clear_q(struct Queue *q);

void purge_q(struct Queue *q);

/* Papers Hashtable */
unsigned int hash_function_int(void *a);

int compare_function_ints(void *a, void *b);

/* Citations Hashtable
 * Key - ID
 * Value - No. Citations
 * Method - Linear Probing
 */
typedef struct cited_paper {
    int64_t *id;
    int citations;
} cited_paper;

typedef struct Citations_HT {
    struct LinkedList *buckets; /* Array of simply-linked buckets */ 
    int hmax;
    unsigned int (*hash_function)(void *);
    int (*compare_function)(void *, void *);
} Citations_HT;


typedef struct venue_paper {
    char *venue;
    int64_t id;
} venue_paper;

typedef struct Venue_HT {
    struct LinkedList *buckets; /* Array of simply-linked buckets */ 
    int hmax;
    unsigned int (*hash_function)(void *);
    int (*compare_function)(void *, void *);
} Venue_HT;


typedef struct field_paper {
    char *field;
    int64_t id;
} field_paper;

typedef struct Field_HT {
    struct LinkedList *buckets; /* Array of simply-linked buckets */ 
    int hmax;
    unsigned int (*hash_function)(void *);
    int (*compare_function)(void *, void *);
} Field_HT;

void init_cit_ht(Citations_HT *ht);

void add_citation(Citations_HT *ht, int64_t cited_paper_id);

int get_no_citations(Citations_HT *ht, int64_t paper_id);

void free_cit_ht(Citations_HT *ht);


void init_venue_ht(Venue_HT *ht);

void add_venue(Venue_HT *ht, char *venue, int64_t id);

void free_venue_ht(Venue_HT *ht);


void init_field_ht(Field_HT *ht);

void add_field(Field_HT *ht, char *field, int64_t id);

void free_field_ht(Field_HT *ht);

#endif /* DATA_STRUCTURES_H_ */
