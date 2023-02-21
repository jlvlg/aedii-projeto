#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"

static void* safe_malloc(size_t size) {
    void *pointer = malloc(size);
    if (pointer == NULL)
        exit(1);
    return pointer;
}

static char* string_malloc(char string[]) {
    return safe_malloc(sizeof(char) * (strlen(string) + 1));
}

static char* init_string(char string[]) {
    char *new_string = string_malloc(string);
    strcpy(new_string, string);
    return new_string;
}

static int lowercmp(char str1[], char str2[]) {
    for (int i = 0; i < strlen(str1) + 1; i++) {
        char c1, c2;
        c1 = tolower(str1[i]);
        c2 = tolower(str2[i]);
        if (c1 > c2)
            return 1;
        if (c1 < c2)
            return -1;
    }
    return 0;
}

static int max(int a, int b) {
    return a > b ? a : b;
}

const struct util_methods util = {
    .safe_malloc = safe_malloc,
    .string_malloc = string_malloc,
    .init_string = init_string,
    .lowercmp = lowercmp,
    .max = max
};