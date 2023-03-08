#include <stdlib.h>
#include <stdio.h>
#include "bst.h"
#include "avl.h"
#include "rb.h"
#include "util.h"
#include "types.h"
#include "index.h"
#include "json.h"

static BST_Index create_bst(Item item, int pos) {
    BST_Index leaf = bst.init(util.safe_malloc(sizeof(struct BST_Index)), item);
    leaf->pos = pos;
    return leaf;
}

static AVL_Index create_avl(Item item, int pos) {
    AVL_Index leaf = avl.init(util.safe_malloc(sizeof(struct AVL_Index)), item);
    leaf->pos = pos;
    return leaf;
}

static RB_Index create_rb(Item item, int pos) {
    RB_Index leaf = rb.init(util.safe_malloc(sizeof(struct RB_Index)), item);
    leaf->pos = pos;
    return leaf;
}

static Junk dump(Junk start, int size, int pos) {
    if (size <= 0)
        return start;
    if (start == NULL || size > start->size) {
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

static void add_bst_to_array(json_object *array, BST_Index node) {
    if (node != NULL) {
        json_object *object = json_object_new_object();
        Item data = ((Tree) node)->item;
        switch (data.type) {
            case INT:
                json_object_object_add(object, "key", json_object_new_int(*(int*) data.data));
                break;
            case CHAR:
            case STRING:
                json_object_object_add(object, "key", json_object_new_string((char*) data.data));
                break;
        }
        json_object_object_add(object, "pos", json_object_new_int(node->pos));
        json_object_array_add(array, object);
        add_bst_to_array(array, ((Tree) node)->l);
        add_bst_to_array(array, ((Tree) node)->r);
    }
}

static int save_bst(BST_Index root, char* filename) {
    int result;
    json_object *json = json_object_new_array();
    add_bst_to_array(json, root);
    result = json_object_to_file(filename, json);
    json_object_put(json);
    return result;
}

static void add_avl_to_array(json_object *array, AVL_Index node) {
    if (node != NULL) {
        json_object *object = json_object_new_object();
        Item data = ((Tree) node)->item;
        switch (data.type) {
            case INT:
                json_object_object_add(object, "key", json_object_new_int(*(int*) data.data));
                break;
            case CHAR:
            case STRING:
                json_object_object_add(object, "key", json_object_new_string((char*) data.data));
                break;
        }
        json_object_object_add(object, "pos", json_object_new_int(node->pos));
        json_object_array_add(array, object);
        add_avl_to_array(array, ((Tree) node)->l);
        add_avl_to_array(array, ((Tree) node)->r);
    }
}

static int save_avl(AVL_Index root, char* filename) {
    int result;
    json_object *json = json_object_new_array();
    add_avl_to_array(json, root);
    result = json_object_to_file(filename, json);
    json_object_put(json);
    return result;
}

static void add_rb_to_array(json_object *array, RB_Index node) {
    if (node != NULL) {
        json_object *object = json_object_new_object();
        Item data = ((Tree) node)->item;
        switch (data.type) {
            case INT:
                json_object_object_add(object, "key", json_object_new_int(*(int*) data.data));
                break;
            case CHAR:
            case STRING:
                json_object_object_add(object, "key", json_object_new_string((char*) data.data));
                break;
        }
        json_object_object_add(object, "pos", json_object_new_int(node->pos));
        json_object_array_add(array, object);
        add_rb_to_array(array, ((Tree) node)->l);
        add_rb_to_array(array, ((Tree) node)->r);
    }
}

static int save_rb(RB_Index root, char* filename) {
    int result;
    json_object *json = json_object_new_array();
    add_rb_to_array(json, root);
    result = json_object_to_file(filename, json);
    json_object_put(json);
    return result;
}

static BST_Index retrieve_bst(char* filename) {
    BST_Index root = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *node, *key, *pos;
    Item data;

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
                return bst.trim(root, root);
        }
        root = bst.insert(root, create_bst(data, json_object_get_int(pos)));
    }
    json_object_put(json);
    return root;
}

static AVL_Index retrieve_avl(char* filename) {
    AVL_Index root = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *node, *key, *pos;
    Item data;
    int changes;

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
                return avl.trim(root, root);
        }
        root = avl.insert(root, create_avl(data, json_object_get_int(pos)), &changes);
    }
    json_object_put(json);
    return root;
}

static RB_Index retrieve_rb(char* filename) {
    RB_Index root = NULL;
    json_object *json = json_object_from_file(filename);
    json_object *node, *key, *pos;
    Item data;

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
                return rb.trim(&root, root);
        }
        rb.insert(&root, create_rb(data, json_object_get_int(pos)));
    }
    json_object_put(json);
    return root;
}

const struct index_methods idx = {
    .save_bst = save_bst,
    .save_avl = save_avl,
    .save_rb = save_rb,
    .retrieve_bst = retrieve_bst,
    .retrieve_avl = retrieve_avl,
    .retrieve_rb = retrieve_rb,
    .dump = dump,
    .recycle = recycle
};