#include <stdio.h>
#include <stdlib.h>

int main() {
    char *p = calloc(1, sizeof(char));
    p = NULL;
    printf("%p\n", p);
    return 0;
}
