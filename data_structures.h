#ifndef DATA_STRUCTURES_H_
#define DATA_STRUCTURES_H_

#include <stddef.h>
#include <stdint.h>

#define HMAX_BIG 5003
#define HMAX_SMALL 503
#define FIRST_CITATION 1
#define LEN_TITLE 300
#define LEN_NAME 300
#define MAX_AUTHORS 100
#define NMAX 20000
#define UNVISITED 0

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

struct Node *remove_first_node(struct LinkedList *list);

int get_size(struct LinkedList *list);

void free_info_list(struct LinkedList **list);

/* Queue */
struct Queue {
  struct LinkedList *list;
};

void init_q(struct Queue *q);

int get_size_q(struct Queue *q);

int is_empty_q(struct Queue *q);

void *front(struct Queue *q);

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
 * Method - Direct Chaining
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

void init_cit_ht(Citations_HT *ht);

void add_citation(Citations_HT *ht, int64_t cited_paper_id);

int get_no_citations(Citations_HT *ht, int64_t paper_id);

void free_cit_ht(Citations_HT *ht);

/* Venue Hashtable
 * Key - Venue
 * Value - Papers published at that venue
 * Method - Direct Chaining
 */
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

void init_venue_ht(Venue_HT *ht);

void add_venue(Venue_HT *ht, char *venue, int64_t id);

void free_venue_ht(Venue_HT *ht);

/* Field Hashtable
 * Key - Field
 * Value - Papers published wihtin that field
 * Method - Direct Chaining
 */
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

void init_field_ht(Field_HT *ht);

void add_field(Field_HT *ht, char *field, int64_t id);

void free_field_ht(Field_HT *ht);

/* Authors Hashtable
 * Key - Author ID
 * Value - Papers published by that author (represented by their ID)
 * Method - Direct Chaining
 */
typedef struct authors_paper {
  int64_t *author_id;
  int64_t paper_id; //
  int paper_year;   // these two could be unified
} authors_paper;

typedef struct Authors_HT {
  struct LinkedList *buckets; /* Array of simply-linked buckets */
  int hmax;
  unsigned int (*hash_function)(void *);
  int (*compare_function)(void *, void *);
} Authors_HT;

void init_authors_ht(Authors_HT *ht);

void add_author(Authors_HT *ht, int64_t author_id, int64_t paper_id,
                int paper_year);

void free_author_ht(Authors_HT *ht);

/* Influenced Papers Hashtable
 * Key - Paper X (ID)
 * Value - Papers which X influences (IDs)
 * Method - Direct Chaining
 */
typedef struct influenced_paper {
  int64_t *key;  // influencer paper id
  int64_t value; // influenced paper id
} influenced_paper;

typedef struct Influence_HT {
  struct LinkedList *buckets; /* Array of simply-linked buckets */
  int hmax;
  unsigned int (*hash_function)(void *);
  int (*compare_function)(void *, void *);
} Influence_HT;

void init_influence_ht(Influence_HT *ht);

void add_influence(Influence_HT *ht, int64_t influencer_id,
                   int64_t imitator_id);

void free_influence_ht(Influence_HT *ht);

/* Markings Hashtable
 * Key - Paper ID
 * Value(s) - visited status & distance to origin
 * Method - Direct Chaining
 */
typedef struct marking {
  int64_t *id;
  int visited;
  int distance;
} marking;

typedef struct Markings_HT {
  struct LinkedList *buckets; /* Array of simply-linked buckets */
  int hmax;
  unsigned int (*hash_function)(void *);
  int (*compare_function)(void *, void *);
} Markings_HT;

void init_markings_ht(Markings_HT *ht);

void add_marking(Markings_HT *ht, int64_t paper_id, int new_distance);

marking *get_markings(Markings_HT *ht, int64_t paper_id);

void free_markings_ht(Markings_HT *ht);

#endif /* DATA_STRUCTURES_H_ */
