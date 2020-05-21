// Copyright [2020] Razvan-Andrei Matisan, Radu-Stefan Minea

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Hashtables.h"
#include "./LinkedList.h"
#include "./publications.h"
#include "./utils.h"

void init_cit_ht(struct Citations_HT *ht) {
  if (ht == NULL) {
    return;
  }

  // Initializing citations HT
  ht->hmax = HMAX_BIG;
  ht->hash_function = hash_function_int;
  ht->compare_function = compare_function_ints;

  // Initializing buckets
  ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
  DIE(ht->buckets == NULL, "Citations_HT: ht->buckets");

  int i;
  for (i = 0; i < ht->hmax; i++) {
    init_list(&ht->buckets[i]);
  }
}

void add_citation(struct Citations_HT *ht, int64_t cited_paper_id) {
  if (ht == NULL) {
    return;
  }

  unsigned int hash = ht->hash_function(&cited_paper_id) % ht->hmax;
  struct LinkedList *bucket = &ht->buckets[hash];

  // Iterate through the bucket until keymatch
  struct Node *it = bucket->head;
  while (it) {
    struct cited_paper *inside_data = (struct cited_paper *)it->data;
    // Key match
    if (ht->compare_function(inside_data->id, &cited_paper_id) == 0) {
      // Already cited => updating count
      inside_data->citations++;
      return;
    }
    it = it->next;
  }

  // First citation => new element in bucket
  struct cited_paper *new_paper = malloc(sizeof(cited_paper));
  DIE(new_paper == NULL, "add_citations -> new_paper malloc");

  // Allocating memory for key
  new_paper->id = malloc(sizeof(cited_paper_id));
  DIE(new_paper->id == NULL, "paper->id");

  memcpy(new_paper->id, &cited_paper_id, sizeof(cited_paper_id)); // copying key
  new_paper->citations = FIRST_CITATION;

  // Add/chain => bascially appending to the current bucket
  add_last_node(bucket, new_paper);
}

int get_no_citations(Citations_HT *ht, int64_t paper_id) {
  if (ht == NULL) {
    return -1;
  }

  unsigned int hash = ht->hash_function(&paper_id) % ht->hmax;
  struct Node *it = ht->buckets[hash].head;

  while (it) {
    cited_paper *inside_data = (cited_paper *)it->data;
    // Key match
    if (ht->compare_function(inside_data->id, &paper_id) == 0) {
      return inside_data->citations;
    }
    it = it->next;
  }

  // Nothing found
  return 0;
}

void free_cit_ht(Citations_HT *ht) {
  if (ht == NULL) {
    return;
  }

  int i;
  for (i = 0; i < ht->hmax; i++) {
    struct LinkedList *bucket = &ht->buckets[i];
    struct Node *it = bucket->head;

    struct Node *prev;
    while (it != NULL) {
      prev = it;
      it = it->next;

      cited_paper *inside_data = (cited_paper *)prev->data;
      free(inside_data->id);
      free(inside_data);
      free(prev);
    }
  }

  free(ht->buckets);
  free(ht);
}

void init_venue_ht(Venue_HT *ht) {
  if (ht == NULL) {
    return;
  }

  // Initializing venue HT
  ht->hmax = HMAX_SMALL;
  ht->hash_function = hash_function_string;
  ht->compare_function = compare_function_strings;

  // Initializing buckets
  ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
  DIE(ht->buckets == NULL, "Venue_HT: ht->buckets");

  int i;
  for (i = 0; i < ht->hmax; i++) {
    init_list(&ht->buckets[i]);
  }
}

void add_venue(Venue_HT *ht, char *venue, int64_t id) {
  if (ht == NULL) {
    return;
  }

  unsigned int hash = ht->hash_function(venue) % ht->hmax;
  struct LinkedList *bucket = &ht->buckets[hash];

  venue_paper *new_paper = (venue_paper *)calloc(1, sizeof(venue_paper));
  DIE(new_paper == NULL, "add_venue -> new_paper malloc");

  // Allocating memory for key
  new_paper->venue = calloc(strlen(venue) + 1, sizeof(char));
  DIE(new_paper->venue == NULL, "new_paper->venue");

  memcpy(new_paper->venue, venue,
         (strlen(venue) + 1) * sizeof(char)); // copying key
  new_paper->id = id;

  // Add/chain => bascially appending to the current bucket
  add_last_node(bucket, new_paper);
}

void free_venue_ht(Venue_HT *ht) {
  if (ht == NULL) {
    return;
  }

  int i;
  for (i = 0; i < ht->hmax; i++) {
    struct LinkedList *bucket = &ht->buckets[i];
    struct Node *it = bucket->head;

    struct Node *prev;
    while (it != NULL) {
      prev = it;
      it = it->next;

      venue_paper *inside_data = (venue_paper *)prev->data;
      free(inside_data->venue);
      free(inside_data);
      free(prev);
    }
  }

  free(ht->buckets);
  free(ht);
}

void init_field_ht(struct Field_HT *ht) {
  if (ht == NULL) {
    return;
  }

  // Initializing field HT
  ht->hmax = HMAX_SMALL;
  ht->hash_function = hash_function_string;
  ht->compare_function = compare_function_strings;

  // Initializing buckets
  ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
  DIE(ht->buckets == NULL, "Field_HT: ht->buckets");

  int i;
  for (i = 0; i < ht->hmax; i++) {
    init_list(&ht->buckets[i]);
  }
}

void add_field(Field_HT *ht, char *field, int64_t id) {
  if (ht == NULL) {
    return;
  }

  unsigned int hash = ht->hash_function(field) % ht->hmax;
  struct LinkedList *bucket = &ht->buckets[hash];

  field_paper *new_paper = (field_paper *)malloc(sizeof(field_paper));
  DIE(new_paper == NULL, "add_field -> new_paper malloc");

  // Allocating memory for key
  new_paper->field = (char *)calloc(strlen(field) + 1, sizeof(char));
  DIE(new_paper->field == NULL, "new_paper->field");

  // long size = sizeof(field);
  memcpy(new_paper->field, field,
         (strlen(field) + 1) * sizeof(char)); // copying key
  new_paper->id = id;

  // Add/chain => bascially appending to the current bucket
  add_last_node(bucket, new_paper);
}

void free_field_ht(Field_HT *ht) {
  if (ht == NULL) {
    return;
  }

  int i;
  for (i = 0; i < ht->hmax; i++) {
    struct LinkedList *bucket = &ht->buckets[i];
    struct Node *it = bucket->head;

    struct Node *prev;
    while (it != NULL) {
      prev = it;
      it = it->next;

      field_paper *inside_data = (field_paper *)prev->data;
      free(inside_data->field);
      free(inside_data);
      free(prev);
    }
  }

  free(ht->buckets);
  free(ht);
}

void init_authors_ht(Authors_HT *ht) {
  if (ht == NULL) {
    return;
  }

  // Initializing authors HT
  ht->hmax = HMAX_SMALL;
  ht->hash_function = hash_function_int;
  ht->compare_function = compare_function_ints;

  // Initializing buckets
  ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
  DIE(ht->buckets == NULL, "Authors_HT: ht->buckets");

  int i;
  for (i = 0; i < ht->hmax; i++) {
    init_list(&ht->buckets[i]);
  }
}

void add_author(Authors_HT *ht, int64_t author_id, int64_t paper_id,
                int paper_year) {
  if (ht == NULL) {
    return;
  }

  unsigned int hash = ht->hash_function(&author_id) % ht->hmax;
  struct LinkedList *bucket = &ht->buckets[hash];

  authors_paper *new_author = (authors_paper *)calloc(1, sizeof(authors_paper));
  DIE(new_author == NULL, "add_author->new_author calloc");

  // Allocating memory for key
  new_author->author_id = calloc(1, sizeof(int64_t));
  DIE(new_author->author_id == NULL, "new_author->author_id calloc");

  memcpy(new_author->author_id, &author_id, sizeof(int64_t)); // copying key

  // Values
  new_author->paper_id = paper_id;
  new_author->paper_year = paper_year;

  // Add/chain => bascially appending to the current bucket
  add_last_node(bucket, new_author);
}

void free_author_ht(Authors_HT *ht) {
  if (ht == NULL) {
    return;
  }

  int i;
  for (i = 0; i < ht->hmax; i++) {
    struct LinkedList *bucket = &ht->buckets[i];
    struct Node *it = bucket->head;

    struct Node *prev;
    while (it != NULL) {
      prev = it;
      it = it->next;

      authors_paper *inside_data = (authors_paper *)prev->data;
      free(inside_data->author_id);
      free(inside_data);
      free(prev);
    }
  }

  free(ht->buckets);
  free(ht);
}

void init_influence_ht(Influence_HT *ht) {
  if (ht == NULL) {
    return;
  }

  // Initializing authors HT
  ht->hmax = HMAX_BIG;
  ht->hash_function = hash_function_int;
  ht->compare_function = compare_function_ints;

  // Initializing buckets
  ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
  DIE(ht->buckets == NULL, "Authors_HT: ht->buckets");

  int i;
  for (i = 0; i < ht->hmax; i++) {
    init_list(&ht->buckets[i]);
  }
}

void add_influence(Influence_HT *ht, int64_t influencer_id,
                   int64_t imitator_id) {
  if (ht == NULL) {
    return;
  }

  unsigned int hash = ht->hash_function(&influencer_id) % ht->hmax;
  struct LinkedList *bucket = &ht->buckets[hash];

  influenced_paper *new_influence =
      (influenced_paper *)calloc(1, sizeof(influenced_paper));
  DIE(new_influence == NULL, "add_influence->new_influence calloc");

  // Allocating memory for key
  new_influence->key = calloc(1, sizeof(int64_t));
  DIE(new_influence->key == NULL, "new_influence->influence_id calloc");

  memcpy(new_influence->key, &influencer_id, sizeof(int64_t)); // copying key

  // Values
  new_influence->value = imitator_id;

  // Add/chain => bascially appending to the current bucket
  add_last_node(bucket, new_influence);
}

void free_influence_ht(Influence_HT *ht) {
  if (ht == NULL) {
    return;
  }

  int i;
  for (i = 0; i < ht->hmax; i++) {
    struct LinkedList *bucket = &ht->buckets[i];
    struct Node *it = bucket->head;

    struct Node *prev;
    while (it != NULL) {
      prev = it;
      it = it->next;

      influenced_paper *inside_data = (influenced_paper *)prev->data;
      free(inside_data->key);
      free(inside_data);
      free(prev);
    }
  }

  free(ht->buckets);
  free(ht);
}

void init_markings_ht(Markings_HT *ht) {
  if (ht == NULL) {
    return;
  }

  // Initializing authors HT
  ht->hmax = HMAX_BIG;
  ht->hash_function = hash_function_int;
  ht->compare_function = compare_function_ints;

  // Initializing buckets
  ht->buckets = calloc(ht->hmax, sizeof(struct LinkedList));
  DIE(ht->buckets == NULL, "Authors_HT: ht->buckets");

  int i;
  for (i = 0; i < ht->hmax; i++) {
    init_list(&ht->buckets[i]);
  }
}

void add_marking(Markings_HT *ht, int64_t paper_id, int new_distance) {
  if (ht == NULL) {
    return;
  }

  unsigned int hash = ht->hash_function(&paper_id) % ht->hmax;
  struct LinkedList *bucket = &ht->buckets[hash];

  // Each marking is unique
  struct marking *new_marking = malloc(sizeof(marking));
  DIE(new_marking == NULL, "add_marking -> new_marking malloc");

  // Allocating memory for key
  new_marking->id = malloc(sizeof(marking));
  DIE(new_marking->id == NULL, "new_marking->id");

  memcpy(new_marking->id, &paper_id, sizeof(paper_id)); // copying key
  new_marking->visited = 1;
  new_marking->distance = new_distance;

  // Add/chain => bascially appending to the current bucket
  add_last_node(bucket, new_marking);
}

marking *get_markings(Markings_HT *ht, int64_t paper_id) {
  if (ht == NULL) {
    return NULL;
  }

  unsigned int hash = ht->hash_function(&paper_id) % ht->hmax;
  struct LinkedList *bucket = &ht->buckets[hash];

  struct Node *it = bucket->head;

  // Iterating through the bucket until keymatch
  while (it) {
    marking *inside_data = (marking *)it->data;
    // Key match
    if (ht->compare_function(inside_data->id, &paper_id) == 0) {
      return inside_data;
    }
    it = it->next;
  }

  // Nothing found
  return NULL;
}

void free_markings_ht(Markings_HT *ht) {
  if (ht == NULL) {
    return;
  }

  int i;
  for (i = 0; i < ht->hmax; i++) {
    struct LinkedList *bucket = &ht->buckets[i];
    struct Node *it = bucket->head;

    struct Node *prev;
    while (it != NULL) {
      prev = it;
      it = it->next;

      marking *inside_data = (marking *)prev->data;
      free(inside_data->id);
      free(inside_data);
      free(prev);
    }
  }

  free(ht->buckets);
  free(ht);
}