#include <stdlib.h>
#include "util.h"
#include "types.h"
#include "tree.h"
#include "rb.h"

static RB as_rb(Tree root) {
    return root;
}

static RB parent(RB node) {
    return node != NULL ? node->parent : NULL;
}

static RB grandparent(RB node) {
    return parent(parent(node));
}

static int is_root(RB node) {
    return parent(node) == NULL;
}

static int is_left(RB node) {
    return tree.as_tree(parent(node))->l == node;
}

static int is_right(RB node) {
    return tree.as_tree(parent(node))->r == node;
}

static RB sibling(RB node) {
    if (node != NULL && !is_root(node)) {
        Tree tree_parent = parent(node);
        return is_left(node) ? tree_parent->r : tree_parent->l;
    }
    return NULL;
}

static RB uncle(RB node) {
    return sibling(parent(node));
}

static RB simple_left_rotation(RB node) {

}

static RB create(Item item) {
    RB leaf = util.safe_malloc(sizeof(RB_Node));
    Tree tree_leaf = leaf;
    tree_leaf->item = item;
    tree_leaf->l = tree_leaf->r = NULL;
    leaf->parent = NULL;
    leaf->is_black = 0;
    return leaf;
}

static void insert(RB *root, RB leaf) {
    Tree tree_leaf = leaf, track = NULL;
    
    for (Tree search = *root; search != NULL;) {
        track = search;
        if (types.cmp(tree_leaf->item, search->item) > 0) {
            search = search->r;
        } else {
            search = search->l;
        }
    }

    leaf->parent = track;

    if (track == NULL) {
        *root = leaf;
    } else if (types.cmp(tree_leaf->item, track->item) > 0) {
        track->r = leaf;
    } else {
        track->l = leaf;
    }
    
    ajustar(root, leaf);
}