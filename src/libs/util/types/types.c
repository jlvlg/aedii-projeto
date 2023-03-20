#include <stdio.h>
#include <string.h>
#include "util.h"
#include "types.h"

Item Int(int val) {
    Item item = util.safe_malloc(sizeof(struct Item));
    int *pointer = util.safe_malloc(sizeof *pointer);
    *pointer = val;
    item->data = pointer;
    item->type = INT;
    return item;
}

Item Float(float val) {
    Item item = util.safe_malloc(sizeof(struct Item));
    float *pointer = util.safe_malloc(sizeof *pointer);
    *pointer = val;
    item->data = pointer;
    item->type = FLOAT;
    return item;
}

Item Double(double val) {
    Item item = util.safe_malloc(sizeof(struct Item));
    double *pointer = util.safe_malloc(sizeof *pointer);
    *pointer = val;
    item->data = pointer;
    item->type = DOUBLE;
    return item;
}

Item Char(char val) {
    Item item = util.safe_malloc(sizeof(struct Item));
    char *pointer = util.safe_malloc(sizeof *pointer);
    *pointer = val;
    item->data = pointer;
    item->type = CHAR;
    return item;
}

Item String(char val[]) {
    Item item = util.safe_malloc(sizeof(struct Item));
    char *pointer = util.init_string(val);
    item->data = pointer;
    item->type = STRING;
    return item;
}

static Item tostring(Item item) {
    switch (item->type) {
        case CHAR: {
            char str[2] = "\0";
            str[0] = *(char*) item->data;
            return String(str);
        }
        case INT: {
            char str[snprintf(NULL, 0, "%d", *(int*) item->data) + 1];
            sprintf(str, "%d", *(int*) item->data);
            return String(str);
        }
    }
    return item;
}

static int cmp(Item item1, Item item2) {
    if (item1->type == INT && item2->type == INT) {
        if (*(int*) item1->data > *(int*) item2->data) 
            return 1;
        if (*(int*) item1->data < *(int*) item2->data)
            return -1;
        return 0;
    }
    int res = strcmp((char*) tostring(item1)->data, (char*) tostring(item2)->data);
    if (res > 0)
        return 1;
    if (res < 0)
        return -1;
    return 0;
}

static void destroy(Item item) {
    free(item->data);
    free(item);
}

static Item copy(Item item) {
    switch (item->type) {
        case INT:
            return Int(*(int*) item->data);
        case FLOAT:
            return Float(*(float*) item->data);
        case DOUBLE:
            return Double(*(double*) item->data);
        case CHAR:
            return Char(*(char*) item->data);
        case STRING:
            return String((char*) item->data);
    }
}

const struct types_methods types = {
    .cmp = cmp,
    .destroy = destroy,
    .copy = copy,
    .Int = Int,
    .Float = Float,
    .Double = Double,
    .Char = Char,
    .String = String,
    .tostring = tostring
};