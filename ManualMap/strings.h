#include <stdlib.h>

int find(const char* source, const char* to_compare, int size);
int get_str_len(char* a); //Infinite LOOP CAUTION // NULL Character Included
void strings_sort(char **a, int n);
int get_char_words(char **a, int size_vector_max); // Possible overflow on scan array
void print_vector_words(char **array, int size);