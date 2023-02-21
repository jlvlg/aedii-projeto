#include <stdio.h>
#include "util.h"
#include "types.h"

Item Int(int val) {
    int *pointer = malloc(sizeof *pointer);
    *pointer = val;
    return (Item) {.data = pointer, .type = INT};
}

Item Float(float val) {
    float *pointer = malloc(sizeof *pointer);
    *pointer = val;
    return (Item) {.data = pointer, .type = FLOAT};
}

Item Double(double val) {
    double *pointer = malloc(sizeof *pointer);
    *pointer = val;
    return (Item) {.data = pointer, .type = DOUBLE};
}

Item Char(char val) {
    char *pointer = malloc(sizeof *pointer);
    *pointer = val;
    return (Item) {.data = pointer, .type = CHAR};
}

Item String(char val[]) {
    char *pointer = util.init_string(val);
    return (Item) {.data = pointer, .type = STRING};
}

static Item tostring(Item item) {
    switch (item.type) {
        case CHAR: {
            char str[2] = "\0";
            str[0] = *(char*) item.data;
            return String(str);
        }
        case INT: {
            char str[snprintf(NULL, 0, "%d", *(int*) item.data) + 1];
            sprintf(str, "%d", *(int*) item.data);
            return String(str);
        }
    }
    return item;
}

static int cmp(Item item1, Item item2) {
    if (item1.type == STRING || item2.type == STRING)
        return util.lowercmp(tostring(item1).data, tostring(item2).data);
    if (*(int*) item1.data > *(int*) item2.data) 
        return 1;
    if (*(int*) item1.data < *(int*) item2.data)
        return -1;
    return 0;
}

static void destroy(Item item) {
    free(item.data);
}

static Item copy(Item item) {
    switch (item.type) {
        case INT:
            return Int(*(int*) item.data);
        case FLOAT:
            return Float(*(float*) item.data);
        case DOUBLE:
            return Double(*(double*) item.data);
        case CHAR:
            return Char(*(char*) item.data);
        case STRING:
            return String((char*) item.data);
    }
}

const struct types_methods types = {
    .cmp = cmp,
    .destroy = destroy,
    .copy = copy
};