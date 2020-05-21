// Copyright [2020] Razvan-Andrei Matisan, Radu-Stefan Minea

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stddef.h>
#include <stdint.h>

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

#endif /* LINKEDLIST_H_ */
