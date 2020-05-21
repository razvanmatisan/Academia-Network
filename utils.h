// Copyright [2020] Razvan-Andrei Matisan, Radu-Stefan Minea

#ifndef UTILS_H_
#define UTILS_H_

#include <stddef.h>
#include <stdint.h>

#define CURR_YEAR 2020
#define MAX_YEAR 2050
#define INITIAL_HISTOGRAM_SIZE 1
#define VISITED 1
#define UNVISITED 0

unsigned int hash_function_int(void *a);

int compare_function_ints(void *a, void *b);

unsigned int hash_function_string(void *a);

int compare_function_strings(void *a, void *b);

Paper *find_paper_with_id(PublData *data, int64_t target_id);

void clean_refs_aux_data(PublData *data, Paper *start_publication);

int compare_task1(PublData *data, Paper *challenger, Paper *titleholder);

int no_papers_with_field(Field_HT *field_ht, const char *field,
                         int64_t ids_with_field[NMAX]);

int compare_task5(PublData *data, Paper *publication1, Paper *publication2);

void swap(int64_t *a, int64_t *b);

int is_in_array(char *arr_to_find, char **arr, int length);

#endif /* UTILS_H_ */
