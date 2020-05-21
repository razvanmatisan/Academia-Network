#ifndef UTILS_H_
#define UTILS_H_

#include <stddef.h>
#include <stdint.h>

unsigned int hash_function_int(void *a);

int compare_function_ints(void *a, void *b);

unsigned int hash_function_string(void *a);

int compare_function_strings(void *a, void *b);

void print_entry(PublData *data, int hash);

Info* find_paper_with_id(PublData *data, int64_t target_id);

void free_aux_data(PublData *data, Info *start_publication);

int compare_task1 (PublData *data, Info *challenger, Info *titleholder);

int no_papers_with_field (Field_HT *field_ht, const char *field, int64_t ids_with_field[NMAX]);

int compare_task5 (PublData *data, Info *publication1, Info *publication2);

void swap (int64_t *a, int64_t *b);

int is_in_array (char *arr_to_find, char **arr, int length);

#endif /* UTILS_H_ */
