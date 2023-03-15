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

static int junk_size(Junk node) {
    return node->epos - node->spos;
}

static Junk insert(Junk start, int spos, int epos) {
    if (epos - spos <= 0)
        return start;
    if (start == NULL || epos - spos >= junk_size(start)) {
        Junk junk = util.safe_malloc(sizeof(JunkNode));
        junk->spos = spos;
        junk->epos = epos;
        return junk;
    }
    start->next = insert(start->next, spos, epos);
    return start;
}

static Junk pop(Junk start, int* out_spos, int* out_epos) {
    Junk new_start = start->next;
    *out_spos = start->spos;
    *out_epos = start->epos;
    free(start);
    return new_start;
}

static void find_junk_by_spos(Junk start, Junk *previous, Junk *found, int spos) {
    *previous = NULL;
    for (*found = start; *found != NULL && (*found)->spos != spos; *found = (*found)->next)
        *previous = *found;
}

static void find_junk_by_epos(Junk start, Junk *previous, Junk *found, int epos) {
    *previous = NULL;
    for (*found = start; *found != NULL && (*found)->epos != epos; *found = (*found)->next)
        *previous = *found;
}

static void merge_helper(Junk *start, Junk track, Junk seek, int *spos, int *epos, int *min_spos, int *max_epos) {
    if (seek != NULL) {
        if (track == NULL)
            *start = pop(seek, spos, epos);
        else
            track->next = pop(seek, spos, epos);
        *min_spos = util.min(*min_spos, *spos);
        *max_epos = util.max(*max_epos, *epos);
    }
}

static int merge_junk(Junk *start, int size, int pos) {
    int _;
    int max_epos = pos + size; 
    int min_spos = pos;
    Junk sseek, strack, eseek, etrack;
    find_junk_by_epos(*start, &etrack, &eseek, pos);
    find_junk_by_spos(*start, &strack, &sseek, pos + size);

    merge_helper(start, strack, sseek, &_, &_, &min_spos, &max_epos);
    merge_helper(start, etrack, eseek, &_, &_, &min_spos, &max_epos);

    if (min_spos != pos || max_epos != pos + size) {
        *start = insert(*start, min_spos, max_epos);
        return 0;
    }
    return 1;
}

static void restore_junk(Junk *start, int size, int pos) {
    int _ = 0, epos = pos + size;
    Junk seek, track;
    find_junk_by_spos(*start, &track, &seek, pos);
    merge_helper(start, track, seek, &_, &epos, &_, &_);

    if (epos != pos + size)
        *start = insert(*start, pos, pos + size);
}

static void dump(Junk *start, int size, int pos) {
    if (merge_junk(start, size, pos)) {
        *start = insert(*start, pos, pos + size);
    }
}

static int recycle(Junk* start, int size) {
    int spos, epos;
    if (*start == NULL || size > junk_size(*start))
        return -1;
    *start = pop(*start, &spos, &epos);
    *start = insert(*start, spos + size, epos);
    return spos;
}

static void add_junk_to_array(json_object *array, Junk node) {
    if (node != NULL) {
        add_junk_to_array(array, node->next);
        json_object *object = json_object_new_object();
        json_object_object_add(object, "spos", json_object_new_int(node->spos));
        json_object_object_add(object, "epos", json_object_new_int(node->epos));
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
    json_object *node, *spos, *epos;

    if (json == NULL)
        return NULL;

    for (int i = 0; i < json_object_array_length(json); i++) {
        node = json_object_array_get_idx(json, i);
        json_object_object_get_ex(node, "spos", &spos);
        json_object_object_get_ex(node, "epos", &epos);
        start = insert(start, json_object_get_int(spos), json_object_get_int(epos));
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
    .restore_junk = restore_junk
};