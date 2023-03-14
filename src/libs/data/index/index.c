#include <stdlib.h>
#include <stdio.h>
#include "bst.h"
#include "avl.h"
#include "rb.h"
#include "util.h"
#include "types.h"
#include "index.h"
#include "json_object.h"
#include "json_util.h"

static Index index(Item key, int pos) {
    Index index = malloc(sizeof(struct Index));
    index->item = *key;
    index->pos = pos;
    free(key);
    return index;
}

static Index copy(Index index) {
    Index new = malloc(sizeof(struct Index));
    Item item = types.copy(index);
    new->item = *item;
    new->pos = index->pos;
    free(item);
    return new;
}

static Junk dump(Junk start, int size, int pos) {
    if (size <= 0)
        return start;
    if (start == NULL || size >= start->size) {
        Junk junk = util.safe_malloc(sizeof(JunkNode));
        junk->size = size;
        junk->pos = pos;
        junk->next = start;
        return junk;
    }
    start->next = dump(start->next, size, pos);
    return start;
}

static Junk pop(Junk start, int* out_size, int* out_pos) {
    Junk new_start = start->next;
    *out_size = start->size;
    *out_pos = start->pos;
    free(start);
    return new_start;
}

static int recycle(Junk* start, int size) {
    if (*start == NULL || size > (*start)->size)
        return -1;
    int old_size, pos;
    *start = pop(*start, &old_size, &pos);
    *start = dump(*start, old_size - size, pos + size);
    return pos;
}

static Junk remove_junk(Junk start, int pos) {
    if (start != NULL) {
        int _;
        if (pos == start->pos)
            start = pop(start, &_, &_);
        start->next = remove_junk(start->next, pos);
    }
    return start;
}

static void add_junk_to_array(json_object *array, Junk node) {
    if (node != NULL) {
        add_junk_to_array(array, node->next);
        json_object *object = json_object_new_object();
        json_object_object_add(object, "size", json_object_new_int(node->size));
        json_object_object_add(object, "pos", json_object_new_int(node->pos));
        json_object_array_add(array, object);
    }
}

static int save_junk(Junk junk, char* filename) {
    int result;
    json_object *json = json_object_new_array();
    add_junk_to_array(json, junk);
    result = json_object_to_file(filename, json);
    json_object_put(json);
    return result;
}

static Junk retrieve_junk(char* filename) {
    Junk start = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *node, *size, *pos;

    if (json == NULL)
        return NULL;

    for (int i = 0; i < json_object_array_length(json); i++) {
        node = json_object_array_get_idx(json, i);
        json_object_object_get_ex(node, "size", &size);
        json_object_object_get_ex(node, "pos", &pos);
        start = dump(start, json_object_get_int(size), json_object_get_int(pos));
    }
    json_object_put(json);
    return start;
}

static Junk destroy_junk(Junk junk) {
    if (junk != NULL) {
        junk->next = destroy_junk(junk->next);
        free(junk);
    }
    return NULL;
}

static void restore_junk(Junk *start, int size, int pos) {
    Junk seek, track = NULL;
    int old_size, old_pos;
    for (seek = *start; seek != NULL && seek->pos != pos + size; seek = seek->next)
        track = seek;
    if (seek == NULL) {
        *start = dump(*start, size, pos);
        return;
    }
    if (track == NULL) {
        *start = pop(*start, &old_size, &old_pos);
        *start = dump(*start, size + old_size, pos);
        return;
    }
    track->next = pop(track->next, &old_size, &old_pos);
    track->next = dump(track->next, size + old_size, pos);
}

static void add_tree_to_array(json_object *array, Tree node) {
    if (node != NULL) {
        json_object *object = json_object_new_object();
        Item index = node->item;
        switch (index->type) {
            case INT:
                json_object_object_add(object, "key", json_object_new_int(*(int*) index->data));
                break;
            case CHAR:
            case STRING:
                json_object_object_add(object, "key", json_object_new_string((char*) index->data));
                break;
        }
        json_object_object_add(object, "pos", json_object_new_int(((Index) index)->pos));
        json_object_array_add(array, object);
        add_tree_to_array(array, node->l);
        add_tree_to_array(array, node->r);
    }
}

static int save_tree(Tree root, char* filename) {
    int result;
    json_object *json = json_object_new_array();
    add_tree_to_array(json, root);
    result = json_object_to_file(filename, json);
    json_object_put(json);
    return result;
}

static BST retrieve_bst(char* filename) {
    BST root = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *node, *key, *pos;
    Item data;
    int success;

    if (json == NULL)
        return NULL;

    for (int i = 0; i < json_object_array_length(json); i++) {
        node = json_object_array_get_idx(json, i);
        json_object_object_get_ex(node, "key", &key);
        json_object_object_get_ex(node, "pos", &pos);
        switch (json_object_get_type(key)) {
            case json_type_int:
                data = types.Int(json_object_get_int(key));
                break;
            case json_type_string:
                data = types.String(json_object_get_string(key));
                break;
            default:
                return tree.clear(root);
        }
        root = bst.insert(root, bst.create(index(data, json_object_get_int(pos))), &success, copy);
    }
    json_object_put(json);
    return root;
}

static AVL retrieve_avl(char* filename) {
    AVL root = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *node, *key, *pos;
    Item data;
    int success, changes;

    if (json == NULL)
        return NULL;

    for (int i = 0; i < json_object_array_length(json); i++) {
        node = json_object_array_get_idx(json, i);
        json_object_object_get_ex(node, "key", &key);
        json_object_object_get_ex(node, "pos", &pos);
        switch (json_object_get_type(key)) {
            case json_type_int:
                data = types.Int(json_object_get_int(key));
                break;
            case json_type_string:
                data = types.String(json_object_get_string(key));
                break;
            default:
                return tree.clear(root);
        }
        root = avl.insert(root, avl.create(index(data, json_object_get_int(pos))), &changes, &success, copy);
    }
    json_object_put(json);
    return root;
}

static RB retrieve_rb(char* filename) {
    RB root = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *node, *key, *pos;
    Item data;
    int success;

    if (json == NULL)
        return NULL;

    for (int i = 0; i < json_object_array_length(json); i++) {
        node = json_object_array_get_idx(json, i);
        json_object_object_get_ex(node, "key", &key);
        json_object_object_get_ex(node, "pos", &pos);
        switch (json_object_get_type(key)) {
            case json_type_int:
                data = types.Int(json_object_get_int(key));
                break;
            case json_type_string:
                data = types.String(json_object_get_string(key));
                break;
            default:
                return tree.clear(root);
        }
        rb.insert(&root, rb.create(index(data, json_object_get_int(pos))), &success, copy);
    }
    json_object_put(json);
    return root;
}



const struct index_methods idx = {
    .index = index,
    .copy = copy,
    .save_junk = save_junk,
    .save_tree = save_tree,
    .retrieve_bst = retrieve_bst,
    .retrieve_avl = retrieve_avl,
    .retrieve_rb = retrieve_rb,
    .retrieve_junk = retrieve_junk,
    .dump = dump,
    .recycle = recycle,
    .remove_junk = remove_junk,
    .restore_junk = restore_junk
};