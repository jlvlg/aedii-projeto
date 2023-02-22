#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "util.h"
#include "tree.h"

static Tree as_tree(Tree root) {
    return root;
}

static void preorder(Tree root, void function(Tree)) {
    if (root != NULL) {
        function(root);
        preorder(root->l, function);
        preorder(root->r, function);
    }
}

static void inorder(Tree root, void function(Tree)) {
    if (root != NULL) {
        inorder(root->l, function);
        function(root);
        inorder(root->r, function);
    }
}
static void postorder(Tree root, void function(Tree)) {
    if (root != NULL) {
        postorder(root->l, function);
        postorder(root->r, function);
        function(root);
    }
}

static void print_node(Tree root) {
    switch (root->item.type) {
        case INT:
            printf("%d ", *(int*) root->item.data);
            break;
        case FLOAT:
            printf("%f ", *(float*) root->item.data);
            break;
        case DOUBLE:
            printf("%lf ", *(double*) root->item.data);
            break;
        case CHAR:
            printf("%c ", *(char*) root->item.data);
            break;
        case STRING:
            printf("%s ", (char*) root->item.data);
    }
}

static void print(Tree root) {
    preorder(root, print_node);
    printf("\n");
}

static int children(Tree node) {
    if (node == NULL)
        return -1;
    if (node->l == NULL && node->r == NULL)
        return 0;
    if (node->l == NULL || node->r == NULL)
        return 1;
    return 2;
}

static Tree kill(Tree leaf) {
    types.destroy(leaf->item);
    free(leaf);
    return NULL;
}

static Tree search(Tree root, Item item) {
    if (root == NULL)
        return NULL;
    switch (types.cmp(item, root->item)) {
        case -1:
            return search(root->l, item);
        case 0:
            return root;
        case 1:
            return search(root->r, item);
    }
}

static Tree max(Tree root) {
    if (root == NULL || root->r == NULL)
        return root;
    return max(root->r);
}

static Tree min(Tree root) {
    if (root == NULL || root->l == NULL)
        return root;
    return min(root->l);
}

static int height(Tree root) {
    if (root == NULL)
        return 0;
    return util.max(height(root->l), height(root->r)) + 1;
}

static Tree simple_rotation_left(Tree p) {
    Tree u = p->r;
    p->r = u->l;
    u->l = p;
    return u;
}

static Tree simple_rotation_right(Tree p) {
    Tree u = p->l;
    p->l = u->r;
    u->r = p;
    return u;
}

static Tree double_rotation_left(Tree p) {
    p->r = simple_rotation_right(p->r);
    return simple_rotation_left(p);
}

static Tree double_rotation_right(Tree p) {
    p->l = simple_rotation_left(p->l);
    return simple_rotation_right(p);
}

const struct tree_methods tree = {
    .as_tree = as_tree,
    .print = print,
    .preorder = preorder,
    .inorder = inorder,
    .postorder = postorder,
    .children = children,
    .kill = kill,
    .max = max,
    .min = min,
    .search = search,
    .height = height,
    .simple_rotation_left = simple_rotation_left,
    .simple_rotation_right = simple_rotation_right,
    .double_rotation_left = double_rotation_left,
    .double_rotation_right = double_rotation_right
};