#include "bst.h"
#include "avl.h"
#include "rb.h"
#include "util.h"
#include "types.h"
#include "index.h"
#include "json_object.h"
#include "json_util.h"

static Index create_index(Item *key, int pos) {
    Index index = util.safe_malloc(sizeof(struct Index));
    index->item = **key;
    index->pos = pos;
    free(*key);
    *key = NULL;
    return index;
}

static Index copy(Index index) {
    Index new = util.safe_malloc(sizeof(struct Index));
    Item item = types.copy((Item) index);
    new->item = *item;
    new->pos = index->pos;
    free(item);
    return new;
}

static int garbage_size(Garbage node) {
    return node->end - node->start;
}

static Garbage insert(Garbage node, int start, int end) {
    if (end - start <= 0)
        return node;
    if (node == NULL || end - start >= garbage_size(node)) {
        Garbage garbage = util.safe_malloc(sizeof(GarbageNode));
        garbage->start = start;
        garbage->end = end;
        garbage->next = node;
        return garbage;
    }
    node->next = insert(node->next, start, end);
    return node;
}

static Garbage pop(Garbage node, int* out_start, int* out_end) {
    Garbage new_start = node->next;
    *out_start = node->start;
    *out_end = node->end;
    free(node);
    return new_start;
}

static void find_garbage_by_start(Garbage node, Garbage *previous, Garbage *found, int start) {
    *previous = NULL;
    for (*found = node; *found != NULL && (*found)->start != start; *found = (*found)->next)
        *previous = *found;
}

static void find_garbage_by_end(Garbage node, Garbage *previous, Garbage *found, int end) {
    *previous = NULL;
    for (*found = node; *found != NULL && (*found)->end != end; *found = (*found)->next)
        *previous = *found;
}

static void merge_helper(Garbage *node, Garbage track, Garbage seek, int *start, int *end, int *min_start, int *max_end) {
    if (seek != NULL) {
        if (track == NULL)
            *node = pop(seek, start, end);
        else
            track->next = pop(seek, start, end);
        *min_start = util.min(*min_start, *start);
        *max_end = util.max(*max_end, *end);
    }
}

static int merge_garbage(Garbage *node, int size, int pos) {
    int start, end;
    int max_end = pos + size; 
    int min_start = pos;
    Garbage sseek, strack, eseek, etrack;
    find_garbage_by_end(*node, &etrack, &eseek, pos);
    merge_helper(node, etrack, eseek, &start, &end, &min_start, &max_end);

    find_garbage_by_start(*node, &strack, &sseek, pos + size);
    merge_helper(node, strack, sseek, &start, &end, &min_start, &max_end);

    if (min_start != pos || max_end != pos + size) {
        *node = insert(*node, min_start, max_end);
        return 0;
    }
    return 1;
}

static void dump(Garbage *start, int size, int pos) {
    if (merge_garbage(start, size, pos)) {
        *start = insert(*start, pos, pos + size);
    }
}

static int recycle(Garbage* node, int size) {
    int start, end;
    if (*node == NULL || size > garbage_size(*node))
        return -1;
    *node = pop(*node, &start, &end);
    *node = insert(*node, start + size, end);
    return start;
}

static void add_garbage_to_array(json_object *array, Garbage node) {
    if (node != NULL) {
        add_garbage_to_array(array, node->next);
        json_object *object = json_object_new_object();
        json_object_object_add(object, "start", json_object_new_int(node->start));
        json_object_object_add(object, "end", json_object_new_int(node->end));
        json_object_array_add(array, object);
    }
}

static int save_garbage(Garbage garbage, char* filename) {
    int result;
    json_object *json = json_object_new_array();
    add_garbage_to_array(json, garbage);
    result = json_object_to_file(filename, json);
    json_object_put(json);
    return result;
}

static Garbage retrieve_garbage(char* filename) {
    Garbage node = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *json_node, *start, *end;

    if (json == NULL)
        return NULL;

    for (int i = 0; i < json_object_array_length(json); i++) {
        json_node = json_object_array_get_idx(json, i);
        json_object_object_get_ex(json_node, "start", &start);
        json_object_object_get_ex(json_node, "end", &end);
        node = insert(node, json_object_get_int(start), json_object_get_int(end));
    }
    json_object_put(json);
    return node;
}

static Garbage destroy_garbage(Garbage garbage) {
    if (garbage != NULL) {
        garbage->next = destroy_garbage(garbage->next);
        free(garbage);
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
                data = types.String((char*) json_object_get_string(key));
                break;
            default:
                return tree.clear(root);
        }
        root = bst.insert(root, bst.create((Item) create_index(&data, json_object_get_int(pos))), &success);
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
                data = types.String((char*) json_object_get_string(key));
                break;
            default:
                return (AVL) tree.clear((Tree) root);
        }
        root = avl.insert(root, avl.create((Item) create_index(&data, json_object_get_int(pos))), &changes, &success);
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
                data = types.String((char*) json_object_get_string(key));
                break;
            default:
                return (RB) tree.clear((Tree) root);
        }
        rb.insert(&root, rb.create((Item) create_index(&data, json_object_get_int(pos))), &success);
    }
    json_object_put(json);
    return root;
}



const struct index_methods idx = {
    .index = create_index,
    .copy = copy,
    .save_garbage = save_garbage,
    .save_tree = save_tree,
    .retrieve_bst = retrieve_bst,
    .retrieve_avl = retrieve_avl,
    .retrieve_rb = retrieve_rb,
    .retrieve_garbage = retrieve_garbage,
    .dump = dump,
    .recycle = recycle,
    .destroy_garbage = destroy_garbage
};