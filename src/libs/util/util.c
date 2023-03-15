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

static void* safe_realloc(void* ptr, size_t size) {
    void *pointer = realloc(ptr, size);
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

static int max(int a, int b) {
    return a > b ? a : b;
}

static int min(int a, int b) {
    return a > b ? b : a;
}

const struct util_methods util = {
    .safe_malloc = safe_malloc,
    .safe_realloc = safe_realloc,
    .string_malloc = string_malloc,
    .init_string = init_string,
    .max = max,
    .min = min
};